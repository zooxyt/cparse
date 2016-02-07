/* CParse : Data Types
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_DT_H
#define CPARSE_DT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus 
typedef enum
{
    cparse_false = 0,
    cparse_true = 1,
} cparse_bool;
#else
typedef enum
{
    cparse_false = false,
    cparse_true = true,
} cparse_bool;
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
#include <stdio.h>
typedef uint32_t cparse_u32;
typedef int32_t cparse_s32;
typedef uint16_t cparse_u16;
typedef int16_t cparse_s16;
typedef uint8_t cparse_u8;
typedef int8_t cparse_s8;
typedef size_t cparse_size_t;
typedef ssize_t cparse_ssize_t;
typedef uint32_t cparse_char_t;
#else
typedef unsigned int cparse_u32;
typedef unsigned short int cparse_u16;
typedef unsigned char cparse_u8;
typedef signed int cparse_s32;
typedef signed short int cparse_s16;
typedef char cparse_s8;
typedef unsigned int cparse_size_t;
typedef signed int cparse_ssize_t;
typedef unsigned int cparse_char_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef __cplusplus
}
#endif

#endif

