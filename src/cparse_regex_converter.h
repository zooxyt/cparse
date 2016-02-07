/* CParse : Regex : Converter (AST -> NFA)
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_REGEX_CONVERTER_H
#define CPARSE_REGEX_CONVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_regex_ast.h"
#include "cparse_regex_nfa.h"


/* Node Count
 * ---------------
 * nodeCount (Char _)          = 1
 * nodeCount (Seq nodes)       = sum (map nodeCount nodes)
 * nodeCount (Group branches)  = 1
 * nodeCount (Branch branches) = sum (map ((-1) . nodeCount) branches) + 1
 *
 */

/* Starting
 * ---------
 *
 * The initial data structure of NFA is empty, to make it work
 * the minimum requirements is the only state which is start state:
 *
 * [S0]
 *
 *
 *
 * Char
 * ----
 *
 * Char is one of the simplest transition condition which accepts a 
 * specified char:
 *
 * [S0] --- (char 'a') --> [S1]
 *
 *
 *
 * Seq
 * ---
 *
 * "ab"
 *
 * [S0] --- (char 'a') --> [S1] --- (char 'b') --> [S2]
 *
 *
 *
 * Group
 * -----
 * 
 * Group accepts a series of conditions, if one of those conditions
 * mets, the transition performs. For example, the pattern of 
 * 'a hex number digit' is "[0-9a-fA-F]":
 *
 *         +--(group.range '0' '9')--+
 *         |                         |
 * [S0] ---+--(group.range 'a' 'f')--+---> [S1]
 *         |                         |
 *         +--(group.range 'A' 'F')--+
 *
 * Exclude group should append two extra state
 *
 *
 *
 * Branch
 * ------
 *
 * Branch is a more freely way to organize multiple possible way
 * of state transition, For example, the pattern "(1|ab)":
 *
 *         +------------(char '1')------------+
 * [S0] ---+-                                 +--> [S1]
 *         +--(char '1')-> [S2] --(char 'b')--+
 *
 *
 *
 * Repeat Limit
 * ------------
 *
 * Example #1: "a*" (repeat matching a for any times)
 *
 *                                      +---------------------+
 *                                      v                     |
 * [S0] ---> [S1 (counter0 := 0)] --> [S2] -- (char 'a') --> [S3, {repeat_min:0, repeat_max:unlimited})] ---> [S4]
 *                                      |                     ^
 *                                      +---------------------+
 *
 *
 * [S0] --- (char 'a') {repmin:0, repmax:unlimited} --> [S1]
 * 
 *
 */

struct cparse_regex_nfa *cparse_regex_convert(struct cparse_regex_ast *ast);


#ifdef __cplusplus
}
#endif

#endif


