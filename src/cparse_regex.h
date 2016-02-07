/* CParse : Regex
 * Copyright(C) Cheryl Natsu */

/* NFA Regular Expression Engine used as the lexeme scanner
 * of CParse */

/* Basic subset of Regular Expression are supported :
 *
 * - Plain Text 
 * - Group 
 * - Branch
 * - Counting
 * - Unicode Code
 */ 

/* Compiling Process
 * =================
 *
 * <Pattern Literal>
 * --- Lexeme Scan --->
 * <Tokens>
 * --- Parse --->
 * <AST>
 * --- Convert --->
 * <NFA>
 * --- Compile --->
 * <IL>
 *
 */

#ifndef CPARSE_REGEX_H
#define CPARSE_REGEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_state.h"

struct cparse_regex;

/* Data Type: Regex */
#ifndef DT_CPARSE_REGEX_T
#define DT_CPARSE_REGEX_T
typedef struct cparse_regex cparse_regex_t;
#endif

/* Create and destroy regex */
cparse_regex_t *cparse_regex_new(cparse_char_t *pattern);
void cparse_regex_destroy(cparse_regex_t *regex);

int cparse_regex_match(cparse_regex_t *regex, cparse_state_t *state);

struct cparse_regex_nfa;
struct cparse_regex_nfa *cparse_regex_nfa(cparse_regex_t *regex);

#ifdef __cplusplus
}
#endif

#endif

