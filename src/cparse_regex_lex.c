/* CParse : Regex : Lexer
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_regex_token.h"
#include "cparse_regex_lex.h"

typedef enum
{
    CPARSE_REGEX_LEX_STATE_NORMAL = 0,
    CPARSE_REGEX_LEX_STATE_NORMAL_ESCAPE,
} cparse_regex_lex_state_t;

struct cparse_regex_token_list *cparse_regex_lex(cparse_char_t *pattern)
{
    cparse_char_t *p = pattern, ch;
    cparse_regex_lex_state_t state = CPARSE_REGEX_LEX_STATE_NORMAL;
    struct cparse_regex_token_list *new_list = NULL;
    struct cparse_regex_token *new_token = NULL;

    if ((new_list = cparse_regex_token_list_new()) == NULL)
    { return NULL; }

    while (*p != 0)
    {
        ch = *p;
        switch (state)
        {
            case CPARSE_REGEX_LEX_STATE_NORMAL:
                switch (ch)
                {
                    case '\\':
                        state = CPARSE_REGEX_LEX_STATE_NORMAL_ESCAPE;
                        break;
                    default:
                        if ((new_token = cparse_regex_token_new( \
                                        CPARSE_REGEX_TOKEN_TYPE_CHAR, ch)) == NULL)
                        { goto fail; }
                        cparse_regex_token_list_push_back(new_list, new_token);
                        new_token = NULL;
                        break;
                }
                break;
            case CPARSE_REGEX_LEX_STATE_NORMAL_ESCAPE:
                goto fail;
        }

        p++;
    }

    goto done;
fail:
    if (new_list != NULL)
    {
        cparse_regex_token_list_destroy(new_list);
        new_list = NULL;
    }
done:
    return new_list;
}

