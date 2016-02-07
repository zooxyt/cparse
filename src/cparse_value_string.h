/* CParse : String
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_STRING_H
#define CPARSE_VALUE_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_value_char.h"

/* Data Type: String */
#ifndef DT_CPARSE_VALUE_STRING_T
#define DT_CPARSE_VALUE_STRING_T
typedef struct cparse_value_string cparse_value_string_t;
#endif

/* Create and destroy string */
cparse_value_string_t *cparse_value_string_new_wide(cparse_value_char_t *s, cparse_size_t len);
cparse_value_string_t *cparse_value_string_new(char *s, cparse_size_t len);
void cparse_value_string_destroy(cparse_value_string_t *s);
void cparse_value_string_dtor(void *p);
cparse_value_string_t *cparse_value_string_clone(cparse_value_string_t *s);
void *cparse_value_string_cctor(void *p);
cparse_bool cparse_value_string_eqp(void *s1, void *s2);

cparse_value_char_t *cparse_value_string_str(cparse_value_string_t *s);
cparse_size_t cparse_value_string_length(cparse_value_string_t *s);


#ifdef __cplusplus
}
#endif

#endif


