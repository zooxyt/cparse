/* CParse : Resource
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_RES_H
#define CPARSE_RES_H

#ifdef __cplusplus
extern "C" {
#endif

void *cparse_malloc(cparse_size_t size);
void cparse_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif


