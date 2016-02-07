/* CParse : List
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_LIST_H
#define CPARSE_VALUE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* Data Type: List */
#ifndef DT_CPARSE_VALUE_LIST_T
#define DT_CPARSE_VALUE_LIST_T
typedef struct cparse_value_list cparse_value_list_t;
#endif

/* Data Type: Node */
#ifndef DT_CPARSE_LIST_NODE_T
#define DT_CPARSE_LIST_NODE_T
typedef struct cparse_value_list_node cparse_value_list_node_t;
#endif

/* Create and destroy list */
cparse_value_list_t *cparse_value_list_new( \
        void *(*cctor)(void *ptr), \
        void (*dtor)(void *ptr));
void cparse_value_list_destroy(cparse_value_list_t *cparse_list);
cparse_value_list_t *cparse_value_list_clone(cparse_value_list_t *cparse_list);

/* Push new node to the end */
int cparse_value_list_push_back(cparse_value_list_t *cparse_list, void *ptr);

/* Number of elements */
cparse_size_t cparse_value_list_size(struct cparse_value_list *cparse_list);

/* Iterating */
cparse_value_list_node_t *cparse_value_list_first(cparse_value_list_t *cparse_list);
cparse_value_list_node_t* cparse_value_list_node_next(cparse_value_list_node_t* node);
void *cparse_value_list_node_value(cparse_value_list_node_t *node);


#ifdef __cplusplus
}
#endif

#endif

