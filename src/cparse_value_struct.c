/* CParse : Struct
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_struct.h"

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_charenc.h"
#include "cparse_value_map.h"
#include "cparse_value_string.h"

struct cparse_value_struct
{
    cparse_char_t *name;
    cparse_value_map_t *members;
};

struct cparse_value;
void cparse_value_dtor(void *p);
void *cparse_value_cctor(void *p);
struct cparse_value *cparse_value_clone(struct cparse_value *value);
void cparse_value_destroy(struct cparse_value *value);

static struct cparse_value_struct *cparse_value_struct_new_blank(cparse_char_t *name)
{
    struct cparse_value_struct *new_struct = NULL;
    cparse_size_t name_len = cparse_unicode_string_strlen(name);

    if ((new_struct = (struct cparse_value_struct *)cparse_malloc( \
                    sizeof(struct cparse_value_struct))) == NULL)
    { return NULL; }
    new_struct->name = NULL;
    new_struct->members = NULL;

    if ((new_struct->name = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (name_len + 1))) == NULL)
    { goto fail; }
    cparse_unicode_string_memcpy(new_struct->name, name, name_len);
    new_struct->name[name_len] = '\0';

    goto done;
fail:
    if (new_struct != NULL)
    {
        cparse_value_struct_destroy(new_struct);
        new_struct = NULL;
    }
done:
    return new_struct;
}

struct cparse_value_struct *cparse_value_struct_new_wide(cparse_char_t *name)
{
    struct cparse_value_struct *new_struct = NULL;

    if ((new_struct = cparse_value_struct_new_blank(name)) == NULL)
    { goto fail; }
    if ((new_struct->members = cparse_value_map_new( \
                    cparse_value_string_eqp, \
                    cparse_value_string_cctor, \
                    cparse_value_string_dtor, \
                    cparse_value_cctor, \
                    cparse_value_dtor)) == NULL)
    { goto fail; }

    goto done;
fail:
done:
    return new_struct;
}

struct cparse_value_struct *cparse_value_struct_new_from_va_list( \
        cparse_bool is_utf8, \
        void *name_raw, va_list ap)
{
    cparse_char_t *new_name_unicode = NULL;
    struct cparse_value_struct *new_struct = NULL;
    void *member_name_raw;
    struct cparse_value *member_value;

    cparse_value_string_t *new_member_name = NULL;
    struct cparse_value *new_member_value = NULL;

    if (is_utf8 == cparse_false)
    {
        /* Unicode */
        if ((new_struct = cparse_value_struct_new_wide(name_raw)) == NULL)
        { return NULL; }
    }
    else
    {
        /* UTF-8 */
        if ((new_name_unicode = cparse_unicode_string_new_from_utf8( \
                        name_raw, cparse_strlen(name_raw))) == NULL)
        { return NULL; }
        if ((new_struct = cparse_value_struct_new_wide(new_name_unicode)) == NULL)
        { goto fail; }
        cparse_free(new_name_unicode);
        new_name_unicode = NULL;
    }


    while ((member_name_raw = va_arg(ap, void *)) != NULL)
    {
        member_value = va_arg(ap, struct cparse_value *);

        if (is_utf8 == cparse_false)
        {
            /* Unicode */
            if ((new_member_name = cparse_value_string_new_wide( \
                            member_name_raw, cparse_unicode_string_strlen(member_name_raw))) == NULL)
            { goto fail; }
        }
        else
        {
            /* UTF-8 */
            if ((new_name_unicode = cparse_unicode_string_new_from_utf8( \
                            member_name_raw, \
                            cparse_strlen(member_name_raw))) == NULL)
            { return NULL; }
            if ((new_member_name = cparse_value_string_new_wide( \
                            new_name_unicode, cparse_unicode_string_strlen(new_name_unicode))) == NULL)
            { goto fail; }
            cparse_free(new_name_unicode);
            new_name_unicode = NULL;
        }
        if ((new_member_value = cparse_value_clone(member_value)) == NULL)
        { goto fail; }

        cparse_value_map_set(new_struct->members, new_member_name, new_member_value);
        new_member_name = NULL;
        new_member_value = NULL;
    }

    goto done;
