/* CParse : Auto Release
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_AUTOREL_H
#define CPARSE_AUTOREL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* Data Type: Auto Release */
#ifndef DT_CPARSE_AUTO_RELEASE_T
#define DT_CPARSE_AUTO_RELEASE_T
typedef struct cparse_auto_release cparse_auto_release_t;
#endif

cparse_auto_release_t *cparse_auto_release_new(void);
void cparse_auto_release_destroy(cparse_auto_release_t *ar);
int cparse_auto_release_add(cparse_auto_release_t *ar, \
        void **ptr_of_ptr, void (*dtor)(void *ptr));

#ifdef __cplusplus
}
#endif

#endif

