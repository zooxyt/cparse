/* CParse : String
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_charenc.h"
#include "cparse_value_char.h"
#include "cparse_value_string.h"

struct cparse_value_string
{
    cparse_value_char_t *buf;
    cparse_size_t len;
};

cparse_value_string_t *cparse_value_string_new_wide(cparse_value_char_t *s, cparse_size_t len)
{
    cparse_value_string_t *new_string = NULL;
    cparse_size_t idx;

    if ((new_string = (cparse_value_string_t *)cparse_malloc( \
                    sizeof(cparse_value_string_t))) == NULL)
    { return NULL; }
    if ((new_string->buf = (cparse_value_char_t *)cparse_malloc( \
            sizeof(cparse_value_char_t) * (len + 1))) == NULL)
    { goto fail; }
    for (idx = 0; idx != len; idx++)
    { new_string->buf[idx] = s[idx]; }
    new_string->buf[len] = '\0';
    new_string->len = len;

    goto done;
fail:
    if (new_string != NULL)
    {
        cparse_value_string_destroy(new_string);
        new_string = NULL;
    }
done:
    return new_string;
}

cparse_value_string_t *cparse_value_string_new(char *s, cparse_size_t len)
{
    cparse_value_string_t *new_string = NULL;

    if ((new_string = (cparse_value_string_t *)cparse_malloc( \
                    sizeof(cparse_value_string_t))) == NULL)
    { return NULL; }

    if ((new_string->buf = cparse_unicode_string_new_from_utf8( \
                    s, len)) == NULL)
    { goto fail; }
    new_string->len = len;
    
    goto done;
fail:
    if (new_string != NULL)
    {
        cparse_value_string_destroy(new_string);
        new_string = NULL;
    }
done:
    return new_string;
}

void cparse_value_string_destroy(cparse_value_string_t *s)
{
    if (s->buf != NULL) cparse_free(s->buf);
    cparse_free(s);
}

void cparse_value_string_dtor(void *p)
{
    cparse_value_string_destroy(p);
}

cparse_value_string_t *cparse_value_string_clone(cparse_value_string_t *s)
{
    return cparse_value_string_new_wide( \
            cparse_value_string_str(s), \
            cparse_value_string_length(s));
}

void *cparse_value_string_cctor(void *p)
{
    return (void *)cparse_value_string_clone(p);
}

cparse_bool cparse_value_string_eqp(void *p1, void *p2)
{
    cparse_value_string_t *s1 = p1, *s2 = p2;
    if (s1->len != s2->len) return cparse_false;
    if (cparse_unicode_string_strncmp(s1->buf, s2->buf, s1->len) != 0) return cparse_false;
    return cparse_true;
}

cparse_value_char_t *cparse_value_string_str(cparse_value_string_t *s)
{
    return s->buf;
}

cparse_size_t cparse_value_string_length(cparse_value_string_t *s)
{
    return s->len;
}

