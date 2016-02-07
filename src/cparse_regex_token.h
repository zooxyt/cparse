/* CParse : Regex : Token
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_REGEX_TOKEN_H
#define CPARSE_REGEX_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

typedef enum
{
    CPARSE_REGEX_TOKEN_TYPE_CHAR = 0,
} cparse_regex_token_type_t;

struct cparse_regex_token
{
    cparse_regex_token_type_t type;
    cparse_char_t ch;

    struct cparse_regex_token *next;
};

struct cparse_regex_token *cparse_regex_token_new( \
        cparse_regex_token_type_t type, \
        cparse_char_t ch);
void cparse_regex_token_destroy( \
        struct cparse_regex_token *token);
struct cparse_regex_token *cparse_regex_token_next( \
        struct cparse_regex_token *token);
cparse_regex_token_type_t cparse_regex_token_type( \
        struct cparse_regex_token *token);
cparse_char_t cparse_regex_token_char( \
        struct cparse_regex_token *token);

struct cparse_regex_token_list *cparse_regex_token_list_new(void);
void cparse_regex_token_list_destroy( \
        struct cparse_regex_token_list *list);
void cparse_regex_token_list_push_back( \
        struct cparse_regex_token_list *list, \
        struct cparse_regex_token *new_token);
struct cparse_regex_token *cparse_regex_token_list_first( \
        struct cparse_regex_token_list *list);


#ifdef __cplusplus
}
#endif



#endif

