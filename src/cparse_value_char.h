/* CParse : Char
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_CHAR_H
#define CPARSE_VALUE_CHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* Data Type: Char */
#ifndef DT_CPARSE_VALUE_CHAR_T
#define DT_CPARSE_VALUE_CHAR_T
typedef cparse_u32 cparse_value_char_t;
#endif

cparse_bool cparse_value_char_eqp(cparse_value_char_t char1, cparse_value_char_t char2);

#ifdef __cplusplus
}
#endif

#endif

