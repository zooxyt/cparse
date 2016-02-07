/* CParse : Regex : Matching
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_state.h"
#include "cparse_regex_nfa.h"
#include "cparse_regex_match_state.h"
#include "cparse_regex_match.h"


static cparse_bool cparse_regex_match_nfa_try_complex_node( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition_complex_node *node);

static cparse_bool cparse_regex_match_nfa_try_complex_node_seq( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    cparse_bool ret = cparse_true;
    struct cparse_regex_nfa_state_condition_complex_node *node_cur;

    cparse_regex_match_nfa_state_stack_save0(state_stack, NULL);
    node_cur = cparse_regex_nfa_state_condition_complex_node_as_and_first(node);
    while (node_cur != NULL)
    {
        if (cparse_regex_match_nfa_try_complex_node( \
                    state_stack, reader, node_cur) == cparse_false)
        {
            ret = cparse_false;
            cparse_regex_match_nfa_state_stack_load(state_stack);
            goto finish;
        }

        node_cur = cparse_regex_nfa_state_condition_complex_node_next(node_cur);
    }
    cparse_regex_match_nfa_state_stack_erase(state_stack);

finish:
    return ret;
}

static cparse_bool cparse_regex_match_nfa_try_complex_node_and( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    cparse_bool ret = cparse_true;
    struct cparse_regex_nfa_state_condition_complex_node *node_cur;

    node_cur = cparse_regex_nfa_state_condition_complex_node_as_and_first(node);
    while (node_cur != NULL)
    {
        if (cparse_regex_match_nfa_try_complex_node(state_stack, reader, node_cur) == cparse_false)
        { ret = cparse_false; goto finish; }

        node_cur = cparse_regex_nfa_state_condition_complex_node_next(node_cur);
    }

finish:
    return ret;
}

static cparse_bool cparse_regex_match_nfa_try_complex_node_or( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    cparse_bool ret = cparse_false;
    struct cparse_regex_nfa_state_condition_complex_node *node_cur;

    node_cur = cparse_regex_nfa_state_condition_complex_node_as_and_first(node);
    while (node_cur != NULL)
    {
        if (cparse_regex_match_nfa_try_complex_node(state_stack, reader, node_cur) == cparse_true)
        { ret = cparse_true; goto finish; }

        node_cur = cparse_regex_nfa_state_condition_complex_node_next(node_cur);
    }

finish:
    return ret;
}

static cparse_bool cparse_regex_match_nfa_try_complex_node( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    cparse_regex_nfa_state_condition_complex_node_type_t type;
    cparse_char_t ch;
    cparse_bool condition_matched = cparse_false;

    type = cparse_regex_nfa_state_condition_complex_node_type(node);
    switch (type)
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_CHAR:
            if (cparse_reader_eof(reader)) { break; }
            ch = (cparse_char_t)cparse_reader_peek_char(reader);
            if (ch != cparse_regex_nfa_state_condition_complex_node_as_char(node)) { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_DIRECT:
            condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_ANY:
            if (cparse_reader_eof(reader)) { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_RANGE:
            if (cparse_reader_eof(reader)) { break; }
            ch = (cparse_char_t)cparse_reader_peek_char(reader);
            if (!((cparse_regex_nfa_state_condition_complex_node_as_range_from(node) <= ch) && \
                        (ch <= cparse_regex_nfa_state_condition_complex_node_as_range_to(node))))
            { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_SEQ:
            condition_matched = cparse_regex_match_nfa_try_complex_node_seq( \
                    state_stack, reader, node);
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND:
            condition_matched = cparse_regex_match_nfa_try_complex_node_and( \
                    state_stack, reader, node);
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR:
            condition_matched = cparse_regex_match_nfa_try_complex_node_or( \
                    state_stack, reader, node);
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_NOT:
            condition_matched = cparse_regex_match_nfa_try_complex_node( \
                    state_stack, reader, cparse_regex_nfa_state_condition_complex_node_as_not(node));
            condition_matched = (condition_matched == cparse_false) ? cparse_true : cparse_false;
            break;
    }

    return condition_matched;
}

/* The initial trying of matching complex (including matching counting)
 * e.g. will match at many as possible in greedy mode or 
 * as less as possible in lazy mode */
