/* CParse : Reader
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_READER_H
#define CPARSE_READER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_reader;

#define CPARSE_READER_STATE_STACK_SIZE_MAX (256)

/* Data Type: Parsing State */
#ifndef DT_CPARSE_READER_T
#define DT_CPARSE_READER_T
typedef struct cparse_reader cparse_reader_t;
#endif

/* Create and Destroy reader */
struct cparse_reader *cparse_reader_new_stringn_from_utf8(const char *s, const cparse_size_t len);
void cparse_reader_destroy(struct cparse_reader *reader);

/* State */
void cparse_reader_save(struct cparse_reader *reader);
void cparse_reader_load(struct cparse_reader *reader);
void cparse_reader_drop(struct cparse_reader *reader);
void cparse_reader_erase(struct cparse_reader *reader);

/* EOF/BOF */
cparse_bool cparse_reader_eof(struct cparse_reader *reader);
cparse_bool cparse_reader_bof(struct cparse_reader *reader);

/* Move */
int cparse_reader_forward(struct cparse_reader *reader);
int cparse_reader_forward_step(struct cparse_reader *reader, int step);

/* Peek Char */
int cparse_reader_peek_char(struct cparse_reader *reader);

/* Get Char */
int cparse_reader_get_char(struct cparse_reader *reader);
/* Get Position */
cparse_size_t cparse_reader_ln(struct cparse_reader *reader);
cparse_size_t cparse_reader_col(struct cparse_reader *reader);

/* Ptr */
cparse_char_t *cparse_reader_ptr(struct cparse_reader *reader);

#ifdef __cplusplus
}
#endif

#endif

