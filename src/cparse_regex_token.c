/* CParse : Regex : Token
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_regex_token.h"

struct cparse_regex_token *cparse_regex_token_new( \
        cparse_regex_token_type_t type, \
        cparse_char_t ch)
{
    struct cparse_regex_token *new_token = NULL;

    if ((new_token = (struct cparse_regex_token *)cparse_malloc( \
                    sizeof(struct cparse_regex_token))) == NULL)
    { return NULL; }
    new_token->type = type;
    new_token->ch = ch;
    new_token->next = NULL;

    return new_token;
}

void cparse_regex_token_destroy(struct cparse_regex_token *token)
{
    cparse_free(token);
}

struct cparse_regex_token *cparse_regex_token_next( \
        struct cparse_regex_token *token)
{
    return token->next;
}

cparse_regex_token_type_t cparse_regex_token_type( \
        struct cparse_regex_token *token)
{
    return token->type;
}

cparse_char_t cparse_regex_token_char( \
        struct cparse_regex_token *token)
{
    return token->ch;
}

struct cparse_regex_token_list
{
    struct cparse_regex_token *begin, *end;
    cparse_size_t size;
};

struct cparse_regex_token_list *cparse_regex_token_list_new(void)
{
    struct cparse_regex_token_list *new_list = NULL;

    if ((new_list = (struct cparse_regex_token_list *)cparse_malloc( \
                    sizeof(struct cparse_regex_token_list))) == NULL)
    { return NULL; }
    new_list->begin = new_list->end = NULL;
    new_list->size = 0;

    return new_list;
}

void cparse_regex_token_list_destroy( \
        struct cparse_regex_token_list *list)
{
    struct cparse_regex_token *token_cur, *token_next;

    token_cur = list->begin;
    while (token_cur != NULL)
    {
        token_next = token_cur->next;
        cparse_regex_token_destroy(token_cur);
        token_cur = token_next;
    }
    cparse_free(list);
}

void cparse_regex_token_list_push_back( \
        struct cparse_regex_token_list *list, \
        struct cparse_regex_token *new_token)
{
    if (list->begin == NULL)
    {
        list->begin = list->end = new_token;
    }
    else
    {
        list->end->next = new_token;
        list->end = new_token;
    }
    list->size++;
}

struct cparse_regex_token *cparse_regex_token_list_first( \
        struct cparse_regex_token_list *list)
{
    return list->begin;
}

