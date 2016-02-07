/* CParse : Value
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_libc.h"
#include "cparse_res.h"

#include "cparse_value.h"
#include "cparse_value_char.h"
#include "cparse_value_bool.h"
#include "cparse_value_list.h"
#include "cparse_value_string.h"
#include "cparse_value_struct.h"
#include "cparse_value_tuple.h"
#include "cparse_value_tag.h"

struct cparse_value
{
    cparse_value_type_t type;
    union
    {
        cparse_value_string_t *part_string;
        cparse_value_list_t *part_list;
        cparse_value_char_t part_char;
        cparse_value_bool_t part_bool;
        cparse_value_tuple_t *part_tuple;
        cparse_value_struct_t *part_struct;
        cparse_value_tag_t *part_tag;
    } u;
};

/* Create Value */

static cparse_value_t *cparse_value_new(cparse_value_type_t type)
{
    cparse_value_t *new_value = NULL;

    if ((new_value = (cparse_value_t *)cparse_malloc( \
                    sizeof(cparse_value_t))) == NULL)
    { return NULL; }
    new_value->type = type;
    switch (type)
    {
        case CPARSE_VALUE_TYPE_STRING:
            new_value->u.part_string = NULL;
            break;
        case CPARSE_VALUE_TYPE_CHAR:
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            break;
        case CPARSE_VALUE_TYPE_LIST:
            new_value->u.part_list = NULL;
            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            new_value->u.part_tuple = NULL;
            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            new_value->u.part_struct = NULL;
            break;
        case CPARSE_VALUE_TYPE_TAG:
            new_value->u.part_tag = NULL;
            break;
    }

    return new_value;
}

cparse_value_t *cparse_value_new_char(cparse_value_char_t ch)
{
    cparse_value_t *new_value = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_CHAR)) == NULL)
    { return NULL; }

    new_value->u.part_char = ch;

    return new_value;
}

cparse_value_t *cparse_value_new_string_wide(cparse_char_t *s, cparse_size_t len)
{
    cparse_value_t *new_value = NULL;
    cparse_value_string_t *new_string = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_STRING)) == NULL)
    { return NULL; }

    if ((new_string = cparse_value_string_new_wide(s, len)) == NULL)
    { goto fail; }
    new_value->u.part_string = new_string; new_string = NULL;

    goto done;
fail:
    if (new_string != NULL) cparse_value_string_destroy(new_string);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_string(char *s, cparse_size_t len)
{
    cparse_value_t *new_value = NULL;
    cparse_value_string_t *new_string = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_STRING)) == NULL)
    { return NULL; }

    if ((new_string = cparse_value_string_new(s, len)) == NULL)
    { goto fail; }
    new_value->u.part_string = new_string; new_string = NULL;

    goto done;
fail:
    if (new_string != NULL) cparse_value_string_destroy(new_string);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_list( \
        void *(*cctor)(void *p), \
        void (*dtor)(void *p))
{
    cparse_value_t *new_value = NULL;
    cparse_value_list_t *new_list = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_LIST)) == NULL)
    { return NULL; }

    if ((new_list = cparse_value_list_new(cctor, dtor)) == NULL)
    { goto fail; }
    new_value->u.part_list = new_list; new_list = NULL;

    goto done;
fail:
    if (new_list != NULL) cparse_value_list_destroy(new_list);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_tuple(cparse_value_t *first, ...)
{
    va_list ap;
    cparse_value_t *new_value = NULL;
    cparse_value_tuple_t *new_tuple = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_TUPLE)) == NULL)
    { return NULL; }

    va_start(ap, first);
    if ((new_tuple = cparse_value_tuple_new_from_va_list(first, ap)) == NULL)
    { goto fail; }
    va_end(ap);
    new_value->u.part_tuple = new_tuple; new_tuple = NULL;

    goto done;
fail:
    if (new_tuple != NULL) cparse_value_tuple_destroy(new_tuple);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_struct_wide(cparse_char_t *name, ...)
{
    va_list ap;
    cparse_value_t *new_value = NULL;
    cparse_value_struct_t *new_struct = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_STRUCT)) == NULL)
    { return NULL; }

    va_start(ap, name);
    if ((new_struct = cparse_value_struct_new_from_va_list(cparse_false, name, ap)) == NULL)
    { goto fail; }
    va_end(ap);
    new_value->u.part_struct = new_struct; new_struct = NULL;

    goto done;
