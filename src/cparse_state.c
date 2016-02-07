/* CParse : State
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_err.h"
#include "cparse_fileloc.h"
#include "cparse_component.h"
#include "cparse_state.h"
#include "cparse_reader.h"
#include "cparse_value_map.h"
#include "cparse_value_set.h"


struct cparse_state
{
    /* Error */
    cparse_err_t *err;
    /* Reader */
    struct cparse_reader *reader;
    /* Components */
    cparse_value_map_t *components;
    /* Block Set */
    cparse_value_set_t *blocks;
};


/* Initialize CParse State with string */
cparse_state_t *cparse_state_new_stringn(char *s, cparse_size_t len)
{
    struct cparse_state *new_cparse_state = NULL;

    if ((new_cparse_state = (cparse_state_t *)cparse_malloc( \
                    sizeof(cparse_state_t))) == NULL)
    { return NULL; }
    new_cparse_state->err = NULL;
    new_cparse_state->reader = NULL;
    new_cparse_state->components = NULL;
    new_cparse_state->blocks = NULL;

    if ((new_cparse_state->err = cparse_err_new()) == NULL)
    { goto fail; }
    if ((new_cparse_state->reader = cparse_reader_new_stringn_from_utf8(s, len)) == NULL)
    { goto fail; }
    if ((new_cparse_state->components = cparse_value_map_new( \
                    cparse_fileloc_eqp, \
                    cparse_fileloc_cctor, \
                    cparse_fileloc_dtor, \
                    NULL, \
                    cparse_component_dtor)) == NULL)
    { goto fail; }
    if ((new_cparse_state->blocks = cparse_value_set_new( \
                    cparse_fileloc_eqp, \
                    cparse_fileloc_cctor, \
                    cparse_fileloc_dtor)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_cparse_state != NULL)
    {
        cparse_state_destroy(new_cparse_state);
        new_cparse_state = NULL;
    }
done:
    return new_cparse_state;
}

struct cparse_state *cparse_state_new_string(char *s)
{
    return cparse_state_new_stringn(s, cparse_strlen(s));
}


/* Destroy an instance of cparse */
void cparse_state_destroy(struct cparse_state *cparse_state)
{
    if (cparse_state->err != NULL) cparse_err_destroy(cparse_state->err);
    if (cparse_state->reader != NULL) cparse_reader_destroy(cparse_state->reader);
    if (cparse_state->components != NULL) cparse_value_map_destroy(cparse_state->components);
    if (cparse_state->blocks != NULL) cparse_value_set_destroy(cparse_state->blocks);
    cparse_free(cparse_state);
}

/* Save */
void cparse_state_save(struct cparse_state *cparse_state)
{
    cparse_reader_save(cparse_state->reader);
}

/* Load */
void cparse_state_load(struct cparse_state *cparse_state)
{
    cparse_reader_load(cparse_state->reader);
}

/* Drop */
void cparse_state_drop(struct cparse_state *cparse_state)
{
    cparse_reader_drop(cparse_state->reader);
}

/* Erase */
void cparse_state_erase(struct cparse_state *cparse_state)
{
    cparse_reader_erase(cparse_state->reader);
}

/* Reader */
cparse_reader_t *cparse_state_reader(cparse_state_t *cparse_state)
{
    return cparse_state->reader;
}

/* Error */
cparse_err_t *cparse_state_err(cparse_state_t *cparse_state)
{
    return cparse_state->err;
}

cparse_err_t *cparse_state_yield_err(cparse_state_t *cparse_state)
{
    cparse_err_t *err = cparse_state->err;
    cparse_state->err = NULL;
    return err;
}

/* Components Pack */
cparse_value_map_t *cparse_state_components(cparse_state_t *cparse_state)
{
    return cparse_state->components;
}

/* Blocks */
cparse_value_set_t *cparse_state_blocks(cparse_state_t *cparse_state)
{
    return cparse_state->blocks;
}

