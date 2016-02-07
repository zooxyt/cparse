/* Test Framework
 * Copyright(C) Cheryl Natsu */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "testfw.h"

struct testfw_fail_case
{
    char *__file__;
    int __line__;
    char *message;
    struct testfw_fail_case *next;
};

struct testfw_fail_case_group
{
    struct testfw_fail_case *begin, *end;
    size_t size;
};

struct testfw
{
    int passed;
    int total;

    struct testfw_fail_case_group *fail_case_group;
};

void testfw_fail_case_destroy(struct testfw_fail_case *tfwc);

struct testfw_fail_case *testfw_fail_case_new(char *__file__, int __line__, char *message)
{
    struct testfw_fail_case *new_fail_case = NULL;
    size_t __file__len = strlen(__file__);
    size_t message_len = strlen(message);

    if ((new_fail_case = (struct testfw_fail_case *)malloc( \
                    sizeof(struct testfw_fail_case))) == NULL)
    { return NULL; }
    new_fail_case->__file__ = NULL;
    new_fail_case->message = NULL;
    new_fail_case->__line__ = __line__;
    new_fail_case->next = NULL;

    if ((new_fail_case->__file__ = (char *)malloc( \
                    sizeof(char) * (__file__len + 1))) == NULL)
    { goto fail; }
    memcpy(new_fail_case->__file__, __file__, __file__len);
    new_fail_case->__file__[__file__len] = '\0';

    if ((new_fail_case->message = (char *)malloc( \
                    sizeof(char) * (message_len + 1))) == NULL)
    { goto fail; }
    memcpy(new_fail_case->message, message, message_len);
    new_fail_case->message[message_len] = '\0';

    goto done;
fail:
    if (new_fail_case != NULL)
    {
        testfw_fail_case_destroy(new_fail_case);
        new_fail_case = NULL;
    }
done:
    return new_fail_case;
}

void testfw_fail_case_destroy(struct testfw_fail_case *tfwc)
{
    if (tfwc->__file__ != NULL) free(tfwc->__file__);
    if (tfwc->message != NULL) free(tfwc->message);
    free(tfwc);
}

void testfw_fail_case_print(struct testfw_fail_case *tfwc)
{
    printf("Assertion Failed at %s:%d, %s\n", \
            tfwc->__file__, tfwc->__line__, tfwc->message);
}

struct testfw_fail_case_group *testfw_fail_case_group_new(void)
{
    struct testfw_fail_case_group *new_group = NULL;

    if ((new_group = (struct testfw_fail_case_group *)malloc( \
                    sizeof(struct testfw_fail_case_group))) == NULL)
    { return NULL; }
    new_group->begin = new_group->end = NULL;

    return new_group;
}

void testfw_fail_case_group_destroy(struct testfw_fail_case_group *tfwcg)
{
    struct testfw_fail_case *case_cur, *case_next;

    case_cur = tfwcg->begin;
    while (case_cur != NULL)
    {
        case_next = case_cur->next;
        testfw_fail_case_destroy(case_cur);
        case_cur = case_next;
    }
    free(tfwcg);
}

void testfw_fail_case_group_print(struct testfw_fail_case_group *tfwcg)
{
    struct testfw_fail_case *case_cur;

    case_cur = tfwcg->begin;
    while (case_cur != NULL)
    {
        testfw_fail_case_print(case_cur);
        case_cur = case_cur->next;
    }
}

void testfw_fail_case_group_push_back(struct testfw_fail_case_group *tfwcg, \
        struct testfw_fail_case *tfwc)
{
    if (tfwcg->begin == NULL)
    {
        tfwcg->begin = tfwcg->end = tfwc;
    }
    else
    {
        tfwcg->end->next = tfwc;
        tfwcg->end = tfwc;
    }
}

testfw_t *testfw_new(void)
{
    testfw_t *new_testfw = NULL;

    if ((new_testfw = (testfw_t *)malloc( \
                    sizeof(testfw_t))) == NULL)
    { return NULL; }

    if ((new_testfw->fail_case_group = testfw_fail_case_group_new()) == NULL)
    { goto fail; }
    new_testfw->passed = 0;
    new_testfw->total = 0;

    goto done;
fail:
    if (new_testfw != NULL)
    {
        testfw_destroy(new_testfw);
        new_testfw = NULL;
    }
done:
    return new_testfw;
}

void testfw_destroy(testfw_t *tfw)
{
    testfw_fail_case_group_destroy(tfw->fail_case_group);
    free(tfw);
}

void testfw_assert_pass(testfw_t *tfw)
{
    tfw->passed++;
    tfw->total++;
}

int testfw_assert_fail(testfw_t *tfw, char *__file__, int __line__, char *message)
{
    struct testfw_fail_case *new_fail_case = NULL;

    if ((new_fail_case = testfw_fail_case_new(__file__, __line__, message)) == NULL)
    { return -1; }
    testfw_fail_case_group_push_back(tfw->fail_case_group, \
            new_fail_case);
    tfw->total++;

    return 0;
}

void testfw_print_result(testfw_t *tfw)
{
    if (tfw->passed == tfw->total)
    {
        if (tfw->total == 0)
        {
            printf("No test case has been added\n");
        }
        else
        {
            printf("All %d test cases passed\n", tfw->total);
        }
    }
    else
    {
        printf("%d of %d test cases passed\n", tfw->passed, tfw->total);
        testfw_fail_case_group_print(tfw->fail_case_group);
    }
}

