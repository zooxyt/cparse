/* CParse : Regex : Parser
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_regex_token.h"
#include "cparse_regex_ast.h"
#include "cparse_regex_parser.h"

typedef enum
{
    CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_INIT = 0,

    /* Probably followed by dash to make range */
    CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_CHAR,

    /* Should folloed by a char */
    CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_DASH,
} cparse_regex_parse_ast_node_group_state_t;


#define CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur) \
    ((cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR) && \
     (cparse_regex_token_char(token_cur) == ']'))

#define CPARSE_REGEX_PARSE_AST_NODE_BRANCH_IS_EXIT(token_cur) \
    ((cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR) && \
     (cparse_regex_token_char(token_cur) == ')'))

#define CPARSE_REGEX_PARSE_AST_NODE_BRANCH_IS_ENDOFBRANCH(token_cur) \
    ((cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR) && \
     (cparse_regex_token_char(token_cur) == '|'))

#define CPARSE_REGEX_PARSE_IS_DIGIT(token_cur) \
    ((token_cur != NULL) && \
     ('0' <= cparse_regex_token_char(token_cur)) && \
     (cparse_regex_token_char(token_cur) <= '9'))

#define CPARSE_REGEX_PARSE_IS_COMMA(token_cur) \
    ((token_cur != NULL) && \
     (cparse_regex_token_char(token_cur) == ','))

#define CPARSE_REGEX_PARSE_IS_CLOSE_CURLY(token_cur) \
    ((token_cur != NULL) && \
     (cparse_regex_token_char(token_cur) == '}'))

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_seq( \
        struct cparse_regex_token **token_cur_io, \
        int depth);

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_group( \
        struct cparse_regex_token **token_cur_io)
{
    cparse_regex_parse_ast_node_group_state_t state = CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_INIT;
    struct cparse_regex_token *token_cur = *token_cur_io;
    struct cparse_regex_ast_node *new_ast_node = NULL;
    struct cparse_regex_ast_node_group *new_ast_node_group = NULL;
    struct cparse_regex_ast_node_group_node *new_ast_node_group_node = NULL;
    cparse_bool exclude = cparse_false;
    cparse_char_t ch = 0, ch2 = 0;

    /* Skip '[' */
    token_cur = cparse_regex_token_next(token_cur);

    /* Empty Group */
    if ((token_cur == NULL) || CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur))
    { return NULL; }

    /* Exclude */
    if ((cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR) && \
            (cparse_regex_token_char(token_cur) == '^'))
    {
        exclude = cparse_true;
        token_cur = cparse_regex_token_next(token_cur);
    }

    /* Empty Group */
    if ((token_cur == NULL) || CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur))
    { return NULL; }

    if ((new_ast_node_group = cparse_regex_ast_node_group_new(exclude)) == NULL)
    { goto fail; }

    while (token_cur != NULL)
    {
        switch (state)
        {
            case CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_INIT:
                /* Exit when meet ']' */
                if (CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur))
                {
                    /* Skip ']' */
                    token_cur = cparse_regex_token_next(token_cur);
                    goto finish;
                }
                else if (cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR)
                {
                    ch = cparse_regex_token_char(token_cur);
                    token_cur = cparse_regex_token_next(token_cur);
                    state = CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_CHAR;
                }
                else
                { goto fail; }

                break;

            case CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_CHAR:
                /* Exit when meet ']' */
                if (CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur))
                {
                    /* Skip ']' */
                    token_cur = cparse_regex_token_next(token_cur);

                    /* Append the char scanned in the previous loop */
                    if ((new_ast_node_group_node = cparse_regex_ast_node_group_node_new_solo(ch)) == NULL)
                    { goto fail; }
                    cparse_regex_ast_node_group_push_back( \
                            new_ast_node_group, \
                            new_ast_node_group_node);
                    new_ast_node_group_node = NULL;

                    goto finish;
                }
                else if (cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR)
                {
                    if (cparse_regex_token_char(token_cur) == '-')
                    {
                        token_cur = cparse_regex_token_next(token_cur);
                        state = CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_DASH;
                    }
                    else
                    {
                        /* Append the char scanned in the previous loop */
                        if ((new_ast_node_group_node = cparse_regex_ast_node_group_node_new_solo(ch)) == NULL)
                        { goto fail; }
                        cparse_regex_ast_node_group_push_back( \
                                new_ast_node_group, \
                                new_ast_node_group_node);
                        new_ast_node_group_node = NULL;

                        /* Read the current char */
                        ch = cparse_regex_token_char(token_cur);
                        token_cur = cparse_regex_token_next(token_cur);

                        /* No need to change state */
                    }
                }
                else
                { goto fail; }

                break;

            case CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_DASH:
                if (CPARSE_REGEX_PARSE_AST_NODE_GROUP_IS_EXIT(token_cur))
                { goto fail; }
                else if (cparse_regex_token_type(token_cur) == CPARSE_REGEX_TOKEN_TYPE_CHAR)
                {
                    ch2 = cparse_regex_token_char(token_cur);
                    token_cur = cparse_regex_token_next(token_cur);

                    /* Inverted range */
                    if (ch > ch2) { goto fail; }

                    /* Append the char scanned in the previous loop */
                    if ((new_ast_node_group_node = cparse_regex_ast_node_group_node_new_range(ch, ch2)) == NULL)
                    { goto fail; }
                    cparse_regex_ast_node_group_push_back( \
                            new_ast_node_group, \
                            new_ast_node_group_node);
                    new_ast_node_group_node = NULL;

                    state = CPARSE_REGEX_PARSE_AST_NODE_GROUP_STATE_INIT;
                }
                else
                { goto fail; }

                break;
        }
    }

