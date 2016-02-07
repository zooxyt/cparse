/* CParse : Char Encoding
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_libc.h"
#include "cparse_res.h"
#include "cparse_charenc.h"


#define CPARSE_IS_UTF8_LEADER(ch) (((ch)&128)!=0?1:0)

/* The bytes number from the leader char */  
cparse_size_t cparse_utf8_char_length(char ch)
{
    cparse_size_t bytes_number;
    cparse_u8 uch = (cparse_u8)ch;
    /* 0xxxxxxx */
    if ((uch & 0x80) == 0) bytes_number = 1;
    /* 110xxxxx, 10xxxxxx */
    else if ((uch & 0xe0) == 0xc0) bytes_number = 2;
    /* 1110xxxx, 10xxxxxx, 10xxxxxx */
    else if ((uch & 0xf0) == 0xe0) bytes_number = 3;
    /* 11110xxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else if ((uch & 0xf8) == 0xf0) bytes_number = 4;
    /* 111110xx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else if ((uch & 0xfc) == 0xf8) bytes_number = 5;
    /* 1111110x, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else if ((uch & 0xfe) == 0xfc) bytes_number = 6;
    else bytes_number = 0;
    return bytes_number;
}

/* The number of unicode chars of a given utf-8 byte stream */
cparse_size_t cparse_utf8_string_length(const char *s, cparse_size_t len)
{
    const char *p = s;
    cparse_size_t utf8_char_len;
    cparse_size_t result = 0;

    while (len-- != 0)
    {
        utf8_char_len = 1;
        if (CPARSE_IS_UTF8_LEADER(*p))
        { utf8_char_len = cparse_utf8_char_length(*s); }

        s += len;
        result += utf8_char_len;
    }

    return result;
}

#define CPARSE_EAT_FROM_UTF_8_NEXT(str_p) \
    do { ++str_p; } while (0)
#define CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, str_p) \
    do { unicode_char = (unicode_char << 6) | ((*str_p) & 0x3f); str_p++; } while (0)

/* Create a null terminated unicode string from utf-8 byte stream */
cparse_char_t *cparse_unicode_string_new_from_utf8(const char *stream_s, cparse_size_t stream_len)
{
    cparse_char_t *new_body = NULL;
    cparse_char_t *unicode_p;
    cparse_char_t unicode_char;
    cparse_size_t unicode_len;
    const char *stream_p = stream_s;
    cparse_size_t unicode_char_bytes;
    char ch_first;

    unicode_len = cparse_utf8_string_length( \
                    stream_s, stream_len);

    if ((new_body = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (unicode_len + 1))) == NULL)
    { return NULL; }
    unicode_p = new_body;

    while (stream_len != 0)
    {
        ch_first = *stream_p++;
        unicode_char_bytes = cparse_utf8_char_length(ch_first);
        switch (unicode_char_bytes)
        {
            case 1:
                unicode_char = (cparse_char_t)ch_first & 0x7f;
                break;
            case 2:
                unicode_char = (cparse_char_t)ch_first & 0x1f;
                CPARSE_EAT_FROM_UTF_8_NEXT(stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                break;
            case 3:
                unicode_char = (cparse_char_t)ch_first & 0x1f;
                CPARSE_EAT_FROM_UTF_8_NEXT(stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                break;
            case 4:
                unicode_char = (cparse_char_t)ch_first & 0x1f;
                CPARSE_EAT_FROM_UTF_8_NEXT(stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                break;
            case 5:
                unicode_char = (cparse_char_t)ch_first & 0x1f;
                CPARSE_EAT_FROM_UTF_8_NEXT(stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                break;
            case 6:
                unicode_char = (cparse_char_t)ch_first & 0x1f;
                CPARSE_EAT_FROM_UTF_8_NEXT(stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                CPARSE_EAT_FROM_UTF_8_READ_FOLLOW(unicode_char, stream_p);
                break;
            default:
                goto fail;
        }
        stream_len -= unicode_char_bytes;
        *unicode_p++ = unicode_char;
    }
    *unicode_p = (cparse_char_t)('\0');

    goto done;
fail:
    if (new_body != NULL)
    {
        cparse_free(new_body);
        new_body = NULL;
    }
done:
    return new_body;
}

/* The bytes number encoded in utf-8 of a unicode char */  
cparse_size_t cparse_utf8_char_length_of_unicode_char(cparse_char_t uch)
{
    if (uch <= 0x7f) { return 1; }
    else if (uch <= 0x7ff) { return 2; }
    else if (uch <= 0xffff) { return 3; }
    else if (uch <= 0x1fffff) { return 4; }
    else if (uch <= 0x3ffffff) { return 5; }
    else if (uch <= 0x7fffffff) { return 6; }
    else return 0;
}

/* The bytes number encoded in utf-8 of a unicode string */  
cparse_size_t cparse_utf8_char_length_of_unicode_string(cparse_char_t *s, cparse_size_t len)
{
    cparse_size_t result = 0;

    while (len-- > 0)
    {
        result += cparse_utf8_char_length_of_unicode_char(*s);
        s++;
    }

    return result;
}

/* Create a null terminated utf-8 byte stream from Unicode string */
char *cparse_unicode_string_to_utf8(cparse_char_t *s, cparse_size_t len)
{
    cparse_size_t utf8_stream_len = cparse_utf8_char_length_of_unicode_string(s, len);
    cparse_char_t *p = s, ucs;
    char *new_string = NULL;
    char *dst_p;

    if ((new_string = (char *)cparse_malloc( \
                    sizeof(char) * (utf8_stream_len + 1))) == NULL)
    { return NULL; }
    dst_p = new_string;

    while (len-- != 0)
    { 
        ucs = *p;
        switch (cparse_utf8_char_length_of_unicode_char(*p))
        {
            case 1:
                *dst_p++ = (char)(ucs & 0x7f);
                break;
            case 2:
                *dst_p++ = (char)((0xc0) | ((ucs >> 6) & 0x1f));
                *dst_p++ = (char)((0x80) | (ucs & 0x3f));
                break;
            case 3:
                *dst_p++ = (char)((0xe0) | ((ucs >> 12) & 0xf));
                *dst_p++ = (char)((0x80) | ((ucs >> 6) & 0x3f));
                *dst_p++ = (char)((0x80) | (ucs & 0x3f));
                break;
            case 4:
                *dst_p++ = (char)((0xf0) | ((ucs >> 18) & 0x7));
                *dst_p++ = (char)((0x80) | ((ucs >> 12) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 6) & 0x3f));
                *dst_p++ = (char)((0x80) | (ucs & 0x3f));
            case 5:
                *dst_p++ = (char)((0xf8) | ((ucs >> 24) & 0x3));
                *dst_p++ = (char)((0x80) | ((ucs >> 18) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 12) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 6) & 0x3f));
                *dst_p++ = (char)((0x80) | (ucs & 0x3f));
                break;
            case 6:
                *dst_p++ = (char)((0xfc) | ((ucs >> 30) & 0x1));
                *dst_p++ = (char)((0x80) | ((ucs >> 24) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 18) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 12) & 0x3f));
                *dst_p++ = (char)((0x80) | ((ucs >> 6) & 0x3f));
                *dst_p++ = (char)((0x80) | (ucs & 0x3f));
                break;
            default:
                goto fail;
        }

        p++;
    }
    *dst_p = '\0';

    goto done;
fail:
    if (new_string != NULL)
    {
        cparse_free(new_string);
        new_string = NULL;
    }
done:
    return new_string;
}

