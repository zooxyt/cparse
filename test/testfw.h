/* Test Framework
 * Copyright(C) Cheryl Natsu */

#ifndef _TESTFW_H_
#define _TESTFW_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct testfw testfw_t;

testfw_t *testfw_new(void);
void testfw_destroy(testfw_t *tfw);
void testfw_assert_pass(testfw_t *tfw);
int testfw_assert_fail(testfw_t *tfw, char *__file__, int __line__, char *message);
void testfw_print_result(testfw_t *tfw);

#define TESTFW_THIS(tfw) \
    testfw_t *__this_tfw__ = (tfw); \
    do { \
        (void)__this_tfw__; \
    } while (0)

#define TESTFW_ASSERT_PASS() \
    do { \
        testfw_assert_pass(__this_tfw__); \
    } while (0)

#define TESTFW_ASSERT_FAIL() \
    do { \
        testfw_assert_fail(__this_tfw__, __FILE__, __LINE__, "Break Point"); \
    } while (0)

#define TESTFW_ASSERT_FAILM(message) \
    do { \
        testfw_assert_fail(__this_tfw__, __FILE__, __LINE__, message); \
    } while (0)

#define TESTFW_ASSERTM(expr, message) \
    do { \
        if (expr) { \
            testfw_assert_pass(__this_tfw__); \
        } else { \
            testfw_assert_fail(__this_tfw__, __FILE__, __LINE__, message); \
        } \
    } while (0)

#define TESTFW_ASSERT(expr) \
    TESTFW_ASSERTM(expr, #expr " is not true")

#define TESTFW_ASSERT_TRUE(expr) \
    TESTFW_ASSERTM(expr, #expr " is not true")

#define TESTFW_ASSERT_FALSE(expr) \
    TESTFW_ASSERTM(!expr, #expr " is not false")

#define TESTFW_ASSERT_EQ(expr1, expr2) \
    TESTFW_ASSERTM((expr1) == (expr2), #expr1 " not equals to " #expr2)

#define TESTFW_ASSERT_NE(expr1, expr2) \
    TESTFW_ASSERTM((expr1) != (expr2), #expr1 " equals to " #expr2)

#define TESTFW_ASSERT_EQP(expr1, expr2, eqp) \
    TESTFW_ASSERTM(eqp(expr1, expr2), #expr1 " not equals to " #expr2)

#define TESTFW_ASSERT_NEP(expr1, expr2, eqp) \
    TESTFW_ASSERTM(!eqp(expr1, expr2), #expr1 " equals to " #expr2)

#ifdef __cplusplus
}
#endif

#endif

