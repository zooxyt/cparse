/* CParse : Regex : AST
 * Copyright(C) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_regex_ast.h"

struct cparse_regex_ast_node;

void cparse_regex_ast_node_destroy( \
        struct cparse_regex_ast_node *node);

struct cparse_regex_ast_node_seq_node
{
    struct cparse_regex_ast_node *node;

    struct cparse_regex_ast_node_seq_node *next;
};

struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_node_new( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_ast_node_seq_node *new_seq_node = NULL;

    if ((new_seq_node = (struct cparse_regex_ast_node_seq_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_seq_node))) == NULL)
    { return NULL; }
    new_seq_node->node = node;
    new_seq_node->next = NULL;

    return new_seq_node;
}

void cparse_regex_ast_node_seq_node_destroy( \
        struct cparse_regex_ast_node_seq_node *node)
{
    if (node->node != NULL)
    { cparse_regex_ast_node_destroy(node->node); }
    cparse_free(node);
}

struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_node_next( \
        struct cparse_regex_ast_node_seq_node *node)
{
    return node->next;
}

struct cparse_regex_ast_node *
cparse_regex_ast_node_seq_node_node( \
        struct cparse_regex_ast_node_seq_node *node)
{
    return node->node;
}

struct cparse_regex_ast_node_seq
{
    struct cparse_regex_ast_node_seq_node *begin, *end;

    cparse_size_t size;
};

struct cparse_regex_ast_node_seq *
cparse_regex_ast_node_seq_new(void)
{
    struct cparse_regex_ast_node_seq *new_node_seq = NULL;

    if ((new_node_seq = (struct cparse_regex_ast_node_seq *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_seq))) == NULL)
    { return NULL; }
    new_node_seq->begin = new_node_seq->end = NULL;
    new_node_seq->size = 0;

    return new_node_seq;
}

void cparse_regex_ast_node_seq_destroy( \
        struct cparse_regex_ast_node_seq *node_seq)
{
    struct cparse_regex_ast_node_seq_node *node_cur, *node_next;

    node_cur = node_seq->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_regex_ast_node_seq_node_destroy(node_cur);
        node_cur = node_next;
    }

    cparse_free(node_seq);
}

void cparse_regex_ast_node_seq_push_back( \
        struct cparse_regex_ast_node_seq *node_seq, \
        struct cparse_regex_ast_node_seq_node *new_node)
{
    if (node_seq->begin == NULL)
    {
        node_seq->begin = node_seq->end = new_node;
    }
    else
    {
        node_seq->end->next = new_node;
        node_seq->end = new_node;
    }
    node_seq->size++;
}

struct cparse_regex_ast_node_seq_node *
cparse_regex_ast_node_seq_first( \
        struct cparse_regex_ast_node_seq *node_seq)
{
    return node_seq->begin;
}


struct cparse_regex_ast_node_branch_node
{
    struct cparse_regex_ast_node *node;

    struct cparse_regex_ast_node_branch_node *next;
};

struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_node_new( \
        struct cparse_regex_ast_node *node)
{
    struct cparse_regex_ast_node_branch_node *new_node = NULL;

    if ((new_node = (struct cparse_regex_ast_node_branch_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_branch_node))) == NULL)
    { return NULL; }
    new_node->node = node;
    new_node->next = NULL;

    return new_node;
}

void cparse_regex_ast_node_branch_node_destroy( \
        struct cparse_regex_ast_node_branch_node *node)
{
    if (node->node != NULL)
    { cparse_regex_ast_node_destroy(node->node); }
    cparse_free(node);
}

struct cparse_regex_ast_node *
cparse_regex_ast_node_branch_node_node( \
        struct cparse_regex_ast_node_branch_node *node)
{
    return node->node;
}

struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_node_next( \
        struct cparse_regex_ast_node_branch_node *node)
{
    return node->next;
}

struct cparse_regex_ast_node_branch
{
    struct cparse_regex_ast_node_branch_node *begin, *end;
    cparse_size_t size;
};

struct cparse_regex_ast_node_branch *cparse_regex_ast_node_branch_new(void)
{
    struct cparse_regex_ast_node_branch *new_branch = NULL;

    if ((new_branch = (struct cparse_regex_ast_node_branch *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_branch))) == NULL)
    { return NULL; }
    new_branch->begin = new_branch->end = NULL;
    new_branch->size = 0;

    return new_branch;
}

void cparse_regex_ast_node_branch_destroy( \
        struct cparse_regex_ast_node_branch *branch)
{
    struct cparse_regex_ast_node_branch_node *node_cur, *node_next;

    node_cur = branch->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_regex_ast_node_branch_node_destroy(node_cur);
        node_cur = node_next;
    }
    cparse_free(branch);
}

void cparse_regex_ast_node_branch_push_back( \
struct cparse_regex_ast_node_branch *branch, \
struct cparse_regex_ast_node_branch_node *new_branch_node)
{
    if (branch->begin == NULL)
    {
        branch->begin = branch->end = new_branch_node;
    }
    else
    {
        branch->end->next = new_branch_node;
        branch->end = new_branch_node;
    }
    branch->size++;
}

struct cparse_regex_ast_node_branch_node *
cparse_regex_ast_node_branch_first( \
        struct cparse_regex_ast_node_branch *branch)
{
    return branch->begin;
}

struct cparse_regex_ast_node_group_node_range
{
    cparse_char_t ch_from, ch_to;
};

struct cparse_regex_ast_node_group_node
{
    cparse_regex_ast_node_group_node_type_t type;
    union
    {
        cparse_char_t part_solo;
        struct cparse_regex_ast_node_group_node_range part_range;
    } u;
    struct cparse_regex_ast_node_group_node *next;
};

struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_new_solo(cparse_char_t ch)
{
    struct cparse_regex_ast_node_group_node *new_node = NULL;
    if ((new_node = (struct cparse_regex_ast_node_group_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_group_node))) == NULL)
    { return NULL; }
    new_node->type = CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_SOLO;
    new_node->u.part_solo = ch;
    new_node->next = NULL;

    return new_node;
}


struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_new_range(cparse_char_t ch_from, cparse_char_t ch_to)
{
    struct cparse_regex_ast_node_group_node *new_node = NULL;
    if ((new_node = (struct cparse_regex_ast_node_group_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_group_node))) == NULL)
    { return NULL; }
    new_node->type = CPARSE_REGEX_AST_NODE_GROUP_NODE_TYPE_RANGE;
    new_node->u.part_range.ch_from = ch_from;
    new_node->u.part_range.ch_to = ch_to;
    new_node->next = NULL;

    return new_node;
}

void cparse_regex_ast_node_group_node_destroy( \
        struct cparse_regex_ast_node_group_node *node)
{
    cparse_free(node);
}

cparse_regex_ast_node_group_node_type_t
cparse_regex_ast_node_group_node_type( \
        struct cparse_regex_ast_node_group_node *node)
{
    return node->type;
}

struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_node_next( \
        struct cparse_regex_ast_node_group_node *node)
{
    return node->next;
}

cparse_char_t cparse_regex_ast_node_group_node_as_solo( \
        struct cparse_regex_ast_node_group_node *node)
{
    return node->u.part_solo;
}

cparse_char_t cparse_regex_ast_node_group_node_as_range_from( \
        struct cparse_regex_ast_node_group_node *node)
{
    return node->u.part_range.ch_from;
}

cparse_char_t cparse_regex_ast_node_group_node_as_range_to( \
        struct cparse_regex_ast_node_group_node *node)
{
    return node->u.part_range.ch_to;
}


struct cparse_regex_ast_node_group
{
    cparse_bool exclude;
    struct cparse_regex_ast_node_group_node *begin, *end;
    cparse_size_t size;
};

struct cparse_regex_ast_node_group *cparse_regex_ast_node_group_new(cparse_bool exclude)
{
    struct cparse_regex_ast_node_group *new_group = NULL;

    if ((new_group = (struct cparse_regex_ast_node_group *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node_group))) == NULL)
    { return NULL; }
    new_group->exclude = exclude;
    new_group->begin = new_group->end = NULL;
    new_group->size = 0;

    return new_group;
}

void cparse_regex_ast_node_group_destroy(struct cparse_regex_ast_node_group *group)
{
    struct cparse_regex_ast_node_group_node *node_cur, *node_next;

    node_cur = group->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_regex_ast_node_group_node_destroy(node_cur);
        node_cur = node_next;
    }

    cparse_free(group);
}

void cparse_regex_ast_node_group_push_back( \
        struct cparse_regex_ast_node_group *group, \
        struct cparse_regex_ast_node_group_node *new_node)
{
    if (group->begin == NULL)
    {
        group->begin = group->end = new_node;
    }
    else
    {
        group->end->next = new_node;
        group->end = new_node;
    }
    group->size++;
}

struct cparse_regex_ast_node_group_node *
cparse_regex_ast_node_group_first( \
        struct cparse_regex_ast_node_group *group)
{
    return group->begin;
}

cparse_bool
cparse_regex_ast_node_group_exclude( \
        struct cparse_regex_ast_node_group *group)
{
    return group->exclude;
}

struct cparse_regex_ast_node
{
    cparse_regex_ast_node_type_t type;
    union
    {
        struct cparse_regex_ast_node_seq *part_seq;
        struct cparse_regex_ast_node_group *part_group;
        struct cparse_regex_ast_node_branch *part_branch;
        cparse_char_t part_char;
    } u;
    int low_bound, high_bound;
    cparse_bool greed;
};

static struct cparse_regex_ast_node *cparse_regex_ast_node_new( \
        cparse_regex_ast_node_type_t type)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = (struct cparse_regex_ast_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast_node))) == NULL)
    { return NULL; }
    new_node->type = type;
    switch (type)
    {
        case CPARSE_REGEX_AST_NODE_TYPE_ANY:
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_CHAR:
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_SEQ:
            new_node->u.part_seq = NULL;
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_GROUP:
            new_node->u.part_group = NULL;
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_BRANCH:
            new_node->u.part_branch = NULL;
            break;
    }
    new_node->low_bound = 1;
    new_node->high_bound = 1;
    new_node->greed = cparse_true;

    return new_node;
}

struct cparse_regex_ast_node *cparse_regex_ast_node_new_any(void)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = cparse_regex_ast_node_new( \
                    CPARSE_REGEX_AST_NODE_TYPE_ANY)) == NULL)
    { return NULL; }

    return new_node;
}

struct cparse_regex_ast_node *cparse_regex_ast_node_new_char( \
        cparse_char_t ch)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = cparse_regex_ast_node_new( \
                    CPARSE_REGEX_AST_NODE_TYPE_CHAR)) == NULL)
    { return NULL; }
    new_node->u.part_char = ch;

    return new_node;
}

struct cparse_regex_ast_node *cparse_regex_ast_node_new_seq( \
        struct cparse_regex_ast_node_seq *seq)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = cparse_regex_ast_node_new( \
                    CPARSE_REGEX_AST_NODE_TYPE_SEQ)) == NULL)
    { return NULL; }
    new_node->u.part_seq = seq;

    return new_node;
}

struct cparse_regex_ast_node *cparse_regex_ast_node_new_group( \
        struct cparse_regex_ast_node_group *group)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = cparse_regex_ast_node_new( \
                    CPARSE_REGEX_AST_NODE_TYPE_GROUP)) == NULL)
    { return NULL; }
    new_node->u.part_group = group;

    return new_node;
}

struct cparse_regex_ast_node *cparse_regex_ast_node_new_branch( \
        struct cparse_regex_ast_node_branch *branch)
{
    struct cparse_regex_ast_node *new_node = NULL;

    if ((new_node = cparse_regex_ast_node_new( \
                    CPARSE_REGEX_AST_NODE_TYPE_BRANCH)) == NULL)
    { return NULL; }
    new_node->u.part_branch = branch;

    return new_node;
}

void cparse_regex_ast_node_destroy( \
        struct cparse_regex_ast_node *node)
{
    switch (node->type)
    {
        case CPARSE_REGEX_AST_NODE_TYPE_ANY:
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_CHAR:
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_SEQ:
            if (node->u.part_seq != NULL)
            { cparse_regex_ast_node_seq_destroy(node->u.part_seq); }
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_GROUP:
            if (node->u.part_group != NULL)
            { cparse_regex_ast_node_group_destroy(node->u.part_group); }
            break;
        case CPARSE_REGEX_AST_NODE_TYPE_BRANCH:
            if (node->u.part_branch)
            { cparse_regex_ast_node_branch_destroy(node->u.part_branch); }
            break;
    }
    cparse_free(node);
}

cparse_regex_ast_node_type_t cparse_regex_ast_node_type( \
        struct cparse_regex_ast_node *node)
{
    return node->type;
}

cparse_char_t 
cparse_regex_ast_node_char(struct cparse_regex_ast_node *node)
{
    return node->u.part_char;
}

struct cparse_regex_ast_node_seq *
cparse_regex_ast_node_seq(struct cparse_regex_ast_node *node)
{
    return node->u.part_seq;
}

struct cparse_regex_ast_node_group *
cparse_regex_ast_node_group(struct cparse_regex_ast_node *node)
{
    return node->u.part_group;
}

struct cparse_regex_ast_node_branch *
cparse_regex_ast_node_branch(struct cparse_regex_ast_node *node)
{
    return node->u.part_branch;
}

struct cparse_regex_ast
{
    struct cparse_regex_ast_node *root;
};

void cparse_regex_ast_node_set_high_bound(struct cparse_regex_ast_node *node, int high_bound)
{
    node->high_bound = high_bound;
}

void cparse_regex_ast_node_set_low_bound(struct cparse_regex_ast_node *node, int low_bound)
{
    node->low_bound = low_bound;
}

int cparse_regex_ast_node_high_bound(struct cparse_regex_ast_node *node)
{
    return node->high_bound;
}

int cparse_regex_ast_node_low_bound(struct cparse_regex_ast_node *node)
{
    return node->low_bound;
}

void cparse_regex_ast_node_set_greed(struct cparse_regex_ast_node *node, cparse_bool greed)
{
    node->greed = greed;
}

cparse_bool cparse_regex_ast_node_greed(struct cparse_regex_ast_node *node)
{
    return node->greed;
}


struct cparse_regex_ast *cparse_regex_ast_new(struct cparse_regex_ast_node *root)
{
    struct cparse_regex_ast *new_ast = NULL;

    if ((new_ast = (struct cparse_regex_ast *)cparse_malloc( \
                    sizeof(struct cparse_regex_ast))) == NULL)
    { return NULL; }
    new_ast->root = root;

    return new_ast;
}

void cparse_regex_ast_destroy(struct cparse_regex_ast *ast)
{
    if (ast->root != NULL) cparse_regex_ast_node_destroy(ast->root);
    cparse_free(ast);
}

struct cparse_regex_ast_node *cparse_regex_ast_root(struct cparse_regex_ast *ast)
{
    return ast->root;
}

