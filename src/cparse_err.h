/* CParse : Error
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_ERR_H
#define CPARSE_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "cparse_dt.h"

typedef enum
{
    CPARSE_ERR_OK = 0,
    CPARSE_ERR_MEM,
    CPARSE_ERR_PARSE,
    CPARSE_ERR_INTERNAL,
    CPARSE_ERR_ABSTRACT,
} cparse_err_num_t;

#ifndef CPARSE_ERR_DESCRIPTION_LEN_MAX 
#define CPARSE_ERR_DESCRIPTION_LEN_MAX (1024)
#endif

struct cparse_err
{
    cparse_bool occurred;
    cparse_err_num_t num;
    char *filename;
    cparse_char_t *description;
};
typedef struct cparse_err cparse_err_t;
cparse_err_t *cparse_err_new(void);
void cparse_err_destroy(cparse_err_t *err);
int cparse_err_update_filename(cparse_err_t *err, char *filename);

/* Parsing error */
int cparse_err_update_description_wide(cparse_err_t *err, cparse_char_t *description);
int cparse_err_update_description(cparse_err_t *err, char *description);
int cparse_err_update_description_printf(cparse_err_t *err, char *fmt, ...);

/* Out of memory error */
int cparse_err_update_out_of_memory(cparse_err_t *err);
/* Internal */
int cparse_err_update_internal(cparse_err_t *err);
/* Abstract */
int cparse_err_update_abstract(cparse_err_t *err);

cparse_bool cparse_err_occurred(cparse_err_t *err);
char *cparse_err_filename(cparse_err_t *err);

/* If description given */
cparse_bool cparse_err_description_given(cparse_err_t *err);
/* pointer to description in 'cparse_char_t' type */
cparse_char_t *cparse_err_description_wide(cparse_err_t *err);
/* pointer to description in 'u32' type */
void *cparse_err_description_u32(cparse_err_t *err);
/* generate description in 'utf8' byte stream */
char *cparse_err_description_to_utf8(cparse_err_t *err);


#ifdef __cplusplus
}
#endif

#endif

