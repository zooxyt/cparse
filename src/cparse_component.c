/* CParse : Component
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_value_char.h"
#include "cparse_value_list.h"
#include "cparse_value_string.h"
#include "cparse_charenc.h"
#include "cparse_regex.h"
#include "cparse_expr.h"
#include "cparse_functional.h"
#include "cparse_component.h"


struct cparse_component_range
{
    cparse_size_t count_min, count_max;
    struct cparse_component *sub_component;
    int greed;
};

static struct cparse_component_range *cparse_component_range_new(\
        cparse_size_t count_min, cparse_size_t count_max, \
        struct cparse_component *sub_component, \
        int greed)
{
    struct cparse_component_range *new_component_range = NULL;

    if ((new_component_range = (struct cparse_component_range *)cparse_malloc( \
                    sizeof(struct cparse_component_range))) == NULL)
    { return NULL; }
    new_component_range->count_min = count_min;
    new_component_range->count_max = count_max;
    new_component_range->sub_component = sub_component;
    new_component_range->greed = greed;

    return new_component_range;
}

static void cparse_component_range_destroy( \
        struct cparse_component_range *component_range)
{
    if (component_range->sub_component != NULL)
    { cparse_component_destroy(component_range->sub_component); }
    cparse_free(component_range);
}

struct cparse_component_between
{
    struct cparse_component *component_open;
    struct cparse_component *component_close;
    struct cparse_component *component_sub;
};

static struct cparse_component_between *cparse_component_between_new(\
        struct cparse_component *component_open, \
        struct cparse_component *component_close, \
        struct cparse_component *component_sub)
{
    struct cparse_component_between *new_component_between = NULL;

    if ((new_component_between = (struct cparse_component_between *)cparse_malloc( \
                    sizeof(struct cparse_component_between))) == NULL)
    { return NULL; }
    new_component_between->component_open = component_open;
    new_component_between->component_close = component_close;
    new_component_between->component_sub = component_sub;

    return new_component_between;
}

static void cparse_component_between_destroy( \
        struct cparse_component_between *component_between)
{
    if (component_between->component_open != NULL)
    { cparse_component_destroy(component_between->component_open); }
    if (component_between->component_close != NULL)
    { cparse_component_destroy(component_between->component_close); }
    if (component_between->component_sub != NULL)
    { cparse_component_destroy(component_between->component_sub); }
    cparse_free(component_between);
}

struct cparse_component_option
{
    cparse_value_t *value_x;
    struct cparse_component *component_p;
};

static struct cparse_component_option *cparse_component_option_new(\
        cparse_value_t *value_x, \
        struct cparse_component *component_p)
{
    struct cparse_component_option *new_component_option = NULL;

    if ((new_component_option = (struct cparse_component_option *)cparse_malloc( \
                    sizeof(struct cparse_component_option))) == NULL)
    { return NULL; }
    new_component_option->value_x = value_x;
    new_component_option->component_p = component_p;

    return new_component_option;
}

static void cparse_component_option_destroy( \
        struct cparse_component_option *component_option)
{
    if (component_option->value_x != NULL)
    { cparse_value_destroy(component_option->value_x); }
    if (component_option->component_p != NULL)
    { cparse_component_destroy(component_option->component_p); }
    cparse_free(component_option);
}

struct cparse_component_sepby
{
    struct cparse_component *component_p;
    struct cparse_component *component_sep;
};

static struct cparse_component_sepby *cparse_component_sepby_new(\
        struct cparse_component *component_p, \
        struct cparse_component *component_sep)
{
    struct cparse_component_sepby *new_component_sepby = NULL;

    if ((new_component_sepby = (struct cparse_component_sepby *)cparse_malloc( \
                    sizeof(struct cparse_component_sepby))) == NULL)
    { return NULL; }
    new_component_sepby->component_p = component_p;
    new_component_sepby->component_sep = component_sep;

    return new_component_sepby;
}

static void cparse_component_sepby_destroy( \
        struct cparse_component_sepby *component_sepby)
{
    if (component_sepby->component_p != NULL)
    { cparse_component_destroy(component_sepby->component_p); }
    if (component_sepby->component_sep != NULL)
    { cparse_component_destroy(component_sepby->component_sep); }
    cparse_free(component_sepby);
}

struct cparse_component
{
    cparse_component_type_t type;
    union
    {
        cparse_native_component_t part_native;
        cparse_value_char_t part_char;
        cparse_value_string_t *part_string;
        struct cparse_component *part_component;
        cparse_bool (*part_predict)(int ch);
        struct cparse_value_list *part_component_list;
        struct cparse_component_range *part_component_range;
        struct cparse_component_between *part_component_between;
        struct cparse_component_option *part_component_option;
        struct cparse_component_sepby *part_component_sepby;
        struct cparse_component_sepby *part_component_manytill;
        struct cparse_component_sepby *part_component_exclude;
        cparse_regex_t *part_regex;
        cparse_expr_t *part_expr;
    } u;
};

/* Create New Component */
static cparse_component_t *cparse_component_new(cparse_component_type_t type)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = (cparse_component_t *)cparse_malloc( \
                    sizeof(cparse_component_t))) == NULL)
    { return NULL; }
    new_component->type = type;
    switch (type)
    {
        case CPARSE_COMPONENT_TYPE_REGEX:
            new_component->u.part_regex = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_EXPR:
            new_component->u.part_expr = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_SPACE:
        case CPARSE_COMPONENT_TYPE_SPACES:
        case CPARSE_COMPONENT_TYPE_ENDOFLINE:
        case CPARSE_COMPONENT_TYPE_ANYCHAR:
        case CPARSE_COMPONENT_TYPE_EOF:
            break;
        case CPARSE_COMPONENT_TYPE_NATIVE:
            new_component->u.part_native = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_CHAR:
            new_component->u.part_char = 0;
            break;
        case CPARSE_COMPONENT_TYPE_STRING:
        case CPARSE_COMPONENT_TYPE_ONEOF:
        case CPARSE_COMPONENT_TYPE_NONEOF:
        case CPARSE_COMPONENT_TYPE_FAIL:
        case CPARSE_COMPONENT_TYPE_FAILLOC:
            new_component->u.part_string = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_MANY:
        case CPARSE_COMPONENT_TYPE_MANY1:
            new_component->u.part_component = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_MUNCH:
        case CPARSE_COMPONENT_TYPE_MUNCH1:
            new_component->u.part_char = 0;
            break;
        case CPARSE_COMPONENT_TYPE_CHOICE:
            new_component->u.part_component_list = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_RANGE:
            new_component->u.part_component_range = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_BETWEEN:
            new_component->u.part_component_between = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_OPTION:
            new_component->u.part_component_option = NULL;
            break;
        case CPARSE_COMPONENT_TYPE_SEPBY:
        case CPARSE_COMPONENT_TYPE_SEPBY1:
        case CPARSE_COMPONENT_TYPE_ENDBY:
        case CPARSE_COMPONENT_TYPE_ENDBY1:
        case CPARSE_COMPONENT_TYPE_SEPENDBY:
        case CPARSE_COMPONENT_TYPE_SEPENDBY1:
        case CPARSE_COMPONENT_TYPE_MANYTILL:
        case CPARSE_COMPONENT_TYPE_EXCLUDE:
            new_component->u.part_component_sepby = NULL;
            break;
    }

    return new_component;
}

