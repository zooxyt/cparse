/* CParse : AST Serialize
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_err.h"
#include "cparse_value.h"
#include "cparse_json.h"
#include "cparse_serialize.h"

static int cparse_json_ast_ctor_in( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value);

static int cparse_json_ast_ctor_in_string( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    cparse_char_t *s = NULL;
    cparse_json_val_t *new_json_val = NULL;

    if ((s = cparse_value_as_string_str_wide(value)) == NULL)
    { cparse_err_update_out_of_memory(err); return -1; }
    if ((new_json_val = cparse_json_val_new_string(s)) == NULL)
    { cparse_err_update_out_of_memory(err); return -1; }

    *json_val_out = new_json_val;
    return 0;
}

static int cparse_json_ast_ctor_in_char( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    cparse_value_char_t ch;
    cparse_char_t s[2];
    cparse_json_val_t *new_json_val = NULL;

    ch = cparse_value_as_char_get(value);
    s[0] = (cparse_char_t)ch;
    s[1] = (cparse_char_t)0;
    if ((new_json_val = cparse_json_val_new_string(s)) == NULL)
    { cparse_err_update_out_of_memory(err); return -1; }

    *json_val_out = new_json_val;
    return 0;
}

static int cparse_json_ast_ctor_in_list( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    int ret = 0;
    cparse_json_val_t *new_json_val_list = NULL;
    cparse_json_val_t *new_json_val_element = NULL;
    cparse_value_list_node_t *node_cur = NULL;
    cparse_value_t *value_cur;

    /* Create a new blank array */
    if ((new_json_val_list = cparse_json_val_new_array()) == NULL)
    { cparse_err_update_out_of_memory(err); return -1; }

    /* Walk though nodes in list and append nodes into the created new array */
    node_cur = cparse_value_as_list_first(value);
    while (node_cur != NULL)
    {
        value_cur = cparse_value_as_list_node_value(node_cur);
        if (cparse_json_ast_ctor_in(err, &new_json_val_element, value_cur) != 0)
        { ret = -1; goto fail; }
        if (cparse_json_val_as_array_push(new_json_val_list, \
                new_json_val_element) != 0)
        { cparse_err_update_out_of_memory(err); return -1; }

        /* Next node */
        node_cur = cparse_value_as_list_node_next(node_cur);
    }

    *json_val_out = new_json_val_list;
    goto done;
fail:
    if (new_json_val_element != NULL)
    { cparse_json_val_destroy(new_json_val_element); }
    if (new_json_val_list != NULL)
    { cparse_json_val_destroy(new_json_val_list); }
done:
    return ret;
}

static int cparse_json_ast_ctor_in_tuple( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    int ret = 0;
    cparse_json_val_t *new_json_val_list = NULL;
    cparse_json_val_t *new_json_val_element = NULL;
    cparse_value_t *value_cur;
    cparse_size_t len = cparse_value_as_tuple_size(value), idx;

    /* Create a new blank array */
    if ((new_json_val_list = cparse_json_val_new_array()) == NULL)
    { cparse_err_update_out_of_memory(err); return -1; }

    /* Walk though nodes in list and append nodes into the created new array */
    for (idx = 0; idx != len; idx++)
    {
        value_cur = cparse_value_as_tuple_ref(value, idx);
        if (cparse_json_ast_ctor_in(err, &new_json_val_element, value_cur) != 0)
        { ret = -1; goto fail; }
        if (cparse_json_val_as_array_push(new_json_val_list, \
                new_json_val_element) != 0)
        { cparse_err_update_out_of_memory(err); return -1; }
    }

    *json_val_out = new_json_val_list;
    goto done;
fail:
    if (new_json_val_element != NULL)
    { cparse_json_val_destroy(new_json_val_element); }
    if (new_json_val_list != NULL)
    { cparse_json_val_destroy(new_json_val_list); }
done:
    return ret;
}

