/* CParse : JSON
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_JSON_H
#define CPARSE_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_value.h"

struct cparse_json_val;
typedef struct cparse_json_val cparse_json_val_t;

cparse_json_val_t *cparse_json_val_new_string(cparse_char_t *s);
cparse_json_val_t *cparse_json_val_new_string_from_utf8(char *s);
cparse_json_val_t *cparse_json_val_new_number(int number);
cparse_json_val_t *cparse_json_val_new_false(void);
cparse_json_val_t *cparse_json_val_new_true(void);
cparse_json_val_t *cparse_json_val_new_null(void);
cparse_json_val_t *cparse_json_val_new_array(void);
cparse_json_val_t *cparse_json_val_new_object(void);

void cparse_json_val_destroy(cparse_json_val_t *val);

int cparse_json_val_as_array_push(cparse_json_val_t *val, \
        cparse_json_val_t *new_element);
int cparse_json_val_as_object_set(cparse_json_val_t *val, \
        cparse_json_val_t *new_element_key, \
        cparse_json_val_t *new_element_value);

int cparse_json_stringify(cparse_char_t **s_out, \
        cparse_size_t *len_out, \
        cparse_json_val_t *val);
int cparse_json_stringify_to_utf8(char **s_out, \
        cparse_size_t *len_out, \
        cparse_json_val_t *val);


#ifdef __cplusplus
}
#endif

#endif

