/* CParse : Regex
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_reader.h"
#include "cparse_regex_token.h"
#include "cparse_regex_lex.h"
#include "cparse_regex_ast.h"
#include "cparse_regex_parser.h"
#include "cparse_regex_nfa.h"
#include "cparse_regex_converter.h"
#include "cparse_regex_match.h"
#include "cparse_regex.h"

struct cparse_regex
{
    struct cparse_regex_nfa *nfa;
};

cparse_regex_t *cparse_regex_new(cparse_char_t *pattern)
{
    cparse_regex_t *new_regex = NULL;
    struct cparse_regex_token_list *new_token_list = NULL;
    struct cparse_regex_ast *new_ast = NULL;
    struct cparse_regex_nfa *new_nfa = NULL;

    if ((new_regex = (cparse_regex_t *)cparse_malloc( \
                    sizeof(cparse_regex_t))) == NULL)
    { return NULL; }
    new_regex->nfa = NULL;

    if ((new_token_list = cparse_regex_lex(pattern)) == NULL)
    { goto fail; }
    if ((new_ast = cparse_regex_parse(new_token_list)) == NULL)
    { goto fail; }
    if ((new_nfa = cparse_regex_convert(new_ast)) == NULL)
    { goto fail; }
    new_regex->nfa = new_nfa; new_nfa = NULL;

    goto done;
fail:
    if (new_regex != NULL)
    {
        cparse_regex_destroy(new_regex);
        new_regex = NULL;
    }
done:
    if (new_token_list != NULL) cparse_regex_token_list_destroy(new_token_list);
    if (new_ast != NULL) cparse_regex_ast_destroy(new_ast);
    if (new_nfa != NULL) cparse_regex_nfa_destroy(new_nfa);
    return new_regex;
}

void cparse_regex_destroy(cparse_regex_t *regex)
{
    if (regex->nfa != NULL) cparse_regex_nfa_destroy(regex->nfa);
    cparse_free(regex);
}

int cparse_regex_match(cparse_regex_t *regex, cparse_state_t *state)
{
    return cparse_regex_match_nfa(regex->nfa, state);
}

struct cparse_regex_nfa *cparse_regex_nfa(cparse_regex_t *regex)
{
    return regex->nfa;
}

