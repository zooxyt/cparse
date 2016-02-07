/* CParse : Mutable String
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_MSTR_H
#define CPARSE_MSTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

#define CPARSE_MSTR_BUFFER_LEN 20
#define CPARSE_MSTR_INIT_CAPACITY 64
#define CPARSE_MSTR_EXTRA_CAPACITY 64

struct cparse_mstr;
typedef struct cparse_mstr cparse_mstr_t;

cparse_mstr_t *cparse_mstr_new(void);
void cparse_mstr_destroy(cparse_mstr_t *mstr);

int cparse_mstr_append_strn(cparse_mstr_t *mstr, cparse_char_t *s, cparse_size_t len);
int cparse_mstr_append_strn_from_utf8(cparse_mstr_t *mstr, char *s, cparse_size_t len);
int cparse_mstr_append_str(cparse_mstr_t *mstr, cparse_char_t *s);
int cparse_mstr_append_str_from_utf8(cparse_mstr_t *mstr, char *s);

cparse_char_t *cparse_mstr_str(cparse_mstr_t *mstr);
cparse_size_t cparse_mstr_len(cparse_mstr_t *mstr);
cparse_char_t *cparse_mstr_yield(cparse_mstr_t *mstr);


#ifdef __cplusplus
}
#endif

#endif

