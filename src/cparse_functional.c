/* CParse : Functional
 * Copyright(c) Cheryl Natsu */

#include "cparse_value.h"
#include "cparse_functional.h"


/* map :: (a -> b) -> [a] -> [b] */
cparse_value_t *cparse_functional_map( \
        cparse_function_1_to_1_t f, \
        cparse_value_t *lst)
{
    cparse_value_t *ret = NULL;
    cparse_value_list_node_t *node_cur = NULL;
    cparse_value_t *value_cur = NULL;
    cparse_value_t *new_value = NULL;

    /* List */
    if (cparse_value_type(lst) != CPARSE_VALUE_TYPE_LIST)
    { return NULL; }

    if ((ret = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { goto fail; }

    node_cur = cparse_value_as_list_first(lst);
    for (;;)
    {
        if (node_cur == NULL) break;

        value_cur = cparse_value_as_list_node_value(node_cur);
        if (value_cur == NULL) goto fail;

        if ((new_value = f(value_cur)) == NULL) { goto fail; }
        if ((cparse_value_as_list_push_back(ret, new_value)) != 0)
        { goto fail; }

        /* Next */
        node_cur = cparse_value_as_list_node_next(node_cur);
        if (node_cur == NULL) break;
    }

fail:
    return ret;
}


/* filter :: (a -> Bool) -> [a] -> [b] */
cparse_value_t *cparse_functional_filter( \
        cparse_function_1_to_1_t f, \
        cparse_value_t *lst)
{
    cparse_value_t *ret = NULL;
    cparse_value_list_node_t *node_cur = NULL;
    cparse_value_t *value_cur = NULL;
    cparse_value_t *new_value = NULL;

    /* List */
    if (cparse_value_type(lst) != CPARSE_VALUE_TYPE_LIST)
    { return NULL; }

    if ((ret = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { goto fail; }

    node_cur = cparse_value_as_list_first(lst);
    for (;;)
    {
        value_cur = cparse_value_as_list_node_value(node_cur);
        if (value_cur == NULL) goto fail;

        if ((new_value = f(value_cur)) == NULL) { goto fail; }
        if ((cparse_value_type(new_value) == CPARSE_VALUE_TYPE_BOOL) && \
                (cparse_value_as_bool_get(new_value) == cparse_true))
        {
            if ((cparse_value_as_list_push_back(ret, cparse_value_clone(value_cur))) != 0)
            { goto fail; }
        }

        /* Next */
        node_cur = cparse_value_as_list_node_next(node_cur);
        if (node_cur == NULL) break;
    }

fail:
    return ret;
}


/* foldl :: (a -> b -> a) -> a -> [b] -> a */
cparse_value_t *cparse_functional_foldl( \
        cparse_function_2_to_1_t f, \
        cparse_value_t *a, \
        cparse_value_t *lst)
{
    cparse_value_t *ret = NULL;

    cparse_value_list_node_t *node_first = NULL;
    cparse_value_t *value_first = NULL;

    cparse_value_t *value_mid = NULL, *value_mid2 = NULL;

    cparse_size_t len;

    /* List */
    if (cparse_value_type(lst) != CPARSE_VALUE_TYPE_LIST)
    { return NULL; }
    len = cparse_value_as_list_size(lst);

    if (len == 0)
    {
        ret = cparse_value_clone(a);
    }
    else
    {
        node_first = cparse_value_as_list_first(lst);
        value_first = cparse_value_as_list_node_value(node_first);
        if (value_first == NULL) goto fail;

        value_mid = f(a, value_first);

        for (;;)
        {
            node_first = cparse_value_as_list_node_next(node_first);
            if (node_first == NULL) break;
            value_first = cparse_value_as_list_node_value(node_first);
            if (value_first == NULL) goto fail;

            value_mid2 = f(value_mid, value_first);
            cparse_value_destroy(value_mid);
            value_mid = value_mid2; value_mid2 = NULL;
        }

        ret = value_mid; value_mid = NULL;
    }

    goto done;
fail:
done:
    if (value_mid != NULL) cparse_value_destroy(value_mid);
    return ret;
}