/* Destroy Component */
void cparse_component_destroy(cparse_component_t *component)
{
    switch (component->type)
    {
        case CPARSE_COMPONENT_TYPE_REGEX:
            if (component->u.part_regex)
            { cparse_regex_destroy(component->u.part_regex); }
            break;
        case CPARSE_COMPONENT_TYPE_EXPR:
            if (component->u.part_expr)
            { cparse_expr_destroy(component->u.part_expr); }
            break;
        case CPARSE_COMPONENT_TYPE_SPACE:
        case CPARSE_COMPONENT_TYPE_SPACES:
        case CPARSE_COMPONENT_TYPE_ENDOFLINE:
        case CPARSE_COMPONENT_TYPE_ANYCHAR:
        case CPARSE_COMPONENT_TYPE_EOF:
            break;
        case CPARSE_COMPONENT_TYPE_NATIVE:
        case CPARSE_COMPONENT_TYPE_CHAR:
            break;
        case CPARSE_COMPONENT_TYPE_STRING:
        case CPARSE_COMPONENT_TYPE_ONEOF:
        case CPARSE_COMPONENT_TYPE_NONEOF:
        case CPARSE_COMPONENT_TYPE_FAIL:
        case CPARSE_COMPONENT_TYPE_FAILLOC:
            if (component->u.part_string != NULL) 
            { cparse_value_string_destroy(component->u.part_string); }
            break;
        case CPARSE_COMPONENT_TYPE_MANY:
        case CPARSE_COMPONENT_TYPE_MANY1:
            if (component->u.part_component != NULL) 
            { cparse_component_destroy(component->u.part_component); }
            break;
        case CPARSE_COMPONENT_TYPE_MUNCH:
        case CPARSE_COMPONENT_TYPE_MUNCH1:
            break;
        case CPARSE_COMPONENT_TYPE_CHOICE:
            if (component->u.part_component_list != NULL) 
            { cparse_value_list_destroy(component->u.part_component_list); }
            break;
        case CPARSE_COMPONENT_TYPE_RANGE:
            if (component->u.part_component_range != NULL)
            { cparse_component_range_destroy(component->u.part_component_range); }
            break;
        case CPARSE_COMPONENT_TYPE_BETWEEN:
            if (component->u.part_component_between != NULL)
            { cparse_component_between_destroy(component->u.part_component_between); }
            break;
        case CPARSE_COMPONENT_TYPE_OPTION:
            if (component->u.part_component_option != NULL)
            { cparse_component_option_destroy(component->u.part_component_option); }
            break;
        case CPARSE_COMPONENT_TYPE_SEPBY:
        case CPARSE_COMPONENT_TYPE_SEPBY1:
        case CPARSE_COMPONENT_TYPE_ENDBY:
        case CPARSE_COMPONENT_TYPE_ENDBY1:
        case CPARSE_COMPONENT_TYPE_SEPENDBY:
        case CPARSE_COMPONENT_TYPE_SEPENDBY1:
        case CPARSE_COMPONENT_TYPE_MANYTILL:
        case CPARSE_COMPONENT_TYPE_EXCLUDE:
            if (component->u.part_component_sepby != NULL)
            { cparse_component_sepby_destroy(component->u.part_component_sepby); }
            break;
    }
    cparse_free(component);
}

