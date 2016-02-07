/* Test : Components */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "testfw.h"
#include "test_components.h"
#include "cparse.h"

#define TEST(tfw, component, source, expected_success, expected_output) \
    test(tfw, component, source, expected_success, expected_output, __LINE__)

static int test(testfw_t *tfw,                /* Test Framework */
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
    if (component != NULL) cparse_component_destroy(component);
    if (err != NULL) cparse_err_destroy(err);
    if (new_err_description_utf8 != NULL) free(new_err_description_utf8);
    cparse_destroy(cparse);

    return 0;
}

cparse_bool is_alpha(int ch)
{
    return ((('a'<=(ch))&&((ch)<='z'))||(('A'<=(ch))&&((ch)<='Z')));
}

int test_components(testfw_t *tfw)
{
    TESTFW_THIS(tfw);

    /* space */
    TEST(tfw, cparse_space(), " ", cparse_true, "{\"type\":\"char\",\"value\":\" \"}");
    TEST(tfw, cparse_space(), "a", cparse_false, "testcase:1:1: error: unexpected char \'a\'");
    TEST(tfw, cparse_space(), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    /* spaces */
    TEST(tfw, cparse_spaces(), "", cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_spaces(), " ", cparse_true, "{\"type\":\"string\",\"value\":\" \"}");
    TEST(tfw, cparse_spaces(), "  ", cparse_true, "{\"type\":\"string\",\"value\":\"  \"}");
    TEST(tfw, cparse_spaces(), "a", cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    /* endofline */
    TEST(tfw, cparse_endofline(), "\n", cparse_true, "{\"type\":\"char\",\"value\":\"\\n\"}");
    TEST(tfw, cparse_endofline(), "\r\n", cparse_true, "{\"type\":\"char\",\"value\":\"\\n\"}");
    TEST(tfw, cparse_endofline(), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_endofline(), "a", cparse_false, "testcase:1:1: error: unexpected char \'a\'");
    /* anychar */
    TEST(tfw, cparse_anychar(), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_anychar(), "a", cparse_true, "{\"type\":\"char\",\"value\":\"a\"}");
    TEST(tfw, cparse_anychar(), "ab", cparse_true, "{\"type\":\"char\",\"value\":\"a\"}");
    /* char */
    TEST(tfw, cparse_char('a'), "a", cparse_true, "{\"type\":\"char\",\"value\":\"a\"}");
    TEST(tfw, cparse_char('b'), "b", cparse_true, "{\"type\":\"char\",\"value\":\"b\"}");
    TEST(tfw, cparse_char('1'), "1", cparse_true, "{\"type\":\"char\",\"value\":\"1\"}");
    TEST(tfw, cparse_char('a'), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_char('a'), "b", cparse_false, "testcase:1:1: error: unexpected char \'b\'");
    /* string */
    TEST(tfw, cparse_string("a"), "a", cparse_true, "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_string("a"), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_string("ab"), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_string("ab"), "a", cparse_false, "testcase:1:1: error: unexpected char \'a\'");
    TEST(tfw, cparse_string("ab"), "ab", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_string("ab"), "abc", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_string("abc"), "abc", cparse_true, "{\"type\":\"string\",\"value\":\"abc\"}");
    TEST(tfw, cparse_string("abc"), "abcd", cparse_true, "{\"type\":\"string\",\"value\":\"abc\"}");
    /* oneof */
    TEST(tfw, cparse_oneof("abc"), "a", cparse_true, "{\"type\":\"char\",\"value\":\"a\"}");
    TEST(tfw, cparse_oneof("abc"), "b", cparse_true, "{\"type\":\"char\",\"value\":\"b\"}");
    TEST(tfw, cparse_oneof("abc"), "c", cparse_true, "{\"type\":\"char\",\"value\":\"c\"}");
    TEST(tfw, cparse_oneof("abc"), "d", cparse_false, "testcase:1:1: error: unexpected char \'d\'");
    /* noneof */
    TEST(tfw, cparse_noneof("abc"), "d", cparse_true, "{\"type\":\"char\",\"value\":\"d\"}");
    TEST(tfw, cparse_noneof("abc"), "e", cparse_true, "{\"type\":\"char\",\"value\":\"e\"}");
    TEST(tfw, cparse_noneof("abc"), "f", cparse_true, "{\"type\":\"char\",\"value\":\"f\"}");
    TEST(tfw, cparse_noneof("abc"), "a", cparse_false, "testcase:1:1: error: unexpected char \'a\'");
    TEST(tfw, cparse_noneof("abc"), "b", cparse_false, "testcase:1:1: error: unexpected char \'b\'");
    TEST(tfw, cparse_noneof("abc"), "c", cparse_false, "testcase:1:1: error: unexpected char \'c\'");
    /* many */
    TEST(tfw, cparse_many(cparse_char('a')), "", cparse_true, "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_many(cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_many(cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_many(cparse_string("ab")), "", cparse_true, "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_many(cparse_string("ab")), "ab", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"string\",\"value\":\"ab\"}]}");
    TEST(tfw, cparse_many(cparse_string("ab")), "abab", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"string\",\"value\":\"ab\"},{\"type\":\"string\",\"value\":\"ab\"}]}");
    /* many1 */
    TEST(tfw, cparse_many1(cparse_char('a')), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_many1(cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_many1(cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_many1(cparse_string("ab")), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_many1(cparse_string("ab")), "ab", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"string\",\"value\":\"ab\"}]}");
    TEST(tfw, cparse_many1(cparse_string("ab")), "abab", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"string\",\"value\":\"ab\"},{\"type\":\"string\",\"value\":\"ab\"}]}");
    /* munch */
    TEST(tfw, cparse_munch(is_alpha), "", cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_munch(is_alpha), "a", cparse_true, "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_munch(is_alpha), "ab", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_munch(is_alpha), "ab1", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_munch(is_alpha), "ab12", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    /* munch1 */
    TEST(tfw, cparse_munch1(is_alpha), "", cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_munch1(is_alpha), "a", cparse_true, "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_munch1(is_alpha), "ab", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_munch1(is_alpha), "ab1", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_munch1(is_alpha), "ab12", cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    /* choice */
    TEST(tfw, cparse_choice(cparse_char('a'), cparse_char('b'), NULL), "a", cparse_true, \
            "{\"type\":\"char\",\"value\":\"a\"}");
    TEST(tfw, cparse_choice(cparse_char('a'), cparse_char('b'), NULL), "b", cparse_true, \
            "{\"type\":\"char\",\"value\":\"b\"}");
    TEST(tfw, cparse_choice(cparse_char('a'), cparse_char('b'), NULL), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_choice(cparse_char('a'), cparse_char('b'), NULL), "c", cparse_false, \
            "testcase:1:1: error: unexpected char \'c\'");
    /* range */
    TEST(tfw, cparse_range(0, 0, cparse_char('a')), "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_range(0, 1, cparse_char('a')), "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_range(0, 1, cparse_char('a')), "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_range(0, 1, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(0, 1, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 1, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 1, cparse_char('a')), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_range(1, 1, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 1, cparse_char('a')), "aaa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 2, cparse_char('a')), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_range(1, 2, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 2, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range(1, 2, cparse_char('a')), "aaa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range_nongreed(1, 2, cparse_char('a')), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_range_nongreed(1, 2, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range_nongreed(1, 2, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_range_nongreed(1, 2, cparse_char('a')), "aaa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    /* count */
    TEST(tfw, cparse_count(0, cparse_char('a')), "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_count(0, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_count(1, cparse_char('a')), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_count(1, cparse_char('a')), "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_count(1, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_count(2, cparse_char('a')), "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_count(2, cparse_char('a')), "a", cparse_false, \
            "testcase:1:2: error: unexpected end of input");
    TEST(tfw, cparse_count(2, cparse_char('a')), "aa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_count(2, cparse_char('a')), "aaa", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* between */
    TEST(tfw, cparse_between( \
                cparse_char('('), cparse_char(')'), \
                cparse_char('a')), \
            "(a)", cparse_true, \
            "{\"type\":\"char\",\"value\":\"a\"}");
    /* option */
    TEST(tfw, cparse_option( \
                cparse_value_new_char('a'), \
                cparse_char('b')), \
            "a", cparse_true, \
            "{\"type\":\"char\",\"value\":\"a\"}");
    TEST(tfw, cparse_option( \
                cparse_value_new_char('a'), \
                cparse_char('b')), \
            "b", cparse_true, \
            "{\"type\":\"char\",\"value\":\"b\"}");
    /* sepBy */
    TEST(tfw, cparse_sepby(cparse_char('a'), cparse_char(',')), \
            "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_sepby(cparse_char('a'), cparse_char(',')), \
            "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby(cparse_char('a'), cparse_char(',')), \
            "a,", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby(cparse_char('a'), cparse_char(',')), \
            "a,a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby(cparse_char('a'), cparse_char(',')), \
            "a,a,", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* sepBy1 */
    TEST(tfw, cparse_sepby1(cparse_char('a'), cparse_char(',')), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_sepby1(cparse_char('a'), cparse_char(',')), \
            "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby1(cparse_char('a'), cparse_char(',')), \
            "a,", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby1(cparse_char('a'), cparse_char(',')), \
            "a,a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sepby1(cparse_char('a'), cparse_char(',')), \
            "a,a,", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* endBy */
    TEST(tfw, cparse_endby(cparse_char('a'), cparse_char(';')), \
            "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_endby(cparse_char('a'), cparse_char(';')), \
            "a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_endby(cparse_char('a'), cparse_char(';')), \
            "a;a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_endby(cparse_char('a'), cparse_char(';')), \
            "a;a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* endBy1 */
    TEST(tfw, cparse_endby1(cparse_char('a'), cparse_char(';')), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_endby1(cparse_char('a'), cparse_char(';')), \
            "a", cparse_false, \
            "testcase:1:2: error: unexpected end of input");
    TEST(tfw, cparse_endby1(cparse_char('a'), cparse_char(';')), \
            "a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_endby1(cparse_char('a'), cparse_char(';')), \
            "a;a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_endby1(cparse_char('a'), cparse_char(';')), \
            "a;a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* sepEndBy */
    TEST(tfw, cparse_sependby(cparse_char('a'), cparse_char(';')), \
            "", cparse_true, \
            "{\"type\":\"list\",\"value\":[]}");
    TEST(tfw, cparse_sependby(cparse_char('a'), cparse_char(';')), \
            "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby(cparse_char('a'), cparse_char(';')), \
            "a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby(cparse_char('a'), cparse_char(';')), \
            "a;a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby(cparse_char('a'), cparse_char(';')), \
            "a;a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* sepEndBy1 */
    TEST(tfw, cparse_sependby1(cparse_char('a'), cparse_char(';')), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_sependby1(cparse_char('a'), cparse_char(';')), \
            "a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby1(cparse_char('a'), cparse_char(';')), \
            "a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby1(cparse_char('a'), cparse_char(';')), \
            "a;a", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_sependby1(cparse_char('a'), cparse_char(';')), \
            "a;a;", cparse_true, \
            "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");
    /* manyTill */
    TEST(tfw, cparse_manytill(cparse_char('a'), cparse_char(';')), "", 
            cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_manytill(cparse_char('a'), cparse_char(';')), "a", \
            cparse_false, "testcase:1:2: error: unexpected end of input");
    TEST(tfw, cparse_manytill(cparse_char('a'), cparse_char(';')), "a;", \
            cparse_true, "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"}]}");
    TEST(tfw, cparse_manytill(cparse_char('a'), cparse_char(';')), "aa;", \
            cparse_true, "{\"type\":\"list\",\"value\":[{\"type\":\"char\",\"value\":\"a\"},{\"type\":\"char\",\"value\":\"a\"}]}");

    /* regex : seq */
    TEST(tfw, cparse_regex(""), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("a"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("a"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a"), \
            "ab", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("ab"), \
            "a", cparse_false, \
            "testcase:1:1: error: unexpected char 'a'"); /* Weired */
    TEST(tfw, cparse_regex("ab"), \
            "ab", cparse_true, \
            "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("ab"), \
            "abc", cparse_true, \
            "{\"type\":\"string\",\"value\":\"ab\"}");

    /* regex : special */
    TEST(tfw, cparse_regex("."), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("."), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("."), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("."), \
            "c", cparse_true, \
            "{\"type\":\"string\",\"value\":\"c\"}");
    TEST(tfw, cparse_regex("."), \
            "ad", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("."), \
            "bd", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("."), \
            "cd", cparse_true, \
            "{\"type\":\"string\",\"value\":\"c\"}");
    TEST(tfw, cparse_regex(".."), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex(".."), \
            "a", cparse_false, \
            "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex(".."), \
            "ab", cparse_true, \
            "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex(".."), \
            "abc", cparse_true, \
            "{\"type\":\"string\",\"value\":\"ab\"}");

    /* regex : group */
    TEST(tfw, cparse_regex("[a]"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[ab]"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[ab]"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[ab]"), \
            "c", cparse_false, \
            "testcase:1:1: error: unexpected char 'c'");
    TEST(tfw, cparse_regex("[a-z]"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "c", cparse_true, \
            "{\"type\":\"string\",\"value\":\"c\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "x", cparse_true, \
            "{\"type\":\"string\",\"value\":\"x\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "y", cparse_true, \
            "{\"type\":\"string\",\"value\":\"y\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "z", cparse_true, \
            "{\"type\":\"string\",\"value\":\"z\"}");
    TEST(tfw, cparse_regex("[a-z]"), \
            "0", cparse_false, \
            "testcase:1:1: error: unexpected char '0'");
    TEST(tfw, cparse_regex("[a-z]"), \
            "9", cparse_false, \
            "testcase:1:1: error: unexpected char '9'");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "0", cparse_true, \
            "{\"type\":\"string\",\"value\":\"0\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "1", cparse_true, \
            "{\"type\":\"string\",\"value\":\"1\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "9", cparse_true, \
            "{\"type\":\"string\",\"value\":\"9\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "z", cparse_true, \
            "{\"type\":\"string\",\"value\":\"z\"}");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            " ", cparse_false, \
            "testcase:1:1: error: unexpected char ' '");
    TEST(tfw, cparse_regex("[0-9a-z]"), \
            "!", cparse_false, \
            "testcase:1:1: error: unexpected char '!'");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "_", cparse_true, \
            "{\"type\":\"string\",\"value\":\"_\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "0", cparse_true, \
            "{\"type\":\"string\",\"value\":\"0\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "1", cparse_true, \
            "{\"type\":\"string\",\"value\":\"1\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "2", cparse_true, \
            "{\"type\":\"string\",\"value\":\"2\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "8", cparse_true, \
            "{\"type\":\"string\",\"value\":\"8\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "9", cparse_true, \
            "{\"type\":\"string\",\"value\":\"9\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[_0-9a-z]"), \
            "z", cparse_true, \
            "{\"type\":\"string\",\"value\":\"z\"}");
    TEST(tfw, cparse_regex("[^a]"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("[^a]"), \
            "a", cparse_false, \
            "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("[^a]"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[^ab]"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("[^ab]"), \
            "a", cparse_false, \
            "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("[^ab]"), \
            "b", cparse_false, \
            "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex("[^ab]"), \
            "c", cparse_true, \
            "{\"type\":\"string\",\"value\":\"c\"}");

    /* regex : branch */
    TEST(tfw, cparse_regex("(a)"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("(a)"), \
            "c", cparse_false, \
            "testcase:1:1: error: unexpected char 'c'");
    TEST(tfw, cparse_regex("(a)"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("(a|b)"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("(a|b)"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("(a|b)"), \
            "b", cparse_true, \
            "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("(a|b)"), \
            "c", cparse_false, \
            "testcase:1:1: error: unexpected char 'c'");
    TEST(tfw, cparse_regex("(a|ab)"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("(ab|a)"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("(ab|ac|a)"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");

    /* repeat limit */
    TEST(tfw, cparse_regex("a?"), \
            "", cparse_true, \
            "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("a?"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a?"), \
            "aa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a*"), \
            "", cparse_true, \
            "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("a*"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a*"), \
            "aa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aa\"}");
    TEST(tfw, cparse_regex("a*"), \
            "aaa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aaa\"}");
    TEST(tfw, cparse_regex("a+"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("a+"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a+"), \
            "aa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aa\"}");
    TEST(tfw, cparse_regex("a+"), \
            "aaa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aaa\"}");
    TEST(tfw, cparse_regex("a{1,2}"), \
            "", cparse_false, \
            "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("a{1,2}"), \
            "a", cparse_true, \
            "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("a{1,2}"), \
            "aa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aa\"}");
    TEST(tfw, cparse_regex("a{1,2}"), \
            "aaa", cparse_true, \
            "{\"type\":\"string\",\"value\":\"aa\"}");

    /* repeat limit sequence */
    TEST(tfw, cparse_regex("(ab)?"), "", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("(ab)?"), "a", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("(ab)?"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)*"), "", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("(ab)*"), "a", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("(ab)*"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)*"), "aba", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)*"), "abb", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)*"), "abab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"abab\"}");
    TEST(tfw, cparse_regex("(ab)+"), "", \
            cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("(ab)+"), "a", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("(ab)+"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)+"), "aba", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)+"), "abb", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("(ab)+"), "abab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"abab\"}");
    TEST(tfw, cparse_regex("[ab]?"), "", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("[ab]?"), "a", \
            cparse_true, "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[ab]?"), "b", \
            cparse_true, "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[ab]?"), "c", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("[ab]*"), "", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("[ab]*"), "a", \
            cparse_true, "{\"type\":\"string\",\"value\":\"a\"}");
    TEST(tfw, cparse_regex("[ab]*"), "b", \
            cparse_true, "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("[ab]*"), "c", \
            cparse_true, "{\"type\":\"string\",\"value\":\"\"}");
    TEST(tfw, cparse_regex("[ab]*"), "aa", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aa\"}");
    TEST(tfw, cparse_regex("[ab]*"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("[ab]*"), "ba", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ba\"}");
    TEST(tfw, cparse_regex("[ab]*"), "bb", \
            cparse_true, "{\"type\":\"string\",\"value\":\"bb\"}");
    TEST(tfw, cparse_regex("[ab]*"), "aac", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aa\"}");
    TEST(tfw, cparse_regex("[ab]*"), "abc", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("[ab]*"), "bac", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ba\"}");
    TEST(tfw, cparse_regex("[ab]*"), "bbc", \
            cparse_true, "{\"type\":\"string\",\"value\":\"bb\"}");
    TEST(tfw, cparse_regex("a[bc]"), "", \
            cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("a[bc]"), "a", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a[bc]"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a[bc]"), "ac", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ac\"}");
    TEST(tfw, cparse_regex("a[bc]"), "ad", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    /* backtrack */
    TEST(tfw, cparse_regex("a?b"), "", \
            cparse_false, "testcase:1:1: error: unexpected end of input");
    TEST(tfw, cparse_regex("a?b"), "a", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a?b"), "b", \
            cparse_true, "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex("a?b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a{1,2}b"), "b", \
            cparse_false, "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex("a{1,2}b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a{1,2}b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a{1,2}b"), "aaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,2}b"), "aaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,3}b"), "b", \
            cparse_false, "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex("a{1,3}b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a{1,3}b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a{1,3}b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a{1,3}b"), "aaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,3}b"), "aaaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");

    /* backtrack : lazy */
    TEST(tfw, cparse_regex("a{1,2}?b"), "b", \
            cparse_false, "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex("a{1,2}?b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a{1,2}?b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a{1,2}?b"), "aaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,2}?b"), "aaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,3}?b"), "b", \
            cparse_false, "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex("a{1,3}?b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a{1,3}?b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a{1,3}?b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a{1,3}?b"), "aaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a{1,3}?b"), "aaaaab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");

    /* backtrack : any */
    TEST(tfw, cparse_regex(".*b"), "b", \
            cparse_true, "{\"type\":\"string\",\"value\":\"b\"}");
    TEST(tfw, cparse_regex(".*b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex(".*b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex(".*b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a.*b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex("a.*b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a.*b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a.*b"), "aaaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaaab\"}");
    TEST(tfw, cparse_regex(".+b"), "b", \
            cparse_false, "testcase:1:1: error: unexpected char 'b'");
    TEST(tfw, cparse_regex(".+b"), "ab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"ab\"}");
    TEST(tfw, cparse_regex(".+b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex(".+b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a.+b"), "ab", \
            cparse_false, "testcase:1:1: error: unexpected char 'a'");
    TEST(tfw, cparse_regex("a.+b"), "aab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aab\"}");
    TEST(tfw, cparse_regex("a.+b"), "aaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaab\"}");
    TEST(tfw, cparse_regex("a.+b"), "aaaab", \
            cparse_true, "{\"type\":\"string\",\"value\":\"aaaab\"}");

    return 0;
}

