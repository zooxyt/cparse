/* CParse 
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_err.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_reader.h"
#include "cparse_state.h"
#include "cparse_fileloc.h"
#include "cparse_component.h"
#include "cparse_value_list.h"
#include "cparse_value_map.h"
#include "cparse_value_set.h"
#include "cparse_value_string.h"
#include "cparse_regex.h"
#include "cparse.h"

static int cparse_perform_string_raw(cparse_state_t *state, \
        cparse_char_t *p, \
        cparse_size_t len, \
        cparse_value_t **value_out);

struct cparse
{
    /* AST Constructor */
    cparse_ast_ctor_t ast_ctor;
    /* AST Destructor */
    cparse_ast_dtor_t ast_dtor;
    /* Entry Point */ 
    cparse_component_t *entry_point;
};

/* Initialize CParse with string */
cparse_t *cparse_new(void)
{
    cparse_t *new_cparse = NULL;

    if ((new_cparse = (cparse_t *)cparse_malloc( \
                    sizeof(cparse_t))) == NULL)
    { return NULL; }
    new_cparse->ast_ctor = NULL;
    new_cparse->ast_dtor = NULL;
    new_cparse->entry_point = NULL;

    return new_cparse;
}

void cparse_destroy(cparse_t *cparse)
{
    cparse_free(cparse);
}

void cparse_set_ast_ctor(cparse_t *cparse, cparse_ast_ctor_t ast_ctor)
{
    cparse->ast_ctor = ast_ctor;
}

void cparse_set_ast_dtor(cparse_t *cparse, cparse_ast_dtor_t ast_dtor)
{
    cparse->ast_dtor = ast_dtor;
}

void cparse_set_entry_point(cparse_t *cparse, cparse_component_t *entry_point)
{
    cparse->entry_point = entry_point;
}

/* Components */
cparse_component_t *cparse_component_define_in( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln, \
        cparse_component_t *component)
{
    cparse_value_map_t *components = cparse_state_components(state);
    cparse_component_t *exist_component;
    cparse_fileloc_t *new_fileloc = NULL;

    if ((new_fileloc = cparse_fileloc_new(filename, ln)) == NULL)
    {
        cparse_component_destroy(component);
        return NULL; 
    }

    if ((exist_component = cparse_value_map_get(components, new_fileloc)) != NULL)
    { 
        cparse_component_destroy(component);
        cparse_fileloc_destroy(new_fileloc);
        return exist_component;
    }

    cparse_value_map_set(components, new_fileloc, component);

    return component;
}

cparse_bool cparse_component_defined( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln, \
        cparse_component_t *component)
{
    cparse_value_map_t *components = cparse_state_components(state);
    cparse_fileloc_t *new_fileloc = NULL;
    cparse_bool is_defined = cparse_false;

    (void)component;

    if ((new_fileloc = cparse_fileloc_new(filename, ln)) == NULL)
    { return cparse_false; }
    is_defined = cparse_value_map_get(components, new_fileloc) != NULL ? cparse_true : cparse_false;

    cparse_fileloc_destroy(new_fileloc);

    return is_defined;
}

int cparse_component_once( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln)
{
    cparse_value_set_t *blocks = cparse_state_blocks(state);
    cparse_fileloc_t *new_fileloc = NULL;
    cparse_bool is_defined = cparse_false;

    if ((new_fileloc = cparse_fileloc_new(filename, ln)) == NULL)
    { return 0; }
    is_defined = cparse_value_set_exist(blocks, new_fileloc);
    if (is_defined == cparse_true)
    { cparse_fileloc_destroy(new_fileloc); }
    else
    { cparse_value_set_set(blocks, new_fileloc); }

    return is_defined == cparse_true ? 1 : 0;
}


/* Framework */

static int cparse_perform_regex(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_regex_t *regex = cparse_component_as_regex_get(component);
    cparse_char_t *p = NULL;
    
    p = cparse_reader_ptr(reader);
    cparse_state_save(state);

    if (cparse_regex_match(regex, state) != 0)
    {
        /* Not matched */
        cparse_state_load(state);
        ret = -1;
        goto fail;
    }
    if ((*value_out = cparse_value_new_string_wide(p, \
                    (cparse_size_t)(cparse_reader_ptr(reader) - p))) == NULL)
    { ret = -1; goto fail; }
    cparse_state_erase(state);

    goto done;
fail:
done:
    return ret;
}


static int cparse_perform_expr_step(cparse_state_t *state, \
        cparse_expr_t *expr, \
        cparse_value_t **value_out, \
        int precedence);

static int cparse_perform_expr_step_non_follow_associate_check(cparse_state_t *state, \
        cparse_expr_associative_t *layout_associative, \
        cparse_expr_associative_t associative_cur)
{
    int ret = 0;

    if (*layout_associative == CPARSE_EXPR_ASSOCIATIVE_UNDEFINED)
    { *layout_associative = associative_cur; }
    else if (*layout_associative != associative_cur)
    {
        cparse_err_update_description_printf( \
                cparse_state_err(state), \
                "can not mix left associative and right associative "
                "in the same infix expression");
        ret = -1;
    }
    return ret;
}

