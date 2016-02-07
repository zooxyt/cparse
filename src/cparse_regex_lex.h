/* CParse : Regex : Lexer
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_REGEX_LEX_H
#define CPARSE_REGEX_LEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_regex_token.h"

struct cparse_regex_token_list *cparse_regex_lex(cparse_char_t *pattern);

#ifdef __cplusplus
}
#endif

#endif

