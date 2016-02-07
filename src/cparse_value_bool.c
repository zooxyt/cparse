/* CParse : Boolean
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_value_bool.h"

cparse_bool cparse_value_bool_eqp(cparse_value_bool_t char1, cparse_value_bool_t char2)
{
    return char1 == char2 ? cparse_true : cparse_false;
}


