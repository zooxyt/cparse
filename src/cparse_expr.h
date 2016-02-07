/* CParse : Expr
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_EXPR_H
#define CPARSE_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_value_char.h"
#include "cparse_functional.h"

struct cparse_component;

/* Data Type: Expr */
#ifndef DT_CPARSE_EXPR_T
#define DT_CPARSE_EXPR_T
typedef struct cparse_expr cparse_expr_t;
#endif

typedef enum
{
    CPARSE_EXPR_OPERATOR_TYPE_UNDEFINED = 0,
    CPARSE_EXPR_OPERATOR_TYPE_PREFIX,
    CPARSE_EXPR_OPERATOR_TYPE_POSTFIX,
    CPARSE_EXPR_OPERATOR_TYPE_INFIX,
} cparse_expr_operator_type_t;

typedef enum
{
    CPARSE_EXPR_ASSOCIATIVE_UNDEFINED = 0,
    CPARSE_EXPR_ASSOCIATIVE_LEFT,
    CPARSE_EXPR_ASSOCIATIVE_NONE,
    CPARSE_EXPR_ASSOCIATIVE_RIGHT,
} cparse_expr_associative_t;

struct cparse_expr_operator;
typedef struct cparse_expr_operator cparse_expr_operator_t;

/* Create and destroy expr */
cparse_expr_t *cparse_expr_new(cparse_size_t precedence_count);
void cparse_expr_destroy(cparse_expr_t *expr);

/* Operations */
int cparse_expr_append_unary_op_wide(cparse_expr_t *expr, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_1_to_1_t assembler_unary);
int cparse_expr_append_binary_op_wide(cparse_expr_t *expr, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_2_to_1_t assembler_binary);
int cparse_expr_set_follow(cparse_expr_t *expr, \
        struct cparse_component *component_follow);
struct cparse_component *cparse_expr_follow(cparse_expr_t *expr);

/* Walk in precedence */

/* When no available precedence */
#define CPARSE_EXPR_PRECEDENCE_EMPTY (-1)

int cparse_expr_first_precedence(cparse_expr_t *expr);
int cparse_expr_next_precedence(cparse_expr_t *expr, int precedence);
cparse_expr_operator_type_t cparse_expr_precedence_op_type(cparse_expr_t *expr, int precedence);
cparse_expr_operator_t *cparse_expr_op_first(cparse_expr_t *expr, int precedence);
cparse_expr_operator_t *cparse_expr_op_next(cparse_expr_operator_t *op);
cparse_expr_operator_type_t cparse_expr_op_type(cparse_expr_operator_t *op);
cparse_expr_associative_t cparse_expr_op_associative(cparse_expr_operator_t *op);
cparse_char_t *cparse_expr_op_str_wide(cparse_expr_operator_t *op);
cparse_function_1_to_1_t cparse_expr_op_assembler_unary(cparse_expr_operator_t *op);
cparse_function_2_to_1_t cparse_expr_op_assembler_binary(cparse_expr_operator_t *op);


#ifdef __cplusplus
}
#endif

#endif