void cparse_component_dtor(void *component)
{
    cparse_component_destroy(component);
}

/* Type */
cparse_component_type_t cparse_component_type(cparse_component_t *component)
{
    return component->type;
}

/* Create Native Component */

static cparse_component_t *cparse_noargs_category( \
        cparse_component_type_t type)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new(type)) == NULL)
    { return NULL; }

    return new_component;
}

cparse_component_t *cparse_regex(char *pattern)
{
    cparse_component_t *new_component = NULL;
    cparse_char_t *new_pattern_unicode = NULL;

    if ((new_pattern_unicode = cparse_unicode_string_new_from_utf8( \
                pattern, cparse_strlen(pattern))) == NULL)
    { return NULL; }

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_REGEX)) == NULL)
    { goto fail; }
    if ((new_component->u.part_regex = cparse_regex_new( \
                    new_pattern_unicode)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    if (new_pattern_unicode != NULL)
    { cparse_free(new_pattern_unicode); }
    return new_component;
}

cparse_component_t *cparse_expr(cparse_size_t precedence_count)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_EXPR)) == NULL)
    { goto fail; }
    if ((new_component->u.part_expr = cparse_expr_new(precedence_count)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_space(void)
{
    return cparse_noargs_category(CPARSE_COMPONENT_TYPE_SPACE);
}

cparse_component_t *cparse_spaces(void)
{
    return cparse_noargs_category(CPARSE_COMPONENT_TYPE_SPACES);
}

cparse_component_t *cparse_endofline(void)
{
    return cparse_noargs_category(CPARSE_COMPONENT_TYPE_ENDOFLINE);
}

cparse_component_t *cparse_anychar(void)
{
    return cparse_noargs_category(CPARSE_COMPONENT_TYPE_ANYCHAR);
}

cparse_component_t *cparse_eof(void)
{
    return cparse_noargs_category(CPARSE_COMPONENT_TYPE_EOF);
}

cparse_component_t *cparse_native(cparse_native_component_t callback)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_NATIVE)) == NULL)
    { return NULL; }
    new_component->u.part_native = callback;

    return new_component;
}

