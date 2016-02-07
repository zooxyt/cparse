/* Test : Expression */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "testfw.h"
#include "test_expr.h"
#include "cparse.h"

#define TEST(tfw, component, source, expected_success, expected_output) \
    test2(tfw, component, source, expected_success, expected_output, __LINE__)

static int test2(testfw_t *tfw,                /* Test Framework */
        cparse_component_t *component, /* Component */
        char *source,                  /* Source to be parsed */
        cparse_bool expected_success,  /* If success */
        char *expected_output,         /* Expected output */
        cparse_size_t __line__)
{
    int retval = 0;
    char buffer[1024];
    cparse_t *cparse = NULL;
    char *json_str = NULL;
    cparse_size_t expected_output_len = strlen(expected_output);
    cparse_size_t json_str_len;
    cparse_char_t *err_description = NULL;
    char *new_err_description_utf8 = NULL;
    cparse_size_t err_description_utf8_len;
    cparse_err_t *err = NULL;

    TESTFW_THIS(tfw);

    if (component == NULL)
    {
        sprintf(buffer, "%d: failed creating component", \
                (int)__line__);
        TESTFW_ASSERT_FAILM(buffer);
        goto fail;
    }

    /* Initialize parser */
    cparse = cparse_new();
    cparse_set_ast_ctor(cparse, cparse_json_ast_ctor);
    cparse_set_ast_dtor(cparse, cparse_json_ast_dtor);
    cparse_set_entry_point(cparse, component);

    retval = cparse_parse(cparse, &err, (void **)&(json_str), \
            "testcase", \
            source, strlen(source));
    if (retval == 0)
    {
        /* Success */
        if (expected_success == cparse_true)
        {
            /* Matched */

            /* Compared the expected AST and produced AST */
            json_str_len = strlen(json_str);
            if ((expected_output_len == json_str_len) && \
                    (strncmp(expected_output, json_str, expected_output_len) == 0))
            { TESTFW_ASSERT_PASS(); }
            else
            {
                sprintf(buffer, \
                        "%d: " \
                        "unexpected AST: \n"
                        "  source:\n"
                        "    \"%s\"\n"
                        "  expected:\n"
                        "    %s\n"
                        "  produced:\n"
                        "    %s\n",
                        (int)__line__, \
                        source, expected_output, json_str);
                TESTFW_ASSERT_FAILM(buffer);
                goto fail;
            }
        }
        else
        {
            sprintf(buffer, "%d: expected failed but successed while parsing \"%s\"", \
                    (int)__line__, \
                    source);
            TESTFW_ASSERT_FAILM(buffer);
            goto fail;
        }
    }
    else
    {
        /* Failed */

        err_description = cparse_err_description_wide(err);
        if (err_description == NULL)
        {
            sprintf(buffer, \
                    "%d: " \
                    "failed parsing \"%s\" " \
                    "with no error description given", \
                    (int)__line__, \
                    source);
            TESTFW_ASSERT_FAILM(buffer);
            goto fail;
        }
        new_err_description_utf8 = cparse_err_description_to_utf8(err);
        err_description_utf8_len = strlen(new_err_description_utf8);

        if (expected_success == cparse_true)
        {
            sprintf(buffer, \
                    "%d: " \
                    "failed parsing \"%s\"\n" \
                    "  expected description:\n"
                    "    %s\n" \
                    "  given description:\n"
                    "    %s\n",\
                    (int)__line__, \
                    source, \
                    expected_output, \
                    new_err_description_utf8);
            TESTFW_ASSERT_FAILM(buffer);
            goto fail;
        }
        else
        {
            /* Compare fail message */ 
            if ((expected_output_len == err_description_utf8_len) && \
                    (strncmp(expected_output, new_err_description_utf8, expected_output_len) == 0))
            { TESTFW_ASSERT_PASS(); }
            else
            {
                sprintf(buffer, \
                        "%d: "
                        "failed parsing \"%s\"\n" \
                        "  expected description:\n"
                        "    %s\n" \
                        "  given description:\n"
                        "    %s\n",\
                        (int)__line__, \
                        source, \
                        expected_output, \
                        new_err_description_utf8);
                TESTFW_ASSERT_FAILM(buffer);
                goto fail;
            }
        }
    }


fail:
    if (json_str != NULL) { free(json_str); }
    if (err != NULL) cparse_err_destroy(err);
    if (new_err_description_utf8 != NULL) free(new_err_description_utf8);
    cparse_destroy(cparse);

    return 0;
}

static cparse_value_t *cparse_expr_tag_add(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Add", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_sub(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Sub", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_mul(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Mul", "left", a, "right", b, NULL); }

static cparse_value_t *cparse_expr_tag_div(cparse_value_t *a, cparse_value_t *b)
{ return cparse_value_new_struct("Div", "left", a, "right", b, NULL); }

cparse_component_t *arithmetic_expression_component_new(void)
{
    cparse_component_t *component = NULL;

    /* Arithmetic Expression Parsing Component */
    component = cparse_expr(2);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 1, "*", cparse_expr_tag_mul);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 1, "/", cparse_expr_tag_div);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 0, "+", cparse_expr_tag_add);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_LEFT, 0, "-", cparse_expr_tag_sub);
    cparse_component_as_expr_set_follow(component, cparse_regex("[0-9]+"));

    return component;
}

