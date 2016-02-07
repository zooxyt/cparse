/* CParse : Set
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_set.h"

#include "cparse_dt.h"
#include "cparse_res.h"

struct cparse_value_set_node
{
    void *key;

    struct cparse_value_set_node *next;
};

struct cparse_value_set
{
    struct cparse_value_set_node *begin, *end;

    cparse_bool (*key_eqp)(void *key1, void *key2);
    void *(*key_cctor)(void *ptr);
    void (*key_dtor)(void *ptr);
};

static struct cparse_value_set_node *cparse_set_node_new(void *key)
{
    struct cparse_value_set_node *new_node = NULL;

    if ((new_node = (struct cparse_value_set_node *)cparse_malloc( \
                    sizeof(struct cparse_value_set_node))) == NULL)
    { return NULL; }
    new_node->key = key;
    new_node->next = NULL;

    return new_node;
}

static void cparse_set_node_destroy(struct cparse_value_set_node *node, \
        void (*key_dtor)(void *key_dtor))
{
    key_dtor(node->key);
    cparse_free(node);
}

/* Create and destroy set */
cparse_value_set_t *cparse_value_set_new( \
        cparse_bool (*key_eqp)(void *key1, void *key2), \
        void *(*key_cctor)(void *ptr), \
        void (*key_dtor)(void *ptr))
{
    cparse_value_set_t *new_set = NULL;

    if ((new_set = (cparse_value_set_t *)cparse_malloc( \
                    sizeof(cparse_value_set_t))) == NULL)
    { return NULL; }
    new_set->begin = new_set->end = NULL;
    new_set->key_eqp = key_eqp;
    new_set->key_cctor = key_cctor;
    new_set->key_dtor = key_dtor;

    return new_set;
}

void cparse_value_set_destroy(cparse_value_set_t *cparse_set)
{
    struct cparse_value_set_node *node_cur, *node_next;

    node_cur = cparse_set->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_set_node_destroy(node_cur, cparse_set->key_dtor);
        node_cur = node_next;
    }
    cparse_free(cparse_set);
}

cparse_value_set_t *cparse_value_set_clone(cparse_value_set_t *cparse_set)
{
    cparse_value_set_t *new_set = NULL;
    struct cparse_value_set_node *node_cur;
    void *new_key = NULL;

    if ((new_set = cparse_value_set_new( \
                    cparse_set->key_eqp, 
                    cparse_set->key_cctor, 
                    cparse_set->key_dtor)) == NULL)
    { return NULL; }

    node_cur = cparse_set->begin;
    while (node_cur != NULL)
    {
        if ((new_key = cparse_set->key_cctor(node_cur->key)) == NULL)
        { goto fail; }

        if (cparse_value_set_set(new_set, new_key) != 0)
        {
            cparse_set->key_dtor(new_key);
            goto fail; 
        }
        new_key = NULL;

        node_cur = node_cur->next;
    }

    goto done;
fail:
    if (new_set != NULL)
    {
        cparse_value_set_destroy(new_set);
        new_set = NULL;
    }
done:
    return new_set;
}

static struct cparse_value_set_node *cparse_set_lookup(cparse_value_set_t *cparse_set, void *key)
{
    struct cparse_value_set_node *node_cur;

    node_cur = cparse_set->begin;
    while (node_cur != NULL)
    {
        if (cparse_set->key_eqp(node_cur->key, key) == cparse_true)
        { return node_cur; }

        node_cur = node_cur->next;
    }

    return NULL;
}

int cparse_value_set_set(cparse_value_set_t *cparse_set, void *key)
{
    int ret = 0;
    struct cparse_value_set_node *cparse_set_node = cparse_set_lookup(cparse_set, key);
    struct cparse_value_set_node *new_node = NULL;

    if (cparse_set_node != NULL)
    {
        /* Do nothing */
    }
    else
    {
        if ((new_node = cparse_set_node_new(key)) == NULL)
        { return -1; }

        if (cparse_set->begin == NULL)
        {
            cparse_set->begin = cparse_set->end = new_node;
        }
        else
        {
            cparse_set->end->next = new_node;
            cparse_set->end = new_node;
        }
    }

    return ret;
}

cparse_bool cparse_value_set_exist(cparse_value_set_t *cparse_set, void *key)
{
    return cparse_set_lookup(cparse_set, key) != NULL ? cparse_true : cparse_false;
}

cparse_value_set_node_t *cparse_value_set_first(cparse_value_set_t *cparse_set)
{
    return cparse_set->begin;
}

cparse_value_set_node_t *cparse_value_set_node_next(cparse_value_set_node_t *node)
{
    return node->next;
}

void *cparse_value_set_node_key(cparse_value_set_node_t *node)
{
    return node->key;
}

