/* Arithmetic Expression AST Constructor
 * Copyright(C) Cheryl Natsu */

#include <stdlib.h>
#include <stdio.h>
#include "cparse.h"
#include "arith_expr_ast_ctor.h"

ast_node_t *ast_node_new(ast_node_type_t type)
{
    ast_node_t *new_ast_node = NULL;
    if ((new_ast_node = (ast_node_t *)malloc( \
                    sizeof(ast_node_t))) == NULL)
    { return NULL; }
    new_ast_node->type = type;
    return new_ast_node;
}

ast_node_t *ast_node_new_num(int num)
{
    ast_node_t *new_ast_node = NULL;
    if ((new_ast_node = ast_node_new(AST_NODE_NUM)) == NULL)
    { return NULL; }
    new_ast_node->u.num = num;
    return new_ast_node;
}

ast_node_t *ast_node_new_binop(ast_node_binop_type_t op, ast_node_t *left, ast_node_t *right)
{
    ast_node_t *new_ast_node = NULL;
    if ((new_ast_node = ast_node_new(AST_NODE_BINOP)) == NULL)
    { return NULL; }
    new_ast_node->u.binop.op = op;
    new_ast_node->u.binop.left = left;
    new_ast_node->u.binop.right = right;
    return new_ast_node;
}

void ast_node_destroy(ast_node_t *node)
{
    switch (node->type)
    {
        case AST_NODE_UNKNOWN:
            break;
        case AST_NODE_BINOP:
            ast_node_destroy(node->u.binop.left);
            ast_node_destroy(node->u.binop.right);
            break;
        case AST_NODE_NUM:
            break;
    }
    free(node);
}

ast_node_t *ast_ctor_string(cparse_value_t *value)
{
    cparse_char_t *unicode_string = cparse_value_as_string_str_wide(value);
    char *s = cparse_unicode_string_to_utf8( \
            unicode_string, \
            cparse_unicode_string_strlen(unicode_string));
    int n = atoi(s);
    free(s);
    return ast_node_new_num(n);
}

void *ast_ctor_normal(cparse_err_t *err, cparse_value_t *value)
{
    cparse_value_type_t type = cparse_value_type(value);
    ast_node_t *ast_node = NULL;

    switch (type)
    {
        case CPARSE_VALUE_TYPE_STRING:
            ast_node = ast_ctor_string(value);
            break;
        case CPARSE_VALUE_TYPE_BOOL:
            break;
        case CPARSE_VALUE_TYPE_CHAR:
            break;
        case CPARSE_VALUE_TYPE_LIST:
            break;
        case CPARSE_VALUE_TYPE_TUPLE:
            break;
        case CPARSE_VALUE_TYPE_TAG:
            break;
        case CPARSE_VALUE_TYPE_STRUCT:
            if (cparse_value_as_struct_is(value, "Add"))
            {
                ast_node = ast_node_new_binop(AST_NODE_BINOP_TYPE_ADD, \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "left")), \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "right")));
            }
            else if (cparse_value_as_struct_is(value, "Sub"))
            {
                ast_node = ast_node_new_binop(AST_NODE_BINOP_TYPE_SUB, \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "left")), \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "right")));
            }
            else if (cparse_value_as_struct_is(value, "Mul"))
            {
                ast_node = ast_node_new_binop(AST_NODE_BINOP_TYPE_MUL, \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "left")), \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "right")));
            }
            else if (cparse_value_as_struct_is(value, "Div"))
            {
                ast_node = ast_node_new_binop(AST_NODE_BINOP_TYPE_DIV, \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "left")), \
                        ast_ctor_normal(err, cparse_value_as_struct_member_get(value, "right")));
            }
            else
            {
                /* Failed */
            }
            break;
    }

    return ast_node;
}

void ast_dtor_normal(void *node)
{
    ast_node_destroy(node);
}

void ast_node_print_in(ast_node_t *node, int depth)
{
    switch (node->type)
    {
        case AST_NODE_UNKNOWN:
            printf("Unknown");
            break;
        case AST_NODE_BINOP:
            if (depth > 0) printf("(");
            switch (node->u.binop.op)
            {
                case AST_NODE_BINOP_TYPE_ADD:
                    printf("Add");
                    break;
                case AST_NODE_BINOP_TYPE_SUB:
                    printf("Sub");
                    break;
                case AST_NODE_BINOP_TYPE_MUL:
                    printf("Mul");
                    break;
                case AST_NODE_BINOP_TYPE_DIV:
                    printf("Div");
                    break;
            }
            printf(" ");
            ast_node_print_in(node->u.binop.left, depth + 1);
            printf(" ");
            ast_node_print_in(node->u.binop.right, depth + 1);
            if (depth > 0) printf(")");
            break;
        case AST_NODE_NUM:
            printf("%d", node->u.num);
            break;
    }
}

void ast_node_print(ast_node_t *node)
{
    ast_node_print_in(node, 0);
}

