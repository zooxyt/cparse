/* CParse : Regex : Converter (AST -> NFA)
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_regex_ast.h"
#include "cparse_regex_nfa.h"
#include "cparse_regex_converter.h"

cparse_char_t 
cparse_regex_ast_node_char(struct cparse_regex_ast_node *node);
struct cparse_regex_ast_node_group *
cparse_regex_ast_node_group(struct cparse_regex_ast_node *node);
struct cparse_regex_ast_node_branch *
cparse_regex_ast_node_branch(struct cparse_regex_ast_node *node);

static cparse_size_t cparse_regex_states_count_node(struct cparse_regex_ast_node *node);

static cparse_size_t cparse_regex_states_count_node_seq(struct cparse_regex_ast_node *node)
{
    cparse_size_t count = 0;
    struct cparse_regex_ast_node_seq *node_seq = cparse_regex_ast_node_seq(node);
    struct cparse_regex_ast_node_seq_node *node_cur = cparse_regex_ast_node_seq_first(node_seq);

    while (node_cur != NULL)
    {
        count += cparse_regex_states_count_node( \
                cparse_regex_ast_node_seq_node_node(node_cur));

        node_cur = cparse_regex_ast_node_seq_node_next(node_cur);
    }

    return count;
}

static cparse_size_t cparse_regex_states_count_node_branch(struct cparse_regex_ast_node *node)
{
    cparse_size_t count = 0;
    struct cparse_regex_ast_node_branch *node_branch = cparse_regex_ast_node_branch(node);
    struct cparse_regex_ast_node_branch_node *node_cur = cparse_regex_ast_node_branch_first(node_branch);

    while (node_cur != NULL)
    {
        count += (cparse_regex_states_count_node( \
                    cparse_regex_ast_node_branch_node_node(node_cur)) - 1);

        node_cur = cparse_regex_ast_node_branch_node_next(node_cur);
    }

    return count + 1;
}

static cparse_size_t cparse_regex_states_count_node( \
        struct cparse_regex_ast_node *node)
{
    cparse_size_t count = 0;
    cparse_regex_ast_node_type_t type = cparse_regex_ast_node_type(node); 

    switch (type)
    {
        case CPARSE_REGEX_AST_NODE_TYPE_ANY:
        case CPARSE_REGEX_AST_NODE_TYPE_CHAR:
            count = 1;
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_SEQ:
            count = cparse_regex_states_count_node_seq(node);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_GROUP:
            count = 1;
            if (cparse_regex_ast_node_group_exclude( \
                        cparse_regex_ast_node_group(node)) == cparse_true)
            {
                /* For containing the trap state */
                count += 2;
            }
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_BRANCH:
            count = cparse_regex_states_count_node_branch(node);
            break;
    }

    return count;
}

static cparse_size_t cparse_regex_states_count(struct cparse_regex_ast *ast)
{
    return cparse_regex_states_count_node(cparse_regex_ast_root(ast));
}

static int cparse_regex_append_states_node(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx);


static int cparse_regex_append_states_node_any(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    (void)node;
    (void)state_idx;

    if (cparse_regex_nfa_state_add_condition_any( \
                nfa, state_in, state_out) != 0)
    { return -1; }

    return 0;
}

static int cparse_regex_append_states_node_char(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    cparse_char_t ch = cparse_regex_ast_node_char(node);

    (void)state_idx;

    if (cparse_regex_nfa_state_add_condition_char( \
                nfa, state_in, state_out, ch) != 0)
    { return -1; }

    return 0;
}

static int cparse_regex_append_states_node_seq(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    struct cparse_regex_ast_node_seq *node_seq;
    struct cparse_regex_ast_node_seq_node *node_seq_node_cur;
    struct cparse_regex_ast_node *node_cur;
    cparse_size_t state_cur = state_in, state_next;

    node_seq = cparse_regex_ast_node_seq(node);
    node_seq_node_cur = cparse_regex_ast_node_seq_first(node_seq);
    while (node_seq_node_cur != NULL)
    {
        if (cparse_regex_ast_node_seq_node_next(node_seq_node_cur) == NULL)
        {
            /* Final one */
            state_next = state_out;
        }
        else
        {
            state_next = (*state_idx)++;
        }

        node_cur = cparse_regex_ast_node_seq_node_node(node_seq_node_cur);
        if (cparse_regex_append_states_node(nfa, node_cur, \
                    state_cur, state_next, state_idx) != 0)
        { return -1; }

        /* Next node */
        node_seq_node_cur = cparse_regex_ast_node_seq_node_next(node_seq_node_cur);
        state_cur = state_next;
    }

    return 0;
}

