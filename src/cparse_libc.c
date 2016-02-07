/* CParse : LibC
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_libc.h"
#include "cparse_res.h"
#include "cparse_charenc.h"

void *cparse_memcpy(void *dest, const void *src, cparse_size_t n)
{
    char *dest_p = dest;
    const char *src_p = src;
    while (n-- != 0)
    {
        *dest_p++ = *src_p++;
    }
    return dest;
}

cparse_size_t cparse_strlen(const char *s)
{
    cparse_size_t len = 0;
    const char *p = s;
    while (*p++ != '\0') len++;
    return len;
}

void *cparse_memset(void *s, int c, cparse_size_t n)
{
    char *p = s;
    while (n-- != 0)
    {
        *p++ = (char)c;
    }
    return s;
}

int cparse_strcmp(const char *s1, const char *s2)
{
    const char *p1 = s1, *p2 = s2;

    if ((p1 == NULL) || (p2 == NULL)) return -1;

    for (;;)
    {
        if ((*p1 != *p2) || (*p1 == '\0')) break;
        p1++; p2++;
    }
    return *p1 - *p2;
}

int cparse_strncmp(const char *s1, const char *s2, cparse_size_t n)
{
    const char *p1 = s1, *p2 = s2;
    
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
            return (unsigned char)*p1 - (unsigned char)*p2;
        }
    }
    return 0;
}

static int my_sprintf_int_width(int value)
{
    int len = 0;

    if (value == 0) return 1;
    else if (value < 0) 
    { value = -value; len++; }

    while (value != 0)
    { value /= 10; len++; } 

    return len;
}

static int my_sprintf_uint_width(unsigned int value)
{
    int len = 0;

    if (value == 0) return 1;

    while (value != 0)
    { value /= 10; len++; } 

    return len;
}

static int my_sprintf_fill_int(char *str_p, int value)
{
    int width, i;
    int sign = 0;

    if (value == 0)
    {
        *str_p++ = '0'; 
        width = 1;
        goto finish;
    }
    else if (value < 0) { sign = 1; value = -value; }

    width = my_sprintf_int_width(value);

    str_p += width + sign;
    for (i = 0; i != width; i++)
    {
        str_p--;
        *str_p = (char)((value % 10) + '0');
        value /= 10;
    }
    str_p--;
    if (sign != 0) *str_p = '-';

finish:

    return width + sign;
}

static int my_sprintf_fill_uint(char *str_p, unsigned int value)
{
    int width, i;

    width = my_sprintf_uint_width(value);

    str_p += width;
    for (i = 0; i != width; i++)
    {
        str_p--;
        *str_p = (char)((value % 10) + '0');
        value /= 10;
    }
    str_p--;

    return width;
}

static int my_sprintf_fill_str(char *str_p, char *s)
{
    cparse_size_t width;

    width = cparse_strlen(s);
    cparse_memcpy(str_p, s, width);

    return (int)width;
}

static int my_sprintf_fill_str_wide(char *str_p, cparse_char_t *s)
{
    cparse_size_t width;
    char *str_p_utf8 = NULL;
    cparse_size_t width_utf8;

    width = cparse_unicode_string_strlen(s);
    if ((str_p_utf8 = cparse_unicode_string_to_utf8(s, width)) == NULL)
    { return -1; }
    width_utf8 = cparse_strlen(str_p_utf8);
    cparse_memcpy(str_p, str_p_utf8, width_utf8);
    str_p[width_utf8] = '\0';

    cparse_free(str_p_utf8);

    return (int)width_utf8;
}

#define CPARSE_VSPRINTF_RAW_CHECK_SPACE(len) \
{ \
    if ((buffer_limit_enabled) && (buffer_remain_space < len)) \
    { return -1; } \
}

static int cparse_vsprintf_raw(char *str, const char *format, va_list ap, \
        cparse_bool buffer_limit_enabled, \
        cparse_size_t buffer_limit_size)
{
    int wrote_len = 0;
    char *str_p = str;
    const char *format_p = format;
    cparse_size_t buffer_remain_space = 0;
    union
    {
        int arg_value_char;
        int arg_value_int;
        unsigned int arg_value_uint;
        char *arg_value_str;
        cparse_char_t *arg_value_str_wide;
    } u;

    if (buffer_limit_enabled != cparse_false)
    {
        /* count '\0' in */
        buffer_remain_space = buffer_limit_size - 1; 
    }

    while (*format_p != '\0')
    {
        switch (*format_p)
        {
            case '%':
                format_p++;
                if (*format_p == '\0') goto finish;
                switch (*format_p)
                {
                    case 'c':
                        CPARSE_VSPRINTF_RAW_CHECK_SPACE(1);
                        u.arg_value_char = va_arg(ap, int);
                        *str_p = (char)(u.arg_value_char);
                        str_p += 1;
                        break;
                    case 'd':
                        u.arg_value_int = va_arg(ap, int);
                        CPARSE_VSPRINTF_RAW_CHECK_SPACE((cparse_size_t)my_sprintf_int_width(u.arg_value_int));
                        str_p += my_sprintf_fill_int(str_p, u.arg_value_int);
                        break;
                    case 'u':
                        u.arg_value_uint = va_arg(ap, unsigned int);
                        CPARSE_VSPRINTF_RAW_CHECK_SPACE((cparse_size_t)my_sprintf_uint_width(u.arg_value_uint));
                        str_p += my_sprintf_fill_uint(str_p, u.arg_value_uint);
                        break;
                    case 's':
                        u.arg_value_str = va_arg(ap, char *);
                        CPARSE_VSPRINTF_RAW_CHECK_SPACE(cparse_strlen(u.arg_value_str));
                        str_p += my_sprintf_fill_str(str_p, u.arg_value_str);
                        break;
                    case 'w':
                        u.arg_value_str_wide = va_arg(ap, cparse_char_t *);
                        str_p += my_sprintf_fill_str_wide(str_p, u.arg_value_str_wide);
                        break;
                    default:
                        break;
                }
                format_p++;
                break;

            default:
                CPARSE_VSPRINTF_RAW_CHECK_SPACE(1);
                *str_p++ = *format_p;
                format_p++;
                break;
        }
    }
