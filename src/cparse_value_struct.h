/* CParse : Struct
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_STRUCT_H
#define CPARSE_VALUE_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_value_map.h"

/* Data Type: Struct */
#ifndef DT_CPARSE_VALUE_STRUCT_T
#define DT_CPARSE_VALUE_STRUCT_T
typedef struct cparse_value_struct cparse_value_struct_t;
#endif

/* Create and destroy struct */
struct cparse_value_struct *cparse_value_struct_new_wide(cparse_char_t *name);
struct cparse_value_struct *cparse_value_struct_new_from_va_list( \
        cparse_bool is_utf8, \
        void *name, va_list ap);
void cparse_value_struct_destroy(cparse_value_struct_t *cparse_struct);
cparse_value_struct_t *cparse_value_struct_clone(cparse_value_struct_t *cparse_struct);

cparse_bool cparse_value_struct_is_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name);
int cparse_value_struct_name_set_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name);
cparse_char_t *cparse_value_struct_name_get_wide(cparse_value_struct_t *cparse_struct);
int cparse_value_struct_member_set_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name, void *ptr);
void *cparse_value_struct_member_get_wide(cparse_value_struct_t *cparse_struct, cparse_char_t *name);

cparse_value_map_node_t *cparse_value_struct_member_first(cparse_value_struct_t *cparse_struct);
cparse_value_map_node_t *cparse_value_struct_member_node_next(cparse_value_map_node_t *node);
cparse_char_t *cparse_value_struct_member_node_key_wide(cparse_value_map_node_t *node);
void *cparse_value_struct_member_node_value(cparse_value_map_node_t *node);



#ifdef __cplusplus
}
#endif

#endif

