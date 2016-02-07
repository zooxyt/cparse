/* CParse : File Location
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_FILELOC_H
#define CPARSE_FILELOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* Data Type: File Location */
#ifndef DT_CPARSE_FILELOC_T
#define DT_CPARSE_FILELOC_T
typedef struct cparse_fileloc cparse_fileloc_t;
#endif

/* Data Type: File Location Pack */
#ifndef DT_CPARSE_FILELOC_PACK_T
#define DT_CPARSE_FILELOC_PACK_T
typedef struct cparse_fileloc_pack cparse_fileloc_pack_t;
#endif

cparse_fileloc_t *cparse_fileloc_new(char *filename, cparse_size_t ln);
void cparse_fileloc_destroy(cparse_fileloc_t *fileloc);
cparse_fileloc_t *cparse_fileloc_clone(cparse_fileloc_t *fileloc);
void *cparse_fileloc_cctor(void *fileloc);
void cparse_fileloc_dtor(void *dtor);

cparse_bool cparse_fileloc_eqp(void *p1, void *p2);


#ifdef __cplusplus
}
#endif

#endif

