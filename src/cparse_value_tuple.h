/* CParse : Tuple
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_TUPLE_H
#define CPARSE_VALUE_TUPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_value.h"

/* Data Type: Tuple */
#ifndef DT_CPARSE_VALUE_TUPLE_T
#define DT_CPARSE_VALUE_TUPLE_T
typedef struct cparse_value_tuple cparse_value_tuple_t;
#endif

/* Create and destroy tuple */
cparse_value_tuple_t *cparse_value_tuple_new_from_va_list(void *first, va_list ap);
cparse_value_tuple_t *cparse_value_tuple_new(void *first, ...);
void cparse_value_tuple_destroy(cparse_value_tuple_t *t);
cparse_value_tuple_t *cparse_value_tuple_clone(cparse_value_tuple_t *t);

cparse_size_t cparse_value_tuple_length(cparse_value_tuple_t *t);
void *cparse_value_tuple_ref(cparse_value_tuple_t *t, cparse_size_t index);


#ifdef __cplusplus
}
#endif

#endif