fail:
    if (new_struct != NULL) cparse_value_struct_destroy(new_struct);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_struct(char *name, ...)
{
    va_list ap;
    cparse_value_t *new_value = NULL;
    cparse_value_struct_t *new_struct = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_STRUCT)) == NULL)
    { return NULL; }

    va_start(ap, name);
    if ((new_struct = cparse_value_struct_new_from_va_list(cparse_true, name, ap)) == NULL)
    { goto fail; }
    va_end(ap);
    new_value->u.part_struct = new_struct; new_struct = NULL;

    goto done;
fail:
    if (new_struct != NULL) cparse_value_struct_destroy(new_struct);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

cparse_value_t *cparse_value_new_tag(int type_tag, cparse_value_t *value)
{
    cparse_value_t *new_value = NULL;
    cparse_value_tag_t *new_tag = NULL;

    if ((new_value = cparse_value_new( \
                    CPARSE_VALUE_TYPE_TAG)) == NULL)
    { return NULL; }

    if ((new_tag = cparse_value_tag_new(type_tag, value)) == NULL)
    { goto fail; }
    new_value->u.part_tag = new_tag; new_tag = NULL;

    goto done;
fail:
    if (new_tag != NULL) cparse_value_tag_destroy(new_tag);
    if (new_value != NULL) 
    {
        cparse_value_destroy(new_value);
        new_value = NULL;
    }
done:
    return new_value;
}

/* Destroy Value */
void cparse_value_destroy(cparse_value_t *value)
{
    switch (value->type)
    {
        case CPARSE_VALUE_TYPE_STRING:
            if (value->u.part_string != NULL) 
            cparse_value_string_destroy(value->u.part_string);
            break;
        case CPARSE_VALUE_TYPE_CHAR:
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            break;
        case CPARSE_VALUE_TYPE_LIST:
            if (value->u.part_list != NULL) 
            { cparse_value_list_destroy(value->u.part_list); }
            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            if (value->u.part_tuple != NULL)
            { cparse_value_tuple_destroy(value->u.part_tuple); }
            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            if (value->u.part_struct != NULL)
            { cparse_value_struct_destroy(value->u.part_struct); }
            break;
        case CPARSE_VALUE_TYPE_TAG:
            if (value->u.part_tag)
            { cparse_value_tag_destroy(value->u.part_tag); }
            break;
    }
    cparse_free(value);
}

void cparse_value_dtor(void *p)
{
    cparse_value_destroy(p);
}

/* Type */
cparse_value_type_t cparse_value_type(cparse_value_t *value)
{
    return value->type;
}

/* Clone */
cparse_value_t *cparse_value_clone(cparse_value_t *value)
{
    cparse_value_t *ret = NULL;

    if ((ret = cparse_value_new(value->type)) == NULL)
    { return NULL; }

    switch (value->type)
    {
        case CPARSE_VALUE_TYPE_STRING:
            if ((ret->u.part_string = cparse_value_string_clone(value->u.part_string)) == NULL)
            { goto fail; }
            break;
        case CPARSE_VALUE_TYPE_CHAR:
            ret->u.part_char = value->u.part_char;
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            ret->u.part_bool = value->u.part_bool;
            break;
        case CPARSE_VALUE_TYPE_LIST:
            if ((ret->u.part_list = cparse_value_list_clone(value->u.part_list)) == NULL)
            { 
                goto fail; 
            }
            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            if ((ret->u.part_tuple = cparse_value_tuple_clone(value->u.part_tuple)) == NULL)
            { goto fail; }
            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            if ((ret->u.part_struct = cparse_value_struct_clone(value->u.part_struct)) == NULL)
            { goto fail; }
            break;
        case CPARSE_VALUE_TYPE_TAG:
            if ((ret->u.part_tag = cparse_value_tag_clone(value->u.part_tag)) == NULL)
            { goto fail; }
            break;
    }

    goto done;
fail:
    if (ret != NULL)
    {
        cparse_value_destroy(ret);
        ret = NULL;
    }
done:
    return ret;
}

