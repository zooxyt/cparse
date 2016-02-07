/* CParse : Mutable String
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_charenc.h"
#include "cparse_mstr.h"

struct cparse_mstr
{
    cparse_char_t *buf;
    cparse_size_t size;
    cparse_size_t capacity;
};

cparse_mstr_t *cparse_mstr_new(void)
{
    cparse_mstr_t *new_mstr = NULL;

    if ((new_mstr = (cparse_mstr_t *)cparse_malloc( \
                    sizeof(cparse_mstr_t))) == NULL)
    { return NULL; }
    new_mstr->size = 0;
    new_mstr->capacity = CPARSE_MSTR_INIT_CAPACITY;
    if ((new_mstr->buf = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (CPARSE_MSTR_INIT_CAPACITY + 1))) == NULL)
    { cparse_free(new_mstr); return NULL; }
    
    return new_mstr;
}

void cparse_mstr_destroy(cparse_mstr_t *mstr)
{
    if (mstr->buf != NULL) cparse_free(mstr->buf);
    cparse_free(mstr);
}

static int cparse_mstr_extend(cparse_mstr_t *mstr, \
        cparse_size_t new_capacity)
{
    cparse_char_t *new_buf = NULL;

    if ((new_buf = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (new_capacity + 1))) == NULL)
    { return -1; }
    cparse_unicode_string_memcpy(new_buf, mstr->buf, mstr->size);
    new_buf[mstr->size] = '\0';
    cparse_free(mstr->buf);
    mstr->buf = new_buf; new_buf = NULL;
    mstr->capacity = new_capacity;

    return 0;
}

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

int cparse_mstr_append_strn(cparse_mstr_t *mstr, \
        cparse_char_t *s, cparse_size_t len)
{
    if (mstr->size + len > mstr->capacity)
    {
        if (cparse_mstr_extend(mstr, \
                    mstr->size + MAX(len, CPARSE_MSTR_EXTRA_CAPACITY)) != 0)
        { return -1; }
    }
    cparse_unicode_string_memcpy(mstr->buf + mstr->size, s, len);
    mstr->buf[mstr->size + len] = '\0';
    mstr->size += len;

    return 0;
}

int cparse_mstr_append_strn_from_utf8(cparse_mstr_t *mstr, char *s, cparse_size_t len)
{
    cparse_char_t *new_unicode_string = NULL;
    cparse_size_t new_unicode_string_len;

    if ((new_unicode_string = cparse_unicode_string_new_from_utf8(s, len)) == NULL)
    { return -1; }
    new_unicode_string_len = cparse_unicode_string_strlen(new_unicode_string);
    cparse_mstr_append_strn(mstr, new_unicode_string, new_unicode_string_len);
    cparse_free(new_unicode_string);

    return 0;
}

int cparse_mstr_append_str(cparse_mstr_t *mstr, cparse_char_t *s)
{
    return cparse_mstr_append_strn(mstr, s, cparse_unicode_string_strlen(s));
}

int cparse_mstr_append_str_from_utf8(cparse_mstr_t *mstr, char *s)
{
    return cparse_mstr_append_strn_from_utf8(mstr, s, cparse_strlen(s));
}

cparse_char_t *cparse_mstr_str(cparse_mstr_t *mstr)
{
    return mstr->buf;
}

cparse_size_t cparse_mstr_len(cparse_mstr_t *mstr)
{
    return mstr->size;
}

cparse_char_t *cparse_mstr_yield(cparse_mstr_t *mstr)
{
    cparse_char_t *ret = mstr->buf;
    mstr->buf = NULL;
    return ret;
}