static int cparse_perform_expr_step_non_follow(cparse_state_t *state, \
        cparse_expr_t *expr, \
        cparse_value_t **value_out, \
        int precedence)
{
    int ret = 0, tmp_ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);

    /* We can ensure all operators in the same precedence are the same */
    cparse_expr_operator_type_t precedence_op_type = cparse_expr_precedence_op_type(expr, precedence);
    /* The next precedence */
    int precedence_next = cparse_expr_next_precedence(expr, precedence);
    /* Operator Iterator */
    cparse_expr_operator_t *op_cur;
    /* Associative in this layout */
    cparse_expr_associative_t layout_associative = CPARSE_EXPR_ASSOCIATIVE_UNDEFINED;

    /* Temporary Values */
    cparse_value_t *first = NULL, *second = NULL, *third = NULL, *tmp_op = NULL;

    /* TODO: Support infix (neither infixl or infixr) */

    /* TODO: Support prefix and postfix operators */
    if ((precedence_op_type == CPARSE_EXPR_OPERATOR_TYPE_PREFIX) || 
            (precedence_op_type == CPARSE_EXPR_OPERATOR_TYPE_POSTFIX))
    {
        cparse_err_update_internal(cparse_state_err(state));
        ret = -1; 
        goto fail;
    }
    else
    {
        /* Current the only possible value of 'op_cur_type'
         * is CPARSE_EXPR_OPERATOR_TYPE_INFIX */
    }

    /* Left Node
     * The first thing to deal with whatever the associative is */
    cparse_reader_save(reader);
    ret = cparse_perform_expr_step(state, expr, &first, precedence_next);
    if (cparse_err_occurred(cparse_state_err(state)))
    { ret = -1; goto fail; }
    if (ret == 0)
    {
        /* Parsed the left node without error,
         * continue finding the possible operator */ 
        cparse_state_erase(state); 
    }
    else
    {
        /* Failed parsing the left node */
        cparse_state_load(state); 
        cparse_state_save(state); 
        goto fail;
    }

retry:
    /* Save the state before 'operator' and 'right node' */
    cparse_reader_save(reader);

    /* Try to match an operator */
    op_cur = cparse_expr_op_first(expr, precedence);
    while (op_cur != NULL)
    {
        if (cparse_expr_op_associative(op_cur) == CPARSE_EXPR_ASSOCIATIVE_LEFT)
        {
            /* Left associative */
            cparse_perform_expr_step_non_follow_associate_check( \
                    state, \
                    &layout_associative, \
                    CPARSE_EXPR_ASSOCIATIVE_LEFT);

            /* Try infix operator */
            if ((cparse_perform_string_raw( \
                            state, \
                            cparse_expr_op_str_wide(op_cur), \
                            cparse_unicode_string_strlen(cparse_expr_op_str_wide(op_cur)), \
                            &tmp_op)) != 0)
            { cparse_state_load(state); cparse_state_save(state); goto next_operator; }
            cparse_value_destroy(tmp_op); tmp_op = NULL;

            /* Try right node */
            tmp_ret = cparse_perform_expr_step(state, expr, &second, precedence_next);
            if (cparse_err_occurred(cparse_state_err(state)))
            { ret = -1; goto fail; }
            if (tmp_ret != 0)
            { cparse_state_load(state); cparse_state_save(state); goto next_operator; }

            /* Assemble */
            if ((third = (cparse_expr_op_assembler_binary(op_cur))(first, second)) == NULL)
            { cparse_err_update_out_of_memory(cparse_state_err(state)); ret = -1; goto fail; }
            cparse_value_destroy(first); first = NULL;
            cparse_value_destroy(second); second = NULL;
            first = third; third = NULL;
            cparse_state_erase(state); 

            /* Retry from the first operator */
            goto retry;
        }
        else
        {
            /* Right and none associative */
            cparse_perform_expr_step_non_follow_associate_check( \
                    state, \
                    &layout_associative, \
                    CPARSE_EXPR_ASSOCIATIVE_RIGHT);

            /* Try infix operator */
            if ((cparse_perform_string_raw( \
                            state, \
                            cparse_expr_op_str_wide(op_cur), \
                            cparse_unicode_string_strlen(cparse_expr_op_str_wide(op_cur)), \
                            &tmp_op)) != 0)
            { cparse_state_load(state); cparse_state_save(state); goto next_operator; }
            cparse_value_destroy(tmp_op); tmp_op = NULL;

            /* Try right node */
            tmp_ret = cparse_perform_expr_step(state, expr, &second, precedence);
            if (cparse_err_occurred(cparse_state_err(state)))
            { ret = -1; goto fail; }
            if (tmp_ret != 0)
            { cparse_state_load(state); cparse_state_save(state); goto next_operator; }

            /* Assemble */
            if ((third = (cparse_expr_op_assembler_binary(op_cur))(first, second)) == NULL)
            { cparse_err_update_out_of_memory(cparse_state_err(state)); ret = -1; goto fail; }
            cparse_value_destroy(first); first = NULL;
            cparse_value_destroy(second); second = NULL;
            first = third; third = NULL;
            cparse_state_erase(state); 

            /* Retry from the first operator */
            goto retry;
        }

next_operator:
        /* Next operator */
        op_cur = cparse_expr_op_next(op_cur);
    }
    if (first != NULL)
    {
        ret = 0;
        *value_out = first; first = NULL; 
    }
    else
    {
        ret = -1;
    }

    goto done;