/* Null terminated string length */
cparse_size_t cparse_unicode_string_strlen(cparse_char_t *s)
{
    cparse_size_t len = 0;

    while (*s++ != 0)
    {
        len++;
    }

    return len;
}

int cparse_unicode_string_strncmp(const cparse_char_t *s1, const cparse_char_t *s2, cparse_size_t n)
{
    const cparse_char_t *p1 = s1, *p2 = s2;
    
    if (n != 0)
    {
        do
        {
            if (*p1 != *p2++) break;
            if (*p1++ == '\0') return 0;
        }
        while (--n != 0);
        if (n > 0)
        {
            if (*p1 == '\0') return -1;
            if (*--p2 == '\0') return 1;
            return (int)*p1 - (int)*p2;
        }
    }
    return 0;
}

cparse_char_t *cparse_unicode_string_memcpy(cparse_char_t *dst, cparse_char_t *src, cparse_size_t n)
{
    cparse_char_t *dst_bak = dst;
    while (n-- != 0)
    { *dst++ = *src++; }
    return dst_bak;
}

cparse_char_t *cparse_unicode_string_duplicate(cparse_char_t *s)
{
    cparse_size_t len = cparse_unicode_string_strlen(s);
    cparse_char_t *new_string = NULL;
    cparse_size_t idx;
   
    if ((new_string = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (len + 1))) == NULL)
    { return NULL; }

    for (idx = 0; idx != len; idx++)
    {
        new_string[idx] = s[idx];
    }
    new_string[len] = (cparse_char_t)'\0';

    return new_string;
}