cparse_component_t *arithmetic_expression_component_right_associative_new(void)
{
    cparse_component_t *component = NULL;

    /* Arithmetic Expression Parsing Component */
    component = cparse_expr(2);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_RIGHT, 1, "*", cparse_expr_tag_mul);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_RIGHT, 1, "/", cparse_expr_tag_div);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_RIGHT, 0, "+", cparse_expr_tag_add);
    cparse_component_as_expr_append_infix_op(component, CPARSE_EXPR_ASSOCIATIVE_RIGHT, 0, "-", cparse_expr_tag_sub);
    cparse_component_as_expr_set_follow(component, cparse_regex("[0-9]+"));

    return component;
}

int test_expr_completeness(testfw_t *tfw)
{
    cparse_component_t *component = arithmetic_expression_component_new();

    TESTFW_THIS(tfw);

    TEST(tfw, component, "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, component, "a", cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, component, " ", cparse_false, "testcase:1:1: error: unexpected char ' '");
    TEST(tfw, component, "0", cparse_true, "{\"type\":\"string\",\"value\":\"0\"}");
    TEST(tfw, component, "1", cparse_true, "{\"type\":\"string\",\"value\":\"1\"}");
    TEST(tfw, component, "9", cparse_true, "{\"type\":\"string\",\"value\":\"9\"}");
    TEST(tfw, component, "123", cparse_true, "{\"type\":\"string\",\"value\":\"123\"}");
    TEST(tfw, component, "+", cparse_false, "testcase:1:1: error: unexpected char '+'");
    TEST(tfw, component, "-", cparse_false, "testcase:1:1: error: unexpected char '-'");
    TEST(tfw, component, "*", cparse_false, "testcase:1:1: error: unexpected char '*'");
    TEST(tfw, component, "/", cparse_false, "testcase:1:1: error: unexpected char '/'");
    TEST(tfw, component, "123+", cparse_true, "{\"type\":\"string\",\"value\":\"123\"}");
    TEST(tfw, component, "123-", cparse_true, "{\"type\":\"string\",\"value\":\"123\"}");
    TEST(tfw, component, "123*", cparse_true, "{\"type\":\"string\",\"value\":\"123\"}");
    TEST(tfw, component, "123/", cparse_true, "{\"type\":\"string\",\"value\":\"123\"}");
    TEST(tfw, component, "+123", cparse_false, "testcase:1:1: error: unexpected char '+'");
    TEST(tfw, component, "-123", cparse_false, "testcase:1:1: error: unexpected char '-'");
    TEST(tfw, component, "*123", cparse_false, "testcase:1:1: error: unexpected char '*'");
    TEST(tfw, component, "/123", cparse_false, "testcase:1:1: error: unexpected char '/'");

    if (component != NULL) cparse_component_destroy(component);

    return 0;
}

int test_expr_left_associative(testfw_t *tfw)
{
    cparse_component_t *component = arithmetic_expression_component_new();

    TESTFW_THIS(tfw);

    TEST(tfw, component, "123+456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123-456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123*456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123/456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123+456*", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123-456/", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123*456+", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123/456-", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    
    TEST(tfw, component, "1+2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1+2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1-2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1-2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");

    TEST(tfw, component, "1*2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1*2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");

    TEST(tfw, component, "1*2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1*2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");

    TEST(tfw, component, "1+2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1+2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");

    if (component != NULL) cparse_component_destroy(component);

    return 0;
}

int test_expr_right_associative(testfw_t *tfw)
{
    cparse_component_t *component = arithmetic_expression_component_right_associative_new();

    TESTFW_THIS(tfw);

    TEST(tfw, component, "123+456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123-456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123*456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123/456", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123+456*", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123-456/", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123*456+", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    TEST(tfw, component, "123/456-", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"123\"},\"right\":{\"type\":\"string\",\"value\":\"456\"}}}}");
    
    TEST(tfw, component, "1+2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1+2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");

    TEST(tfw, component, "1*2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1*2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1/2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1/2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");

    TEST(tfw, component, "1*2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1*2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2+3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");
    TEST(tfw, component, "1/2-3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"string\",\"value\":\"2\"}}}},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}");

    TEST(tfw, component, "1+2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1+2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Add\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2*3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Mul\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");
    TEST(tfw, component, "1-2/3", cparse_true, "{\"type\":\"struct\",\"value\":{\"name\":\"Sub\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"1\"},\"right\":{\"type\":\"struct\",\"value\":{\"name\":\"Div\",\"members\":{\"left\":{\"type\":\"string\",\"value\":\"2\"},\"right\":{\"type\":\"string\",\"value\":\"3\"}}}}}}}");

    if (component != NULL) cparse_component_destroy(component);

    return 0;
}

int test_expr(testfw_t *tfw)
{
    test_expr_completeness(tfw);
    test_expr_left_associative(tfw);
    test_expr_right_associative(tfw);

    return 0;
}

