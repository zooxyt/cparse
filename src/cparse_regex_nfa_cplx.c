/* CParse : Regex : Matching : Complex
 * Copyright(c) Cheryl Natsu */

#include <stdarg.h>
#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_regex_nfa_cplx.h"

struct cparse_regex_nfa_state_condition_complex_node_list;

static void cparse_regex_nfa_state_condition_complex_node_list_destroy( \
struct cparse_regex_nfa_state_condition_complex_node_list *node_list);

/* Complex Condition */

struct cparse_regex_nfa_state_condition_complex_range
{
    cparse_char_t ch_from, ch_to;
};

struct cparse_regex_nfa_state_condition_complex_node
{
    cparse_regex_nfa_state_condition_complex_node_type_t type;
    union
    {
        cparse_char_t part_char;
        struct cparse_regex_nfa_state_condition_complex_range part_range;
        struct cparse_regex_nfa_state_condition_complex_node_list *part_and;
        struct cparse_regex_nfa_state_condition_complex_node_list *part_or;
        struct cparse_regex_nfa_state_condition_complex_node_list *part_seq;
        struct cparse_regex_nfa_state_condition_complex_node *part_not;
    } u;
    struct cparse_regex_nfa_state_condition_complex_node *next;
};

struct cparse_regex_nfa_state_condition_complex_node_list
{
    struct cparse_regex_nfa_state_condition_complex_node *begin, *end;
    cparse_size_t size;
};

struct cparse_regex_nfa_state_condition_complex
{
    struct cparse_regex_nfa_state_condition_complex_node *node;
    cparse_bool greedy;
    int rep_min, rep_max;
};

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new( \
        cparse_regex_nfa_state_condition_complex_node_type_t type)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = (struct cparse_regex_nfa_state_condition_complex_node *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa_state_condition_complex_node))) == NULL)
    { return NULL; }
    new_node->type = type;
    new_node->next = NULL;
    switch (type)
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_CHAR:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_DIRECT:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_ANY:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_RANGE:
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND:
            new_node->u.part_and = NULL;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR:
            new_node->u.part_or = NULL;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_SEQ:
            new_node->u.part_seq = NULL;
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_NOT:
            new_node->u.part_not = NULL;
            break;
    }

    return new_node;
}

void cparse_regex_nfa_state_condition_complex_node_destroy( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    switch (node->type)
    {
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_CHAR:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_DIRECT:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_ANY:
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_RANGE:
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND:
            if (node->u.part_and != NULL)
            { cparse_regex_nfa_state_condition_complex_node_list_destroy(node->u.part_and); }
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR:
            if (node->u.part_or != NULL)
            { cparse_regex_nfa_state_condition_complex_node_list_destroy(node->u.part_or); }
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_SEQ:
            if (node->u.part_seq != NULL)
            { cparse_regex_nfa_state_condition_complex_node_list_destroy(node->u.part_seq); }
            break;
        case CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_NOT:
            if (node->u.part_not != NULL)
            { cparse_regex_nfa_state_condition_complex_node_destroy(node->u.part_not); }
            break;
    }
    cparse_free(node);
}

static struct cparse_regex_nfa_state_condition_complex_node_list *
cparse_regex_nfa_state_condition_complex_node_list_new(void)
{
    struct cparse_regex_nfa_state_condition_complex_node_list *new_list = NULL;

    if ((new_list = (struct cparse_regex_nfa_state_condition_complex_node_list *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa_state_condition_complex_node_list))) == NULL)
    { return NULL; }

    new_list->begin = new_list->end = NULL;
    new_list->size = 0;

    return new_list;
}

static void cparse_regex_nfa_state_condition_complex_node_list_destroy( \
struct cparse_regex_nfa_state_condition_complex_node_list *node_list)
{
    struct cparse_regex_nfa_state_condition_complex_node *node_cur, *node_next; 

    node_cur = node_list->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_regex_nfa_state_condition_complex_node_destroy(node_cur);
        node_cur = node_next;
    }
    cparse_free(node_list);
}

static void cparse_regex_nfa_state_condition_complex_node_list_push_back( \
struct cparse_regex_nfa_state_condition_complex_node_list *node_list, \
struct cparse_regex_nfa_state_condition_complex_node *new_node)
{
    if (node_list->begin == NULL)
    {
        node_list->begin = node_list->end = new_node;
    }
    else
    {
        node_list->end->next = new_node;
        node_list->end = new_node;
    }
    node_list->size++;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_char(cparse_char_t ch)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_CHAR)) == NULL)
    { return NULL; }
    new_node->u.part_char = ch;

    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_direct(void)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_DIRECT)) == NULL)
    { return NULL; }

    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_any(void)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_ANY)) == NULL)
    { return NULL; }

    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_range(cparse_char_t ch_from, cparse_char_t ch_to)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_RANGE)) == NULL)
    { return NULL; }
    new_node->u.part_range.ch_from = ch_from;
    new_node->u.part_range.ch_to = ch_to;

    return new_node;
}

static struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_list_category( \
        cparse_regex_nfa_state_condition_complex_node_type_t type, \
        struct cparse_regex_nfa_state_condition_complex_node *first, \
        va_list ap)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;
    struct cparse_regex_nfa_state_condition_complex_node *node_cur = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new(type)) == NULL)
    { return NULL; }
    if ((new_node->u.part_seq = cparse_regex_nfa_state_condition_complex_node_list_new()) == NULL)
    { goto fail; }
    
    if (first == NULL) return NULL;
    cparse_regex_nfa_state_condition_complex_node_list_push_back( \
            new_node->u.part_and, node_cur);

    for (;;)
    {
        node_cur = va_arg(ap, struct cparse_regex_nfa_state_condition_complex_node *);
        if (node_cur == NULL) break;
        cparse_regex_nfa_state_condition_complex_node_list_push_back( \
                new_node->u.part_and, node_cur);
    }

    goto done;