void *cparse_value_cctor(void *p)
{
    return (void *)cparse_value_clone(p);
}

/* Equality */
cparse_bool cparse_value_eqp(cparse_value_t *value1, cparse_value_t *value2)
{
    cparse_bool ret = cparse_false;

    if (value1->type != value2->type)
    { return cparse_false; }

    switch (value1->type)
    {
        case CPARSE_VALUE_TYPE_CHAR:
            ret = cparse_value_char_eqp(value1->u.part_char, value2->u.part_char);
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            ret = cparse_value_bool_eqp(value1->u.part_bool, value2->u.part_bool);
            break;
        case CPARSE_VALUE_TYPE_STRING:
            ret = cparse_value_string_eqp(value1->u.part_string, value2->u.part_string);
            break;
        case CPARSE_VALUE_TYPE_LIST:
            ret = cparse_false;
            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            ret = cparse_false;
            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            ret = cparse_false;
            break;
        case CPARSE_VALUE_TYPE_TAG:
            ret = cparse_false;
            break;
    }

    return ret;
}


/* Operatings */

cparse_value_char_t cparse_value_as_char_get(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_CHAR)
    { return 0; }

    return value->u.part_char;
}

cparse_value_t *cparse_value_as_char_to_string(cparse_value_t *value)
{
    cparse_value_char_t s[2];
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_CHAR)
    { return NULL; }
    s[0] = cparse_value_as_char_get(value);
    s[1] = '\0';

    return cparse_value_new_string_wide(s, 1);
}

cparse_bool cparse_value_as_bool_get(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_BOOL)
    { return cparse_false; }

    return value->u.part_bool;
}

cparse_char_t *cparse_value_as_string_str_wide(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_STRING)
    { return NULL; }

    return cparse_value_string_str(value->u.part_string);
}

cparse_size_t cparse_value_as_string_length(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_STRING)
    { return 0; }

    return cparse_value_string_length(value->u.part_string);
}

cparse_bool cparse_value_as_string_is(cparse_value_t *value, char *s)
{
    cparse_bool ret;
    cparse_char_t *str_wide = NULL;
    cparse_size_t str_wide_len;
    cparse_char_t *new_str_wide_rhs = NULL;

    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_STRING)
    { return cparse_false; }

    str_wide = cparse_value_as_string_str_wide(value);
    str_wide_len = cparse_value_as_string_length(value);

    if ((new_str_wide_rhs = cparse_unicode_string_new_from_utf8( \
                    s, cparse_strlen(s))) == NULL)
    { return cparse_false; }

    ret = ((cparse_unicode_string_strlen(new_str_wide_rhs) == str_wide_len) && \
            (cparse_unicode_string_strncmp(str_wide, new_str_wide_rhs, str_wide_len) == 0)) ? cparse_true : cparse_false;

    cparse_free(new_str_wide_rhs);

    return ret;
}

int cparse_value_as_string_to_integer(cparse_value_t *value)
{
    cparse_char_t *str_wide = NULL;
    cparse_size_t str_wide_len;
    cparse_bool negative = cparse_false;
    int ret = 0;

    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_STRING)
    { return cparse_false; }

    str_wide = cparse_value_as_string_str_wide(value);
    str_wide_len = cparse_value_as_string_length(value);
    if (str_wide_len == 0) return 0;
    if (*str_wide == '-')
    {
        negative = cparse_true;
        str_wide++;
        str_wide_len--;
    }

    while (str_wide_len-- != 0)
    {
        ret = ret * 10 + (int)(*str_wide - (int)'0');
        str_wide++;
    }

    if (negative == cparse_true) ret = -ret;

    return ret;
}

cparse_size_t cparse_value_as_list_size(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_LIST)
    { return 0; }

    return cparse_value_list_size(value->u.part_list);
}

int cparse_value_as_list_push_back(cparse_value_t *value, cparse_value_t *new_element)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_LIST)
    { return 0; }

    return cparse_value_list_push_back(value->u.part_list, new_element);
}

cparse_value_list_node_t *cparse_value_as_list_first(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_LIST)
    { return NULL; }

    return cparse_value_list_first(value->u.part_list);
}

cparse_value_list_node_t *cparse_value_as_list_node_next(cparse_value_list_node_t *node)
{
    return cparse_value_list_node_next(node);
}

