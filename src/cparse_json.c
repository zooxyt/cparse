/* CParse : JSON
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_libc.h"
#include "cparse_res.h"
#include "cparse_mstr.h"
#include "cparse_charenc.h"
#include "cparse_json.h"

typedef enum
{
    CPARSE_JSON_VAL_STRING = 0,
    CPARSE_JSON_VAL_NUMBER,
    CPARSE_JSON_VAL_OBJECT,
    CPARSE_JSON_VAL_ARRAY,
    CPARSE_JSON_VAL_TRUE,
    CPARSE_JSON_VAL_FALSE,
    CPARSE_JSON_VAL_NULL,
} cparse_json_val_type_t;

typedef struct cparse_json_val_object_node
{
    cparse_json_val_t *key;
    cparse_json_val_t *value;

    struct cparse_json_val_object_node *prev, *next;
} cparse_json_val_object_node_t;

typedef struct cparse_json_val_object
{
    cparse_json_val_object_node_t *begin, *end;
    cparse_size_t size;
} cparse_json_val_object_t;

typedef struct cparse_json_val_array_node
{
    cparse_json_val_t *val;
    struct cparse_json_val_array_node *prev, *next;
} cparse_json_val_array_node_t;

typedef struct cparse_json_val_array
{
    cparse_json_val_array_node_t *begin, *end;
    cparse_size_t size;
} cparse_json_val_array_t;

struct cparse_json_val
{
    cparse_json_val_type_t type;
    union
    {
        int part_number;
        cparse_char_t *part_string;
        struct cparse_json_val_object *part_object;
        struct cparse_json_val_array *part_array;
    } u;
};


static cparse_bool cparse_json_val_eqp( \
        cparse_json_val_t *value1, \
        cparse_json_val_t *value2);
static int cparse_json_stringify_val(cparse_mstr_t *mstr, cparse_json_val_t *val);

static cparse_json_val_object_node_t *cparse_json_val_object_node_new( \
        cparse_json_val_t *key, \
        cparse_json_val_t *value)
{
    cparse_json_val_object_node_t *new_object_node = NULL;

    if ((new_object_node = (cparse_json_val_object_node_t *)cparse_malloc( \
                    sizeof(cparse_json_val_object_node_t))) == NULL)
    { return NULL; }
    new_object_node->key = key;
    new_object_node->value = value;
    new_object_node->next = new_object_node->prev = NULL;

    return new_object_node;
}

static cparse_json_val_object_t *cparse_json_val_object_new(void)
{
    cparse_json_val_object_t *new_val_object = NULL;

    if ((new_val_object = (cparse_json_val_object_t *)cparse_malloc( \
                    sizeof(cparse_json_val_object_t))) == NULL)
    { return NULL; }
    new_val_object->begin = new_val_object->end = NULL;
    new_val_object->size = 0;

    return new_val_object;
}

static void cparse_json_val_object_node_destroy(cparse_json_val_object_node_t *node)
{
    if (node->key != NULL) cparse_json_val_destroy(node->key);
    if (node->value != NULL) cparse_json_val_destroy(node->value);
    cparse_free(node);
}

static void cparse_json_val_object_destroy(cparse_json_val_object_t *val_object)
{
    cparse_json_val_object_node_t *node_cur, *node_next;

    node_cur = val_object->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_json_val_object_node_destroy(node_cur);
        node_cur = node_next;
    }
    cparse_free(val_object);
}

static cparse_json_val_array_node_t *cparse_json_val_array_node_new(cparse_json_val_t *val)
{
    cparse_json_val_array_node_t *new_array_node = NULL;

    if ((new_array_node = (cparse_json_val_array_node_t *)cparse_malloc( \
                    sizeof(cparse_json_val_array_node_t))) == NULL)
    { return NULL; }
    new_array_node->val = val;
    new_array_node->next = new_array_node->prev = NULL;

    return new_array_node;
}

static void cparse_json_val_array_node_destroy(cparse_json_val_array_node_t *node)
{
    cparse_json_val_destroy(node->val);
    cparse_free(node);
}

static cparse_json_val_array_t *cparse_json_val_array_new(void)
{
    cparse_json_val_array_t *new_val_array = NULL;

    if ((new_val_array = (cparse_json_val_array_t *)cparse_malloc( \
                    sizeof(cparse_json_val_array_t))) == NULL)
    { return NULL; }
    new_val_array->begin = new_val_array->end = NULL;
    new_val_array->size = 0;

    return new_val_array;
}

static void cparse_json_val_array_destroy(cparse_json_val_array_t *val_array)
{
    cparse_json_val_array_node_t *node_cur, *node_next;

    node_cur = val_array->begin;
    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        cparse_json_val_array_node_destroy(node_cur);
        node_cur = node_next;
    }
    cparse_free(val_array);
}

static int cparse_json_val_array_push(cparse_json_val_array_t *val_array, \
        cparse_json_val_t *new_element)
{
    cparse_json_val_array_node_t *new_node = NULL;

    if ((new_node = cparse_json_val_array_node_new(new_element)) == NULL)
    { return -1; }

    if (val_array->begin == NULL)
    {
        val_array->begin = val_array->end = new_node;
    }
    else
    {
        new_node->prev = val_array->end;
        val_array->end->next = new_node;
        val_array->end = new_node;
    }
    val_array->size++;

    return 0;
}

static cparse_json_val_object_node_t *cparse_json_val_object_find( \
        cparse_json_val_object_t *val_object, \
        cparse_json_val_t *element_key)
{
    cparse_json_val_object_node_t *node_cur;

    node_cur = val_object->begin;
    while (node_cur != NULL)
    {
        if (cparse_json_val_eqp(node_cur->key, element_key) == cparse_true)
        { return node_cur; }

        node_cur = node_cur->next;
    }

    return NULL;
}

static int cparse_json_val_object_set(cparse_json_val_object_t *val_object, \
        cparse_json_val_t *new_element_key, \
        cparse_json_val_t *new_element_value)
{
    cparse_json_val_object_node_t *node_target = NULL;
    cparse_json_val_object_node_t *new_node = NULL;

    node_target = cparse_json_val_object_find(val_object, new_element_key);
    if (node_target != NULL)
    {
        cparse_json_val_destroy(new_element_key);
        cparse_json_val_destroy(node_target->value);
        node_target->value = new_element_value;
    }
    else
    {
        if ((new_node = cparse_json_val_object_node_new(new_element_key, \
                        new_element_value)) == NULL)
        { return -1; }
        if (val_object->begin == NULL)
        {
            val_object->begin = val_object->end = new_node;
        }
        else
        {
            new_node->prev = val_object->end;
            val_object->end->next = new_node;
            val_object->end = new_node;
        }
        val_object->size++;
    }

    return 0;
}

static cparse_json_val_t *cparse_json_val_new(cparse_json_val_type_t type)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = (cparse_json_val_t *)cparse_malloc( \
                    sizeof(cparse_json_val_t))) == NULL)
    { return NULL; }
    new_val->type = type;
    switch (type)
    {

        case CPARSE_JSON_VAL_STRING:
            new_val->u.part_string = NULL;
            break;
        case CPARSE_JSON_VAL_NUMBER:
            break;
        case CPARSE_JSON_VAL_OBJECT:
            new_val->u.part_object = NULL;
            break;
        case CPARSE_JSON_VAL_ARRAY:
            new_val->u.part_array = NULL;
            break;
        case CPARSE_JSON_VAL_TRUE:
        case CPARSE_JSON_VAL_FALSE:
        case CPARSE_JSON_VAL_NULL:
            break;
    }

    return new_val;
}

void cparse_json_val_destroy(cparse_json_val_t *val)
{
    switch (val->type)
    {
        case CPARSE_JSON_VAL_STRING:
            if (val->u.part_string != NULL)
            { cparse_free(val->u.part_string); }
            break;
        case CPARSE_JSON_VAL_NUMBER:
            break;
        case CPARSE_JSON_VAL_OBJECT:
            if (val->u.part_object != NULL)
            { cparse_json_val_object_destroy(val->u.part_object); }
            break;
        case CPARSE_JSON_VAL_ARRAY:
            if (val->u.part_array != NULL)
            { cparse_json_val_array_destroy(val->u.part_array); }
            break;
        case CPARSE_JSON_VAL_TRUE:
        case CPARSE_JSON_VAL_FALSE:
        case CPARSE_JSON_VAL_NULL:
            break;
    }
    cparse_free(val);
}

cparse_json_val_t *cparse_json_val_new_string(cparse_char_t *s)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = cparse_json_val_new( \
                    CPARSE_JSON_VAL_STRING)) == NULL)
    { return NULL; }
    if ((new_val->u.part_string = cparse_unicode_string_duplicate(s)) == NULL)
    { cparse_json_val_destroy(new_val); return NULL; }

    return new_val;
}

cparse_json_val_t *cparse_json_val_new_string_from_utf8(char *s)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = cparse_json_val_new( \
                    CPARSE_JSON_VAL_STRING)) == NULL)
    { return NULL; }
    if ((new_val->u.part_string = cparse_unicode_string_new_from_utf8(s, cparse_strlen(s))) == NULL)
    { cparse_json_val_destroy(new_val); return NULL; }

    return new_val;
}


cparse_json_val_t *cparse_json_val_new_number(int number)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = cparse_json_val_new( \
                    CPARSE_JSON_VAL_NUMBER)) == NULL)
    { return NULL; }
    new_val->u.part_number = number;

    return new_val;
}

cparse_json_val_t *cparse_json_val_new_false(void)
{
    return cparse_json_val_new(CPARSE_JSON_VAL_FALSE);
}

cparse_json_val_t *cparse_json_val_new_true(void)
{
    return cparse_json_val_new(CPARSE_JSON_VAL_TRUE);
}

cparse_json_val_t *cparse_json_val_new_null(void)
{
    return cparse_json_val_new(CPARSE_JSON_VAL_NULL);
}

cparse_json_val_t *cparse_json_val_new_array(void)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = cparse_json_val_new( \
                    CPARSE_JSON_VAL_ARRAY)) == NULL)
    { return NULL; }
    if ((new_val->u.part_array = cparse_json_val_array_new()) == NULL)
    { cparse_json_val_destroy(new_val); return NULL; }

    return new_val;
}

cparse_json_val_t *cparse_json_val_new_object(void)
{
    cparse_json_val_t *new_val = NULL;

    if ((new_val = cparse_json_val_new( \
                    CPARSE_JSON_VAL_OBJECT)) == NULL)
    { return NULL; }
    if ((new_val->u.part_object = cparse_json_val_object_new()) == NULL)
    { cparse_json_val_destroy(new_val); return NULL; }

    return new_val;
}

static cparse_bool cparse_json_val_eqp( \
        cparse_json_val_t *value1, \
        cparse_json_val_t *value2)
{
    cparse_bool ret = cparse_false;
    cparse_size_t len1, len2;

    if (value1->type != value2->type) return cparse_false;

    switch (value1->type)
    {
        case CPARSE_JSON_VAL_NUMBER:
            ret = (value1->u.part_number == value2->u.part_number) ? \
                  cparse_true : cparse_false;
            break;
        case CPARSE_JSON_VAL_TRUE:
        case CPARSE_JSON_VAL_FALSE:
        case CPARSE_JSON_VAL_NULL:
            ret = cparse_true;
            break;
        case CPARSE_JSON_VAL_ARRAY:
        case CPARSE_JSON_VAL_OBJECT:
            ret = cparse_false;
            break;
        case CPARSE_JSON_VAL_STRING:
            len1 = cparse_unicode_string_strlen(value1->u.part_string);
            len2 = cparse_unicode_string_strlen(value2->u.part_string);
            ret = ((len1 == len2) && \
                    (cparse_unicode_string_strncmp(value1->u.part_string, \
                                    value2->u.part_string, \
                                    len1) == 0)) ? \
                  cparse_true : cparse_false;
            break;
    }

    return ret;
}

int cparse_json_val_as_array_push(cparse_json_val_t *val, \
        cparse_json_val_t *new_element)
{
    return cparse_json_val_array_push(val->u.part_array, \
            new_element);
}

int cparse_json_val_as_object_set(cparse_json_val_t *val, \
        cparse_json_val_t *new_element_key, \
        cparse_json_val_t *new_element_value)
{
    return cparse_json_val_object_set(val->u.part_object, \
            new_element_key, new_element_value);
}

static cparse_size_t cparse_json_unescaped_len(cparse_char_t *s)
{
    cparse_char_t *p = s, *endp = p + cparse_unicode_string_strlen(p);
    cparse_size_t len = 0;

    while (p != endp)
    {
        switch (*p)
        {
            case '\v':
            case '\t':
            case '\r':
            case '\n':
            case '\f':
                len++;
                break;
            default:
                break;
        }
        p++;
        len++;
    }

    return len;
}

static void cparse_json_memcpy_unescaped(cparse_char_t *dst, cparse_char_t *src, cparse_size_t len)
{
    while (len-- != 0)
    {
        switch (*src)
        {
            case '\v': *dst++ = '\\'; *dst++ = 'v'; break;
            case '\t': *dst++ = '\\'; *dst++ = 't'; break;
            case '\r': *dst++ = '\\'; *dst++ = 'r'; break;
            case '\n': *dst++ = '\\'; *dst++ = 'n'; break;
            case '\f': *dst++ = '\\'; *dst++ = 'f'; break;
            default:
                *dst++ = *src++;
                break;
        }
    }
}

static int cparse_json_stringify_val_string(cparse_mstr_t *mstr, \
        cparse_char_t *s)
{
    cparse_size_t unescaped_len = cparse_json_unescaped_len(s);
    cparse_char_t *buf = NULL;
    
    if ((buf = (cparse_char_t *)cparse_malloc( \
                    sizeof(cparse_char_t) * (unescaped_len + 1))) == NULL)
    { return -1; }
    cparse_json_memcpy_unescaped(buf, s, cparse_unicode_string_strlen(s));
    buf[unescaped_len] = '\0';

    cparse_mstr_append_str_from_utf8(mstr, "\"");
    cparse_mstr_append_str(mstr, buf);
    cparse_mstr_append_str_from_utf8(mstr, "\"");

    cparse_free(buf);
    return 0;
}

static int cparse_json_stringify_val_number(cparse_mstr_t *mstr, \
        int number)
{
    char buffer[CPARSE_MSTR_BUFFER_LEN]; 
    cparse_size_t len;
    len = (cparse_size_t)cparse_snprintf(buffer, CPARSE_MSTR_BUFFER_LEN, "%d", number);
    return cparse_mstr_append_strn_from_utf8(mstr, buffer, len);
}

static int cparse_json_stringify_val_object(cparse_mstr_t *mstr, \
        cparse_json_val_object_t *val_object)
{
    cparse_bool first = cparse_true;
    cparse_json_val_object_node_t *node_cur; 

    cparse_mstr_append_str_from_utf8(mstr, "{");
    node_cur = val_object->begin;
    while (node_cur != NULL)
    {
        if (first == cparse_true) first = cparse_false;
        else cparse_mstr_append_str_from_utf8(mstr, ",");

        if (cparse_json_stringify_val(mstr, node_cur->key) != 0)
        { return -1; }
        cparse_mstr_append_str_from_utf8(mstr, ":");

        if (cparse_json_stringify_val(mstr, node_cur->value) != 0)
        { return -1; }

        node_cur = node_cur->next;
    }
    cparse_mstr_append_str_from_utf8(mstr, "}");

    return 0;
}

static int cparse_json_stringify_val_array(cparse_mstr_t *mstr, \
        cparse_json_val_array_t *val_array)
{
    cparse_bool first = cparse_true;
    cparse_json_val_array_node_t *node_cur; 

    cparse_mstr_append_str_from_utf8(mstr, "[");
    node_cur = val_array->begin;
    while (node_cur != NULL)
    {
        if (first == cparse_true) first = cparse_false;
        else cparse_mstr_append_str_from_utf8(mstr, ",");

        if (cparse_json_stringify_val(mstr, node_cur->val) != 0)
        { return -1; }

        node_cur = node_cur->next;
    }
    cparse_mstr_append_str_from_utf8(mstr, "]");

    return 0;
}

static int cparse_json_stringify_val(cparse_mstr_t *mstr, cparse_json_val_t *val)
{
    int ret = 0;

    switch (val->type)
    {
        case CPARSE_JSON_VAL_STRING:
            ret = cparse_json_stringify_val_string(mstr, val->u.part_string);
            break;
        case CPARSE_JSON_VAL_NUMBER:
            ret = cparse_json_stringify_val_number(mstr, val->u.part_number);
            break;
        case CPARSE_JSON_VAL_OBJECT:
            ret = cparse_json_stringify_val_object(mstr, val->u.part_object);
            break;
        case CPARSE_JSON_VAL_ARRAY:
            ret = cparse_json_stringify_val_array(mstr, val->u.part_array);
            break;
        case CPARSE_JSON_VAL_TRUE:
            ret = cparse_mstr_append_str_from_utf8(mstr, "true");
            break;
        case CPARSE_JSON_VAL_FALSE:
            ret = cparse_mstr_append_str_from_utf8(mstr, "false");
            break;
        case CPARSE_JSON_VAL_NULL:
            ret = cparse_mstr_append_str_from_utf8(mstr, "null");
            break;
    }

    return ret;
}

int cparse_json_stringify(cparse_char_t **s_out, \
        cparse_size_t *len_out, \
        cparse_json_val_t *val)
{
    cparse_mstr_t *new_mstr = NULL;

    if ((new_mstr = cparse_mstr_new()) == NULL)
    { return -1; }

    if (cparse_json_stringify_val(new_mstr, val) != 0)
    { cparse_mstr_destroy(new_mstr); return -1; }

    if (len_out != NULL)
    { *len_out = cparse_mstr_len(new_mstr); }
    if (s_out != NULL)
    { *s_out = cparse_mstr_yield(new_mstr); }

    cparse_mstr_destroy(new_mstr);

    return 0;
}

int cparse_json_stringify_to_utf8(char **s_out, \
        cparse_size_t *len_out, \
        cparse_json_val_t *val)
{
    int ret = 0;
    cparse_char_t *json_unicode_str = NULL;
    char *json_unicode_str_utf8 = NULL;
    cparse_size_t json_unicode_str_len;

    if (cparse_json_stringify(&json_unicode_str, \
                &json_unicode_str_len, \
                val) != 0)
    { return -1; }

    if ((json_unicode_str_utf8 = cparse_unicode_string_to_utf8( \
                    json_unicode_str, json_unicode_str_len)) == NULL)
    { ret = -1; goto fail; }

    if (s_out != NULL) *s_out = json_unicode_str_utf8; 
    if (len_out != NULL) *len_out = cparse_strlen(json_unicode_str_utf8);
    json_unicode_str_utf8 = NULL;

    goto done;
fail:
done:
    if (json_unicode_str != NULL) cparse_free(json_unicode_str);
    return ret;
}

