/* CParse : Char Encoding
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_CHARENC_H
#define CPARSE_CHARENC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

/* UTF-8 Related */

/* The bytes number from the leader char */  
cparse_size_t cparse_utf8_char_length(char ch);

/* The number of unicode chars of a given utf-8 byte stream */
cparse_size_t cparse_utf8_string_length(const char *s, cparse_size_t len);

/* Create a null terminated unicode string from utf-8 byte stream */
cparse_char_t *cparse_unicode_string_new_from_utf8(const char *s, cparse_size_t len);

/* The bytes number encoded in utf-8 of a unicode char */  
cparse_size_t cparse_utf8_char_length_of_unicode_char(cparse_char_t uch);

/* The bytes number encoded in utf-8 of a unicode string */  
cparse_size_t cparse_utf8_char_length_of_unicode_string(cparse_char_t *s, cparse_size_t len);

/* Create a null terminated utf-8 byte stream from Unicode string */
char *cparse_unicode_string_to_utf8(cparse_char_t *s, cparse_size_t len);

/* Null terminated string length */
cparse_size_t cparse_unicode_string_strlen(cparse_char_t *s);

int cparse_unicode_string_strncmp(const cparse_char_t *s1, const cparse_char_t *s2, cparse_size_t n);

cparse_char_t *cparse_unicode_string_memcpy(cparse_char_t *dst, cparse_char_t *src, cparse_size_t n);

cparse_char_t *cparse_unicode_string_duplicate(cparse_char_t *s);


#ifdef __cplusplus
}
#endif

#endif

