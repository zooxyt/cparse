/* Arithmetic Expression Syntax Definitions : Direct
 * Copyright(C) Cheryl Natsu */

#include "cparse.h"
#include "cparse_functional.h"
#include "arith_expr_syntax_direct.h"

/*
 * E -> T E'
 * E' -> + T E' | epsilon
 * T -> F T'
 * T' -> * F T' | epsilon
 * F -> [0-9]+
 */

cparse_value_t *parse_e_and_t_lambda_1(cparse_value_t *a, cparse_value_t *b);
int parse_e(CPARSE_COMPONENT_ARGS);
cparse_value_t *parse_e_lambda_1(cparse_value_t *a, cparse_value_t *b);
int parse_e1(CPARSE_COMPONENT_ARGS);
int parse_t(CPARSE_COMPONENT_ARGS);
cparse_value_t *parse_t_lambda_1(cparse_value_t *a, cparse_value_t *b);
int parse_t1(CPARSE_COMPONENT_ARGS);
int parse_factor(CPARSE_COMPONENT_ARGS);

cparse_bool parse_isdigit(int ch)
{
    return (('0' <= ch) && (ch <= '9')) ? cparse_true : cparse_false;
}

/* f1 :: String (op :: Char, f2 :: String) -> (Char, String) */
cparse_value_t *parse_t_and_e_lambda_1(cparse_value_t *a, cparse_value_t *b)
{
    cparse_value_t *ret = NULL;
    cparse_value_t *value_f1 = a;
    cparse_value_t *value_op = cparse_value_as_tuple_ref(b, 0);
    cparse_value_t *value_f2 = cparse_value_as_tuple_ref(b, 1);
    cparse_value_char_t op = cparse_value_as_char_get(value_op);

    /* (\f1 (op,f2) -> */
    /*   case op of */
    /*    '+' -> Add f1 f2 */
    /*    '-' -> Sub f1 f2 */
    /*    '*' -> Mul f1 f2 */
    /*    '/' -> Div f1 f2 */
    /*    _ -> f1) */
    switch (op)
    {
        case '+':
            ret = cparse_value_new_struct( \
                    "Add", \
                    "left", value_f1, \
                    "right", value_f2, \
                    NULL);
            break;
        case '-':
            ret = cparse_value_new_struct( \
                    "Sub", \
                    "left", value_f1, \
                    "right", value_f2, \
                    NULL);
            break;
        case '*':
            ret = cparse_value_new_struct( \
                    "Mul", \
                    "left", value_f1, \
                    "right", value_f2, \
                    NULL);
            break;
        case '/':
            ret = cparse_value_new_struct( \
                    "Div", 
                    "left", value_f1, \
                    "right", value_f2, \
                    NULL);
            break;
        default:
            break;
    }

    return ret;
}

int parse_e_direct(CPARSE_COMPONENT_ARGS)
{
    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value_first);
    CPARSE_COMPONENT_VAR(value_rest);
    CPARSE_COMPONENT_VAR(value_final);
    {
        /* first <- parserT */
        CPARSE_COMPONENT_BIND(value_first, cparse_native(parse_t));

        /* rest <- many parserE' */
        CPARSE_COMPONENT_BIND(value_rest, cparse_many(cparse_native(parse_e1)));

        /* return $ foldl (\f1 (op,f2) -> */
        /*                  case op of */
        /*                   '+' -> Add f1 f2 */
        /*                   '-' -> Sub f1 f2 */
        /*                   _ -> f1) */
        /*   f_first rest */
        value_final = cparse_functional_foldl( \
                parse_t_lambda_1, value_first, value_rest);

        CPARSE_COMPONENT_RETURN(value_final);
    }
    CPARSE_COMPONENT_END();
}


/* f1 :: String (op :: Char, f2 :: String) -> (Char, String) */
cparse_value_t *parse_e_lambda_1(cparse_value_t *a, cparse_value_t *b)
{
    return parse_t_and_e_lambda_1(a, b);
}

int parse_e1(CPARSE_COMPONENT_ARGS)
{
    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value_op);
    CPARSE_COMPONENT_VAR(value_f);
    CPARSE_COMPONENT_VAR(value_final);
    {
        /* op <- choice [char '+', char '-'] */
        CPARSE_COMPONENT_BIND(value_op, cparse_choice(cparse_char('+'), cparse_char('-'), NULL));

        /* f <- parserT */
        CPARSE_COMPONENT_BIND(value_f, cparse_native(parse_t));

        /* return (op, f) */
        value_final = cparse_value_new_tuple(value_op, value_f, NULL);

        CPARSE_COMPONENT_RETURN(value_final);
    }
    CPARSE_COMPONENT_END();
}

int parse_t(CPARSE_COMPONENT_ARGS)
{
    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value_first);
    CPARSE_COMPONENT_VAR(value_rest);
    CPARSE_COMPONENT_VAR(value_final);
    {
        /* first <- parserFactor */
        CPARSE_COMPONENT_BIND(value_first, cparse_native(parse_factor));

        /* rest <- many parserT' */
        CPARSE_COMPONENT_BIND(value_rest, cparse_many(cparse_native(parse_t1)));

        /* return $ foldl (\f1 (op,f2) -> */
        /*                  case op of */
        /*                   '*' -> Mul f1 f2 */
        /*                   '/' -> Div f1 f2 */
        /*                   _ -> f1) */
        /*   f_first rest */
        value_final = cparse_functional_foldl( \
                parse_t_lambda_1, value_first, value_rest);
        CPARSE_COMPONENT_RETURN(value_final);
    }
    CPARSE_COMPONENT_END();
}

/* f1 :: String (op :: Char, f2 :: String) -> (Char, String) */
cparse_value_t *parse_t_lambda_1(cparse_value_t *a, cparse_value_t *b)
{
    return parse_t_and_e_lambda_1(a, b);
}

int parse_t1(CPARSE_COMPONENT_ARGS)
{
    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value_op);
    CPARSE_COMPONENT_VAR(value_f);
    CPARSE_COMPONENT_VAR(value_final);
    {
        /* op <- choice [char '*', char '/'] */
        CPARSE_COMPONENT_BIND(value_op, cparse_choice(cparse_char('*'), cparse_char('/'), NULL));

        /* f <- parserFactor */
        CPARSE_COMPONENT_BIND(value_f, cparse_native(parse_factor));

        /* return (op, f) */
        value_final = cparse_value_new_tuple(value_op, value_f, NULL);
        CPARSE_COMPONENT_RETURN(value_final);
    }
    CPARSE_COMPONENT_END();
}

int parse_factor(CPARSE_COMPONENT_ARGS)
{
    CPARSE_COMPONENT_BEGIN();
    CPARSE_COMPONENT_VAR(value);
    {
        /* n <- munch1 isDigit */
        CPARSE_COMPONENT_BIND(value, cparse_munch1(parse_isdigit));

        /* return n */
        CPARSE_COMPONENT_RETURN(value);
    }
    CPARSE_COMPONENT_END();
}

