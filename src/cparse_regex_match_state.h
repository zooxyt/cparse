/* CParse : Regex : Matching : State
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_REGEX_MATCH_STATE_H
#define CPARSE_REGEX_MATCH_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_state.h"
#include "cparse_regex_nfa.h"

#define CPARSE_REGEX_NFA_STATE_STACK_SIZE 256

struct cparse_regex_match_nfa_state;
struct cparse_regex_match_nfa_state_stack;

struct cparse_regex_match_nfa_state_stack *
cparse_regex_match_nfa_state_stack_new(void);

void cparse_regex_match_nfa_state_stack_destroy( \
struct cparse_regex_match_nfa_state_stack *stack);

void cparse_regex_match_nfa_state_stack_set_cparse_state( \
struct cparse_regex_match_nfa_state_stack *stack, \
cparse_state_t *cparse_state);

cparse_regex_nfa_state_condition_type_t
cparse_regex_match_nfa_state_stack_top_type( \
struct cparse_regex_match_nfa_state_stack *stack);

int cparse_regex_match_nfa_state_stack_top_match_count( \
struct cparse_regex_match_nfa_state_stack *stack);

struct cparse_regex_nfa_state_condition *
cparse_regex_match_nfa_state_stack_top_condition( \
struct cparse_regex_match_nfa_state_stack *stack);

cparse_bool cparse_regex_match_nfa_state_stack_meet_bottom( \
        struct cparse_regex_match_nfa_state_stack *stack);

void cparse_regex_match_nfa_state_stack_save( \
        struct cparse_regex_match_nfa_state_stack *state, \
        struct cparse_regex_nfa_state_condition *condition);
void cparse_regex_match_nfa_state_stack_save0( \
        struct cparse_regex_match_nfa_state_stack *state, \
        struct cparse_regex_nfa_state_condition *condition);
struct cparse_regex_nfa_state_condition *cparse_regex_match_nfa_state_stack_load( \
        struct cparse_regex_match_nfa_state_stack *state);
struct cparse_regex_nfa_state_condition *cparse_regex_match_nfa_state_stack_drop( \
        struct cparse_regex_match_nfa_state_stack *state);
void cparse_regex_match_nfa_state_stack_erase( \
        struct cparse_regex_match_nfa_state_stack *state);

void cparse_regex_match_nfa_state_stack_increase( \
        struct cparse_regex_match_nfa_state_stack *state);
void cparse_regex_match_nfa_state_stack_decrease( \
        struct cparse_regex_match_nfa_state_stack *state);


#ifdef __cplusplus
}
#endif

#endif

