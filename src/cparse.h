/* CParse 
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_H
#define CPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_err.h"
#include "cparse_state.h"
#include "cparse_component.h"
#include "cparse_value.h"
#include "cparse_autorel.h"

struct cparse;
struct cparse_state;
struct cparse_value_string;

/* Data Type: Combinators */
#ifndef DT_CPARSE_T
#define DT_CPARSE_T
typedef struct cparse cparse_t;
#endif

/* Data Type: AST Node Constructor */
#ifndef DT_CPARSE_AST_CTOR_T
#define DT_CPARSE_AST_CTOR_T
typedef void *(*cparse_ast_ctor_t)(cparse_err_t *err, cparse_value_t *value);
#endif

/* Data Type: AST Node Destructor */
#ifndef DT_CPARSE_AST_DTOR_T
#define DT_CPARSE_AST_DTOR_T
typedef void (*cparse_ast_dtor_t)(void *ast);
#endif

/* Resource */
void *cparse_malloc(cparse_size_t size);
void cparse_free(void *ptr);

/* Initialize CParse with string */
cparse_t *cparse_new(void);
void cparse_destroy(cparse_t *cparse);
void cparse_set_ast_ctor(cparse_t *cparse, cparse_ast_ctor_t ast_ctor);
void cparse_set_ast_dtor(cparse_t *cparse, cparse_ast_dtor_t ast_dtor);
void cparse_set_entry_point(cparse_t *cparse, cparse_component_t *entry_point);

/* Components */
cparse_component_t *cparse_component_define_in( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln, \
        cparse_component_t *component);
cparse_bool cparse_component_defined( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln, \
        cparse_component_t *component);

int cparse_component_once( \
        cparse_state_t *state, \
        char *filename, cparse_size_t ln);

#define cparse_component_define(state, component) \
    cparse_component_define_in(state, __FILE__, __LINE__, component)

/* Framework */
int cparse_perform(cparse_state_t *state, cparse_value_t **value_out, \
        cparse_component_t *component);

#define cparse_define_and_perform(state, value_out, component) \
    cparse_perform(state, value_out, (cparse_component_define_in(state, __FILE__, __LINE__, component)))


/* Value */

cparse_value_type_t cparse_value_type(cparse_value_t *value);

cparse_char_t *cparse_value_as_string_str_wide(cparse_value_t *value);
cparse_size_t cparse_value_as_string_length(cparse_value_t *value);

cparse_size_t cparse_value_as_list_size(cparse_value_t *value);
cparse_value_list_node_t *cparse_value_as_list_first(cparse_value_t *value);
cparse_value_list_node_t *cparse_value_as_list_node_next(cparse_value_list_node_t *node);
void *cparse_value_as_list_node_value(cparse_value_list_node_t *node);

cparse_size_t cparse_value_as_tuple_size(cparse_value_t *value);
void *cparse_value_as_tuple_ref(cparse_value_t *value, cparse_size_t index);

/* Use as parameter list of every native component */
#define CPARSE_COMPONENT_ARGS cparse_state_t *__cparse_state, cparse_value_t **__cparse_value_out

/* Use at the beginning of native component */
#define CPARSE_COMPONENT_BEGIN() \
    int __cparse_ret = 0; \
    (void)__cparse_state; \
    (void)__cparse_value_out; \
    cparse_auto_release_t *__cparse_autorel = NULL; \
    do { \
        if ((__cparse_autorel = cparse_auto_release_new()) == NULL) \
        { goto __cparse_fail; } \
    } while (0)

/* Define variables */
#define CPARSE_COMPONENT_VAR(var) \
    cparse_value_t *var = NULL; \
    if ((cparse_auto_release_add(__cparse_autorel, (void **)&var, cparse_value_dtor)) != 0) \
    { goto __cparse_fail; }

/* Bind a value to a variable */
#define CPARSE_COMPONENT_BIND(var, component) \
    do { \
        if (cparse_define_and_perform(__cparse_state, &var, \
                    component) != 0) \
        { __cparse_ret = -1; goto __cparse_fail; } \
        if (cparse_err_occurred(cparse_state_err(__cparse_state))) \
        { __cparse_ret = -1; goto __cparse_fail; } \
    } while(0)

/* Perform parsing and ignore the resurned value */
#define CPARSE_COMPONENT_VOID(component) \
    do { \
        if (cparse_define_and_perform(__cparse_state, NULL, \
                    component) != 0) \
        { __cparse_ret = -1; goto __cparse_fail; } \
        if (cparse_err_occurred(cparse_state_err(__cparse_state))) \
        { __cparse_ret = -1; goto __cparse_fail; } \
    } while(0)

#define CPARSE_COMPONENT_RETURN(var) \
    *__cparse_value_out = (var); var = NULL;

/* Use at the end of native component */
#define CPARSE_COMPONENT_END() \
    __cparse_fail: \
    cparse_auto_release_destroy(__cparse_autorel); \
    return __cparse_ret

#define CPARSE_COMPONENT_DIRECT(_component) \
    do { \
        CPARSE_COMPONENT_BEGIN(); \
        CPARSE_COMPONENT_VAR(x); \
        { \
            CPARSE_COMPONENT_BIND(x, (_component)); \
            CPARSE_COMPONENT_RETURN(x); \
        } \
        CPARSE_COMPONENT_END(); \
    } while (0)

#define CPARSE_COMPONENT_DIRECT_TAG(_tag, _component) \
    do { \
        CPARSE_COMPONENT_BEGIN(); \
        CPARSE_COMPONENT_VAR(x); \
        CPARSE_COMPONENT_VAR(y); \
        { \
            CPARSE_COMPONENT_BIND(x, (_component)); \
            y = cparse_value_new_tag(_tag, x); \
            CPARSE_COMPONENT_RETURN(y); \
        } \
        CPARSE_COMPONENT_END(); \
    } while (0)

#define CPARSE_COMPONENT_DEFINE(_name, _component) \
    do { \
        _name = cparse_component_define(__cparse_state, _component); \
    } while (0)

#define CPARSE_COMPONENT_DEFINE_BEGIN(_name, _component) \
    do { \
        _name = cparse_component_define(__cparse_state, _component); \
    } while (0)

#define CPARSE_COMPONENT_DEFINE_END() \
    do {} while (0)

#define CPARSE_COMPONENT_ONCE(state) \
    if (cparse_component_once(__cparse_state, __FILE__, __LINE__) == 0)

#define CPARSE_COMPONENT_PERFORM(_name, _component) \
    if (cparse_perform(__cparse_state, &_name, _component) != 0) \
    { __cparse_ret = -1; goto __cparse_fail; }

/* Serialize to JSON String */
void *cparse_json_ast_ctor_wide(cparse_err_t *err, cparse_value_t *value);
void *cparse_json_ast_ctor(cparse_err_t *err, cparse_value_t *value);
void cparse_json_ast_dtor_wide(void *ptr);
void cparse_json_ast_dtor(void *ptr);


/* Parsing Interface */

/* General Parse
 * Return 0 if success or -1 if fail */
int cparse_parse( \
        cparse_t *cparse,           /* CParse */ \
        cparse_err_t **err_out,     /* Error Information (If failed) */ \
        void **ast_out,             /* AST (If success) */ \
        char *filename,             /* Filename of source */ \
        char *s, cparse_size_t len  /* Data to be parsed */ \
        );


#ifdef __cplusplus
}
#endif


#endif

