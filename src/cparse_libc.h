/* CParse : LibC
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_LIBC_H
#define CPARSE_LIBC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "cparse_dt.h"

void *cparse_memcpy(void *dest, const void *src, cparse_size_t n);
cparse_size_t cparse_strlen(const char *s);
void *cparse_memset(void *s, int c, cparse_size_t n);

int cparse_strcmp(const char *s1, const char *s2);
int cparse_vsprintf(char *str, const char *format, va_list ap);
int cparse_sprintf(char *str, const char *format, ...);

int cparse_strncmp(const char *s1, const char *s2, cparse_size_t n);
int cparse_vsnprintf(char *str, cparse_size_t size, const char *format, va_list ap);
int cparse_snprintf(char *str, cparse_size_t size, const char *format, ...);

const char *cparse_strnchr(const char *s, cparse_size_t n, int c);
const char *cparse_strnstrn(const char *haystack, const cparse_size_t haystack_len, \
        const char *needle, const cparse_size_t needle_len);
int cparse_atoin(int *number_out, const char *s, cparse_size_t n);

#ifdef __cplusplus
}
#endif

#endif

