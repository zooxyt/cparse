/* CParse : Component
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_COMPONENT_H
#define CPARSE_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_regex.h"
#include "cparse_expr.h"
#include "cparse_value_char.h"
#include "cparse_functional.h"
#include "cparse_value.h"

/* Data Type: State */
#ifndef DT_CPARSE_STATE_T
#define DT_CPARSE_STATE_T
typedef struct cparse_state cparse_state_t;
#endif

/* Data Type: Component */
#ifndef DT_CPARSE_COMPONENT_T
#define DT_CPARSE_COMPONENT_T
typedef struct cparse_component cparse_component_t;
#endif

/* Data Type: Native Component */
#ifndef DT_CPARSE_NATIVE_COMPONENT_T
#define DT_CPARSE_NATIVE_COMPONENT_T
typedef int (*cparse_native_component_t)(cparse_state_t *state, cparse_value_t **value_out);
#endif

/* Data Type: Native Predict */
#ifndef DT_CPARSE_NATIVE_PREDICT_T
#define DT_CPARSE_NATIVE_PREDICT_T
typedef cparse_bool (*cparse_native_predict_t)(int ch);
#endif

/* Data Type: Composite Component */
#ifndef DT_CPARSE_COMPOSITE_COMPONENT_T
#define DT_CPARSE_COMPOSITE_COMPONENT_T
typedef struct cparse_composite_component cparse_composite_component_t;
#endif


typedef enum
{
    CPARSE_COMPONENT_TYPE_REGEX = 0,
    CPARSE_COMPONENT_TYPE_EXPR,
    CPARSE_COMPONENT_TYPE_SPACE,
    CPARSE_COMPONENT_TYPE_SPACES,
    CPARSE_COMPONENT_TYPE_ENDOFLINE,
    CPARSE_COMPONENT_TYPE_ANYCHAR,
    CPARSE_COMPONENT_TYPE_EOF,
    CPARSE_COMPONENT_TYPE_NATIVE,
    CPARSE_COMPONENT_TYPE_CHAR,
    CPARSE_COMPONENT_TYPE_STRING,
    CPARSE_COMPONENT_TYPE_ONEOF,
    CPARSE_COMPONENT_TYPE_NONEOF,
    CPARSE_COMPONENT_TYPE_MANY,
    CPARSE_COMPONENT_TYPE_MANY1,
    CPARSE_COMPONENT_TYPE_MUNCH,
    CPARSE_COMPONENT_TYPE_MUNCH1,
    CPARSE_COMPONENT_TYPE_CHOICE,
    CPARSE_COMPONENT_TYPE_RANGE,
    CPARSE_COMPONENT_TYPE_BETWEEN,
    CPARSE_COMPONENT_TYPE_OPTION,
    CPARSE_COMPONENT_TYPE_SEPBY,
    CPARSE_COMPONENT_TYPE_SEPBY1,
    CPARSE_COMPONENT_TYPE_ENDBY,
    CPARSE_COMPONENT_TYPE_ENDBY1,
    CPARSE_COMPONENT_TYPE_SEPENDBY,
    CPARSE_COMPONENT_TYPE_SEPENDBY1,
    CPARSE_COMPONENT_TYPE_EXCLUDE,
    CPARSE_COMPONENT_TYPE_MANYTILL,
    CPARSE_COMPONENT_TYPE_FAIL,
    CPARSE_COMPONENT_TYPE_FAILLOC,
} cparse_component_type_t;

/* Destroy Component */
void cparse_component_destroy(cparse_component_t *component);
void cparse_component_dtor(void *component);

/* Type */
cparse_component_type_t cparse_component_type(cparse_component_t *component);

/* Create Native Component */
cparse_component_t *cparse_native(cparse_native_component_t callback);

