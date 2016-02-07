/* CParse : Regex : Parser
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_REGEX_PARSER_H
#define CPARSE_REGEX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_regex_token.h"
#include "cparse_regex_ast.h"

struct cparse_regex_ast *cparse_regex_parse( \
        struct cparse_regex_token_list *token_list);

#ifdef __cplusplus
}
#endif

#endif

