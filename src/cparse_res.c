/* CParse : Resource
 * Copyright(c) Cheryl Natsu */

#include <stdlib.h>
#include "cparse_dt.h"
#include "cparse_res.h"

void *cparse_malloc(cparse_size_t size)
{
    return malloc(size);
}

void cparse_free(void *ptr)
{
    free(ptr);
}

