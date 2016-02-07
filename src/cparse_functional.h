/* CParse : Functional
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_FUNCTIONAL_H
#define CPARSE_FUNCTIONAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_value.h"

/* type cparse_function_1_to_1_t = (a -> b) */
typedef cparse_value_t *(*cparse_function_1_to_1_t)( \
        cparse_value_t *a);


/* type cparse_function_2_to_1_t = (a -> b -> a) */
typedef cparse_value_t *(*cparse_function_2_to_1_t)( \
        cparse_value_t *a, \
        cparse_value_t *b);

/* map :: (a -> b) -> [a] -> [b] */
cparse_value_t *cparse_functional_map( \
        cparse_function_1_to_1_t f, \
        cparse_value_t *lst);

/* filter :: (a -> Bool) -> [a] -> [b] */
cparse_value_t *cparse_functional_filter( \
        cparse_function_1_to_1_t f, \
        cparse_value_t *lst);

/* foldl :: (a -> b -> a) -> a -> [b] -> a */
cparse_value_t *cparse_functional_foldl( \
        cparse_function_2_to_1_t f, \
        cparse_value_t *a, \
        cparse_value_t *lst);


#ifdef __cplusplus
}
#endif

#endif

