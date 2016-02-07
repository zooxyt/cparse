/* CParse : Regex : AST
 * Copyright(C) Cheryl Natsu */

#ifndef CPARSE_REGEX_AST_H
#define CPARSE_REGEX_AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_regex_ast_node_seq_node;
struct cparse_regex_ast_node_seq;
struct cparse_regex_ast_node_branch_node;
struct cparse_regex_ast_node_branch;
struct cparse_regex_ast_node_group_node_range;
struct cparse_regex_ast_node_group_node;
struct cparse_regex_ast_node_group;
struct cparse_regex_ast_node;
struct cparse_regex_ast;

/* AST Node : Seq : Node */
struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_node_new( \
        struct cparse_regex_ast_node *node);
void cparse_regex_ast_node_seq_node_destroy( \
        struct cparse_regex_ast_node_seq_node *node);
struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_node_next( \
        struct cparse_regex_ast_node_seq_node *node);
struct cparse_regex_ast_node *
cparse_regex_ast_node_seq_node_node( \
        struct cparse_regex_ast_node_seq_node *node);

/* AST Node : Seq */
struct cparse_regex_ast_node_seq *
cparse_regex_ast_node_seq_new(void);
void cparse_regex_ast_node_seq_destroy( \
        struct cparse_regex_ast_node_seq *node_seq);
void cparse_regex_ast_node_seq_push_back( \
        struct cparse_regex_ast_node_seq *node_seq, \
        struct cparse_regex_ast_node_seq_node *new_node);
struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_first( \
        struct cparse_regex_ast_node_seq *node_seq);

/* AST Node : Branch : Node */
struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_node_new( \
        struct cparse_regex_ast_node *node);
void cparse_regex_ast_node_branch_node_destroy( \
        struct cparse_regex_ast_node_branch_node *node);
struct cparse_regex_ast_node *
cparse_regex_ast_node_branch_node_node( \
        struct cparse_regex_ast_node_branch_node *node);
struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_node_next( \
        struct cparse_regex_ast_node_branch_node *node);

/* AST Node : Branch */
struct cparse_regex_ast_node_branch * \
cparse_regex_ast_node_branch_new(void);
void cparse_regex_ast_node_branch_destroy( \
struct cparse_regex_ast_node_branch *branch);
void cparse_regex_ast_node_branch_push_back( \
struct cparse_regex_ast_node_branch *branch, \
struct cparse_regex_ast_node_branch_node *new_branch_node);


struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_first( \
        struct cparse_regex_ast_node_branch *branch);

/* AST Node : Group : Node */

typedef enum
{
    CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_SOLO = 0,
    CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_RANGE,
} cparse_regex_ast_node_group_node_type_t;

struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_new_solo(cparse_char_t ch);
struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_new_range(cparse_char_t ch_from, cparse_char_t ch_to);
void cparse_regex_ast_node_group_node_destroy( \
        struct cparse_regex_ast_node_group_node *node);

cparse_regex_ast_node_group_node_type_t
cparse_regex_ast_node_group_node_type( \
        struct cparse_regex_ast_node_group_node *node);

struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_next( \
        struct cparse_regex_ast_node_group_node *node);
cparse_char_t cparse_regex_ast_node_group_node_as_solo( \
        struct cparse_regex_ast_node_group_node *node);
cparse_char_t cparse_regex_ast_node_group_node_as_range_from( \
        struct cparse_regex_ast_node_group_node *node);
cparse_char_t cparse_regex_ast_node_group_node_as_range_to( \
        struct cparse_regex_ast_node_group_node *node);

/* AST Node : Group */
struct cparse_regex_ast_node_group *
cparse_regex_ast_node_group_new(cparse_bool exclude);
void cparse_regex_ast_node_group_destroy( \
        struct cparse_regex_ast_node_group *group);
void cparse_regex_ast_node_group_push_back( \
        struct cparse_regex_ast_node_group *group, \
        struct cparse_regex_ast_node_group_node *new_node);
struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_first( \
        struct cparse_regex_ast_node_group *group);
cparse_bool
cparse_regex_ast_node_group_exclude( \
        struct cparse_regex_ast_node_group *group);

/* AST Node */

typedef enum
{
    CPARSE_REGEX_AST_NODE_TYPE_ANY = 0,
    CPARSE_REGEX_AST_NODE_TYPE_CHAR,
    CPARSE_REGEX_AST_NODE_TYPE_SEQ,
    CPARSE_REGEX_AST_NODE_TYPE_GROUP,
    CPARSE_REGEX_AST_NODE_TYPE_BRANCH,
} cparse_regex_ast_node_type_t;

struct cparse_regex_ast_node *cparse_regex_ast_node_new_any(void);
struct cparse_regex_ast_node *cparse_regex_ast_node_new_char( \
        cparse_char_t ch);
struct cparse_regex_ast_node *cparse_regex_ast_node_new_seq( \
        struct cparse_regex_ast_node_seq *seq);
struct cparse_regex_ast_node *cparse_regex_ast_node_new_group( \
        struct cparse_regex_ast_node_group *group);
struct cparse_regex_ast_node *cparse_regex_ast_node_new_branch( \
        struct cparse_regex_ast_node_branch *branch);
void cparse_regex_ast_node_destroy( \
        struct cparse_regex_ast_node *node);

cparse_regex_ast_node_type_t cparse_regex_ast_node_type( \
        struct cparse_regex_ast_node *node);

cparse_char_t 
cparse_regex_ast_node_char(struct cparse_regex_ast_node *node);
struct cparse_regex_ast_node_seq *
cparse_regex_ast_node_seq(struct cparse_regex_ast_node *node);
struct cparse_regex_ast_node_group *
cparse_regex_ast_node_group(struct cparse_regex_ast_node *node);
struct cparse_regex_ast_node_branch *
cparse_regex_ast_node_branch(struct cparse_regex_ast_node *node);

#define CPARSE_REGEX_AST_NODE_HIGH_BOUND_UNLIMITED (-1)
void cparse_regex_ast_node_set_high_bound(struct cparse_regex_ast_node *node, int high_bound);
void cparse_regex_ast_node_set_low_bound(struct cparse_regex_ast_node *node, int low_bound);
int cparse_regex_ast_node_high_bound(struct cparse_regex_ast_node *node);
int cparse_regex_ast_node_low_bound(struct cparse_regex_ast_node *node);
void cparse_regex_ast_node_set_greed(struct cparse_regex_ast_node *node, cparse_bool greed);
cparse_bool cparse_regex_ast_node_greed(struct cparse_regex_ast_node *node);


/* AST */
struct cparse_regex_ast *cparse_regex_ast_new(struct cparse_regex_ast_node *root);
void cparse_regex_ast_destroy(struct cparse_regex_ast *ast);
struct cparse_regex_ast_node *cparse_regex_ast_root(struct cparse_regex_ast *ast);


#ifdef __cplusplus
}
#endif

#endif

