/* CParse : State
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_STATE_H
#define CPARSE_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h" 
#include "cparse_err.h" 
#include "cparse_reader.h" 
#include "cparse_value_map.h" 
#include "cparse_value_set.h" 

struct cparse_state;

/* Data Type: Parsing State */
#ifndef DT_CPARSE_STATE_T
#define DT_CPARSE_STATE_T
typedef struct cparse_state cparse_state_t;
#endif

/* Initialize CParse State with string */
cparse_state_t *cparse_state_new_stringn(char *s, cparse_size_t len);
cparse_state_t *cparse_state_new_string(char *s);

/* Destroy an instance of cparse */
void cparse_state_destroy(cparse_state_t *cparse_state);

/* Save and Load State */
void cparse_state_save(cparse_state_t *cparse_state);
void cparse_state_load(cparse_state_t *cparse_state);
void cparse_state_drop(cparse_state_t *cparse_state);
void cparse_state_erase(cparse_state_t *cparse_state);

/* Reader */
cparse_reader_t *cparse_state_reader(cparse_state_t *cparse_state);

/* Error */
cparse_err_t *cparse_state_err(cparse_state_t *cparse_state);
cparse_err_t *cparse_state_yield_err(cparse_state_t *cparse_state);

/* Components Pack */
cparse_value_map_t *cparse_state_components(cparse_state_t *cparse_state);

/* Blocks */
cparse_value_set_t *cparse_state_blocks(cparse_state_t *cparse_state);


#ifdef __cplusplus
}
#endif

#endif

