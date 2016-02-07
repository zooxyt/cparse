/* CParse : Map
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_map.h"

#include "cparse_dt.h"
#include "cparse_res.h"

struct cparse_value_map_node
{
    void *key;
    void *value;

    struct cparse_value_map_node *next;
};

struct cparse_value_map
{
    struct cparse_value_map_node *begin, *end;

    cparse_bool (*key_eqp)(void *key1, void *key2);
    void *(*key_cctor)(void *ptr);
    void (*key_dtor)(void *ptr);
    void *(*value_cctor)(void *ptr);
    void (*value_dtor)(void *ptr);
};

static struct cparse_value_map_node *cparse_map_node_new(void *key, void *value)
{
    struct cparse_value_map_node *new_node = NULL;

    if ((new_node = (struct cparse_value_map_node *)cparse_malloc( \
                    sizeof(struct cparse_value_map_node))) == NULL)
    { return NULL; }
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;

    return new_node;
}

static void cparse_map_node_destroy(struct cparse_value_map_node *node, \
        void (*key_dtor)(void *key_dtor), \
        void (*value_dtor)(void *key_dtor))
{
    key_dtor(node->key);
    value_dtor(node->value);
    cparse_free(node);
}

/* Create and destroy map */
cparse_value_map_t *cparse_value_map_new( \
        cparse_bool (*key_eqp)(void *key1, void *key2), \
        void *(*key_cctor)(void *ptr), \
        void (*key_dtor)(void *ptr), \
        void *(*value_cctor)(void *ptr), \
        void (*value_dtor)(void *ptr))
{
    cparse_value_map_t *new_map = NULL;

    if ((new_map = (cparse_value_map_t *)cparse_malloc( \
                    sizeof(cparse_value_map_t))) == NULL)
    { return NULL; }
    new_map->begin = new_map->end = NULL;
    new_map->key_eqp = key_eqp;
    new_map->key_cctor = key_cctor;
    new_map->key_dtor = key_dtor;
    new_map->value_cctor = value_cctor;
    new_map->value_dtor = value_dtor;

    return new_map;
}

void cparse_value_map_destroy(cparse_value_map_t *cparse_map)
{
    struct cparse_value_map_node *node_cur, *node_next;

    node_cur = cparse_map->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_map_node_destroy(node_cur, cparse_map->key_dtor, cparse_map->value_dtor);
        node_cur = node_next;
    }
    cparse_free(cparse_map);
}

cparse_value_map_t *cparse_value_map_clone(cparse_value_map_t *cparse_map)
{
    cparse_value_map_t *new_map = NULL;
    struct cparse_value_map_node *node_cur;
    void *new_key = NULL, *new_value = NULL;

    if ((new_map = cparse_value_map_new( \
                    cparse_map->key_eqp, 
                    cparse_map->key_cctor, 
                    cparse_map->key_dtor, 
                    cparse_map->value_cctor, 
                    cparse_map->value_dtor)) == NULL)
    { return NULL; }

    node_cur = cparse_map->begin;
    while (node_cur != NULL)
    {
        if ((new_key = cparse_map->key_cctor(node_cur->key)) == NULL)
        { goto fail; }
        if ((new_value = cparse_map->value_cctor(node_cur->value)) == NULL)
        { cparse_map->key_dtor(new_key); goto fail; }

        if (cparse_value_map_set(new_map, new_key, new_value) != 0)
        {
            cparse_map->key_dtor(new_key);
            cparse_map->value_dtor(new_value);
            goto fail; 
        }
        new_key = NULL;
        new_value = NULL;

        node_cur = node_cur->next;
    }

    goto done;
fail:
    if (new_map != NULL)
    {
        cparse_value_map_destroy(new_map);
        new_map = NULL;
    }
done:
    return new_map;
}

static struct cparse_value_map_node *cparse_map_lookup(cparse_value_map_t *cparse_map, void *key)
{
    struct cparse_value_map_node *node_cur;

    node_cur = cparse_map->begin;
    while (node_cur != NULL)
    {
        if (cparse_map->key_eqp(node_cur->key, key) == cparse_true)
        { return node_cur; }

        node_cur = node_cur->next;
    }

    return NULL;
}

int cparse_value_map_set(cparse_value_map_t *cparse_map, void *key, void *value)
{
    int ret = 0;
    struct cparse_value_map_node *cparse_map_node = cparse_map_lookup(cparse_map, key);
    struct cparse_value_map_node *new_node = NULL;

    if (cparse_map_node != NULL)
    {
        cparse_map->value_dtor(cparse_map_node->value);
        cparse_map_node->value = value;
    }
    else
    {
        if ((new_node = cparse_map_node_new(key, value)) == NULL)
        { return -1; }

        if (cparse_map->begin == NULL)
        {
            cparse_map->begin = cparse_map->end = new_node;
        }
        else
        {
            cparse_map->end->next = new_node;
            cparse_map->end = new_node;
        }
    }

    return ret;
}

int cparse_value_map_clone_and_set(cparse_value_map_t *cparse_map, void *key, void *value)
{
    int ret = 0;
    void *new_key = cparse_map->key_cctor(key);
    void *new_value = cparse_map->value_cctor(value);

    if ((new_key == NULL) || (new_value == NULL))
    { ret = -1; goto fail; }

    if ((ret = cparse_value_map_set(cparse_map, new_key, new_value)) != 0)
    { goto fail; }

    goto done;
fail:
    if (new_key != NULL) cparse_map->key_dtor(new_key);
    if (new_value != NULL) cparse_map->value_dtor(new_value);
done:
    return ret;
}

void *cparse_value_map_get(cparse_value_map_t *cparse_map, void *key)
{
    struct cparse_value_map_node *cparse_map_node = cparse_map_lookup(cparse_map, key);
    if (cparse_map_node == NULL) return NULL;
    return cparse_map_node->value;
}

cparse_value_map_node_t *cparse_value_map_first(cparse_value_map_t *cparse_map)
{
    return cparse_map->begin;
}

cparse_value_map_node_t *cparse_value_map_node_next(cparse_value_map_node_t *node)
{
    return node->next;
}

void *cparse_value_map_node_key(cparse_value_map_node_t *node)
{
    return node->key;
}

void *cparse_value_map_node_value(cparse_value_map_node_t *node)
{
    return node->value;
}

