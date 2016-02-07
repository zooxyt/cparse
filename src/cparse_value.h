/* CParse : Value
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_H
#define CPARSE_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h" 
#include "cparse_charenc.h"
#include "cparse_value_char.h"
#include "cparse_value_bool.h"
#include "cparse_value_string.h"
#include "cparse_value_list.h"
#include "cparse_value_tuple.h"
#include "cparse_value_map.h"
#include "cparse_value_set.h"
#include "cparse_value_struct.h"

typedef enum cparse_value_type
{
    CPARSE_VALUE_TYPE_STRING = 0,
    CPARSE_VALUE_TYPE_BOOL,
    CPARSE_VALUE_TYPE_CHAR,
    CPARSE_VALUE_TYPE_LIST,
    CPARSE_VALUE_TYPE_TUPLE,
    CPARSE_VALUE_TYPE_STRUCT,
    CPARSE_VALUE_TYPE_TAG,
} cparse_value_type_t;

struct cparse_value;
struct cparse_value_node;

/* Data Type: Value */
#ifndef DT_CPARSE_VALUE_T
#define DT_CPARSE_VALUE_T
typedef struct cparse_value cparse_value_t;
#endif

/* Create Value */
cparse_value_t *cparse_value_new_bool(cparse_bool value);
cparse_value_t *cparse_value_new_char(cparse_value_char_t ch);
cparse_value_t *cparse_value_new_string_wide(cparse_char_t *s, cparse_size_t len);
cparse_value_t *cparse_value_new_string(char *s, cparse_size_t len);
cparse_value_t *cparse_value_new_list( \
        void *(*cctor)(void *p), \
        void (*dtor)(void *p));
cparse_value_t *cparse_value_new_tuple(cparse_value_t *first, ...);
cparse_value_t *cparse_value_new_struct_wide(cparse_char_t *name, ...);
cparse_value_t *cparse_value_new_struct(char *name, ...);
cparse_value_t *cparse_value_new_tag(int type_tag, cparse_value_t *value);

/* Destroy Value */
void cparse_value_destroy(cparse_value_t *value);
void cparse_value_dtor(void *p);

/* Type */
cparse_value_type_t cparse_value_type(cparse_value_t *value);

/* Clone */
cparse_value_t *cparse_value_clone(cparse_value_t *value);
void *cparse_value_cctor(void *p);

/* Equality */
cparse_bool cparse_value_eqp(cparse_value_t *value1, cparse_value_t *value2);

/* Operating */

cparse_value_char_t cparse_value_as_char_get(cparse_value_t *value);
cparse_value_t *cparse_value_as_char_to_string(cparse_value_t *value);

cparse_bool cparse_value_as_bool_get(cparse_value_t *value);

cparse_char_t *cparse_value_as_string_str_wide(cparse_value_t *value);
cparse_size_t cparse_value_as_string_length(cparse_value_t *value);
cparse_bool cparse_value_as_string_is(cparse_value_t *value, char *s);
int cparse_value_as_string_to_integer(cparse_value_t *value);

cparse_size_t cparse_value_as_list_size(cparse_value_t *value);
int cparse_value_as_list_push_back(cparse_value_t *value, cparse_value_t *new_element);
cparse_value_list_node_t *cparse_value_as_list_first(cparse_value_t *value);
cparse_value_list_node_t *cparse_value_as_list_node_next(cparse_value_list_node_t *node);
void *cparse_value_as_list_node_value(cparse_value_list_node_t *node);

cparse_size_t cparse_value_as_tuple_size(cparse_value_t *value);
void *cparse_value_as_tuple_ref(cparse_value_t *value, cparse_size_t index);

cparse_bool cparse_value_as_struct_is_wide(cparse_value_t *value, cparse_char_t *name);
cparse_bool cparse_value_as_struct_is(cparse_value_t *value, char *name);
cparse_char_t *cparse_value_as_struct_name_get_wide(cparse_value_t *value);
char *cparse_value_as_struct_name_get(cparse_value_t *value);
void *cparse_value_as_struct_member_get_wide(cparse_value_t *value, cparse_char_t *name);
void *cparse_value_as_struct_member_get(cparse_value_t *value, char *name);
cparse_value_map_node_t *cparse_value_as_struct_member_first(cparse_value_t *value);
cparse_value_map_node_t *cparse_value_as_struct_member_node_next(cparse_value_map_node_t *node);
cparse_char_t *cparse_value_as_struct_member_node_key_wide(cparse_value_map_node_t *node);
void *cparse_value_as_struct_member_node_value(cparse_value_map_node_t *node);

cparse_bool cparse_value_as_tag_is(cparse_value_t *value, int type_tag);
int cparse_value_as_tag_id(cparse_value_t *value);
struct cparse_value *cparse_value_as_tag_value(cparse_value_t *value);

/* Combinations */

cparse_value_t *cparse_value_string_concat(cparse_value_t *v1, cparse_value_t *v2);


#ifdef __cplusplus
}
#endif

#endif

