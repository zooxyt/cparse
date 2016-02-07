/* CParse : List
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_list.h"

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"

struct cparse_value_list_node
{
    void *ptr;

    struct cparse_value_list_node *prev, *next;
};

struct cparse_value_list
{
    void *(*cctor)(void *ptr);
    void (*dtor)(void *ptr);
    struct cparse_value_list_node *begin, *end;
    cparse_size_t size;
};

static struct cparse_value_list_node *cparse_list_node_new(void *ptr)
{
    struct cparse_value_list_node *new_node = NULL;

    if ((new_node = (struct cparse_value_list_node *)cparse_malloc( \
            sizeof(struct cparse_value_list_node))) == NULL)
    { return NULL; }
    new_node->prev = new_node->next = NULL;
    new_node->ptr = ptr;

    return new_node;
}

static void cparse_list_node_destroy(struct cparse_value_list_node *node, \
        void (*dtor)(void *ptr))
{
    dtor(node->ptr);
    cparse_free(node);
}

static struct cparse_value_list_node *cparse_list_node_clone( \
        struct cparse_value_list_node *node, \
        void *(*cctor)(void *ptr))
{
    return cctor(node->ptr);
}

cparse_value_list_t *cparse_value_list_new( \
        void *(*cctor)(void *ptr), \
        void (*dtor)(void *ptr))
{
    struct cparse_value_list *new_list = NULL;

    if ((new_list = (struct cparse_value_list *)cparse_malloc( \
            sizeof(struct cparse_value_list))) == NULL)
    { return NULL; }
    new_list->cctor = cctor;
    new_list->dtor = dtor;
    new_list->begin = new_list->end = NULL;
    new_list->size = 0;

    return new_list;
}

void cparse_value_list_destroy(struct cparse_value_list *cparse_list)
{
    struct cparse_value_list_node *node_cur, *node_next;

    node_cur = cparse_list->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_list_node_destroy(node_cur, cparse_list->dtor);
        node_cur = node_next;
    }
    cparse_free(cparse_list);
}

cparse_value_list_t *cparse_value_list_clone(cparse_value_list_t *cparse_list)
{
    struct cparse_value_list_node *node_cur, *new_node = NULL;
    struct cparse_value_list *new_list = NULL;

    if (cparse_list->cctor == NULL) return NULL;

    if ((new_list = cparse_value_list_new( \
                    cparse_list->cctor, \
                    cparse_list->dtor)) == NULL)
    { return NULL; }

    node_cur = cparse_list->begin;
    while (node_cur != NULL)
    {
        if ((new_node = cparse_list_node_clone( \
                        node_cur, \
                        cparse_list->cctor)) == NULL)
        { goto fail; }
        cparse_value_list_push_back(new_list, new_node);
        node_cur = node_cur->next;
    }

    goto done;
fail:
    cparse_list_node_destroy(new_node, cparse_list->dtor);
    if (new_list != NULL)
    {
        cparse_value_list_destroy(new_list);
        new_list = NULL;
    }
done:
    return new_list;
}

int cparse_value_list_push_back(struct cparse_value_list *cparse_list, void *ptr)
{
    struct cparse_value_list_node *new_node = cparse_list_node_new(ptr);
    if (new_node == NULL) return -1;
    if (cparse_list->begin == NULL)
    {
        cparse_list->begin = cparse_list->end = new_node;
    }
    else
    {
        new_node->prev = cparse_list->end;
        cparse_list->end->next = new_node;
        cparse_list->end = new_node;
    }
    cparse_list->size++;

    return 0;
}

cparse_size_t cparse_value_list_size(struct cparse_value_list *cparse_list)
{
    return cparse_list->size;
}

/* Iterating */
cparse_value_list_node_t *cparse_value_list_first(cparse_value_list_t *cparse_list)
{
    return cparse_list->begin;
}

cparse_value_list_node_t* cparse_value_list_node_next(cparse_value_list_node_t* node)
{
    return node->next;
}

void *cparse_value_list_node_value(cparse_value_list_node_t *node)
{
    return node->ptr;
}