fail:
    if (new_node == NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_node);
        new_node = NULL;
    }
done:
    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_not( \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_NOT)) == NULL)
    { return NULL; }
    new_node->u.part_not = node;

    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_and_preset( \
        struct cparse_regex_nfa_state_condition_complex_node *first, ...)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;
    va_list ap;
    va_start(ap, first);
    new_node = cparse_regex_nfa_state_condition_complex_node_new_list_category( \
            CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND, \
            first, ap);
    va_end(ap);
    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_or_preset( \
        struct cparse_regex_nfa_state_condition_complex_node *first, ...)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;
    va_list ap;
    va_start(ap, first);
    new_node = cparse_regex_nfa_state_condition_complex_node_new_list_category( \
            CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR, \
            first, ap);
    va_end(ap);
    return new_node;
}


struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_and(void)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_AND)) == NULL)
    { return NULL; }
    if ((new_node->u.part_and = cparse_regex_nfa_state_condition_complex_node_list_new()) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_node);
        new_node = NULL;
    }
done:
    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_or(void)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_OR)) == NULL)
    { return NULL; }
    if ((new_node->u.part_or = cparse_regex_nfa_state_condition_complex_node_list_new()) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_node);
        new_node = NULL;
    }
done:
    return new_node;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_new_seq(void)
{
    struct cparse_regex_nfa_state_condition_complex_node *new_node = NULL;

    if ((new_node = cparse_regex_nfa_state_condition_complex_node_new( \
                    CPARSE_REGEX_NFA_STATE_CONDITION_COMPLEX_NODE_TYPE_SEQ)) == NULL)
    { return NULL; }
    if ((new_node->u.part_seq = cparse_regex_nfa_state_condition_complex_node_list_new()) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_node != NULL)
    {
        cparse_regex_nfa_state_condition_complex_node_destroy(new_node);
        new_node = NULL;
    }
done:
    return new_node;
}

cparse_regex_nfa_state_condition_complex_node_type_t
cparse_regex_nfa_state_condition_complex_node_type( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->type;
}

cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_char( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_char;
}

cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_range_from( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_range.ch_from;
}

cparse_char_t
cparse_regex_nfa_state_condition_complex_node_as_range_to( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_range.ch_to;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_not( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_not;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_and_first( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_and->begin;
}

void cparse_regex_nfa_state_condition_complex_node_as_and_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node)
{
    cparse_regex_nfa_state_condition_complex_node_list_push_back( \
            node->u.part_and, sub_node);
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_or_first( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_or->begin;
}

void cparse_regex_nfa_state_condition_complex_node_as_or_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node)
{
    cparse_regex_nfa_state_condition_complex_node_list_push_back( \
            node->u.part_or, sub_node);
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_as_seq_first( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->u.part_seq->begin;
}

void cparse_regex_nfa_state_condition_complex_node_as_seq_push_back( \
struct cparse_regex_nfa_state_condition_complex_node *node, \
struct cparse_regex_nfa_state_condition_complex_node *sub_node)
{
    cparse_regex_nfa_state_condition_complex_node_list_push_back( \
            node->u.part_seq, sub_node);
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_node_next( \
struct cparse_regex_nfa_state_condition_complex_node *node)
{
    return node->next;
}

struct cparse_regex_nfa_state_condition_complex *
cparse_regex_nfa_state_condition_complex_new( \
        struct cparse_regex_nfa_state_condition_complex_node *node)
{
    struct cparse_regex_nfa_state_condition_complex *new_complex = NULL;

    if ((new_complex = (struct cparse_regex_nfa_state_condition_complex *)cparse_malloc( \
                    sizeof(struct cparse_regex_nfa_state_condition_complex))) == NULL)
    { return NULL; }
    new_complex->node = node;
    new_complex->rep_min = 1;
    new_complex->rep_max = 1;

    return new_complex;
}

void cparse_regex_nfa_state_condition_complex_destroy( \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    if (cplx->node != NULL) cparse_regex_nfa_state_condition_complex_node_destroy(cplx->node);
    cparse_free(cplx);
}

void cparse_regex_nfa_state_condition_complex_set_rep_min( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        int rep_min)
{
    cplx->rep_min = rep_min;
}

void cparse_regex_nfa_state_condition_complex_set_rep_max( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        int rep_max)
{
    cplx->rep_max = rep_max;
}

int cparse_regex_nfa_state_condition_complex_rep_min( \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    return cplx->rep_min;
}

int cparse_regex_nfa_state_condition_complex_rep_max( \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    return cplx->rep_max;
}

void cparse_regex_nfa_state_condition_complex_set_greedy( \
        struct cparse_regex_nfa_state_condition_complex *cplx, \
        cparse_bool greedy)
{
    cplx->greedy = greedy;
}

cparse_bool cparse_regex_nfa_state_condition_complex_greedy( \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    return cplx->greedy;
}

struct cparse_regex_nfa_state_condition_complex_node *
cparse_regex_nfa_state_condition_complex_root( \
        struct cparse_regex_nfa_state_condition_complex *cplx)
{
    return cplx->node;
}

