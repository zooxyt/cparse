/* CParse : Regex : Matching : Complex
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_REGEX_NFA_CPLX_H
#define CPARSE_REGEX_NFA_CPLX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_regex_nfa_state_condition_complex_node;
struct cparse_regex_nfa_state_condition_complex_node_list;

typedef enum
{
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_CHAR = 0,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_DIRECT,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_ANY,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_RANGE,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_SEQ,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR,
    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_NOT,
} cparse_regex_nfa_state_condition_complex_node_type_t;

#define CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_REPMAX_UNLIMITED (-1)

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_char(cparse_char_t ch);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_direct(void);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_any(void);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_range(cparse_char_t ch_from, cparse_char_t ch_to);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_not( \
        struct cparse_regex_nfa_state_condition_complex_node *node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_and_preset( \
        struct cparse_regex_nfa_state_condition_complex_node *first, ...);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_or_preset( \
        struct cparse_regex_nfa_state_condition_complex_node *first, ...);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_and(void);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_or(void);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_seq(void);

void cparse_regex_nfa_state_condition_complex_node_destroy( \
struct cparse_regex_nfa_state_condition_complex_node *node);

cparse_regex_nfa_state_condition_complex_node_type_t
cparse_regex_nfa_state_condition_complex_node_type( \
struct cparse_regex_nfa_state_condition_complex_node *node);

cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_char( \
struct cparse_regex_nfa_state_condition_complex_node *node);

cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_range_from( \
struct cparse_regex_nfa_state_condition_complex_node *node);
cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_range_to( \
struct cparse_regex_nfa_state_condition_complex_node *node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_not( \
struct cparse_regex_nfa_state_condition_complex_node *node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_and_first( \
struct cparse_regex_nfa_state_condition_complex_node *node);

void cparse_regex_nfa_state_condition_complex_node_as_and_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_or_first( \
struct cparse_regex_nfa_state_condition_complex_node *node);

void cparse_regex_nfa_state_condition_complex_node_as_or_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_seq_first( \
struct cparse_regex_nfa_state_condition_complex_node *node);

void cparse_regex_nfa_state_condition_complex_node_as_seq_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_next( \
struct cparse_regex_nfa_state_condition_complex_node *node);

struct cparse_regex_nfa_state_condition_complex *
cparse_regex_nfa_state_condition_complex_new( \
        struct cparse_regex_nfa_state_condition_complex_node *node);
void cparse_regex_nfa_state_condition_complex_destroy( \
        struct cparse_regex_nfa_state_condition_complex *cplx);

void cparse_regex_nfa_state_condition_complex_set_rep_min( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        int rep_min);
void cparse_regex_nfa_state_condition_complex_set_rep_max( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        int rep_max);
int cparse_regex_nfa_state_condition_complex_rep_min( \
        struct cparse_regex_nfa_state_condition_complex *cplx);
int cparse_regex_nfa_state_condition_complex_rep_max( \
        struct cparse_regex_nfa_state_condition_complex *cplx);
void cparse_regex_nfa_state_condition_complex_set_greedy( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        cparse_bool greedy);
cparse_bool cparse_regex_nfa_state_condition_complex_greedy( \
        struct cparse_regex_nfa_state_condition_complex *cplx);

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_root( \
        struct cparse_regex_nfa_state_condition_complex *cplx);


#ifdef __cplusplus
}
#endif

#endif

