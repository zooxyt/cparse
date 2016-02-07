/* CParse : Expr
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_value_char.h"
#include "cparse_charenc.h"
#include "cparse_functional.h"
#include "cparse_expr.h"

struct cparse_component;
void cparse_component_destroy(struct cparse_component *component);


struct cparse_expr_operator
{
    cparse_expr_operator_type_t type;
    union
    {
        cparse_function_2_to_1_t part_binary;
        cparse_function_1_to_1_t part_unary;
    } callback_assembler;
    cparse_expr_associative_t associative;
    cparse_char_t *str;

    struct cparse_expr_operator *prev, *next;
};

static void cparse_expr_operator_destroy(struct cparse_expr_operator *op);

static cparse_expr_operator_t *cparse_expr_operator_new_unary_wide( \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_char_t *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    cparse_expr_operator_t *new_operator = NULL;

    if ((new_operator = (cparse_expr_operator_t *)cparse_malloc( \
                    sizeof(cparse_expr_operator_t))) == NULL)
    { return NULL; }
    new_operator->type = type;
    new_operator->associative = associative;
    new_operator->callback_assembler.part_unary = assembler_unary;
    new_operator->prev = new_operator->next = NULL;
    if ((new_operator->str = cparse_unicode_string_duplicate(str)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_operator != NULL)
    { cparse_expr_operator_destroy(new_operator); new_operator = NULL; }
done:
    return new_operator;
}

static cparse_expr_operator_t *cparse_expr_operator_new_binary_wide( \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_char_t *str, \
        cparse_function_2_to_1_t assembler_binary)
{
    cparse_expr_operator_t *new_operator = NULL;

    if ((new_operator = (cparse_expr_operator_t *)cparse_malloc( \
                    sizeof(cparse_expr_operator_t))) == NULL)
    { return NULL; }
    new_operator->type = type;
    new_operator->associative = associative;
    new_operator->callback_assembler.part_binary = assembler_binary;
    new_operator->prev = new_operator->next = NULL;
    if ((new_operator->str = cparse_unicode_string_duplicate(str)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_operator != NULL)
    { cparse_expr_operator_destroy(new_operator); new_operator = NULL; }
done:
    return new_operator;
}

static void cparse_expr_operator_destroy(struct cparse_expr_operator *op)
{
    if (op->str != NULL) cparse_free(op->str);
    cparse_free(op);
}

struct cparse_expr_operator_list
{
    cparse_expr_operator_t *begin, *end;
    cparse_size_t size;
};
typedef struct cparse_expr_operator_list cparse_expr_operator_list_t;

static cparse_expr_operator_list_t *cparse_expr_operator_list_new(void)
{
    cparse_expr_operator_list_t *new_operator_list = NULL;

    if ((new_operator_list = (cparse_expr_operator_list_t *)cparse_malloc( \
                    sizeof(cparse_expr_operator_list_t))) == NULL)
    { return NULL; }
    new_operator_list->begin = new_operator_list->end = NULL;
    new_operator_list->size = 0;
        
    return new_operator_list;
}

static void cparse_expr_operator_list_destroy(cparse_expr_operator_list_t *operator_list)
{
    cparse_expr_operator_t *operator_cur, *operator_next;

    operator_cur = operator_list->begin;
    while (operator_cur != NULL)
    {
        operator_next = operator_cur->next;
        cparse_expr_operator_destroy(operator_cur);
        operator_cur = operator_next;
    }

    cparse_free(operator_list);
}

static void cparse_expr_operator_list_push_back( \
        cparse_expr_operator_list_t *operator_list, \
        cparse_expr_operator_t *new_operator)
{
    if (operator_list->begin == NULL)
    {
        operator_list->begin = operator_list->end = new_operator;
    }
    else
    {
        new_operator->prev = operator_list->end;
        operator_list->end->next = new_operator;
        operator_list->end = new_operator;
    }
    operator_list->size++;
}

struct cparse_expr_precedence
{
    struct cparse_expr_operator_list *operators;
    cparse_expr_operator_type_t type;
};
typedef struct cparse_expr_precedence cparse_expr_precedence_t;

struct cparse_expr
{
    cparse_expr_precedence_t *precedence_tbl;
    cparse_size_t precedence_count;

    struct cparse_component *component_follow;
};

/* Create and destroy expr */
cparse_expr_t *cparse_expr_new(cparse_size_t precedence_count)
{
    cparse_expr_t *new_expr = NULL;
    cparse_size_t idx;

    if ((new_expr = (cparse_expr_t *)cparse_malloc( \
                    sizeof(cparse_expr_t))) == NULL)
    { return NULL; }
    new_expr->component_follow = NULL;
    new_expr->precedence_count = precedence_count;
    if ((new_expr->precedence_tbl = (cparse_expr_precedence_t *)cparse_malloc( \
                    sizeof(cparse_expr_precedence_t) * precedence_count)) == NULL)
    { goto fail; }
    for (idx = 0; idx != precedence_count; idx++)
    {
        new_expr->precedence_tbl[idx].operators = NULL;
        new_expr->precedence_tbl[idx].type = CPARSE_EXPR_OPERATOR_TYPE_UNDEFINED;
    }

    goto done;
fail:
    if (new_expr != NULL)
    { cparse_expr_destroy(new_expr); new_expr = NULL; }
done:
    return new_expr;
}

