/* Test : Regex */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "testfw.h"
#include "test_regex.h"
#include "cparse_component.h"
#include "cparse_regex.h"
#include "cparse_regex_nfa.h"

#define TEST_REGEX_STATES_COUNT(tfw, pattern, tests_count) \
    test_regex_states_count(tfw, pattern, tests_count, __LINE__)

int test_regex_states_count(testfw_t *tfw, \
        char *pattern, cparse_size_t tests_count, \
        cparse_size_t __line__)
{
    char buffer[1024];
    cparse_component_t *new_component = NULL;
    struct cparse_regex_nfa *nfa = NULL;

    TESTFW_THIS(tfw);

    new_component = cparse_regex(pattern);
    if (new_component == NULL)
    {
        sprintf(buffer, \
                "%d: "
                "failed compiling regex \"%s\"\n", \
                (int)__line__, \
                pattern);
        TESTFW_ASSERT_FAILM(buffer);
        goto fail;
    }
    nfa = cparse_regex_nfa(cparse_component_as_regex_get(new_component));
    TESTFW_ASSERT_EQ(tests_count, cparse_regex_nfa_states_count(nfa));

fail:
    if (new_component != NULL) cparse_component_destroy(new_component);
    return 0;
}

int test_regex(testfw_t *tfw)
{
    TESTFW_THIS(tfw);

    /* Seq */
    TEST_REGEX_STATES_COUNT(tfw, "", 1);
    TEST_REGEX_STATES_COUNT(tfw, "a", 2);
    TEST_REGEX_STATES_COUNT(tfw, "ab", 3);
    TEST_REGEX_STATES_COUNT(tfw, "abc", 4);

    /* Group */
    TEST_REGEX_STATES_COUNT(tfw, "[a]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[ab]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[a-z]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[a|z]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[0-9a-zA-Z]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[^0-9a-zA-Z]", 4);
    TEST_REGEX_STATES_COUNT(tfw, "[_0-9a-zA-Z]", 2);
    TEST_REGEX_STATES_COUNT(tfw, "[^_0-9a-zA-Z]", 4);

    /* Branch */
    TEST_REGEX_STATES_COUNT(tfw, "(a)", 2);
    TEST_REGEX_STATES_COUNT(tfw, "(a|b)", 2);
    TEST_REGEX_STATES_COUNT(tfw, "(a|b|c)", 2);
    TEST_REGEX_STATES_COUNT(tfw, "1(a|b|c)", 3);
    TEST_REGEX_STATES_COUNT(tfw, "1(a|b|c)2", 4);
    TEST_REGEX_STATES_COUNT(tfw, "(a|b)(c|d)", 3);

    return 0;
}