static int cparse_json_ast_ctor_in_struct( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    int ret = 0;
    cparse_json_val_t *new_json_val_name = NULL;
    cparse_json_val_t *new_json_val_members = NULL;
    cparse_json_val_t *new_json_val_final = NULL;
    cparse_json_val_t *new_json_val_member_name = NULL;
    cparse_json_val_t *new_json_val_member_value = NULL;
    cparse_value_map_node_t *member_cur;
    cparse_char_t *member_cur_key;
    cparse_value_t *member_cur_value;

    /* {
     *   name : "name",
     *   members : { name : value, name : value ... }
     * } */

    /* Name */
    if ((new_json_val_name = cparse_json_val_new_string( \
                    cparse_value_as_struct_name_get_wide(value))) == NULL)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }

    /* Members */
    if ((new_json_val_members = cparse_json_val_new_object()) == NULL)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }
    member_cur = cparse_value_as_struct_member_first(value);
    while (member_cur != NULL)
    {
        member_cur_key = cparse_value_as_struct_member_node_key_wide(member_cur);
        member_cur_value = cparse_value_as_struct_member_node_value(member_cur);

        if ((new_json_val_member_name = cparse_json_val_new_string(member_cur_key)) == NULL)
        { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }
        if (cparse_json_ast_ctor_in(err, &new_json_val_member_value, \
                    member_cur_value) != 0)
        { ret = -1; goto fail; }
        cparse_json_val_as_object_set(new_json_val_members, \
                new_json_val_member_name, new_json_val_member_value);
        new_json_val_member_name = NULL;
        new_json_val_member_value = NULL;

        /* Next Member */
        member_cur = cparse_value_as_struct_member_node_next(member_cur);
    }

    /* Final */
    if ((new_json_val_final = cparse_json_val_new_object()) == NULL)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }
    /* Final : Name */
    if (cparse_json_val_as_object_set(new_json_val_final, \
            cparse_json_val_new_string_from_utf8("name"), \
            new_json_val_name) != 0)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }
    new_json_val_name = NULL;
    /* Final : Members */
    if (cparse_json_val_as_object_set(new_json_val_final, \
            cparse_json_val_new_string_from_utf8("members"), \
            new_json_val_members) != 0)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }
    new_json_val_members = NULL;

    *json_val_out = new_json_val_final; new_json_val_final = NULL;
    goto done;
fail:
done:
    if (new_json_val_name != NULL) cparse_json_val_destroy(new_json_val_name);
    if (new_json_val_members != NULL) cparse_json_val_destroy(new_json_val_members);
    if (new_json_val_final != NULL) cparse_json_val_destroy(new_json_val_final);
    return ret;
}

static int cparse_json_ast_ctor_in_bool( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    int ret = 0;
    cparse_json_val_t *new_json_val = NULL;
    cparse_bool b = cparse_value_as_bool_get(value);
    if (b == cparse_false)
    { new_json_val = cparse_json_val_new_false(); }
    else
    { new_json_val = cparse_json_val_new_true(); }

    if (new_json_val == NULL)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }

    *json_val_out = new_json_val;
fail:
    return ret;
}

static int cparse_json_ast_ctor_in( \
        cparse_err_t *err, \
        cparse_json_val_t **json_val_out, \
        cparse_value_t *value)
{
    int ret = 0;
    cparse_value_type_t type = cparse_value_type(value);
    cparse_json_val_t *new_json_val = NULL;
    cparse_json_val_t *new_json_final = NULL;

    if ((new_json_final = cparse_json_val_new_object()) == NULL)
    { cparse_err_update_out_of_memory(err); ret = -1; goto fail; }

    switch (type)
    {
        case CPARSE_VALUE_TYPE_STRING:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("string"));

            if (cparse_json_ast_ctor_in_string(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("bool"));

            if (cparse_json_ast_ctor_in_bool(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);

            break;
        case CPARSE_VALUE_TYPE_CHAR:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("char"));

            if (cparse_json_ast_ctor_in_char(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);

            break;
        case CPARSE_VALUE_TYPE_LIST:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("list"));

            if (cparse_json_ast_ctor_in_list(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);

            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("tuple"));

            if (cparse_json_ast_ctor_in_tuple(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);

            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("struct"));

            if (cparse_json_ast_ctor_in_struct(err, &new_json_val, value) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);
            break;
        case CPARSE_VALUE_TYPE_TAG:
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("type"), \
                    cparse_json_val_new_string_from_utf8("tag"));

            if (cparse_json_ast_ctor_in(err, &new_json_val, \
                        cparse_value_as_tag_value(value)) != 0)
            { goto fail; }
            cparse_json_val_as_object_set(new_json_final, \
                    cparse_json_val_new_string_from_utf8("value"), \
                    new_json_val);

            break;
    }

    *json_val_out = new_json_final;
fail:
    return ret;
}

void *cparse_json_ast_ctor_wide(cparse_err_t *err, cparse_value_t *value)
{
    cparse_char_t *s = NULL;
    cparse_json_val_t *new_json_val = NULL;

    if (cparse_json_ast_ctor_in(err, &new_json_val, value) != 0)
    { goto fail; }

    if (cparse_json_stringify(&s, NULL, new_json_val) != 0)
    { cparse_err_update_out_of_memory(err); goto fail; }

    goto done;
fail:
done:
    if (new_json_val != NULL) cparse_json_val_destroy(new_json_val);
    return s;
}

void *cparse_json_ast_ctor(cparse_err_t *err, cparse_value_t *value)
{
    char *s = NULL;
    cparse_json_val_t *new_json_val = NULL;

    if (cparse_json_ast_ctor_in(err, &new_json_val, value) != 0)
    { goto fail; }

    if (cparse_json_stringify_to_utf8(&s, NULL, new_json_val) != 0)
    { cparse_err_update_out_of_memory(err); goto fail; }

    goto done;
fail:
done:
    if (new_json_val != NULL) cparse_json_val_destroy(new_json_val);
    return s;
}

void cparse_json_ast_dtor_wide(void *ptr)
{
    cparse_free(ptr);
}

void cparse_json_ast_dtor(void *ptr)
{
    cparse_free(ptr);
}