fail:
    if (new_member_name != NULL)
    { cparse_value_string_destroy(new_member_name); }
    if (new_member_value != NULL)
    { cparse_value_destroy(new_member_value); }
    if (new_struct != NULL)
    { cparse_value_struct_destroy(new_struct); new_struct = NULL; }
done:
    if (new_name_unicode != NULL)
    { cparse_free(new_name_unicode); }
    return new_struct;
}


void cparse_value_struct_destroy(cparse_value_struct_t *cparse_struct)
{
    if (cparse_struct->name != NULL) cparse_free(cparse_struct->name);
    if (cparse_struct->members != NULL) cparse_value_map_destroy(cparse_struct->members);
    cparse_free(cparse_struct);
}

cparse_value_struct_t *cparse_value_struct_clone(cparse_value_struct_t *cparse_struct)
{
    cparse_value_struct_t *new_struct = NULL;

    if ((new_struct = cparse_value_struct_new_blank(cparse_struct->name)) == NULL)
    { return NULL; }
    if ((new_struct->members = cparse_value_map_clone(cparse_struct->members)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_struct != NULL)
    {
        cparse_value_struct_destroy(new_struct);
        new_struct = NULL;
    }
done:
    return new_struct;
}

cparse_bool cparse_value_struct_is_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name)
{
    cparse_size_t name_len = cparse_unicode_string_strlen(name);
    if (cparse_unicode_string_strlen(cparse_struct->name) != name_len)
    { return cparse_false; }
    if (cparse_unicode_string_strncmp(cparse_struct->name, name, name_len) != 0)
    { return cparse_false; }

    return cparse_true;
}

int cparse_value_struct_name_set_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name)
{
    cparse_size_t name_len = cparse_unicode_string_strlen(name);
    cparse_char_t *new_name = NULL;

    if ((new_name = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (name_len + 1))) == NULL)
    { return -1; }
    cparse_unicode_string_memcpy(new_name, name, name_len);
    new_name[name_len] = '\0';

    if (cparse_struct->name != NULL)
    { cparse_free(cparse_struct->name); }
    cparse_struct->name = new_name;

    return 0;
}

cparse_char_t *cparse_value_struct_name_get_wide(cparse_value_struct_t *cparse_struct)
{
    return cparse_struct->name;
}

int cparse_value_struct_member_set_wide(cparse_value_struct_t *cparse_struct, \
        cparse_char_t *name, \
        void *ptr)
{
    int ret = 0;
    cparse_value_string_t *new_string = cparse_value_string_new_wide( \
            name, cparse_unicode_string_strlen(name));
    if (new_string == NULL) { ret = -1; goto fail; }
    if (cparse_value_map_clone_and_set(cparse_struct->members, new_string, ptr) != 0) 
    { ret = -1; goto fail; }
fail:
    if (new_string != NULL) cparse_value_string_destroy(new_string);
    return ret;
}

void *cparse_value_struct_member_get_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name)
{
    void *p = NULL;
    cparse_value_string_t *new_string = NULL;

    new_string = cparse_value_string_new_wide(name, cparse_unicode_string_strlen(name));
    if (new_string == NULL) goto fail;
    p = cparse_value_map_get(cparse_struct->members, new_string);
fail:
    if (new_string != NULL) cparse_value_string_destroy(new_string);
    return p;
}

cparse_value_map_node_t *cparse_value_struct_member_first(cparse_value_struct_t *cparse_struct)
{
    return cparse_value_map_first(cparse_struct->members);
}

cparse_value_map_node_t *cparse_value_struct_member_node_next(cparse_value_map_node_t *node)
{
    return cparse_value_map_node_next(node);
}

cparse_char_t *cparse_value_struct_member_node_key_wide(cparse_value_map_node_t *node)
{
    cparse_value_string_t *str = cparse_value_map_node_key(node);
    return cparse_value_string_str(str);
}

void *cparse_value_struct_member_node_value(cparse_value_map_node_t *node)
{
    return cparse_value_map_node_value(node);
}

