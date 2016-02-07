/* CParse : Regex : Matching : State
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_state.h"
#include "cparse_regex_nfa.h"
#include "cparse_regex_match_state.h"


/* The state which recorded before deep into a condition should covers
 * the following information:
 * - Reader State
 * - Condition Choice
 *
 * CParse state which holds the reader already has abbility to record the state,
 * choice doesn't, so reinvent such is the task to do.
 */

struct cparse_regex_match_nfa_state
{
    struct cparse_regex_nfa_state_condition *chose_condition;
    int match_count;
};

struct cparse_regex_match_nfa_state_stack
{
    cparse_state_t *cparse_state;
    struct cparse_regex_match_nfa_state *body;
    struct cparse_regex_match_nfa_state *sp;
    cparse_size_t size;
};

struct cparse_regex_match_nfa_state_stack *
cparse_regex_match_nfa_state_stack_new(void)
{
    struct cparse_regex_match_nfa_state_stack *new_stack = NULL;

    if ((new_stack = (struct cparse_regex_match_nfa_state_stack *)cparse_malloc( \
                    sizeof(struct cparse_regex_match_nfa_state_stack))) == NULL)
    { return NULL; }
    if ((new_stack->body = (struct cparse_regex_match_nfa_state *)cparse_malloc( \
                    sizeof(struct cparse_regex_match_nfa_state) * CPARSE_REGEX_NFA_STATE_STACK_SIZE)) == NULL)
    { goto fail; }
    new_stack->sp = new_stack->body;
    new_stack->size = 0;

    goto done;
fail:
    if (new_stack != NULL)
    {
        cparse_regex_match_nfa_state_stack_destroy(new_stack);
        new_stack = NULL;
    }
done:
    return new_stack;
}

void cparse_regex_match_nfa_state_stack_destroy( \
struct cparse_regex_match_nfa_state_stack *stack)
{
    if (stack->body != NULL) cparse_free(stack->body);
    cparse_free(stack);
}

void cparse_regex_match_nfa_state_stack_set_cparse_state( \
struct cparse_regex_match_nfa_state_stack *stack, \
cparse_state_t *cparse_state)
{
    stack->cparse_state = cparse_state;
}

cparse_regex_nfa_state_condition_type_t
cparse_regex_match_nfa_state_stack_top_type( \
struct cparse_regex_match_nfa_state_stack *stack)
{
    return cparse_regex_nfa_state_condition_type((stack->sp - 1)->chose_condition);
}

int cparse_regex_match_nfa_state_stack_top_match_count( \
struct cparse_regex_match_nfa_state_stack *stack)
{
    return (stack->sp - 1)->match_count;
}

struct cparse_regex_nfa_state_condition *
cparse_regex_match_nfa_state_stack_top_condition( \
struct cparse_regex_match_nfa_state_stack *stack)
{
    return (stack->sp - 1)->chose_condition;
}

cparse_bool cparse_regex_match_nfa_state_stack_meet_bottom( \
        struct cparse_regex_match_nfa_state_stack *stack)
{
    return stack->size == 0 ? cparse_true : cparse_false;
}

void cparse_regex_match_nfa_state_stack_save( \
        struct cparse_regex_match_nfa_state_stack *state, \
        struct cparse_regex_nfa_state_condition *condition)
{
    cparse_state_save(state->cparse_state);
    state->sp->chose_condition = condition;
    state->sp->match_count = 1;
    state->sp++;
    state->size++;
}

void cparse_regex_match_nfa_state_stack_save0( \
        struct cparse_regex_match_nfa_state_stack *state, \
        struct cparse_regex_nfa_state_condition *condition)
{
    cparse_state_save(state->cparse_state);
    state->sp->chose_condition = condition;
    state->sp->match_count = 0;
    state->sp++;
    state->size++;
}

void cparse_regex_match_nfa_state_stack_increase( \
        struct cparse_regex_match_nfa_state_stack *state)
{
    cparse_state_save(state->cparse_state);
    state->sp->chose_condition = (state->sp - 1)->chose_condition;
    state->sp->match_count = (state->sp - 1)->match_count + 1;
    state->sp++;
    state->size++;
}

void cparse_regex_match_nfa_state_stack_decrease( \
        struct cparse_regex_match_nfa_state_stack *state)
{
    cparse_state_load(state->cparse_state);
    state->sp--;
    state->size--;
}


struct cparse_regex_nfa_state_condition *
cparse_regex_match_nfa_state_stack_load( \
        struct cparse_regex_match_nfa_state_stack *state)
{
    cparse_state_load(state->cparse_state);
    state->sp--;
    state->size--;
    return state->sp->chose_condition;
}

struct cparse_regex_nfa_state_condition *
cparse_regex_match_nfa_state_stack_drop( \
        struct cparse_regex_match_nfa_state_stack *state)
{
    cparse_state_drop(state->cparse_state);
    state->sp--;
    state->size--;
    return state->sp->chose_condition;
}

void cparse_regex_match_nfa_state_stack_erase( \
        struct cparse_regex_match_nfa_state_stack *state)
{
    cparse_state_erase(state->cparse_state);
    state->sp--;
    state->size--;
}