fail:
    if (first != NULL) { cparse_value_destroy(first); }
    if (second != NULL) { cparse_value_destroy(second); }
done:
    return ret;
}

static int cparse_perform_expr_step_follow(cparse_state_t *state, \
        cparse_expr_t *expr, \
        cparse_value_t **value_out)
{
    cparse_component_t *follow = cparse_expr_follow(expr);

    /* No follow, stop continue trying */
    if (follow == NULL) { return -1; }

    return cparse_perform(state, value_out, follow);
}

static int cparse_perform_expr_step(cparse_state_t *state, \
        cparse_expr_t *expr, \
        cparse_value_t **value_out, \
        int precedence)
{
    int ret = 0;

    if (precedence == CPARSE_EXPR_PRECEDENCE_EMPTY)
    { ret = cparse_perform_expr_step_follow(state, expr, value_out); }
    else
    { ret = cparse_perform_expr_step_non_follow(state, expr, value_out, precedence); }

    return ret;
}

static int cparse_perform_expr(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_expr_t *expr = cparse_component_as_expr_get(component);
    int precedence = cparse_expr_first_precedence(expr);
    return cparse_perform_expr_step(state, expr, value_out, precedence);
}

#define CPARSE_IS_SPACE(ch) \
    (((ch) == '\t') || ((ch) == '\n') || \
     ((ch) == '\r') || ((ch) == '\f') || \
     ((ch) == '\v') || ((ch) == ' '))