static cparse_bool cparse_regex_match_nfa_try_complex( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition *condition)
{
    struct cparse_regex_nfa_state_condition_complex *condtion_complex;
    struct cparse_regex_nfa_state_condition_complex_node *root;
    cparse_bool ret = cparse_false, tmp_ret;
    int rep_min, rep_max;
    int count = 0;
    cparse_bool greedy;

    condtion_complex = cparse_regex_nfa_state_condition_as_complex(condition);
    rep_min = cparse_regex_nfa_state_condition_complex_rep_min(condtion_complex);
    rep_max = cparse_regex_nfa_state_condition_complex_rep_max(condtion_complex);
    greedy = cparse_regex_nfa_state_condition_complex_greedy(condtion_complex);

    root = cparse_regex_nfa_state_condition_complex_root(condtion_complex);

    /* Save state */
    cparse_regex_match_nfa_state_stack_save0(state_stack, condition);
    /* This subroutine do the first matching in greedy mode */
    for (;;)
    {
        if (greedy == cparse_true)
        {
            if ((rep_max != CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_REPMAX_UNLIMITED) && \
                    (count == rep_max)) 
            { break; }
        }
        else 
        {
            if (count == rep_min)
            { break; }
        }

        /* Try parsing */
        tmp_ret = cparse_regex_match_nfa_try_complex_node(state_stack, reader, root);
        if (tmp_ret == cparse_false) { break; }
        /* Increase counter */
        cparse_regex_match_nfa_state_stack_increase(state_stack);
        count++;
    }

    if (count < rep_min)
    {
        /* Not reach the minimum requirement */
        while (count-- != 0)
        { cparse_regex_match_nfa_state_stack_decrease(state_stack); }
        (void)cparse_regex_match_nfa_state_stack_load(state_stack);
        ret = cparse_false;
    }
    else
    {
        ret = cparse_true;
    }

    return ret;
}

static cparse_bool cparse_regex_match_nfa_try( \
        struct cparse_regex_match_nfa_state_stack *state_stack, \
        cparse_reader_t *reader, \
        struct cparse_regex_nfa_state_condition *condition)
{
    cparse_bool condition_matched = cparse_false;

    cparse_char_t ch;