/* Create Composite Component */
cparse_component_t *cparse_regex(char *pattern);
cparse_component_t *cparse_expr(cparse_size_t precedence_count);
cparse_component_t *cparse_space(void);
cparse_component_t *cparse_spaces(void);
cparse_component_t *cparse_endofline(void);
cparse_component_t *cparse_anychar(void);
cparse_component_t *cparse_eof(void);
cparse_component_t *cparse_char(cparse_value_char_t ch);
cparse_component_t *cparse_string(char *s);
cparse_component_t *cparse_oneof(char *s);
cparse_component_t *cparse_noneof(char *s);
cparse_component_t *cparse_many(cparse_component_t *component);
cparse_component_t *cparse_many1(cparse_component_t *component);
cparse_component_t *cparse_munch(cparse_native_predict_t predict);
cparse_component_t *cparse_munch1(cparse_native_predict_t predict);
cparse_component_t *cparse_choice(cparse_component_t *first_component, ...);
cparse_component_t *cparse_range_greed( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component);
cparse_component_t *cparse_range_nongreed( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component);
cparse_component_t *cparse_range( \
        cparse_size_t count_min, cparse_size_t count_max, \
        cparse_component_t *sub_component);
cparse_component_t *cparse_count( \
        cparse_size_t count, \
        cparse_component_t *sub_component);
cparse_component_t *cparse_between( \
        cparse_component_t *component_open, \
        cparse_component_t *component_close, \
        cparse_component_t *component_sub);
cparse_component_t *cparse_option( \
        cparse_value_t *value_x, \
        cparse_component_t *component_p);
cparse_component_t *cparse_sepby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_sepby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_endby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_endby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_sependby( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_sependby1( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_manytill( \
        cparse_component_t *component_p, \
        cparse_component_t *component_sep);
cparse_component_t *cparse_exclude( \
        cparse_component_t *component_p, \
        cparse_component_t *component_exclude);
cparse_component_t *cparse_fail(char *message);
cparse_component_t *cparse_failloc(char *message);
cparse_component_t *cparse_undefined(void);
cparse_component_t *cparse_undefinedloc(void);

/* Extract */
cparse_native_component_t cparse_component_as_native_extract( \
        cparse_component_t *component);
cparse_native_predict_t cparse_component_as_predict_extract( \
        cparse_component_t *component);
cparse_value_list_t *cparse_component_as_component_list_extract( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_component_extract( \
        cparse_component_t *component);
cparse_value_char_t cparse_component_as_char_extract( \
        cparse_component_t *component);
cparse_value_string_t *cparse_component_as_string_extract( \
        cparse_component_t *component);

/* range, count */
cparse_size_t cparse_component_as_range_extract_min( \
        cparse_component_t *component);
cparse_size_t cparse_component_as_range_extract_max( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_range_extract_sub_component( \
        cparse_component_t *component);
int cparse_component_as_range_extract_greed( \
        cparse_component_t *component);

/* between */
cparse_component_t *cparse_component_as_between_component_open( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_between_component_close( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_between_component_sub( \
        cparse_component_t *component);

/* option */
cparse_component_t *cparse_component_as_option_predict( \
        cparse_component_t *component);
cparse_value_t *cparse_component_as_option_prepared( \
        cparse_component_t *component);

/* sepby */
cparse_component_t *cparse_component_as_sepby_predict( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_sepby_sep( \
        cparse_component_t *component);

/* endBy */
cparse_component_t *cparse_component_as_endby_predict( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_endby_sep( \
        cparse_component_t *component);

/* manyTill */
cparse_component_t *cparse_component_as_manyTill_p( \
        cparse_component_t *component);
cparse_component_t *cparse_component_as_manyTill_end( \
        cparse_component_t *component);

/* regex */
cparse_regex_t *cparse_component_as_regex_get( \
        cparse_component_t *component);

/* expr */
cparse_expr_t *cparse_component_as_expr_get( \
        cparse_component_t *component);

/* Prefix */
int cparse_component_as_expr_append_prefix_op( \
        cparse_component_t *component, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_1_to_1_t assembler_unary);
/* Postfix */
int cparse_component_as_expr_append_postfix_op( \
        cparse_component_t *component, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_1_to_1_t assembler_unary);
/* Infix */
int cparse_component_as_expr_append_infix_op( \
        cparse_component_t *component, \
        cparse_expr_associative_t associative, \
        cparse_size_t precedence, \
        char *str, \
        cparse_function_2_to_1_t assembler_binary);

int cparse_component_as_expr_set_follow( \
        cparse_component_t *component, \
        struct cparse_component *component_follow);

struct cparse_component *cparse_component_as_expr_follow( \
        cparse_component_t *component);


#ifdef __cplusplus
}
#endif

#endif