static int cparse_regex_append_states_node_group(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    struct cparse_regex_ast_node_group *node_group;
    struct cparse_regex_ast_node_group_node *node_group_node_cur;
    cparse_bool exclude;
    cparse_size_t state_0 = 0;
    cparse_size_t state_trap = 0;
    cparse_size_t state_to_normal_patterns = 0;
    cparse_size_t state_from_normal_patterns = 0;

    node_group = cparse_regex_ast_node_group(node);
    /* [^ ... ] */
    exclude = cparse_regex_ast_node_group_exclude(node_group);
    if (exclude == cparse_false)
    {
        state_to_normal_patterns = state_in;
        state_from_normal_patterns = state_out;
    }
    else
    {
        state_0 = (*state_idx)++;
        state_trap = (*state_idx)++;
        state_to_normal_patterns = state_0;
        state_from_normal_patterns = state_trap;
        cparse_regex_nfa_state_set_type( \
                cparse_regex_nfa_state(nfa, state_trap), \
                CPARSE_REGEX_NFA_STATE_TYPE_TRAP);
    }
    node_group_node_cur = cparse_regex_ast_node_group_first(node_group);

    while (node_group_node_cur != NULL)
    {
        switch (cparse_regex_ast_node_group_node_type(node_group_node_cur))
        {
            case CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_SOLO:
                if (cparse_regex_nfa_state_add_condition_char( \
                            nfa, state_to_normal_patterns, state_from_normal_patterns, \
                            cparse_regex_ast_node_group_node_as_solo(node_group_node_cur)) != 0)
                { return -1; }
                break;

            case CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_RANGE:
                if (cparse_regex_nfa_state_add_condition_range( \
                            nfa, state_to_normal_patterns, state_from_normal_patterns, \
                            cparse_regex_ast_node_group_node_as_range_from(node_group_node_cur), \
                            cparse_regex_ast_node_group_node_as_range_to(node_group_node_cur)) != 0)
                { return -1; }
                break;
        }
        node_group_node_cur = cparse_regex_ast_node_group_node_next(node_group_node_cur);
    }

    if (exclude == cparse_false)
    {
        /* Nothing to do 
         *
         *           +---(a)---+
         * [start] --+         +--> [end]
         *           +---(b)---+
         */
    }
    else
    {
        /* Append an any pattern
         *
         *                                +---(a)---+
         * [start]---(direct) ---> [S0] --+         +--> [Strap]
         *                                +---(b)---+     
         *                                +               
         *                                +----(any)----------> [end]
         *                       
         * Strap backtract two steps to start then fail or try other branch.
         *
         */

        /* [start]---(direct)--> [S0] */
        if (cparse_regex_nfa_state_add_condition_direct( \
                    nfa, state_in, state_0) != 0)
        { return -1; }
        /* [S0]---(any)--> [end] */
        if (cparse_regex_nfa_state_add_condition_any( \
                    nfa, state_0, state_out) != 0)
        { return -1; }
    }

    return 0;
}

static int cparse_regex_append_states_node_branch(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    struct cparse_regex_ast_node_branch *node_branch;
    struct cparse_regex_ast_node_branch_node *node_branch_node_cur;
    struct cparse_regex_ast_node *node_cur;

    node_branch = cparse_regex_ast_node_branch(node);
    node_branch_node_cur = cparse_regex_ast_node_branch_first(node_branch);

    while (node_branch_node_cur != NULL)
    {
        node_cur = cparse_regex_ast_node_branch_node_node(node_branch_node_cur);
        if (cparse_regex_append_states_node(nfa, node_cur, state_in, state_out, state_idx) != 0)
        { return -1; }

        node_branch_node_cur = cparse_regex_ast_node_branch_node_next(node_branch_node_cur);
    }

    return 0;
}

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_from_ast_node( \
        struct cparse_regex_ast_node *node);

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_from_ast_node_seq( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node = NULL;
    struct cparse_regex_ast_node_seq_node *node_cur = NULL;
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_sub_node = NULL;

    if ((new_complex_node = cparse_regex_nfa_state_condition_complex_node_new_seq()) == NULL)
    { goto fail; }

    node_cur = cparse_regex_ast_node_seq_first(cparse_regex_ast_node_seq(node));
    while (node_cur != NULL)
    {
        if ((new_complex_sub_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node( \
                        cparse_regex_ast_node_seq_node_node(node_cur))) == NULL)
        { goto fail; }
        cparse_regex_nfa_state_condition_complex_node_as_seq_push_back( \
                new_complex_node, new_complex_sub_node);
        new_complex_sub_node = NULL;

        node_cur = cparse_regex_ast_node_seq_node_next(node_cur);
    }

    goto done;
fail:
    if (new_complex_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_complex_node);
        new_complex_node = NULL;
    }
