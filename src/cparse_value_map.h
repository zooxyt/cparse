/* CParse : Map
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_MAP_H
#define CPARSE_VALUE_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_value_map_node;
struct cparse_value_map;

/* Data Type: Map */
#ifndef DT_CPARSE_VALUE_MAP_T
#define DT_CPARSE_VALUE_MAP_T
typedef struct cparse_value_map cparse_value_map_t;
#endif

/* Data Type: Map Node */
#ifndef DT_CPARSE_VALUE_MAP_NODE_T
#define DT_CPARSE_VALUE_MAP_NODE_T
typedef struct cparse_value_map_node cparse_value_map_node_t;
#endif

/* Create and destroy map */
cparse_value_map_t *cparse_value_map_new( \
        cparse_bool (*key_eqp)(void *key1, void *key2), \
        void *(*key_cctor)(void *ptr), \
        void (*key_dtor)(void *ptr), \
        void *(*value_cctor)(void *ptr), \
        void (*value_dtor)(void *ptr));
void cparse_value_map_destroy(cparse_value_map_t *cparse_map);
cparse_value_map_t *cparse_value_map_clone(cparse_value_map_t *cparse_map);

int cparse_value_map_set(cparse_value_map_t *cparse_map, void *key, void *value);
int cparse_value_map_clone_and_set(cparse_value_map_t *cparse_map, void *key, void *value);
void *cparse_value_map_get(cparse_value_map_t *cparse_map, void *key);

cparse_value_map_node_t *cparse_value_map_first(cparse_value_map_t *cparse_map);
cparse_value_map_node_t *cparse_value_map_node_next(cparse_value_map_node_t *node);
void *cparse_value_map_node_key(cparse_value_map_node_t *node);
void *cparse_value_map_node_value(cparse_value_map_node_t *node);

#ifdef __cplusplus
}
#endif

#endif