finish:
    if ((new_ast_node = cparse_regex_ast_node_new_group(new_ast_node_group)) == NULL)
    { goto fail; }
    new_ast_node_group = NULL;

    goto done;
fail:
    if (new_ast_node_group_node != NULL)
    { cparse_regex_ast_node_group_node_destroy(new_ast_node_group_node); }
    if (new_ast_node_group != NULL)
    { cparse_regex_ast_node_group_destroy(new_ast_node_group); }
    if (new_ast_node != NULL)
    {
        cparse_regex_ast_node_destroy(new_ast_node); 
        new_ast_node = NULL;
    }
done:
    *token_cur_io = token_cur;
    return new_ast_node;
}

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_branch( \
        struct cparse_regex_token **token_cur_io, \
        int depth)
{
    struct cparse_regex_token *token_cur = *token_cur_io;
    struct cparse_regex_ast_node *new_ast_node = NULL;
    struct cparse_regex_ast_node_branch *new_ast_node_branch = NULL;
    struct cparse_regex_ast_node_branch_node *new_ast_node_branch_node = NULL;

    if ((new_ast_node_branch = cparse_regex_ast_node_branch_new()) == NULL)
    { return NULL; }

    /* Skip '(' */
    token_cur = cparse_regex_token_next(token_cur);

    while (token_cur != NULL)
    {
        /* Parse as a sequence */
        new_ast_node = cparse_regex_parse_ast_node_seq(&token_cur, depth);
        if (new_ast_node == NULL) { goto fail; }

        if (token_cur == NULL)
        { goto finish; }

        if (CPARSE_REGEX_PARSE_AST_NODE_BRANCH_IS_EXIT(token_cur))
        {
            /* Skip ')' */
            token_cur = cparse_regex_token_next(token_cur);

            if ((new_ast_node_branch_node = cparse_regex_ast_node_branch_node_new(new_ast_node)) == NULL)
            { goto fail; }
            new_ast_node = NULL;
            cparse_regex_ast_node_branch_push_back(new_ast_node_branch, new_ast_node_branch_node);
            new_ast_node_branch_node = NULL;
            goto finish;
        }
        else if (CPARSE_REGEX_PARSE_AST_NODE_BRANCH_IS_ENDOFBRANCH(token_cur))
        {
            /* Skip '|' */
            token_cur = cparse_regex_token_next(token_cur);

            if ((new_ast_node_branch_node = cparse_regex_ast_node_branch_node_new(new_ast_node)) == NULL)
            { goto fail; }
            new_ast_node = NULL;
            cparse_regex_ast_node_branch_push_back(new_ast_node_branch, new_ast_node_branch_node);
            new_ast_node_branch_node = NULL;
        }
        else { goto fail; }
    }

finish:
    if ((new_ast_node = cparse_regex_ast_node_new_branch(new_ast_node_branch)) == NULL)
    { goto fail; }
    new_ast_node_branch = NULL;

    goto done;
fail:
    if (new_ast_node_branch_node != NULL)
    { cparse_regex_ast_node_branch_node_destroy(new_ast_node_branch_node); }
    if (new_ast_node_branch != NULL)
    { cparse_regex_ast_node_branch_destroy(new_ast_node_branch); }
    if (new_ast_node != NULL)
    {
        cparse_regex_ast_node_destroy(new_ast_node); 
        new_ast_node = NULL;
    }
done:
    *token_cur_io = token_cur;
    return new_ast_node;
}

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_any( \
        struct cparse_regex_token **token_cur_io)
{
    struct cparse_regex_token *token_cur = *token_cur_io;
    struct cparse_regex_ast_node *new_ast_node = NULL;

