/* CParse : Tuple
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_tuple.h"

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_value.h"

struct cparse_value_tuple
{
    void **body;

    cparse_size_t size;
};

/* Create and destroy tuple */
cparse_value_tuple_t *cparse_value_tuple_new_from_va_list(void *first, va_list ap)
{
    cparse_value_t *cur;
    cparse_value_tuple_t *new_tuple = NULL;
    cparse_size_t size = 0, idx = 0;
    va_list ap_bak;

    va_copy(ap_bak, ap);

    /* Size of tuple */ 
    if (first != NULL)
    {
        size = 1;
        while ((cur = va_arg(ap, cparse_value_t *)) != NULL)
        { size++; }
    }

    if ((new_tuple = (cparse_value_tuple_t *)cparse_malloc( \
                    sizeof(cparse_value_tuple_t))) == NULL)
    { return NULL; }
    new_tuple->size = size;
    new_tuple->body = NULL;
    if (size == 0) { return new_tuple; }

    if ((new_tuple->body = (void **)cparse_malloc( \
                    sizeof(void *) * size)) == NULL)
    {
        cparse_free(new_tuple);
        return NULL;
    }
    for (idx = 0; idx != size; idx++)
    {
        new_tuple->body[idx] = NULL;
    }

    new_tuple->body[0] = cparse_value_clone(first);
    idx = 1;

    va_copy(ap, ap_bak);
    while ((cur = va_arg(ap, cparse_value_t *)) != NULL)
    {
        new_tuple->body[idx] = cparse_value_clone(cur);

        idx++;
    }

    return new_tuple;
}

cparse_value_tuple_t *cparse_value_tuple_new(void *first, ...)
{
    cparse_value_tuple_t *ret = NULL;
    va_list ap;
    va_start(ap, first);
    ret = cparse_value_tuple_new_from_va_list(first, ap);
    va_end(ap);
    return ret;
}

void cparse_value_tuple_destroy(cparse_value_tuple_t *t)
{
    cparse_size_t idx;
    for (idx = 0; idx != t->size; idx++)
    {
        cparse_value_destroy(t->body[idx]);
    }
    if (t->body != NULL) cparse_free(t->body);
    cparse_free(t);
}

cparse_value_tuple_t *cparse_value_tuple_clone(cparse_value_tuple_t *t)
{
    cparse_value_tuple_t *new_tuple = NULL;
    cparse_size_t size = t->size, idx = 0;

    if ((new_tuple = (cparse_value_tuple_t *)cparse_malloc( \
                    sizeof(cparse_value_tuple_t))) == NULL)
    { return NULL; }
    new_tuple->size = t->size;
    new_tuple->body = NULL;
    if (size == 0) { return new_tuple; }

    if ((new_tuple->body = (void **)cparse_malloc( \
                    sizeof(void *) * size)) == NULL)
    {
        cparse_free(new_tuple);
        return NULL;
    }
    for (idx = 0; idx != size; idx++)
    { new_tuple->body[idx] = NULL; }

    for (idx = 0; idx != size; idx++)
    {
        if ((new_tuple->body[idx] = cparse_value_clone(t->body[idx])) == NULL)
        {
            cparse_value_tuple_destroy(new_tuple);
            return NULL;
        }
    }

    return new_tuple;
}

cparse_size_t cparse_value_tuple_length(cparse_value_tuple_t *t)
{
    return t->size;
}

void *cparse_value_tuple_ref(cparse_value_tuple_t *t, cparse_size_t index)
{
    return t->body[index];
}

