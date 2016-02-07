/* Arithmetic Expression Parser using CParse
 * Copyright(C) Cheryl Natsu */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "argsparse.h"
#include "cparse.h"
#include "arith_expr_syntax_direct.h"
#include "arith_expr_syntax_expr.h"
#include "arith_expr_ast_ctor.h"

typedef enum
{
    SYNTAX_FORMAT_NORMAL = 0,
    SYNTAX_FORMAT_JSON
} syntax_format_t;

typedef enum
{
    PARSER_TYPE_DIRECT = 0,
    PARSER_TYPE_EXPR
} parser_type_t;

int parse_arithmetic_expression(char *expression, \
        syntax_format_t syntax_format, \
        parser_type_t parser_type)
{
    int retval = 0;
    void *ast_ctor = ast_ctor_normal;
    void *ast_dtor = ast_dtor_normal;
    cparse_component_t *entry_point = NULL;
    cparse_t *cparse = NULL;
    cparse_err_t *err = NULL;
    union
    {
        ast_node_t *ast_node;
        char *json_str;
    } u;

    memset(&u, 0, sizeof(u));
    
    switch (syntax_format)
    {
        case SYNTAX_FORMAT_NORMAL:
            ast_ctor = ast_ctor_normal;
            ast_dtor = ast_dtor_normal;
            break;
        case SYNTAX_FORMAT_JSON:
            ast_ctor = cparse_json_ast_ctor;
            ast_dtor = cparse_json_ast_dtor;
            break;
    }

    /* Components */
    switch (parser_type)
    {
        case PARSER_TYPE_DIRECT:
            entry_point = cparse_native(parse_e_direct);
            break;
        case PARSER_TYPE_EXPR:
            entry_point = cparse_native(parse_e_expr);
            break;
    }

    /* Initialize parser */
    cparse = cparse_new();
    cparse_set_ast_ctor(cparse, ast_ctor);
    cparse_set_ast_dtor(cparse, ast_dtor);
    cparse_set_entry_point(cparse, entry_point);

    switch (syntax_format)
    {
        case SYNTAX_FORMAT_NORMAL:
            retval = cparse_parse(cparse, &err, (void **)&(u.ast_node), \
                    "repl", \
                    expression, strlen(expression));
            break;
        case SYNTAX_FORMAT_JSON:
            retval = cparse_parse(cparse, &err, (void **)&(u.ast_node), \
                    "repl", \
                    expression, strlen(expression));
            break;
    }

    if (retval != 0)
    {
        printf("error: failed to parse %s\n", expression);
        goto fail;
    }

    switch (syntax_format)
    {
        case SYNTAX_FORMAT_NORMAL:
            if (u.ast_node != NULL)
            {
                ast_node_print(u.ast_node);
                ast_dtor_normal(u.ast_node);
            }
            break;
        case SYNTAX_FORMAT_JSON:
            if (u.json_str != NULL)
            {
                printf("%s", u.json_str);
                free(u.json_str);
            }
            break;
    }
    printf("\n");

fail:
    if (err != NULL) cparse_err_destroy(err);
    if (entry_point != NULL) cparse_component_destroy(entry_point);
    cparse_destroy(cparse);

    return retval;
}

void show_help(void)
{
    const char *message = 
        "Arithmetic Expression Parser\n"
        "\n"
        " --expression\n"
        " --parser-type   [direct|expr]\n"
        " --syntax-format [normal|json]\n"
        "\n"
        " --help\n";
    puts(message);
}

int main(int argc, char *argv[])
{
    int retval = 0;
    argparse_t argsparse;
    char *expression = NULL;
    syntax_format_t syntax_format = SYNTAX_FORMAT_NORMAL;
    parser_type_t parser_type = PARSER_TYPE_DIRECT;

    argsparse_init(&argsparse, (int)argc, (char **)argv);

    while (argsparse_available(&argsparse) != 0)
    {
        if (argsparse_match_str(&argsparse, "--help"))
        { show_help(); goto done; }
        else if (argsparse_match_str(&argsparse, "--expression"))
        {
            argsparse_next(&argsparse);
            if (argsparse_request(&argsparse, &expression) != 0)
            { show_help(); goto done; }
        }
        else if (argsparse_match_str(&argsparse, "--syntax-format"))
        {
            argsparse_next(&argsparse);
            if (argsparse_available(&argsparse) == 0)
            { show_help(); goto done; }

            if (argsparse_match_str(&argsparse, "normal"))
            { syntax_format = SYNTAX_FORMAT_NORMAL; }
            else if (argsparse_match_str(&argsparse, "json"))
            { syntax_format = SYNTAX_FORMAT_JSON; }
            else
            { show_help(); goto done; }
        }
        else if (argsparse_match_str(&argsparse, "--parser-type"))
        {
            argsparse_next(&argsparse);
            if (argsparse_available(&argsparse) == 0)
            { show_help(); goto done; }

            if (argsparse_match_str(&argsparse, "direct"))
            { parser_type = PARSER_TYPE_DIRECT; }
            else if (argsparse_match_str(&argsparse, "expr"))
            { parser_type = PARSER_TYPE_EXPR; }
            else
            { show_help(); goto done; }
        }
        else
        { show_help(); goto done; }

        argsparse_next(&argsparse);
    }

    if (expression == NULL)
    { show_help(); goto done; }

    if ((retval = parse_arithmetic_expression( \
                    expression, syntax_format, parser_type)) != 0)
    { goto fail; }

    goto done;
fail:
done:
    return 0;
}

