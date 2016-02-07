/* CParse : AST Serialize
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_SERIALIZE_H
#define CPARSE_SERIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_err.h"

void *cparse_json_ast_ctor_wide(cparse_err_t *err, cparse_value_t *value);
void *cparse_json_ast_ctor(cparse_err_t *err, cparse_value_t *value);
void cparse_json_ast_dtor_wide(void *ptr);
void cparse_json_ast_dtor(void *ptr);

#ifdef __cplusplus
}
#endif

#endif

