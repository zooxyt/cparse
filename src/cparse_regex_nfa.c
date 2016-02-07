/* CParse : Regex : NFA
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_regex_nfa_cplx.h"
#include "cparse_regex_nfa.h"


/* Condition */

struct cparse_regex_nfa_state_condition_range
{
    cparse_char_t ch_from, ch_to;
};

struct cparse_regex_nfa_state_condition
{
    cparse_regex_nfa_state_condition_type_t type;
    union
    {
        cparse_char_t part_char;
        struct cparse_regex_nfa_state_condition_range part_range;
        struct cparse_regex_nfa_state_condition_complex *part_complex; 
    } u;
    cparse_size_t state_idx_dst;
    struct cparse_regex_nfa_state_condition *next;
};

static struct cparse_regex_nfa_state_condition *
cparse_regex_nfa_state_condition_new( \
    cparse_regex_nfa_state_condition_type_t type, \
    cparse_size_t state_idx_dst)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = (struct cparse_regex_nfa_state_condition *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa_state_condition))) == NULL)
    { return NULL; }
    new_condition->type = type;
    new_condition->state_idx_dst = state_idx_dst;
    new_condition->next = NULL;
    switch (type)
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_CHAR:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_ANY:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_RANGE:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_DIRECT:
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX:
            new_condition->u.part_complex = NULL;
            break;
    }

    return new_condition;
}

static void cparse_regex_nfa_state_condition_destroy( \
        struct cparse_regex_nfa_state_condition *condition)
{
    switch (condition->type)
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_CHAR:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_ANY:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_RANGE:
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_DIRECT:
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX:
            if (condition->u.part_complex != NULL)
            { cparse_regex_nfa_state_condition_complex_destroy(condition->u.part_complex); }
            break;
    }
    cparse_free(condition);
}


struct cparse_regex_nfa_state_condition_list
{
    struct cparse_regex_nfa_state_condition *begin, *end;
    cparse_size_t size;
};

static struct cparse_regex_nfa_state_condition_list *
cparse_regex_nfa_state_condition_list_new(void)
{
    struct cparse_regex_nfa_state_condition_list *new_list = NULL;

    if ((new_list = (struct cparse_regex_nfa_state_condition_list *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa_state_condition_list))) == NULL)
    { return NULL; }
    new_list->begin = new_list->end = NULL;
    new_list->size = 0;

    return new_list;
}

static void cparse_regex_nfa_state_condition_list_destroy( \
        struct cparse_regex_nfa_state_condition_list *list)
{
    struct cparse_regex_nfa_state_condition *condition_cur, *condition_next;

    condition_cur = list->begin;
    while (condition_cur != NULL)
    {
        condition_next = condition_cur->next;
        cparse_regex_nfa_state_condition_destroy(condition_cur);
        condition_cur = condition_next;
    }
    cparse_free(list);
}

static void cparse_regex_nfa_state_condition_list_push_back( \
        struct cparse_regex_nfa_state_condition_list *list, \
        struct cparse_regex_nfa_state_condition *new_condition)
{
    if (list->begin == NULL)
    {
        list->begin = list->end = new_condition;
    }
    else
    {
        list->end->next = new_condition;
        list->end = new_condition;
    }
    list->size++;
}

struct cparse_regex_nfa_state
{
    cparse_regex_nfa_state_type_t type;
    struct cparse_regex_nfa_state_condition_list *conditions;
};

struct cparse_regex_nfa_state_condition *
cparse_regex_nfa_state_condition_next( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->next;
}

cparse_size_t
cparse_regex_nfa_state_condition_dest_state( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->state_idx_dst;
}

cparse_regex_nfa_state_condition_type_t
cparse_regex_nfa_state_condition_type( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->type;
}

cparse_char_t
cparse_regex_nfa_state_condition_as_char( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->u.part_char;
}

cparse_char_t
cparse_regex_nfa_state_condition_as_range_from( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->u.part_range.ch_from;
}

cparse_char_t
cparse_regex_nfa_state_condition_as_range_to( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->u.part_range.ch_to;
}

struct cparse_regex_nfa_state_condition_complex *
cparse_regex_nfa_state_condition_as_complex( \
struct cparse_regex_nfa_state_condition *condition)
{
    return condition->u.part_complex;
}

cparse_regex_nfa_state_type_t 
cparse_regex_nfa_state_type( \
        struct cparse_regex_nfa_state *state)
{
    return state->type;
}

void cparse_regex_nfa_state_set_type( \
        struct cparse_regex_nfa_state *state, \
        cparse_regex_nfa_state_type_t type)
{
    state->type = type;
}

struct cparse_regex_nfa_state_condition *
cparse_regex_nfa_state_first_condition(struct cparse_regex_nfa_state *state)
{
    return state->conditions->begin;
}


struct cparse_regex_nfa
{
    struct cparse_regex_nfa_state *states;
    cparse_size_t states_count;
    cparse_size_t state_start, state_end;
};