static int cparse_perform_space(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_char_t ch;

    (void)component;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
    if (CPARSE_IS_SPACE(ch))
    {
        if ((*value_out = cparse_value_new_char(ch)) == NULL)
        { return -1; }
        cparse_reader_forward(reader);
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

static int cparse_perform_endofline(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_char_t ch;

    (void)component;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
    if (ch == '\n')
    {
        cparse_reader_forward(reader);
        if ((*value_out = cparse_value_new_char('\n')) == NULL)
        { return -1; }
        ret = 0;
    }
    else if (ch == '\r')
    {
        cparse_reader_forward(reader);
        if (cparse_reader_eof(reader) != cparse_true)
        {
            ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
            if (ch == '\n')
            { cparse_reader_forward(reader); }
        }

        if ((*value_out = cparse_value_new_char('\n')) == NULL)
        { return -1; }
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

static int cparse_perform_anychar(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_char_t ch;

    (void)component;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
    cparse_reader_forward(reader);
    if ((*value_out = cparse_value_new_char(ch)) == NULL)
    { return -1; }

    return ret;
}

static int cparse_perform_eof(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret;

    (void)component;

    ret = (cparse_reader_eof(cparse_state_reader(state)) == cparse_true) ? 0 : -1;
    if (ret == 0)
    {
        if ((*value_out = cparse_value_new_char(0)) == NULL)
        { return -1; }
    }

    return ret;
}

static int cparse_perform_spaces(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_char_t ch;
    cparse_char_t *p;

    (void)component;

    /* Record the position of first char */
    p = cparse_reader_ptr(reader);

    for (;;)
    {
        if (cparse_reader_eof(reader) == cparse_true)
        { break; }

        ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
        if (CPARSE_IS_SPACE(ch))
        { cparse_reader_forward(reader); }
        else
        {
            break;
        }
    }

    if ((*value_out = cparse_value_new_string_wide(p, \
                    (cparse_size_t)(cparse_reader_ptr(reader) - p))) == NULL)
    { return -1; }

    return 0;
}

static int cparse_perform_native(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_native_component_t callback = cparse_component_as_native_extract(component);
    return callback(state, value_out);
}

static int cparse_perform_char(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_value_char_t ch, ch_to_predict = (cparse_value_char_t)cparse_component_as_char_extract(component);
    cparse_reader_t *reader = cparse_state_reader(state);

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
    if (ch != ch_to_predict)
    { return -1; }

    if ((*value_out = cparse_value_new_char(ch)) == NULL)
    { return -1; }

    cparse_reader_forward(reader);

    return 0;
}

static int cparse_perform_string_raw(cparse_state_t *state, \
        cparse_char_t *p, \
        cparse_size_t len, \
        cparse_value_t **value_out)
{
    cparse_value_char_t ch;
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_char_t *p_cur = p;
    cparse_size_t len_remain = len;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    /* Save state */
    cparse_reader_save(reader);
    while (len_remain > 0)
    {
        if (cparse_reader_eof(reader) == cparse_true)
        { cparse_reader_load(reader); return -1; }
        ch = (cparse_value_char_t)cparse_reader_peek_char(reader);
        if ((cparse_value_char_t)ch != (cparse_value_char_t)*p_cur)
        { cparse_reader_load(reader); return -1; }

        cparse_reader_forward(reader);
        p_cur++; len_remain--;
    }
    cparse_reader_erase(reader);

    if ((*value_out = cparse_value_new_string_wide( \
                    p, \
                    len)) == NULL)
    { return -1; }

    return 0;
}

static int cparse_perform_string(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_value_string_t *string_to_predict = cparse_component_as_string_extract(component);
    cparse_char_t *p = cparse_value_string_str(string_to_predict);
    cparse_size_t len = cparse_value_string_length(string_to_predict);
    return cparse_perform_string_raw(state, p, len, value_out);
}

static int cparse_perform_many_with_min_requirement(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out, \
        cparse_size_t min_requirement)
{
    int retval;
    cparse_component_t *repeated_component = cparse_component_as_component_extract(component);
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value = NULL;
    cparse_size_t count = 0;

    if ((many_values = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { return -1; }

    /* Save state */
    cparse_state_save(state);

    for (;;)
    {
        retval = cparse_perform(state, &new_value, repeated_component);
        if (cparse_err_occurred(cparse_state_err(state))) { return -1; }
        if (retval != 0)
        {
            cparse_state_load(state);
            break;
        }
        else
        {
            count++;
            cparse_state_erase(state);
            cparse_state_save(state);
            cparse_value_as_list_push_back(many_values, new_value);
            new_value = NULL;
        }
    }

    if (count < min_requirement)
    {
        cparse_value_destroy(many_values);
        return -1;
    }

    *value_out = many_values;
    return 0;
}

static int cparse_perform_many(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_many_with_min_requirement( \
            state, component, value_out, 0);
}

static int cparse_perform_many1(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_many_with_min_requirement( \
            state, component, value_out, 1);
}

static int cparse_perform_munch(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_native_predict_t predict = cparse_component_as_predict_extract( \
            component);
    int retval = 0;
    int ch;
    cparse_char_t *p;

    /* Record the position of first char */
    p = cparse_reader_ptr(reader);

    /* Rest chars */
    while (cparse_reader_eof(reader) == cparse_false)
    {
        ch = cparse_reader_peek_char(reader);
        if (predict(ch) == cparse_false) { break; }
        cparse_reader_forward(reader);
    }

    if ((*value_out = cparse_value_new_string_wide(p, \
            (cparse_size_t)(cparse_reader_ptr(reader) - p))) == NULL)
    { return -1; }

    return retval;
}

static int cparse_perform_munch1(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_native_predict_t predict = cparse_component_as_predict_extract( \
            component);
    int retval = 0;
    int ch;
    cparse_char_t *p;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }

    /* Record the position of first char */
    p = cparse_reader_ptr(reader);

    /* First char */
    ch = cparse_reader_peek_char(reader);
    if (predict(ch) == cparse_false)
    { return -1; }
    cparse_reader_forward(reader);

    /* Rest chars */
    while (cparse_reader_eof(reader) == cparse_false)
    {
        ch = cparse_reader_peek_char(reader);
        if (predict(ch) == cparse_false) { break; }
        cparse_reader_forward(reader);
    }

    if ((*value_out = cparse_value_new_string_wide(p, \
                    (cparse_size_t)(cparse_reader_ptr(reader) - p))) == NULL)
    { return -1; }

    return retval;
}

static int cparse_perform_choice(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int retval = 0;
    struct cparse_value_list *components = NULL;
    cparse_value_list_node_t *component_node_cur;
    cparse_component_t *component_cur;

    components = cparse_component_as_component_list_extract(component);

    /* Save state */
    cparse_state_save(state);

    component_node_cur = cparse_value_list_first(components);
    while (component_node_cur != NULL)
    {
        component_cur = cparse_value_list_node_value(component_node_cur);
        retval = cparse_perform(state, value_out, component_cur);
        if (cparse_err_occurred(cparse_state_err(state))) { return -1; }
        /* Perform */
        if (retval == 0)
        {
            /* Parsed without error */ 
            cparse_state_erase(state); 
            return 0;
        }
        else
        {
            cparse_state_load(state); 
            cparse_state_save(state); 
        }

        /* Next component */
        component_node_cur = cparse_value_list_node_next(component_node_cur);
    }

    return -1;
}

typedef enum
{
    CPARSE_PERFORM_RANGE_MODE_NONGREED, \
    CPARSE_PERFORM_RANGE_MODE_GREED, \
} cparse_perform_range_mode_t;
static int cparse_perform_range(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    int retval = 0;
    cparse_size_t count_min, count_max;
    cparse_component_t *sub_component;
    cparse_size_t count = 0;
    cparse_perform_range_mode_t mode = CPARSE_PERFORM_RANGE_MODE_NONGREED;
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value = NULL;

    if ((many_values = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { return -1; }

    count_min = cparse_component_as_range_extract_min(component);
    count_max = cparse_component_as_range_extract_max(component);
    sub_component = cparse_component_as_range_extract_sub_component(component);
    if (cparse_component_as_range_extract_greed(component))
    { mode = CPARSE_PERFORM_RANGE_MODE_GREED; }

    /* Save state */
    cparse_state_save(state);

    for (;;)
    {
        /* Reach the low bound */
        if ((count >= count_min) && \
                (mode == CPARSE_PERFORM_RANGE_MODE_NONGREED))
        { break; }
        /* Exceed the high bound */
        if (count >= count_max) { break; }

        retval = cparse_perform(state, &new_value, sub_component);
        if (cparse_err_occurred(cparse_state_err(state))) { return -1; }
        if (retval != 0)
        {
            cparse_state_load(state);
            break;
        }
        else
        {
            cparse_state_erase(state);
            cparse_state_save(state);
            cparse_value_as_list_push_back(many_values, new_value);
            new_value = NULL;
            count++;
        }
    }

    if (count < count_min)
    {
        /* not reach the minimum requirement */

        /* TODO: Error Information */
        ret = -1;
        goto fail;
    }

    *value_out = many_values;
    goto done;
fail:
    if (many_values != NULL) cparse_value_destroy(many_values);
done:
    return ret;
}

static int cparse_perform_between(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    int retval = 0;
    cparse_component_t *component_open = cparse_component_as_between_component_open(component);
    cparse_component_t *component_close = cparse_component_as_between_component_close(component);
    cparse_component_t *component_sub = cparse_component_as_between_component_sub(component);
    cparse_value_t *new_value_to_ignore = NULL;
    cparse_value_t *new_value_sub = NULL;

    /* Save state */
    cparse_state_save(state);

    /* Open */
    retval = cparse_perform(state, &new_value_to_ignore, component_open);
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0) { cparse_state_load(state); ret = -1; goto fail; }
    cparse_value_destroy(new_value_to_ignore); new_value_to_ignore = NULL;

    /* Sub */
    retval = cparse_perform(state, &new_value_sub, component_sub);
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0) { cparse_state_load(state); ret = -1; goto fail; }

    /* Close */
    retval = cparse_perform(state, &new_value_to_ignore, component_close);
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0) { cparse_state_load(state); ret = -1; goto fail; }
    cparse_value_destroy(new_value_to_ignore); new_value_to_ignore = NULL;

    *value_out = new_value_sub; new_value_sub = NULL;
    cparse_state_erase(state);

    goto done;
fail:
    if (new_value_sub != NULL) cparse_value_destroy(new_value_sub);
done:
    return ret;
}

static int cparse_perform_option(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    int retval = 0;
    cparse_value_t *new_value = NULL;

    /* If it consumes some input */
    retval = cparse_perform(state, &new_value, \
            cparse_component_as_option_predict(component));
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0)
    {
        /* Not matched, return the prepared value */
        if ((new_value = cparse_value_clone( \
                        cparse_component_as_option_prepared(component))) == NULL)
        { ret = -1; goto fail; }
    }
    *value_out = new_value; new_value = NULL;

fail:
    return ret;
}

static int cparse_perform_sepby_raw(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out, \
        cparse_size_t min_requirement)
{
    int ret = 0;
    int retval = 0;
    cparse_value_t *new_value = NULL;
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value_sep = NULL;
    cparse_size_t count = 0;

    if ((many_values = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { return -1; }

    retval = cparse_perform(state, &new_value, \
            cparse_component_as_sepby_predict(component));
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0)
    {
        /* Not even match the first element */
        if (min_requirement == 0)
        { goto finish; }
        else
        { ret = -1; goto fail; }
    }
    if ((cparse_value_as_list_push_back(many_values, new_value)) != 0)
    { ret = -1; goto fail; }
    new_value = NULL;
    count = 1;

    /* Match the rest */
    for (;;)
    {
        /* Save state */
        cparse_state_save(state);

        /* sep */
        retval = cparse_perform(state, &new_value_sep, \
                cparse_component_as_sepby_sep(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) { break; }
        cparse_value_destroy(new_value_sep); new_value_sep = NULL;

        /* p */
        retval = cparse_perform(state, &new_value, \
                cparse_component_as_sepby_predict(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0)
        {
            cparse_state_load(state);
            break;
        }
        cparse_state_erase(state);
        if ((cparse_value_as_list_push_back(many_values, new_value)) != 0)
        { ret = -1; goto fail; }
        new_value = NULL;

        count++;
    }

    if (count < min_requirement)
    { ret = -1; goto fail; }
    
finish:
    *value_out = many_values; many_values = NULL;
    goto done;
fail:
    if (new_value_sep != NULL) { cparse_value_destroy(new_value_sep); }
    if (many_values != NULL) { cparse_value_destroy(many_values); }
done:
    if (new_value != NULL) { cparse_value_destroy(new_value); }
    return ret;
}

static int cparse_perform_sepby(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_sepby_raw(state, component, value_out, 0);
}

static int cparse_perform_sepby1(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_sepby_raw(state, component, value_out, 1);
}

static int cparse_perform_endby_raw(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out, \
        cparse_size_t min_requirement)
{
    int ret = 0;
    int retval = 0;
    cparse_value_t *new_value = NULL;
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value_sep = NULL;
    cparse_size_t count = 0;

    if ((many_values = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { return -1; }

    /* Match the rest */
    for (;;)
    {
        /* Save state */
        cparse_state_save(state);

        /* p */
        retval = cparse_perform(state, &new_value, \
                cparse_component_as_sepby_predict(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) { break; }

        /* sep */
        retval = cparse_perform(state, &new_value_sep, \
                cparse_component_as_sepby_sep(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) 
        {
            if (count >= min_requirement)
            {
                cparse_state_load(state);
                break; 
            }
            else
            {
                cparse_state_erase(state);
                ret = -1;
                goto fail;
            }
        }
        cparse_value_destroy(new_value_sep); new_value_sep = NULL;

        cparse_state_erase(state);
        if ((cparse_value_as_list_push_back(many_values, new_value)) != 0)
        { ret = -1; goto fail; }
        new_value = NULL;

        /* Increase counter */
        count++;
    }

    if (count < min_requirement)
    { ret = -1; goto fail; }
    
    *value_out = many_values; many_values = NULL;
    goto done;
fail:
    if (new_value_sep != NULL) { cparse_value_destroy(new_value_sep); }
    if (many_values != NULL) { cparse_value_destroy(many_values); }
done:
    if (new_value != NULL) { cparse_value_destroy(new_value); }
    return ret;
}

static int cparse_perform_endby(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_endby_raw(state, component, value_out, 0);
}

static int cparse_perform_endby1(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_endby_raw(state, component, value_out, 1);
}

static int cparse_perform_sependby_raw(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out, \
        cparse_size_t min_requirement)
{
    int ret = 0;
    int retval = 0;
    cparse_value_t *new_value = NULL;
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value_sep = NULL;
    cparse_size_t count = 0;

    if ((many_values = cparse_value_new_list(NULL, cparse_value_dtor)) == NULL)
    { return -1; }

    /* Match the rest */
    for (;;)
    {
        /* Save state */
        cparse_state_save(state);

        /* p */
        retval = cparse_perform(state, &new_value, \
                cparse_component_as_sepby_predict(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) { break; }
        /* Increase counter */
        count++;
        if ((cparse_value_as_list_push_back(many_values, new_value)) != 0)
        { ret = -1; goto fail; }
        new_value = NULL;

        /* sep */
        retval = cparse_perform(state, &new_value_sep, \
                cparse_component_as_sepby_sep(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) 
        {
            if (count >= min_requirement)
            {
                cparse_state_erase(state);
                break; 
            }
            else
            {
                cparse_state_erase(state);
                ret = -1;
                goto fail;
            }
        }
        cparse_value_destroy(new_value_sep); new_value_sep = NULL;

        cparse_state_erase(state);
    }

    if (count < min_requirement)
    { ret = -1; goto fail; }
    
    *value_out = many_values; many_values = NULL;
    goto done;
fail:
    if (many_values != NULL) { cparse_value_destroy(many_values); }
done:
    if (new_value_sep != NULL) { cparse_value_destroy(new_value_sep); }
    if (new_value != NULL) { cparse_value_destroy(new_value); }
    return ret;
}

static int cparse_perform_sependby(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_sependby_raw(state, component, value_out, 0);
}

static int cparse_perform_sependby1(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    return cparse_perform_sependby_raw(state, component, value_out, 1);
}

static int cparse_perform_manytill(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    int retval = 0;
    cparse_value_t *new_value = NULL;
    cparse_value_t *many_values = NULL;
    cparse_value_t *new_value_sep = NULL;

    if ((many_values = cparse_value_new_list(cparse_value_cctor, cparse_value_dtor)) == NULL)
    { return -1; }

    /* Match the rest */
    for (;;)
    {
        /* Save state */
        cparse_state_save(state);

        /* p */
        retval = cparse_perform(state, &new_value, \
                cparse_component_as_sepby_predict(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) { break; }

        if ((cparse_value_as_list_push_back(many_values, new_value)) != 0)
        { ret = -1; goto fail; }
        new_value = NULL;

        /* end */
        retval = cparse_perform(state, &new_value_sep, \
                cparse_component_as_sepby_sep(component));
        if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
        if (retval != 0) 
        {
            /* Not reach end, continue */
            cparse_state_erase(state);
        }
        else
        {
            /* Reach end */
            cparse_state_erase(state);
            goto finish;
        }
        if (new_value_sep != NULL)
        {
            cparse_value_destroy(new_value_sep);
            new_value_sep = NULL;
        }
    }

    /* Not seen end */
    ret = -1;
    goto fail;

finish:

    *value_out = many_values; many_values = NULL;
    goto done;
fail:
    if (many_values != NULL) { cparse_value_destroy(many_values); }
done:
    if (new_value_sep != NULL) { cparse_value_destroy(new_value_sep); }
    if (new_value != NULL) { cparse_value_destroy(new_value); }
    return ret;
}

static int cparse_perform_exclude(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    int ret = 0;
    int retval;
    cparse_value_t *new_value = NULL;

    /* exclude */
    cparse_state_save(state);
    retval = cparse_perform(state, &new_value, \
            cparse_component_as_sepby_sep(component));
    cparse_state_load(state);

    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval == 0)
    {
        /* Matched exclude, failed */
        ret = -1;
        goto fail;
    }
    cparse_value_destroy(new_value); new_value = NULL;

    /* p */
    cparse_state_save(state);
    retval = cparse_perform(state, &new_value, \
            cparse_component_as_sepby_sep(component));
    if (cparse_err_occurred(cparse_state_err(state))) { ret = -1; goto fail; }
    if (retval != 0)
    {
        /* failed */
        cparse_state_load(state);
        ret = -1;
        goto fail;
    }

    *value_out = new_value; new_value = NULL;

    goto done;
fail:
    if (new_value != NULL) cparse_value_destroy(new_value);
done:
    return ret;
}

static int cparse_perform_oneof(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_string_t *string_to_choice = cparse_component_as_string_extract(component);
    cparse_char_t *char_cur = cparse_value_string_str(string_to_choice);
    cparse_size_t len = cparse_value_string_length(string_to_choice);
    cparse_value_char_t ch;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }
    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);

    while (len-- != 0)
    {
        if ((cparse_value_char_t)ch == (cparse_value_char_t)*char_cur)
        {
            cparse_reader_forward(reader);
            *value_out = cparse_value_new_char(ch);
            return 0;
        }
        else
        {
            char_cur++;
        }
    }

    return -1;
}

static int cparse_perform_noneof(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_reader_t *reader = cparse_state_reader(state);
    cparse_value_string_t *string_to_choice = cparse_component_as_string_extract(component);
    cparse_char_t *char_cur = cparse_value_string_str(string_to_choice);
    cparse_size_t len = cparse_value_string_length(string_to_choice);
    cparse_value_char_t ch;

    if (cparse_reader_eof(reader) == cparse_true)
    { return -1; }
    ch = (cparse_value_char_t)cparse_reader_peek_char(reader);

    while (len-- != 0)
    {
        if ((cparse_value_char_t)ch == (cparse_value_char_t)*char_cur)
        {
            return -1;
        }
        else
        {
            char_cur++;
        }
    }

    cparse_reader_forward(reader);
    *value_out = cparse_value_new_char(ch);
    return 0;
}

static int cparse_perform_fail(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_value_string_t *string_error_msg = cparse_component_as_string_extract(component);
    cparse_value_char_t *p = cparse_value_string_str(string_error_msg);
    cparse_err_t *err = cparse_state_err(state);
    cparse_err_update_description_wide(err, p);

    (void)value_out;

    return -1;
}

static int cparse_perform_failloc(cparse_state_t *state, \
        cparse_component_t *component, \
        cparse_value_t **value_out)
{
    cparse_value_string_t *string_error_msg = cparse_component_as_string_extract(component);
    cparse_value_char_t *p = cparse_value_string_str(string_error_msg);
    cparse_err_t *err = cparse_state_err(state);
    /* cparse_err_update_description_wide(err, p); */

    cparse_err_update_description_printf( \
            cparse_state_err(state), \
            "%s%s%u:%u: error: %w", \
            cparse_err_filename(err) != NULL ? cparse_err_filename(err) : "", \
            cparse_err_filename(err) != NULL ? ":" : "", \
            cparse_reader_ln(cparse_state_reader(state)), \
            cparse_reader_col(cparse_state_reader(state)), \
            p);

    (void)value_out;

    return -1;
}

int cparse_perform(cparse_state_t *state, \
        cparse_value_t **value_out, \
        cparse_component_t *component)
{
    int ret = 0;
    cparse_value_t *new_value = NULL;

    if (cparse_err_occurred(cparse_state_err(state)) != cparse_false)
    { return -1; }

    switch (cparse_component_type(component))
    {
        case CPARSE_COMPONENT_TYPE_REGEX:
            ret = cparse_perform_regex(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_EXPR:
            ret = cparse_perform_expr(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SPACE:
            ret = cparse_perform_space(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SPACES:
            ret = cparse_perform_spaces(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_ENDOFLINE:
            ret = cparse_perform_endofline(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_ANYCHAR:
            ret = cparse_perform_anychar(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_EOF:
            ret = cparse_perform_eof(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_NATIVE:
            ret = cparse_perform_native(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_CHAR:
            ret = cparse_perform_char(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_STRING:
            ret = cparse_perform_string(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_ONEOF:
            ret = cparse_perform_oneof(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_NONEOF:
            ret = cparse_perform_noneof(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_MANY:
            ret = cparse_perform_many(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_MANY1:
            ret = cparse_perform_many1(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_MUNCH:
            ret = cparse_perform_munch(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_MUNCH1:
            ret = cparse_perform_munch1(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_CHOICE:
            ret = cparse_perform_choice(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_RANGE:
            ret = cparse_perform_range(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_BETWEEN:
            ret = cparse_perform_between(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_OPTION:
            ret = cparse_perform_option(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SEPBY:
            ret = cparse_perform_sepby(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SEPBY1:
            ret = cparse_perform_sepby1(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_ENDBY:
            ret = cparse_perform_endby(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_ENDBY1:
            ret = cparse_perform_endby1(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SEPENDBY:
            ret = cparse_perform_sependby(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_SEPENDBY1:
            ret = cparse_perform_sependby1(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_MANYTILL:
            ret = cparse_perform_manytill(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_EXCLUDE:
            ret = cparse_perform_exclude(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_FAIL:
            ret = cparse_perform_fail(state, component, &new_value);
            break;
        case CPARSE_COMPONENT_TYPE_FAILLOC:
            ret = cparse_perform_failloc(state, component, &new_value);
            break;
    }
    if (value_out != NULL)
    { 
        *value_out = new_value;
    }
    else
    { if (new_value != NULL) cparse_value_destroy(new_value); }

    return ret;
}

/* Parsing Interface
 * Return 0 if success or -1 if fail */
int cparse_parse( \
        cparse_t *cparse,           /* CParse */ \
        cparse_err_t **err_out,     /* Error Information (If failed) */ \
        void **ast_out,             /* AST (If success) */ \
        char *filename,             /* Filename of source */ \
        char *s, cparse_size_t len  /* Data to be parsed */ \
        )
{
    int ret = 0;
    cparse_state_t *new_cparse_state = NULL; 
    cparse_value_t *value = NULL;
    void *ast = NULL;

    /* Initialize the state */
    if ((new_cparse_state = cparse_state_new_stringn(s, len)) == NULL)
    {
        return -1; 
    }
    if (cparse->entry_point == NULL) 
    {
        cparse_err_update_description( \
                cparse_state_err(new_cparse_state), \
                "no entry point");
        ret = -1; goto fail; 
    }
    if (cparse->ast_ctor == NULL)
    {
        cparse_err_update_description( \
                cparse_state_err(new_cparse_state), \
                "no ast constructor");
        ret = -1; goto fail; 
    }
    if (cparse->ast_dtor == NULL)
    {
        cparse_err_update_description( \
                cparse_state_err(new_cparse_state), \
                "no ast destructor");
        ret = -1; goto fail; 
    }
    if (filename != NULL)
    {
        if (cparse_err_update_filename( \
                    cparse_state_err(new_cparse_state), \
                    filename) != 0)
        { ret = -1; goto fail; }
    }

    /* Perform parsing */
    if ((cparse_perform(new_cparse_state, &value, cparse->entry_point)) != 0)
    {
        /* Failed */

        /* The most outside level */
        if (cparse_err_description_given( \
                    cparse_state_err( \
                        new_cparse_state)) == cparse_false)
        {
            /* Error description not given */
            if (cparse_reader_eof( \
                        cparse_state_reader( \
                            new_cparse_state)) == cparse_true)
            {
                /* EOF */
                cparse_err_update_description_printf( \
                    cparse_state_err(new_cparse_state), \
                    "%s%s%u:%u: error: unexpected end of input", \
                    filename != NULL ? filename : "", \
                    filename != NULL ? ":" : "", \
                    cparse_reader_ln(cparse_state_reader(new_cparse_state)), \
                    cparse_reader_col(cparse_state_reader(new_cparse_state)));
            }
            else
            {
                /* Some char remain */
                cparse_err_update_description_printf( \
                    cparse_state_err(new_cparse_state), \
                    "%s%s%u:%u: error: unexpected char \'%c\'", \
                    filename != NULL ? filename : "", \
                    filename != NULL ? ":" : "", \
                    cparse_reader_ln(cparse_state_reader(new_cparse_state)), \
                    cparse_reader_col(cparse_state_reader(new_cparse_state)), \
                    cparse_reader_peek_char(cparse_state_reader(new_cparse_state)));
            }
        }

        /* Throw Error */
        *err_out = cparse_state_yield_err(new_cparse_state);
        ret = -1;
        goto fail; 
    }
    else
    {
        /* Success */

        /* Convert value to final AST */
        if ((ast = cparse->ast_ctor( \
                        cparse_state_err(new_cparse_state), \
                        value)) == NULL)
        {
            if (cparse_err_update_description( \
                    cparse_state_err(new_cparse_state), \
                    "failed to construct AST") != 0)
            { ret = -1; goto fail; }
            *err_out = cparse_state_yield_err(new_cparse_state);
            ret = -1;
            goto fail; 
        }

        /* Throw AST */
        *ast_out = ast; ast = NULL;
        ret = 0;
    }

    goto done;
fail:
done:
    if (new_cparse_state != NULL) cparse_state_destroy(new_cparse_state);
    if (value != NULL) cparse_value_destroy(value);
    return ret;
}