    if ((new_ast_node = cparse_regex_ast_node_new_any()) == NULL)
    { goto fail; }
    token_cur = cparse_regex_token_next(token_cur);

fail:
    *token_cur_io = token_cur;
    return new_ast_node;
}

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_char( \
        struct cparse_regex_token **token_cur_io)
{
    struct cparse_regex_token *token_cur = *token_cur_io;
    struct cparse_regex_ast_node *new_ast_node = NULL;

    if ((new_ast_node = cparse_regex_ast_node_new_char( \
                    cparse_regex_token_char(token_cur))) == NULL)
    { goto fail; }
    token_cur = cparse_regex_token_next(token_cur);

fail:
    *token_cur_io = token_cur;
    return new_ast_node;
}

static int cparse_regex_parse_ast_node_repeat_limit( \
        struct cparse_regex_ast_node *ast_node, \
        struct cparse_regex_token **token_cur_io)
{
    int ret = 0;
    int value = 0;
    struct cparse_regex_token *token_cur = *token_cur_io;

    if (token_cur == NULL) goto done;
    if (token_cur->type != CPARSE_REGEX_TOKEN_TYPE_CHAR) goto done;

    if (token_cur->ch == '+')
    {
        cparse_regex_ast_node_set_low_bound(ast_node, 1);
        cparse_regex_ast_node_set_high_bound(ast_node, CPARSE_REGEX_AST_NODE_HIGH_BOUND_UNLIMITED);
        token_cur = cparse_regex_token_next(token_cur);
    }
    else if (token_cur->ch == '*')
    {
        cparse_regex_ast_node_set_low_bound(ast_node, 0);
        cparse_regex_ast_node_set_high_bound(ast_node, CPARSE_REGEX_AST_NODE_HIGH_BOUND_UNLIMITED);
        token_cur = cparse_regex_token_next(token_cur);
    }
    else if (token_cur->ch == '?')
    {
        cparse_regex_ast_node_set_low_bound(ast_node, 0);
        cparse_regex_ast_node_set_high_bound(ast_node, 1);
        token_cur = cparse_regex_token_next(token_cur);
    }
    else if (token_cur->ch == '{')
    {
        token_cur = cparse_regex_token_next(token_cur);
        if (token_cur == NULL) goto fail;

        /* low bound */
        value = 0;
        while (CPARSE_REGEX_PARSE_IS_DIGIT(token_cur))
        {
            value = value * 10 + ((int)cparse_regex_token_char(token_cur) - '0');
            token_cur = cparse_regex_token_next(token_cur);
        }
        cparse_regex_ast_node_set_low_bound(ast_node, value);

        if (CPARSE_REGEX_PARSE_IS_CLOSE_CURLY(token_cur))
        {
            /* {number} */
            cparse_regex_ast_node_set_high_bound(ast_node, value);
            token_cur = cparse_regex_token_next(token_cur);
        }
        else if (CPARSE_REGEX_PARSE_IS_COMMA(token_cur))
        {
            /* {number, */
            token_cur = cparse_regex_token_next(token_cur);

            /* high bound */
            value = 0;
            while (CPARSE_REGEX_PARSE_IS_DIGIT(token_cur))
            {
                value = value * 10 + ((int)cparse_regex_token_char(token_cur) - '0');
                token_cur = cparse_regex_token_next(token_cur);
            }
            cparse_regex_ast_node_set_high_bound(ast_node, value);

            if (CPARSE_REGEX_PARSE_IS_CLOSE_CURLY(token_cur))
            {
                token_cur = cparse_regex_token_next(token_cur);
            }
            else
            { goto fail; }
        }
        else
        { goto fail; }
    }

