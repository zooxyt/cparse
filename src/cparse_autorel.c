/* CParse : Auto Release
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_autorel.h"

struct cparse_auto_release_node
{
    void **ptr_of_ptr;
    void (*dtor)(void *ptr);
    struct cparse_auto_release_node *next;
};

static struct cparse_auto_release_node *cparse_auto_release_node_new( \
        void **ptr_of_ptr, void (*dtor)(void *ptr))
{
    struct cparse_auto_release_node *new_node = NULL;

    if ((new_node = (struct cparse_auto_release_node *)cparse_malloc( \
                    sizeof(struct cparse_auto_release_node))) == NULL)
    { return NULL; }
    new_node->ptr_of_ptr = ptr_of_ptr;
    new_node->dtor = dtor;
    new_node->next = NULL;

    return new_node;
}

static void cparse_auto_release_node_destroy( \
        struct cparse_auto_release_node *node)
{
    if (*node->ptr_of_ptr != NULL) node->dtor(*node->ptr_of_ptr);
    cparse_free(node);
}

struct cparse_auto_release
{
    struct cparse_auto_release_node *begin, *end;
};

cparse_auto_release_t *cparse_auto_release_new(void)
{
    cparse_auto_release_t *new_ar = NULL;

    if ((new_ar = (cparse_auto_release_t *)cparse_malloc( \
                    sizeof(cparse_auto_release_t))) == NULL)
    { return NULL; }
    new_ar->begin = new_ar->end = NULL;

    return new_ar;
}

void cparse_auto_release_destroy(cparse_auto_release_t *ar)
{
    struct cparse_auto_release_node *node_cur, *node_next; 

    node_cur = ar->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_auto_release_node_destroy(node_cur);
        node_cur = node_next;
    }
    cparse_free(ar);
}

int cparse_auto_release_add(cparse_auto_release_t *ar, \
        void **ptr_of_ptr, void (*dtor)(void *ptr))
{
    struct cparse_auto_release_node *new_node = NULL;

    if ((new_node = cparse_auto_release_node_new(ptr_of_ptr, dtor)) == NULL)
    { return -1; }

    if (ar->begin == NULL)
    {
        ar->begin = ar->end = new_node;
    }
    else
    {
        ar->end->next = new_node;
        ar->end = new_node;
    }

    return 0;
}

