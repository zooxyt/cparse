/* Arithmetic Expression Syntax Definitions : Expr
 * Copyright(C) Cheryl Natsu */

#include "cparse.h"
#include "cparse_functional.h"
#include "arith_expr_syntax_expr.h"


static cparse_value_t *cparse_expr_tag_add(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Add", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_sub(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Sub", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_mul(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Mul", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_div(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Div", "left", a, "right", b, NULL); }

int parse_e_expr(CPARSE_COMPONENT_ARGS)
{
    cparse_component_t *component = NULL;

    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value);
    {
        CPARSE_COMPONENT_DEFINE(component, cparse_expr(2));
        CPARSE_COMPONENT_ONCE()
        {
            cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 1, "*", cparse_expr_tag_mul);
            cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 1, "/", cparse_expr_tag_div);
            cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 0, "+", cparse_expr_tag_add);
            cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 0, "-", cparse_expr_tag_sub);
            cparse_component_as_expr_set_follow(component, cparse_regex("[0-9]+"));
        }
        CPARSE_COMPONENT_PERFORM(value, component);
        CPARSE_COMPONENT_RETURN(value);
    }
    CPARSE_COMPONENT_END();
}