    goto done;
fail:
done:
    *token_cur_io = token_cur;
    return ret;
}

static int cparse_regex_parse_ast_node_greedy( \
        struct cparse_regex_ast_node *ast_node, \
        struct cparse_regex_token **token_cur_io)
{
    int ret = 0;
    struct cparse_regex_token *token_cur = *token_cur_io;

    if (token_cur == NULL) goto done;
    if (token_cur->type != CPARSE_REGEX_TOKEN_TYPE_CHAR) goto done;

    if (token_cur->ch == '?')
    {
        /* Lazy */
        token_cur = cparse_regex_token_next(token_cur);
        cparse_regex_ast_node_set_greed(ast_node, cparse_false);
    }

done:
    *token_cur_io = token_cur;
    return ret;
}

static struct cparse_regex_ast_node *cparse_regex_parse_ast_node_seq( \
        struct cparse_regex_token **token_cur_io, \
        int depth)
{
    struct cparse_regex_token *token_cur = *token_cur_io;
    struct cparse_regex_ast_node_seq *new_ast_node_seq = NULL;
    struct cparse_regex_ast_node_seq_node *new_ast_node_seq_node = NULL;
    struct cparse_regex_ast_node *new_ast_node = NULL;

    if ((new_ast_node_seq = cparse_regex_ast_node_seq_new()) == NULL)
    { return NULL; }

    for (;;)
    {
        if (token_cur == NULL) break;
        switch (cparse_regex_token_char(token_cur))
        {
            case '[':
                if ((new_ast_node = cparse_regex_parse_ast_node_group(&token_cur)) == NULL) { goto fail; }
                break;
            case '(':
                if ((new_ast_node = cparse_regex_parse_ast_node_branch(&token_cur, depth + 1)) == NULL) { goto fail; }
                break;
            case '|':
            case ')':
                if (depth > 0) { goto finish; }
                else { goto fail; }
            case '.':
                if ((new_ast_node = cparse_regex_parse_ast_node_any(&token_cur)) == NULL) { goto fail; }
                break;
            default:
                if ((new_ast_node = cparse_regex_parse_ast_node_char(&token_cur)) == NULL) { goto fail; }
                break;
        }

        /* Matching Repeat Limit */
        if (cparse_regex_parse_ast_node_repeat_limit(new_ast_node, &token_cur) != 0) 
        { goto fail; }
        /* Greedy */
        if (cparse_regex_parse_ast_node_greedy(new_ast_node, &token_cur) != 0) 
        { goto fail; }

        /* Append to the current sequence */
        if ((new_ast_node_seq_node = cparse_regex_ast_node_seq_node_new(new_ast_node)) == NULL)
        { return NULL; }
        new_ast_node = NULL;
        cparse_regex_ast_node_seq_push_back(new_ast_node_seq, new_ast_node_seq_node);
        new_ast_node_seq_node = NULL;
    }

finish:

    if ((new_ast_node = cparse_regex_ast_node_new_seq( \
                    new_ast_node_seq)) == NULL)
    { goto fail; }
    new_ast_node_seq = NULL;

    goto done;
fail:
    if (new_ast_node_seq != NULL) 
    { cparse_regex_ast_node_seq_destroy(new_ast_node_seq); }
    if (new_ast_node_seq_node != NULL) 
    { cparse_regex_ast_node_seq_node_destroy(new_ast_node_seq_node); }
    if (new_ast_node != NULL)
    {
        cparse_regex_ast_node_destroy(new_ast_node);
        new_ast_node = NULL;
    }
done:
    *token_cur_io = token_cur;
    return new_ast_node;
}

struct cparse_regex_ast *cparse_regex_parse( \
        struct cparse_regex_token_list *token_list)
{
    struct cparse_regex_ast_node *new_node = NULL;
    struct cparse_regex_token *token_cur = cparse_regex_token_list_first(token_list);
    new_node = cparse_regex_parse_ast_node_seq(&token_cur, 0);
    if (new_node == NULL) return NULL;
    return cparse_regex_ast_new(new_node);
}

