#include <stdio.h>
#include "testfw.h"
#include "test_allfail.h"
#include "test_components.h"
#include "test_regex.h"
#include "test_expr.h"

int main(void)
{
    testfw_t *new_testfw = NULL;

    if ((new_testfw = testfw_new()) == NULL)
    { return -1; }

    /* test_allfailed(new_testfw); */
    test_components(new_testfw);
    test_regex(new_testfw);
    test_expr(new_testfw);

    testfw_print_result(new_testfw);
    testfw_destroy(new_testfw);

    return 0;
}

