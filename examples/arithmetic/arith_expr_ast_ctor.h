/* Arithmetic Expression AST Constructor
 * Copyright(C) Cheryl Natsu */

#ifndef _ARITH_EXPR_AST_CTOR_H_
#define _ARITH_EXPR_AST_CTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse.h"

typedef enum
{
    AST_NODE_BINOP_TYPE_ADD = 0,
    AST_NODE_BINOP_TYPE_SUB,
    AST_NODE_BINOP_TYPE_MUL,
    AST_NODE_BINOP_TYPE_DIV,
} ast_node_binop_type_t;

typedef enum
{
    AST_NODE_UNKNOWN = 0,
    AST_NODE_BINOP,
    AST_NODE_NUM,
} ast_node_type_t;

struct ast_node
{
    ast_node_type_t type;
    union
    {
        struct
        {
            ast_node_binop_type_t op;
            struct ast_node *left;
            struct ast_node *right;
        } binop;
        int num;
    } u;
};
typedef struct ast_node ast_node_t;

void *ast_ctor_normal(cparse_err_t *err, cparse_value_t *value);
void ast_dtor_normal(void *node);
void ast_node_print(ast_node_t *node);


#ifdef __cplusplus
}
#endif

#endif

