/* Test : All Failed */

#include <stdio.h>
#include "testfw.h"
#include "test_allfail.h"

static int eqp(int a, int b)
{
    return (a == b);
}

int test_allfailed(testfw_t *tfw)
{
    TESTFW_THIS(tfw);

    TESTFW_ASSERTM(0, "0 should be true");
    TESTFW_ASSERT(0);
    TESTFW_ASSERT_TRUE(0);
    TESTFW_ASSERT_FALSE(1);
    TESTFW_ASSERT_EQ(0, 1);
    TESTFW_ASSERT_NE(1, 1);
    TESTFW_ASSERT_EQP(0, 1, eqp);
    TESTFW_ASSERT_NEP(1, 1, eqp);

    return 0;
}