done:
    return new_complex_node;
}

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_from_ast_node_group( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node = NULL;
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node_not = NULL;
    struct cparse_regex_ast_node_group_node *node_cur = NULL;
    cparse_regex_ast_node_group_node_type_t type;
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_sub_node = NULL;
    cparse_bool exclude = cparse_regex_ast_node_group_exclude( \
            cparse_regex_ast_node_group(node));

    if ((new_complex_node = cparse_regex_nfa_state_condition_complex_node_new_or()) == NULL)
    { goto fail; }

    node_cur = cparse_regex_ast_node_group_first(cparse_regex_ast_node_group(node));
    while (node_cur != NULL)
    {
        type = cparse_regex_ast_node_group_node_type(node_cur);
        switch (type)
        {
            case CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_SOLO:
                if ((new_complex_sub_node = cparse_regex_nfa_state_condition_complex_node_new_char( \
                                cparse_regex_ast_node_group_node_as_solo(node_cur))) == NULL)
                { goto fail; }
                break;
            case CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_RANGE:
                if ((new_complex_sub_node = cparse_regex_nfa_state_condition_complex_node_new_range( \
                                cparse_regex_ast_node_group_node_as_range_from(node_cur), \
                                cparse_regex_ast_node_group_node_as_range_to(node_cur))) == NULL)
                { goto fail; }
                break;
        }
        cparse_regex_nfa_state_condition_complex_node_as_or_push_back( \
                new_complex_node, new_complex_sub_node);
        new_complex_sub_node = NULL;

        node_cur = cparse_regex_ast_node_group_node_next(node_cur);
    }

    if (exclude == cparse_true)
    {
        if ((new_complex_node_not = cparse_regex_nfa_state_condition_complex_node_new_not( \
                        new_complex_node)) == NULL)
        { goto fail; }
        new_complex_node = new_complex_node_not; new_complex_node_not = NULL;
    }

    goto done;
fail:
    if (new_complex_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_complex_node);
        new_complex_node = NULL;
    }
    if (new_complex_node_not != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_complex_node_not);
    }
done:
    return new_complex_node;
}

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_from_ast_node_branch( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node = NULL;
    struct cparse_regex_ast_node_branch_node *node_cur = NULL;
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_sub_node = NULL;

    if ((new_complex_node = cparse_regex_nfa_state_condition_complex_node_new_or()) == NULL)
    { goto fail; }

    node_cur = cparse_regex_ast_node_branch_first(cparse_regex_ast_node_branch(node));
    while (node_cur != NULL)
    {
        if ((new_complex_sub_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node( \
                        cparse_regex_ast_node_branch_node_node( \
                            node_cur))) == NULL)
        { goto fail; }
        cparse_regex_nfa_state_condition_complex_node_as_or_push_back( \
                new_complex_node, new_complex_sub_node);
        new_complex_sub_node = NULL;

        node_cur = cparse_regex_ast_node_branch_node_next(node_cur);
    }

    goto done;
fail:
    if (new_complex_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_complex_node);
        new_complex_node = NULL;
    }
done:
    return new_complex_node;
}

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_from_ast_node( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node = NULL;
    cparse_regex_ast_node_type_t type = cparse_regex_ast_node_type(node); 

    switch (type)
    {
        case CPARSE_REGEX_AST_NODE_TYPE_ANY:
            new_complex_node = cparse_regex_nfa_state_condition_complex_node_new_any();
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_CHAR:
            new_complex_node = cparse_regex_nfa_state_condition_complex_node_new_char( \
                    cparse_regex_ast_node_char(node));
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_SEQ:
            new_complex_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node_seq( \
                    node);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_GROUP:
            new_complex_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node_group( \
                    node);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_BRANCH:
            new_complex_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node_branch( \
                    node);
            break;
    }

    return new_complex_node;
}

