/* CParse : Regex : Matching
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_REGEX_MATCH_H
#define CPARSE_REGEX_MATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"
#include "cparse_state.h"
#include "cparse_regex_nfa.h"

int cparse_regex_match_nfa(struct cparse_regex_nfa *nfa, cparse_state_t *state);

#ifdef __cplusplus
}
#endif

#endif