struct cparse_regex_nfa *cparse_regex_nfa_new(cparse_size_t states_count)
{
    struct cparse_regex_nfa *new_nfa = NULL;
    cparse_size_t idx;

    if ((new_nfa = (struct cparse_regex_nfa *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa))) == NULL)
    { return NULL; }
    new_nfa->states_count = states_count;
    if ((new_nfa->states = (struct cparse_regex_nfa_state *)cparse_malloc( \
            sizeof(struct cparse_regex_nfa_state) * states_count)) == NULL)
    { cparse_free(new_nfa); return NULL; }
    for (idx = 0; idx != states_count; idx++)
    {
        new_nfa->states[idx].conditions = NULL;
    }
    for (idx = 0; idx != states_count; idx++)
    {
        cparse_regex_nfa_state_set_type( \
                cparse_regex_nfa_state(new_nfa, idx), 
                CPARSE_REGEX_NFA_STATE_TYPE_NORMAL);
        if ((new_nfa->states[idx].conditions = cparse_regex_nfa_state_condition_list_new()) == NULL)
        { goto fail; }
    }
    new_nfa->state_start = 0;
    new_nfa->state_end = 0;

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

void cparse_regex_nfa_destroy(struct cparse_regex_nfa *nfa)
{
    cparse_size_t idx;

    for (idx = 0; idx != nfa->states_count; idx++)
    {
        if (nfa->states[idx].conditions != NULL)
        { cparse_regex_nfa_state_condition_list_destroy( \
                nfa->states[idx].conditions); }
    }
    cparse_free(nfa->states);
    cparse_free(nfa);
}

void cparse_regex_nfa_set_state_start(struct cparse_regex_nfa *nfa, \
        cparse_size_t state_start)
{
    nfa->state_start = state_start;
}

void cparse_regex_nfa_set_state_end(struct cparse_regex_nfa *nfa, \
        cparse_size_t state_end)
{
    nfa->state_end = state_end;
}

cparse_size_t cparse_regex_nfa_state_start(struct cparse_regex_nfa *nfa)
{
    return nfa->state_start;
}

cparse_size_t cparse_regex_nfa_state_end(struct cparse_regex_nfa *nfa)
{
    return nfa->state_end;
}

struct cparse_regex_nfa_state *cparse_regex_nfa_state(struct cparse_regex_nfa *nfa, \
        cparse_size_t idx)
{
    if (idx >= nfa->states_count) return NULL;
    return &(nfa->states[idx]);
}

static int cparse_regex_nfa_state_add_condition( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        struct cparse_regex_nfa_state_condition *condition)
{
    if (state_idx_src >= nfa->states_count) { return -1; }
    cparse_regex_nfa_state_condition_list_push_back( \
                nfa->states[state_idx_src].conditions, condition);

    return 0;
}

int cparse_regex_nfa_state_add_condition_direct( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = cparse_regex_nfa_state_condition_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_DIRECT, \
                    state_idx_dst)) == NULL)
    { return -1; }
    if (cparse_regex_nfa_state_add_condition( \
                nfa, \
                state_idx_src, \
                new_condition) != 0)
    { return -1; }

    return 0;
}

int cparse_regex_nfa_state_add_condition_any( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = cparse_regex_nfa_state_condition_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_ANY, \
                    state_idx_dst)) == NULL)
    { return -1; }
    if (cparse_regex_nfa_state_add_condition( \
                nfa, \
                state_idx_src, \
                new_condition) != 0)
    { return -1; }

    return 0;
}

int cparse_regex_nfa_state_add_condition_char( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        cparse_char_t ch)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = cparse_regex_nfa_state_condition_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_CHAR, \
                    state_idx_dst)) == NULL)
    { return -1; }
    new_condition->u.part_char = ch;
    if (cparse_regex_nfa_state_add_condition( \
                nfa, \
                state_idx_src, \
                new_condition) != 0)
    { return -1; }

    return 0;
}

int cparse_regex_nfa_state_add_condition_range( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        cparse_char_t ch_from, \
        cparse_char_t ch_to)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = cparse_regex_nfa_state_condition_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_RANGE, \
                    state_idx_dst)) == NULL)
    { return -1; }
    new_condition->u.part_range.ch_from = ch_from;
    new_condition->u.part_range.ch_to = ch_to;
    if (cparse_regex_nfa_state_add_condition( \
                nfa, \
                state_idx_src, \
                new_condition) != 0)
    { return -1; }

    return 0;
}

int cparse_regex_nfa_state_add_condition_complex( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    struct cparse_regex_nfa_state_condition *new_condition = NULL;

    if ((new_condition = cparse_regex_nfa_state_condition_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX, \
                    state_idx_dst)) == NULL)
    { return -1; }
    new_condition->u.part_complex = cplx;
    if (cparse_regex_nfa_state_add_condition( \
                nfa, \
                state_idx_src, \
                new_condition) != 0)
    { return -1; }

    return 0;
}

/* Debug and test */
cparse_size_t cparse_regex_nfa_states_count( \
        struct cparse_regex_nfa *nfa)
{
    return nfa->states_count;
}

