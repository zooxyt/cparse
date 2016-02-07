/* CParse : Char
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_value_char.h"

cparse_bool cparse_value_char_eqp(cparse_value_char_t char1, cparse_value_char_t char2)
{
    return char1 == char2 ? cparse_true : cparse_false;
}

