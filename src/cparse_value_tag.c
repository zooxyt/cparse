/* CParse : Tag
 * Copyright(c) Cheryl Natsu */

#include "cparse_value_tag.h"

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_libc.h"
#include "cparse_value.h"

struct cparse_value_tag
{
    int type_tag;
    cparse_value_t *value;
};

/* Create and destroy tag */
struct cparse_value_tag *cparse_value_tag_new(int type_tag, cparse_value_t *value)
{
    struct cparse_value_tag *new_tag = NULL;

    if ((new_tag = (struct cparse_value_tag *)cparse_malloc( \
                    sizeof(struct cparse_value_tag))) == NULL)
    { return NULL; }
    new_tag->type_tag = type_tag;
    if ((new_tag->value = cparse_value_clone(value)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_tag != NULL)
    {
        cparse_value_tag_destroy(new_tag);
        new_tag = NULL;
    }
done:
    return new_tag;
}

void cparse_value_tag_destroy(cparse_value_tag_t *cparse_tag)
{
    if (cparse_tag->value) cparse_value_destroy(cparse_tag->value);
    cparse_free(cparse_tag);
}

cparse_value_tag_t *cparse_value_tag_clone(cparse_value_tag_t *cparse_tag)
{
    return cparse_value_tag_new(cparse_tag->type_tag, cparse_tag->value);
}

cparse_bool cparse_value_tag_is(cparse_value_tag_t *cparse_tag, int type_tag)
{
    return cparse_tag->type_tag == type_tag ? cparse_true : cparse_false;
}

int cparse_value_tag_id(cparse_value_tag_t *cparse_tag)
{
    return cparse_tag->type_tag;
}

cparse_value_t *cparse_value_tag_value(cparse_value_tag_t *cparse_tag)
{
    return cparse_tag->value;
}