/* Create Composite Component */
cparse_component_t *cparse_char(cparse_value_char_t ch)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_CHAR)) == NULL)
    { return NULL; }
    new_component->u.part_char = ch;

    return new_component;
}


static cparse_component_t *cparse_component_new_string_category( \
        cparse_component_type_t type, char *s)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    type)) == NULL)
    { return NULL; }
    if ((new_component->u.part_string = cparse_value_string_new( \
                    s, cparse_strlen(s))) == NULL)
    {
        cparse_component_destroy(new_component);
        return NULL;
    }

    return new_component;
}

cparse_component_t *cparse_string(char *s)
{
    return cparse_component_new_string_category( \
            CPARSE_COMPONENT_TYPE_STRING, s);
}

cparse_component_t *cparse_oneof(char *s)
{
    return cparse_component_new_string_category( \
            CPARSE_COMPONENT_TYPE_ONEOF, s);
}

cparse_component_t *cparse_noneof(char *s)
{
    return cparse_component_new_string_category( \
            CPARSE_COMPONENT_TYPE_NONEOF, s);
}

cparse_component_t *cparse_many(cparse_component_t *component)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_MANY)) == NULL)
    { return NULL; }
    new_component->u.part_component = component;

    return new_component;
}

cparse_component_t *cparse_many1(cparse_component_t *component)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_MANY1)) == NULL)
    { return NULL; }
    new_component->u.part_component = component;

    return new_component;
}

cparse_component_t *cparse_munch(cparse_native_predict_t predict)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_MUNCH)) == NULL)
    { return NULL; }
    new_component->u.part_predict = predict;

    return new_component;
}

cparse_component_t *cparse_munch1(cparse_native_predict_t predict)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_MUNCH1)) == NULL)
    { return NULL; }
    new_component->u.part_predict = predict;

    return new_component;
}

cparse_component_t *cparse_choice(cparse_component_t *first_component, ...)
{
    va_list ap;
    cparse_value_list_t *new_component_list = NULL;
    cparse_component_t *component_cur;
    cparse_component_t *new_component = NULL;

    if ((new_component_list = cparse_value_list_new( \
                    NULL, \
                    cparse_component_dtor)) == NULL)
    { return NULL; }

    /* First Component */
    cparse_value_list_push_back(new_component_list, first_component);

    va_start(ap, first_component);
    for (;;)
    {
        component_cur = va_arg(ap, cparse_component_t *);
        if (component_cur == NULL) { break; }

        if (cparse_value_list_push_back(new_component_list, component_cur) != 0)
        {
            cparse_value_list_destroy(new_component_list);
            return NULL;
        }
    }

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_CHOICE)) == NULL)
    { return NULL; }
    new_component->u.part_component_list = new_component_list;

    return new_component;
}

cparse_component_t *cparse_range_greed( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_RANGE)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_range = cparse_component_range_new( \
            count_min, count_max, sub_component, 1)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_range_nongreed( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_RANGE)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_range = cparse_component_range_new( \
            count_min, count_max, sub_component, 0)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_range( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component)
{
    return cparse_range_greed(count_min, count_max, sub_component);
}

cparse_component_t *cparse_count( \
        cparse_size_t count, \
        cparse_component_t *sub_component)
{
    return cparse_range(count, count, sub_component);
}

cparse_component_t *cparse_between( \
        cparse_component_t *component_open, \
        cparse_component_t *component_close, \
        cparse_component_t *component_sub)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_BETWEEN)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_between = cparse_component_between_new( \
            component_open, \
            component_close, \
            component_sub)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_option( \
        cparse_value_t *value_x, \
        cparse_component_t *component_p)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_OPTION)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_option = cparse_component_option_new( \
                    value_x, component_p)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