static int cparse_regex_append_states_node_complex(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    int ret = 0;
    struct cparse_regex_nfa_state_condition_complex *new_complex = NULL;
    struct cparse_regex_nfa_state_condition_complex_node *new_complex_node = NULL;

    (void)state_idx;
    
    if ((new_complex_node = cparse_regex_nfa_state_condition_complex_node_from_ast_node( \
                    node)) == NULL)
    { ret = -1; goto fail; }
    if ((new_complex = cparse_regex_nfa_state_condition_complex_new( \
                    new_complex_node)) == NULL)
    { ret = -1; goto fail; }
    new_complex_node = NULL;

    cparse_regex_nfa_state_condition_complex_set_greedy( \
            new_complex, cparse_regex_ast_node_greed(node));
    cparse_regex_nfa_state_condition_complex_set_rep_min( \
            new_complex, cparse_regex_ast_node_low_bound(node));
    cparse_regex_nfa_state_condition_complex_set_rep_max( \
            new_complex, cparse_regex_ast_node_high_bound(node));

    if (cparse_regex_nfa_state_add_condition_complex( \
                nfa, state_in, state_out, \
                new_complex) != 0)
    { ret = -1; goto fail; }

    new_complex = NULL;
    goto done;
fail:
    if (new_complex != NULL)
    { cparse_regex_nfa_state_condition_complex_destroy(new_complex); }
    if (new_complex_node != NULL)
    { cparse_regex_nfa_state_condition_complex_node_destroy(new_complex_node); }
done:
    return ret;
}

static int cparse_regex_append_states_node(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast_node *node, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    int ret = 0;
    cparse_regex_ast_node_type_t type = cparse_regex_ast_node_type(node); 

    if (!((cparse_regex_ast_node_high_bound(node) == 1) && \
                (cparse_regex_ast_node_low_bound(node))))
    {
        return cparse_regex_append_states_node_complex(nfa, \
                node, state_in, state_out, state_idx);
    }

    switch (type)
    {
        case CPARSE_REGEX_AST_NODE_TYPE_ANY:
            ret = cparse_regex_append_states_node_any(nfa, node, \
                    state_in, state_out, \
                    state_idx);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_CHAR:
            ret = cparse_regex_append_states_node_char(nfa, node, \
                    state_in, state_out, \
                    state_idx);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_SEQ:
            ret = cparse_regex_append_states_node_seq(nfa, node, \
                    state_in, state_out, \
                    state_idx);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_GROUP:
            ret = cparse_regex_append_states_node_group(nfa, node, \
                    state_in, state_out, \
                    state_idx);
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_BRANCH:
            ret = cparse_regex_append_states_node_branch(nfa, node, \
                    state_in, state_out, \
                    state_idx);
            break;
    }

    return ret;
}

static int cparse_regex_append_states(struct cparse_regex_nfa *nfa, \
        struct cparse_regex_ast *ast, \
        cparse_size_t state_in, \
        cparse_size_t state_out, \
        cparse_size_t *state_idx)
{
    return cparse_regex_append_states_node(nfa, cparse_regex_ast_root(ast), \
            state_in, state_out, \
            state_idx);
}

struct cparse_regex_nfa *cparse_regex_convert(struct cparse_regex_ast *ast)
{
    cparse_size_t states_count = 0;
    struct cparse_regex_nfa *new_nfa = NULL;
    cparse_size_t states_idx;

    /* Count all states (initial state is excluded) */
    states_count = cparse_regex_states_count(ast);

    /* Create NFA */
    if ((new_nfa = cparse_regex_nfa_new( \
                    states_count + 1)) == NULL)
    { return NULL; }

    /* Set the start state #0 */
    cparse_regex_nfa_set_state_start(new_nfa, 0);

    if (states_count == 0)
    {
        /* Empty Pattern */ 
        /* Set the end state #0 */
        cparse_regex_nfa_set_state_end(new_nfa, 0);
    }
    else
    {
        /* Set the end state #1 */
        cparse_regex_nfa_set_state_end(new_nfa, 1);

        /* Next usable state number from #1 */
        states_idx = 2;

        if (cparse_regex_append_states(new_nfa, ast, \
                    0, 1, \
                    &states_idx) != 0)
        { goto fail; }
    }

    goto done;
fail:
    if (new_nfa != NULL)
    {
        cparse_regex_nfa_destroy(new_nfa);
        new_nfa = NULL;
    }
done:
    return new_nfa;
}

