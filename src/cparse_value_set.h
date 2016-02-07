/* CParse : Set
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_SET_H
#define CPARSE_VALUE_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_value_set_node;
struct cparse_value_set;

/* Data Type: Set */
#ifndef DT_CPARSE_VALUE_SET_T
#define DT_CPARSE_VALUE_SET_T
typedef struct cparse_value_set cparse_value_set_t;
#endif

/* Data Type: Set Node */
#ifndef DT_CPARSE_VALUE_SET_NODE_T
#define DT_CPARSE_VALUE_SET_NODE_T
typedef struct cparse_value_set_node cparse_value_set_node_t;
#endif

/* Create and destroy set */
cparse_value_set_t *cparse_value_set_new( \
        cparse_bool (*key_eqp)(void *key1, void *key2), \
        void *(*key_cctor)(void *ptr), \
        void (*key_dtor)(void *ptr));
void cparse_value_set_destroy(cparse_value_set_t *cparse_set);
cparse_value_set_t *cparse_value_set_clone(cparse_value_set_t *cparse_set);

int cparse_value_set_set(cparse_value_set_t *cparse_set, void *key);
cparse_bool cparse_value_set_exist(cparse_value_set_t *cparse_set, void *key);

cparse_value_set_node_t *cparse_value_set_first(cparse_value_set_t *cparse_set);
cparse_value_set_node_t *cparse_value_set_node_next(cparse_value_set_node_t *node);
void *cparse_value_set_node_key(cparse_value_set_node_t *node);

#ifdef __cplusplus
}
#endif

#endif