void *cparse_value_as_list_node_value(cparse_value_list_node_t *node)
{
    return cparse_value_list_node_value(node);
}

cparse_size_t cparse_value_as_tuple_size(cparse_value_t *value)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_TUPLE)
    { return 0; }

    return cparse_value_tuple_length(value->u.part_tuple);
}

void *cparse_value_as_tuple_ref(cparse_value_t *value, cparse_size_t index)
{
    if (cparse_value_type(value) != CPARSE_VALUE_TYPE_TUPLE)
    { return NULL; }

    return cparse_value_tuple_ref(value->u.part_tuple, index);
}

cparse_bool cparse_value_as_struct_is_wide(cparse_value_t *value, cparse_char_t *name)
{
    return cparse_value_struct_is_wide(value->u.part_struct, name);
}

cparse_bool cparse_value_as_struct_is(cparse_value_t *value, char *name)
{
    cparse_char_t *name_unicode = cparse_unicode_string_new_from_utf8(name, cparse_strlen(name));
    cparse_bool ret = cparse_value_as_struct_is_wide(value, name_unicode);
    cparse_free(name_unicode);
    return ret;
}

cparse_char_t *cparse_value_as_struct_name_get_wide(cparse_value_t *value)
{
    return cparse_value_struct_name_get_wide(value->u.part_struct);
}

char *cparse_value_as_struct_name_get(cparse_value_t *value)
{
    cparse_char_t *unicode_string = cparse_value_as_struct_name_get_wide(value);
    return cparse_unicode_string_to_utf8( \
            unicode_string, \
            cparse_unicode_string_strlen(unicode_string));
}

void *cparse_value_as_struct_member_get_wide(cparse_value_t *value, cparse_char_t *name)
{
    return cparse_value_struct_member_get_wide(value->u.part_struct, name);
}

void *cparse_value_as_struct_member_get(cparse_value_t *value, char *name)
{
    cparse_char_t *name_unicode = cparse_unicode_string_new_from_utf8(name, cparse_strlen(name));
    void *ret = cparse_value_struct_member_get_wide(value->u.part_struct, name_unicode);
    cparse_free(name_unicode);
    return ret;
}

cparse_value_map_node_t *cparse_value_as_struct_member_first(cparse_value_t *value)
{
    return cparse_value_struct_member_first(value->u.part_struct);
}

cparse_value_map_node_t *cparse_value_as_struct_member_node_next(cparse_value_map_node_t *node)
{
    return cparse_value_struct_member_node_next(node);
}

cparse_char_t *cparse_value_as_struct_member_node_key_wide(cparse_value_map_node_t *node)
{
    return cparse_value_struct_member_node_key_wide(node);
}

void *cparse_value_as_struct_member_node_value(cparse_value_map_node_t *node)
{
    return cparse_value_struct_member_node_value(node);
}

cparse_bool cparse_value_as_tag_is(cparse_value_t *value, int type_tag)
{
    return cparse_value_tag_is(value->u.part_tag, type_tag);
}

int cparse_value_as_tag_id(cparse_value_t *value)
{
    return cparse_value_tag_id(value->u.part_tag);
}

struct cparse_value *cparse_value_as_tag_value(cparse_value_t *value)
{
    return cparse_value_tag_value(value->u.part_tag);
}


/* Combinations */

cparse_value_t *cparse_value_string_concat(cparse_value_t *v1, cparse_value_t *v2)
{
    cparse_size_t len1, len2;
    cparse_char_t *s1, *s2;
    cparse_char_t *s_final = NULL;
    cparse_value_t *ret = NULL;

    len1 = cparse_value_as_string_length(v1);
    len2 = cparse_value_as_string_length(v2);
    s1 = cparse_value_as_string_str_wide(v1);
    s2 = cparse_value_as_string_str_wide(v2);
    if ((s_final = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (len1 + len2 + 1))) == NULL)
    { return NULL; }
    cparse_unicode_string_memcpy(s_final, s1, len1);
    cparse_unicode_string_memcpy(s_final + len1, s2, len2);
    s_final[len1 + len2] = '\0';
    ret = cparse_value_new_string_wide(s_final, len1 + len2);
    cparse_free(s_final);

    return ret;
}