static cparse_component_t *cparse_sepby_category( \
        cparse_component_type_t type, \
        cparse_component_t *component_first, \
        cparse_component_t *component_second)
{
    cparse_component_t *new_component = NULL;

    if ((new_component = cparse_component_new( \
                    type)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_sepby = cparse_component_sepby_new( \
                    component_first, component_second)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_sepby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_SEPBY, component_p, component_sep);
}

cparse_component_t *cparse_sepby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_SEPBY1, component_p, component_sep);
}

cparse_component_t *cparse_endby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_ENDBY, component_p, component_sep);
}

cparse_component_t *cparse_endby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_ENDBY1, component_p, component_sep);
}

cparse_component_t *cparse_sependby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_SEPENDBY, component_p, component_sep);
}

cparse_component_t *cparse_sependby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_SEPENDBY1, component_p, component_sep);
}

cparse_component_t *cparse_manytill( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep)
{
    return cparse_sepby_category(CPARSE_COMPONENT_TYPE_MANYTILL, component_p, component_sep);
}

cparse_component_t *cparse_exclude( \
        cparse_component_t *component_p, \
        cparse_component_t *component_exclude)
{
    cparse_component_t *new_component = NULL;

    /* Match 'p' but not match 'exclude' */
    if ((new_component = cparse_component_new( \
                    CPARSE_COMPONENT_TYPE_EXCLUDE)) == NULL)
    { return NULL; }
    if ((new_component->u.part_component_exclude = cparse_component_sepby_new( \
                    component_p, component_exclude)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_component != NULL)
    {
        cparse_component_destroy(new_component);
        new_component = NULL;
    }
done:
    return new_component;
}

cparse_component_t *cparse_fail(char *message)
{
    return cparse_component_new_string_category( \
            CPARSE_COMPONENT_TYPE_FAIL, message);
}

cparse_component_t *cparse_failloc(char *message)
{
    return cparse_component_new_string_category( \
            CPARSE_COMPONENT_TYPE_FAILLOC, message);
}

cparse_component_t *cparse_undefined(void)
{
    return cparse_fail("undefined");
}

cparse_component_t *cparse_undefinedloc(void)
{
    return cparse_failloc("undefined");
}


/* Extract */

cparse_native_component_t cparse_component_as_native_extract( \
        cparse_component_t *component)
{
    return component->u.part_native;
}

cparse_native_predict_t cparse_component_as_predict_extract( \
        cparse_component_t *component)
{
    return component->u.part_predict;
}

cparse_value_list_t *cparse_component_as_component_list_extract( \
        cparse_component_t *component)
{
    return component->u.part_component_list;
}

cparse_component_t *cparse_component_as_component_extract( \
        cparse_component_t *component)
{
    return component->u.part_component;
}

cparse_value_char_t cparse_component_as_char_extract( \
        cparse_component_t *component)
{
    return component->u.part_char;
}

cparse_value_string_t *cparse_component_as_string_extract( \
        cparse_component_t *component)
{
    return component->u.part_string;
}

cparse_size_t cparse_component_as_range_extract_min( \
        cparse_component_t *component)
{
    return component->u.part_component_range->count_min;
}

cparse_size_t cparse_component_as_range_extract_max( \
        cparse_component_t *component)
{
    return component->u.part_component_range->count_max;
}

cparse_component_t *cparse_component_as_range_extract_sub_component( \
        cparse_component_t *component)
{
    return component->u.part_component_range->sub_component;
}

int cparse_component_as_range_extract_greed( \
        cparse_component_t *component)
{
    return component->u.part_component_range->greed;
}

/* between */
cparse_component_t *cparse_component_as_between_component_open( \
        cparse_component_t *component)
{
    return component->u.part_component_between->component_open;
}

cparse_component_t *cparse_component_as_between_component_close( \
        cparse_component_t *component)
{
    return component->u.part_component_between->component_close;
}

cparse_component_t *cparse_component_as_between_component_sub( \
        cparse_component_t *component)
{
    return component->u.part_component_between->component_sub;
}

/* option */
cparse_component_t *cparse_component_as_option_predict( \
        cparse_component_t *component)
{
    return component->u.part_component_option->component_p;
}

cparse_value_t *cparse_component_as_option_prepared( \
        cparse_component_t *component)
{
    return component->u.part_component_option->value_x;
}

/* sepby */
cparse_component_t *cparse_component_as_sepby_predict( \
        cparse_component_t *component)
{
    return component->u.part_component_sepby->component_p;
}

cparse_component_t *cparse_component_as_sepby_sep( \
        cparse_component_t *component)
{
    return component->u.part_component_sepby->component_sep;
}

/* endby */
cparse_component_t *cparse_component_as_endby_predict( \
        cparse_component_t *component)
{
    return component->u.part_component_sepby->component_p;
}

cparse_component_t *cparse_component_as_endby_sep( \
        cparse_component_t *component)
{
    return component->u.part_component_sepby->component_sep;
}

/* regex */
cparse_regex_t *cparse_component_as_regex_get( \
        cparse_component_t *component)
{
    return component->u.part_regex;
}

/* expr */
cparse_expr_t *cparse_component_as_expr_get( \
        cparse_component_t *component)
{
    return component->u.part_expr;
}

static int cparse_component_as_expr_append_unary_op_wide( \
        cparse_component_t *component, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    cparse_expr_t *expr = cparse_component_as_expr_get(component);

    return cparse_expr_append_unary_op_wide(expr, type, associative, precedence, str, assembler_unary);
}

static int cparse_component_as_expr_append_infix_op_wide( \
        cparse_component_t *component, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_2_to_1_t assembler_binary)
{
    cparse_expr_t *expr = cparse_component_as_expr_get(component);

    return cparse_expr_append_binary_op_wide(expr, type, associative, precedence, str, assembler_binary);
}

static int cparse_component_as_expr_append_unary_op( \
        cparse_component_t *component, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    int ret = 0;
    cparse_char_t *s_wide = NULL;

    if ((s_wide = cparse_unicode_string_new_from_utf8( \
                    str, cparse_strlen(str))) == NULL)
    { return -1; }

    ret = cparse_component_as_expr_append_unary_op_wide( \
            component, \
            type, \
            associative, \
            precedence, \
            s_wide, \
            assembler_unary);

    cparse_free(s_wide);

    return ret;
}

int cparse_component_as_expr_append_infix_op( \
        cparse_component_t *component, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_2_to_1_t assembler_binary)
{
    int ret = 0;
    cparse_char_t *s_wide = NULL;

    if ((s_wide = cparse_unicode_string_new_from_utf8( \
                    str, cparse_strlen(str))) == NULL)
    { return -1; }

    ret = cparse_component_as_expr_append_infix_op_wide( \
            component, \
            CPARSE_EXPR_OPERATOR_TYPE_INFIX, \
            associative, \
            precedence, \
            s_wide, \
            assembler_binary);

    cparse_free(s_wide);

    return ret;
}

int cparse_component_as_expr_append_prefix_op( \
        cparse_component_t *component, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    return cparse_component_as_expr_append_unary_op( \
            component, \
            CPARSE_EXPR_OPERATOR_TYPE_PREFIX, \
            CPARSE_EXPR_ASSOCIATIVE_UNDEFINED, \
            precedence, \
            str, \
            assembler_unary);
}

int cparse_component_as_expr_append_postfix_op( \
        cparse_component_t *component, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    return cparse_component_as_expr_append_unary_op( \
            component, \
            CPARSE_EXPR_OPERATOR_TYPE_POSTFIX, \
            CPARSE_EXPR_ASSOCIATIVE_UNDEFINED, \
            precedence, \
            str, \
            assembler_unary);
}


int cparse_component_as_expr_set_follow( \
        cparse_component_t *component, \
        struct cparse_component *component_follow)
{
    cparse_expr_t *expr = cparse_component_as_expr_get(component);

    return cparse_expr_set_follow(expr, component_follow);
}

struct cparse_component *cparse_component_as_expr_follow( \
        cparse_component_t *component)
{
    cparse_expr_t *expr = cparse_component_as_expr_get(component);

    return cparse_expr_follow(expr);
}

