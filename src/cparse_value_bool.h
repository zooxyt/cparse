/* CParse : Boolean
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_BOOL_H
#define CPARSE_VALUE_BOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* Data Type: Boolean */
#ifndef DT_CPARSE_VALUE_BOOL_T
#define DT_CPARSE_VALUE_BOOl_T
typedef cparse_bool cparse_value_bool_t;
#endif

cparse_bool cparse_value_bool_eqp(cparse_value_bool_t char1, cparse_value_bool_t char2);

#ifdef __cplusplus
}
#endif

#endif


