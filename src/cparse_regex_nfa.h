/* CParse : Regex : NFA
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_REGEX_NFA_H
#define CPARSE_REGEX_NFA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_regex_nfa_cplx.h"

/* struct cparse_regex_nfa_state_condition_combination; */
struct cparse_regex_nfa_state_condition;
struct cparse_regex_nfa_state;
struct cparse_regex_nfa;


/* Condition */

typedef enum
{
    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_CHAR = 0,
    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_DIRECT,
    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_ANY,
    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_RANGE,
    CPARSE_REGEX_NFA_STATE_CONDITION_TYPE_COMPLEX,
} cparse_regex_nfa_state_condition_type_t;

struct cparse_regex_nfa_state_condition *
cparse_regex_nfa_state_condition_next( \
struct cparse_regex_nfa_state_condition *condition);

cparse_size_t
cparse_regex_nfa_state_condition_dest_state( \
struct cparse_regex_nfa_state_condition *condition);

cparse_regex_nfa_state_condition_type_t
cparse_regex_nfa_state_condition_type( \
struct cparse_regex_nfa_state_condition *condition);

cparse_char_t
cparse_regex_nfa_state_condition_as_char( \
struct cparse_regex_nfa_state_condition *condition);

cparse_char_t
cparse_regex_nfa_state_condition_as_range_from( \
struct cparse_regex_nfa_state_condition *condition);
cparse_char_t
cparse_regex_nfa_state_condition_as_range_to( \
struct cparse_regex_nfa_state_condition *condition);

struct cparse_regex_nfa_state_condition_complex *
cparse_regex_nfa_state_condition_as_complex( \
struct cparse_regex_nfa_state_condition *condition);


typedef enum
{
    CPARSE_REGEX_NFA_STATE_TYPE_NORMAL = 0,
    CPARSE_REGEX_NFA_STATE_TYPE_TRAP,
} cparse_regex_nfa_state_type_t;

cparse_regex_nfa_state_type_t 
cparse_regex_nfa_state_type( \
        struct cparse_regex_nfa_state *state);

void cparse_regex_nfa_state_set_type( \
        struct cparse_regex_nfa_state *state, \
        cparse_regex_nfa_state_type_t type);

struct cparse_regex_nfa_state_condition *
cparse_regex_nfa_state_first_condition( \
        struct cparse_regex_nfa_state *state);


/* NFA */

struct cparse_regex_nfa *cparse_regex_nfa_new(cparse_size_t states_count);
void cparse_regex_nfa_destroy(struct cparse_regex_nfa *nfa);

void cparse_regex_nfa_set_state_start(struct cparse_regex_nfa *nfa, \
        cparse_size_t state_start);
void cparse_regex_nfa_set_state_end(struct cparse_regex_nfa *nfa, \
        cparse_size_t state_end);
cparse_size_t cparse_regex_nfa_state_start(struct cparse_regex_nfa *nfa);
cparse_size_t cparse_regex_nfa_state_end(struct cparse_regex_nfa *nfa);
struct cparse_regex_nfa_state *cparse_regex_nfa_state(struct cparse_regex_nfa *nfa, \
        cparse_size_t idx);

int cparse_regex_nfa_state_add_condition_direct( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst);
int cparse_regex_nfa_state_add_condition_any( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst);
int cparse_regex_nfa_state_add_condition_char( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        cparse_char_t ch);
int cparse_regex_nfa_state_add_condition_range( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        cparse_char_t ch_from, \
        cparse_char_t ch_to);
int cparse_regex_nfa_state_add_condition_complex( \
        struct cparse_regex_nfa *nfa, \
        cparse_size_t state_idx_src, \
        cparse_size_t state_idx_dst, \
        struct cparse_regex_nfa_state_condition_complex *cplx);


/* Debug and test */
cparse_size_t cparse_regex_nfa_states_count( \
        struct cparse_regex_nfa *nfa);


#ifdef __cplusplus
}
#endif

#endif


