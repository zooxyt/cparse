/* CParse : File Location
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_libc.h"
#include "cparse_res.h"
#include "cparse_fileloc.h"

#include "cparse_value_map.h"

struct cparse_fileloc
{
    char *filename;
    cparse_size_t filename_len;
    cparse_size_t ln;
};

struct cparse_fileloc_pack
{
    cparse_value_map_t *map_fileloc;
};

struct cparse_fileloc *cparse_fileloc_new(char *filename, cparse_size_t ln)
{
    struct cparse_fileloc *new_fileloc = NULL;
    cparse_size_t len;

    if ((new_fileloc = (struct cparse_fileloc *)cparse_malloc( \
                    sizeof(struct cparse_fileloc))) == NULL)
    { return NULL; }
    len = cparse_strlen(filename);
    if ((new_fileloc->filename = (char *)cparse_malloc( \
                    sizeof(char) * (len + 1))) == NULL)
    { cparse_free(new_fileloc); return NULL; }
    cparse_memcpy(new_fileloc->filename, filename, len);
    new_fileloc->filename[len] = '\0';
    new_fileloc->filename_len = len;
    new_fileloc->ln = ln;

    return new_fileloc;
}

void cparse_fileloc_destroy(cparse_fileloc_t *fileloc)
{
    if (fileloc->filename != NULL) cparse_free(fileloc->filename);
    cparse_free(fileloc);
}

cparse_fileloc_t *cparse_fileloc_clone(cparse_fileloc_t *fileloc)
{
    return cparse_fileloc_new(fileloc->filename, fileloc->ln);
}

void *cparse_fileloc_cctor(void *fileloc)
{
    return (void *)cparse_fileloc_clone(fileloc);
}

void cparse_fileloc_dtor(void *dtor)
{
    cparse_fileloc_destroy(dtor);
}

cparse_bool cparse_fileloc_eqp(void *p1, void *p2)
{
    cparse_fileloc_t *fileloc1 = p1;
    cparse_fileloc_t *fileloc2 = p2;

    if (fileloc1->ln != fileloc2->ln) return cparse_false;
    if (fileloc1->filename_len != fileloc2->filename_len) return cparse_false;
    if (cparse_strncmp(fileloc1->filename, fileloc2->filename, fileloc1->filename_len) != 0)
    { return cparse_false; }
    return cparse_true;
}