    switch (cparse_regex_nfa_state_condition_type(condition))
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_CHAR:
            if (cparse_reader_eof(reader)) { break; }
            ch = (cparse_char_t)cparse_reader_peek_char(reader);
            if (ch != cparse_regex_nfa_state_condition_as_char(condition)) { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_DIRECT:
            condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_ANY:
            if (cparse_reader_eof(reader)) { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_RANGE:
            if (cparse_reader_eof(reader)) { break; }
            ch = (cparse_char_t)cparse_reader_peek_char(reader);
            if (!((cparse_regex_nfa_state_condition_as_range_from(condition) <= ch) && \
                        (ch <= cparse_regex_nfa_state_condition_as_range_to(condition))))
            { break; }
            cparse_reader_forward(reader); condition_matched = cparse_true;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX:
            condition_matched = cparse_regex_match_nfa_try_complex( \
                    state_stack, reader, condition);
            break;
    }

    return condition_matched;
}

int cparse_regex_match_nfa(struct cparse_regex_nfa *nfa, cparse_state_t *state)
{
    int ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_size_t state_idx_cur;
    struct cparse_regex_nfa_state *state_cur;
    struct cparse_regex_nfa_state_condition *condition_cur = NULL;
    cparse_bool condition_matched;
    struct cparse_regex_match_nfa_state_stack *new_nfa_state_stack = NULL;
    int count = 0;

    if ((new_nfa_state_stack = cparse_regex_match_nfa_state_stack_new()) == NULL)
    { ret = -1; goto fail; }
    cparse_regex_match_nfa_state_stack_set_cparse_state(new_nfa_state_stack, state);

    /* Null NFA */
    if (cparse_regex_nfa_state_start(nfa) == cparse_regex_nfa_state_end(nfa))
    { ret = 0; goto done; }

    state_idx_cur = cparse_regex_nfa_state_start(nfa);
    state_cur = cparse_regex_nfa_state(nfa, state_idx_cur);
    condition_cur = cparse_regex_nfa_state_first_condition(state_cur);

    for (;;)
    {
        while (condition_cur == NULL)
        {
            /* No condition, no way to transit to another state, 
             * backtrack */
            if (cparse_regex_match_nfa_state_stack_meet_bottom( \
                        new_nfa_state_stack) == cparse_true)
            { ret = -1; goto fail; }

            if (cparse_regex_match_nfa_state_stack_top_type( \
                        new_nfa_state_stack) == CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX)
            {

                if (cparse_regex_nfa_state_condition_complex_greedy( \
                            cparse_regex_nfa_state_condition_as_complex( \
                                cparse_regex_match_nfa_state_stack_top_condition(new_nfa_state_stack))) == cparse_true)
                {
                    /* greedy, backtrack by popping */
                    count = cparse_regex_match_nfa_state_stack_top_match_count(new_nfa_state_stack);
                    if (count == 0)
                    {
                        /* Already reach 0, directly backtrack */

                        /* pop complex 0 */
                        (void)cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                        /* pop complex try */
                        condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                        condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
                    }
                    else
                    {
                        /* Firstly, pop one to match one less time */
                        condition_cur = cparse_regex_match_nfa_state_stack_drop(new_nfa_state_stack);
                        count--;

                        if (count >= \
                                cparse_regex_nfa_state_condition_complex_rep_min( \
                                    cparse_regex_nfa_state_condition_as_complex(condition_cur)))
                        {
                            /* Matched, transit to new state */
                            state_idx_cur = cparse_regex_nfa_state_condition_dest_state(condition_cur); 
                            /* Extract current state */
                            state_cur = cparse_regex_nfa_state(nfa, state_idx_cur);
                            /* Previous condition no exists, find first */ 
                            condition_cur = cparse_regex_nfa_state_first_condition(state_cur);
                            /* Could not be end or trap */
                        }
                        else
                        {
                            /* Reach the least requirement, whole complex part failed */
                            while (count-- != 0)
                            { condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack); }
                            condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
                        }
                    }
                }
                else
                {
                    /* Non greedy */
                    condition_cur = cparse_regex_match_nfa_state_stack_top_condition(new_nfa_state_stack);
                    count = cparse_regex_match_nfa_state_stack_top_match_count(new_nfa_state_stack);
                    if (count >= \
                            cparse_regex_nfa_state_condition_complex_rep_max( \
                                cparse_regex_nfa_state_condition_as_complex( \
                                    condition_cur)))
                    {
                        /* Reach the limit, backtrack by popping all saved complex states */
                        /* Pop complex subs */
                        while (count-- >= 0)
                        {
                            (void)cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                        }
                        /* Pop complex try */
                        condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                        /* Next condition */
                        condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
                    }
                    else
                    {
                        /* Try matching one more */

                        if (cparse_regex_match_nfa_try_complex_node( \
                                new_nfa_state_stack, \
                                reader, \
                                cparse_regex_nfa_state_condition_complex_root( \
                                    cparse_regex_nfa_state_condition_as_complex( \
                                        condition_cur))) == cparse_false)
                        {
                            /* Reach the least requirement, whole complex part failed */
                            while (count-- != 0)
                            { condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack); }
                            condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
                        }
                        else
                        {
                            /* Increase counter */
                            cparse_regex_match_nfa_state_stack_increase(new_nfa_state_stack);
                            /* Matched, transit to new state */
                            state_idx_cur = cparse_regex_nfa_state_condition_dest_state(condition_cur); 
                            /* Extract current state */
                            state_cur = cparse_regex_nfa_state(nfa, state_idx_cur);
                            /* Previous condition no exists, find first */ 
                            condition_cur = cparse_regex_nfa_state_first_condition(state_cur);
                            /* Could not be end or trap */
                        }
                    }
                }
            }
            else
            {
                /* Pop the top one to replace the current NULL condition */ 
                condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
            }
        }

        /* Save State */
        cparse_regex_match_nfa_state_stack_save(new_nfa_state_stack, condition_cur);
        /* Try to process the condition */
        condition_matched = cparse_regex_match_nfa_try(new_nfa_state_stack, reader, condition_cur);
        if (condition_matched == cparse_true)
        {
            /* Matched, transit to new state */
            state_idx_cur = cparse_regex_nfa_state_condition_dest_state(condition_cur); 
            /* Extract current state */
            state_cur = cparse_regex_nfa_state(nfa, state_idx_cur);
            /* Previous condition no exists, find first */ 
            condition_cur = cparse_regex_nfa_state_first_condition(state_cur);

            /* End? */
            if (state_idx_cur == cparse_regex_nfa_state_end(nfa)) break;
            /* Trap? */
            if (cparse_regex_nfa_state_type( \
                        cparse_regex_nfa_state( \
                            nfa, state_idx_cur)) == \
                    CPARSE_REGEX_NFA_STATE_TYPE_TRAP)
            {
                /* Backtrack 2 steps */
                (void)cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
                condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
            }
        }
        else
        {
            condition_cur = cparse_regex_match_nfa_state_stack_load(new_nfa_state_stack);
            condition_cur = cparse_regex_nfa_state_condition_next(condition_cur);
        }
    }
fail:
done:
    if (new_nfa_state_stack != NULL) 
    { cparse_regex_match_nfa_state_stack_destroy(new_nfa_state_stack); }
    return ret;
}