void cparse_expr_destroy(cparse_expr_t *expr)
{
    cparse_size_t idx;

    if (expr->component_follow != NULL) cparse_component_destroy(expr->component_follow);

    for (idx = 0; idx != expr->precedence_count; idx++)
    {
        if (expr->precedence_tbl[idx].operators != NULL)
        { cparse_expr_operator_list_destroy(expr->precedence_tbl[idx].operators); }
    }
    if (expr->precedence_tbl != NULL) { cparse_free(expr->precedence_tbl); }

    cparse_free(expr);
}

/* Operations */
int cparse_expr_append_unary_op_wide(cparse_expr_t *expr, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_1_to_1_t assembler_unary)
{
    cparse_expr_operator_t *new_operator = NULL;

    if (precedence >= expr->precedence_count) return -1;

    /* Operators does not exist */
    if (expr->precedence_tbl[precedence].operators == NULL)
    {
        if ((expr->precedence_tbl[precedence].operators = cparse_expr_operator_list_new()) == NULL)
        { return -1; }
    }
    if (expr->precedence_tbl[precedence].type == CPARSE_EXPR_OPERATOR_TYPE_UNDEFINED)
    {
        /* Set the type of operators in the whole precedence */
        expr->precedence_tbl[precedence].type = type;
    }
    else
    {
        if (expr->precedence_tbl[precedence].type != type)
        {
            /* The type of new operator should matches the exist ones */
            return -1;
        }
    }

    if ((new_operator = cparse_expr_operator_new_unary_wide( \
                    type, associative, str, assembler_unary)) == NULL)
    { return -1; }

    cparse_expr_operator_list_push_back( \
            expr->precedence_tbl[precedence].operators, \
            new_operator);

    return 0;
}

int cparse_expr_append_binary_op_wide(cparse_expr_t *expr, \
        cparse_expr_operator_type_t type, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        cparse_char_t *str, \
        cparse_function_2_to_1_t assembler_binary)
{
    cparse_expr_operator_t *new_operator = NULL;

    if (precedence >= expr->precedence_count) return -1;

    /* Operators does not exist */
    if (expr->precedence_tbl[precedence].operators == NULL)
    {
        if ((expr->precedence_tbl[precedence].operators = cparse_expr_operator_list_new()) == NULL)
        { return -1; }
    }
    if (expr->precedence_tbl[precedence].type == CPARSE_EXPR_OPERATOR_TYPE_UNDEFINED)
    {
        /* Set the type of operators in the whole precedence */
        expr->precedence_tbl[precedence].type = type;
    }
    else
    {
        if (expr->precedence_tbl[precedence].type != type)
        {
            /* The type of new operator should matches the exist ones */
            return -1;
        }
    }

    if ((new_operator = cparse_expr_operator_new_binary_wide( \
                    type, associative, str, assembler_binary)) == NULL)
    { return -1; }

    cparse_expr_operator_list_push_back( \
            expr->precedence_tbl[precedence].operators, \
            new_operator);

    return 0;
}

int cparse_expr_set_follow(cparse_expr_t *expr, \
        struct cparse_component *component_follow)
{
    if (expr->component_follow != NULL)
    { cparse_component_destroy(expr->component_follow); }
    expr->component_follow = component_follow;

    return 0;
}

struct cparse_component *cparse_expr_follow(cparse_expr_t *expr)
{
    return expr->component_follow;
}

int cparse_expr_first_precedence(cparse_expr_t *expr)
{
    int precedence = 0;

    while (precedence < (int)expr->precedence_count)
    {
        if (expr->precedence_tbl[precedence].operators != NULL)
        { return precedence; }
        precedence++;
    }

    return CPARSE_EXPR_PRECEDENCE_EMPTY;
}

int cparse_expr_next_precedence(cparse_expr_t *expr, int precedence)
{
    precedence++;

    while (precedence < (int)expr->precedence_count)
    {
        if (expr->precedence_tbl[precedence].operators != NULL)
        { return precedence; }
        precedence++;
    }

    return CPARSE_EXPR_PRECEDENCE_EMPTY;
}

cparse_expr_operator_type_t cparse_expr_precedence_op_type(cparse_expr_t *expr, int precedence)
{
    return expr->precedence_tbl[precedence].type;
}

cparse_expr_operator_t *cparse_expr_op_first(cparse_expr_t *expr, int precedence)
{
    return expr->precedence_tbl[precedence].operators->begin;
}

cparse_expr_operator_t *cparse_expr_op_next(cparse_expr_operator_t *op)
{
    return op->next;
}

cparse_expr_operator_type_t cparse_expr_op_type(cparse_expr_operator_t *op)
{
    return op->type;
}

cparse_expr_associative_t cparse_expr_op_associative(cparse_expr_operator_t *op)
{
    return op->associative;
}

cparse_char_t *cparse_expr_op_str_wide(cparse_expr_operator_t *op)
{
    return op->str;
}

cparse_function_1_to_1_t cparse_expr_op_assembler_unary(cparse_expr_operator_t *op)
{
    return op->callback_assembler.part_unary;
}

cparse_function_2_to_1_t cparse_expr_op_assembler_binary(cparse_expr_operator_t *op)
{
    return op->callback_assembler.part_binary;
}

