/* CParse : Tag
 * Copyright(c) Cheryl Natsu */

#ifndef CPARSE_VALUE_TAG_H
#define CPARSE_VALUE_TAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cparse_dt.h"

struct cparse_value;

/* Data Type: Tag */
#ifndef DT_CPARSE_VALUE_TAG_T
#define DT_CPARSE_VALUE_TAG_T
typedef struct cparse_value_tag cparse_value_tag_t;
#endif

/* Create and destroy tag */
struct cparse_value_tag *cparse_value_tag_new(int type_tag, struct cparse_value *value);
void cparse_value_tag_destroy(cparse_value_tag_t *cparse_tag);
cparse_value_tag_t *cparse_value_tag_clone(cparse_value_tag_t *cparse_tag);

cparse_bool cparse_value_tag_is(cparse_value_tag_t *cparse_tag, int type_tag);
int cparse_value_tag_id(cparse_value_tag_t *cparse_tag);
struct cparse_value *cparse_value_tag_value(cparse_value_tag_t *cparse_tag);

#ifdef __cplusplus
}
#endif

#endif

