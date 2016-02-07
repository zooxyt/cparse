/* CParse : Error
 * Copyright(c) Cheryl Natsu */

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_charenc.h"
#include "cparse_err.h"

cparse_err_t *cparse_err_new(void)
{
    cparse_err_t *new_err = NULL;

    if ((new_err = (cparse_err_t *)cparse_malloc( \
            sizeof(cparse_err_t))) == NULL)
    { return NULL; }
    new_err->occurred = cparse_false;
    new_err->num = CPARSE_ERR_OK;
    new_err->filename = NULL;
    new_err->description = NULL;

    return new_err;
}

void cparse_err_destroy(cparse_err_t *err)
{
    if (err->filename != NULL) cparse_free(err->filename);
    if (err->description != NULL) cparse_free(err->description);
    cparse_free(err);
}

int cparse_err_update_filename(cparse_err_t *err, char *filename)
{
    cparse_size_t filename_len = cparse_strlen(filename);
    char *new_filename = NULL;
    
    if ((new_filename = (char *)cparse_malloc( \
            sizeof(char) * (filename_len + 1))) == NULL)
    { return -1; }
    cparse_memcpy(new_filename, filename, filename_len);
    new_filename[filename_len] = '\0';

    if (err->filename != NULL) cparse_free(err->filename);
    err->filename = new_filename;

    return 0;
}

static int cparse_err_update_description_direct(cparse_err_t *err, cparse_char_t *description)
{
    err->num = CPARSE_ERR_PARSE;
    err->occurred = cparse_true;

    if (err->description != NULL) cparse_free(err->description);
    err->description = description;

    return 0;
}

int cparse_err_update_description_wide(cparse_err_t *err, cparse_char_t *description)
{
    cparse_size_t description_len = cparse_unicode_string_strlen(description);
    cparse_char_t *new_description = NULL;
    cparse_size_t idx;

    err->num = CPARSE_ERR_PARSE;
    err->occurred = cparse_true;

    if ((new_description = (cparse_char_t *)cparse_malloc( \
            sizeof(cparse_char_t) * (description_len + 1))) == NULL)
    { return -1; }
    for (idx = 0; idx != description_len; idx++)
    {
        new_description[idx] = description[idx];
    }
    new_description[description_len] = (cparse_char_t)('\0');

    if (err->description != NULL) cparse_free(err->description);
    err->description = new_description;

    return 0;
}

int cparse_err_update_description(cparse_err_t *err, char *description)
{
    cparse_char_t *new_str = cparse_unicode_string_new_from_utf8( \
            description, cparse_strlen(description));

    return cparse_err_update_description_direct(err, new_str);
}

int cparse_err_update_description_printf(cparse_err_t *err, char *fmt, ...)
{
    char buffer[CPARSE_ERR_DESCRIPTION_LEN_MAX];
    va_list ap;

    va_start(ap, fmt);
    cparse_vsnprintf(buffer, CPARSE_ERR_DESCRIPTION_LEN_MAX, fmt, ap);
    va_end(ap);
    return cparse_err_update_description(err, buffer);
}

/* Out of memory error */
int cparse_err_update_out_of_memory(cparse_err_t *err)
{
    err->occurred = cparse_true;
    err->num = CPARSE_ERR_MEM;
    return 0;
}

/* Internal */
int cparse_err_update_internal(cparse_err_t *err)
{
    err->occurred = cparse_true;
    err->num = CPARSE_ERR_INTERNAL;
    return 0;
}

/* Abstract */
int cparse_err_update_abstract(cparse_err_t *err)
{
    err->occurred = cparse_true;
    err->num = CPARSE_ERR_ABSTRACT;
    return 0;
}

cparse_bool cparse_err_occurred(cparse_err_t *err)
{
    return err->occurred;
}

char *cparse_err_filename(cparse_err_t *err)
{
    return err->filename;
}

/* If description given */
cparse_bool cparse_err_description_given(cparse_err_t *err)
{
    return (err->description != NULL) ? cparse_true : cparse_false;
}

/* pointer to description in 'cparse_char_t' type */
cparse_char_t *cparse_err_description_wide(cparse_err_t *err)
{
    return err->description;
}

/* pointer to description in 'u32' type */
void *cparse_err_description_u32(cparse_err_t *err)
{
    return (void *)cparse_err_description_wide(err);
}

/* generate description in 'utf8' byte stream */
char *cparse_err_description_to_utf8(cparse_err_t *err)
{
    if (err->description == NULL) return NULL;
    return cparse_unicode_string_to_utf8( \
            err->description, cparse_unicode_string_strlen(err->description));
}