finish:
    *str_p = '\0';
    wrote_len = (int)(str_p - str);
    return wrote_len;
}

int cparse_vsprintf(char *str, const char *format, va_list ap)
{
    return cparse_vsprintf_raw(str, format, ap, \
            cparse_false, \
            0);
}

int cparse_sprintf(char *str, const char *format, ...)
{
    int ret;
    va_list ap;

    va_start(ap, format);
    ret = cparse_vsprintf(str, format, ap);
    va_end(ap);

    return ret;
}

int cparse_vsnprintf(char *str, cparse_size_t size, const char *format, va_list ap)
{
    return cparse_vsprintf_raw(str, format, ap, \
            cparse_true, \
            size);
}

int cparse_snprintf(char *str, cparse_size_t size, const char *format, ...)
{
    int ret;
    va_list ap;

    va_start(ap, format);
    ret = cparse_vsnprintf(str, size, format, ap);
    va_end(ap);

    return ret;
}

const char *cparse_strnchr(const char *s, cparse_size_t n, int c)
{
    const char *p = s;
    while (n-- != 0)
    {
        if (*p == c) return p;
        p++;
    }
    return NULL;
}

const char *cparse_strnstrn(const char *haystack, const cparse_size_t haystack_len, \
        const char *needle, const cparse_size_t needle_len)
{
    const char *haystack_p = haystack;
    const char *haystack_subp;
    const char *needle_p;
    cparse_size_t needle_len_sub;
    cparse_size_t haystack_remain = (cparse_size_t)haystack_len;

    for (;;)
    {
        /* One Turn */
        if (haystack_remain < needle_len) return NULL;
        haystack_subp = haystack_p;
        needle_p = needle;
        needle_len_sub = needle_len;
        while (needle_len_sub-- != 0)
        {
            if (*needle_p != *haystack_subp)
            { /* Not Match */ goto next_turn; }
            needle_p++; haystack_subp++;
        }
        /* Match */
        return haystack_p;
next_turn:
        haystack_p++;
        haystack_remain--;
    }
}

int cparse_atoin(int *number_out, const char *s, cparse_size_t n)
{
    int value = 0;
    int negative = 0;
    const char *p = s;

    if (n == 0) return -1;

    if (*p == '-') 
    {
        p++;
        n--;
        negative = 1;
    }

    while (n-- != 0)
    {
        if (('0' <= *p)&&(*p <= '9'))
        { value = value * 10 + (*p - '0'); }
        else
        { return -1; }
        p++;
    }

    if (negative) value = -value;
    *number_out = value;

    return 0;
}

