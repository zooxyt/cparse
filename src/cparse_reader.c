/* CParse : Reader
 * Copyright(c) Cheryl Natsu */

#include "cparse_dt.h"
#include "cparse_res.h"
#include "cparse_charenc.h"
#include "cparse_reader.h"

#define CPARSE_READER_CR 13
#define CPARSE_READER_LF 10

struct cparse_reader_state
{
    cparse_char_t *cur;
    cparse_size_t ln, col;
};

struct cparse_reader_state_stack
{
    struct cparse_reader_state *body;
    /* struct cparse_reader_state *sp; */
    cparse_size_t size, capacity;
};

struct cparse_reader
{
    /* Source */
    cparse_char_t *s;
    cparse_size_t len;

    /* Current State */
    cparse_char_t *cur;
    cparse_size_t ln, col;

    /* Preserved States */
    struct cparse_reader_state_stack *state_stack;
};

static struct cparse_reader_state_stack *cparse_reader_state_stack_new(cparse_size_t capacity)
{
    struct cparse_reader_state_stack *new_stack = NULL;

    if ((new_stack = (struct cparse_reader_state_stack *)cparse_malloc( \
                    sizeof(struct cparse_reader_state_stack))) == NULL)
    { return NULL; }
    new_stack->size = 0;
    new_stack->capacity = capacity;
    if ((new_stack->body = (struct cparse_reader_state *)cparse_malloc( \
                    sizeof(struct cparse_reader_state) * capacity)) == NULL)
    { cparse_free(new_stack); return NULL; }
    /* new_stack->sp = new_stack->body; */

    return new_stack;
}

static void cparse_reader_state_stack_destroy(struct cparse_reader_state_stack *state_stack)
{
    if (state_stack->body != NULL) cparse_free(state_stack->body);
    cparse_free(state_stack);
}

struct cparse_reader *cparse_reader_new_stringn_from_utf8(const char *s, const cparse_size_t len)
{
    struct cparse_reader *new_reader = NULL;
    cparse_size_t new_unicode_string_len;
    cparse_char_t *new_unicode_string = NULL;

    if ((new_unicode_string = cparse_unicode_string_new_from_utf8(s, len)) == NULL)
    { return NULL; }
    new_unicode_string_len = cparse_unicode_string_strlen(new_unicode_string);

    if ((new_reader = (struct cparse_reader *)cparse_malloc( \
                    sizeof(struct cparse_reader))) == NULL)
    { goto fail; }
    new_reader->s = new_unicode_string;
    new_reader->len = new_unicode_string_len;
    new_reader->cur = new_reader->s;
    new_reader->ln = 1;
    new_reader->col = 1;
    if ((new_reader->state_stack = cparse_reader_state_stack_new( \
                    CPARSE_READER_STATE_STACK_SIZE_MAX)) == NULL)
    { goto fail; }

    goto done;
fail:
    if (new_reader != NULL)
    {
        cparse_reader_destroy(new_reader);
        new_reader = NULL;
    }
done:
    return new_reader;
}

void cparse_reader_destroy(struct cparse_reader *cparse_reader)
{
    if (cparse_reader->state_stack != NULL)
    { cparse_reader_state_stack_destroy(cparse_reader->state_stack); }
    if (cparse_reader->s != NULL)
    { cparse_free(cparse_reader->s); }
    cparse_free(cparse_reader);
}

/* State */
void cparse_reader_save(struct cparse_reader *reader)
{
    /* struct cparse_reader_state *state_dst = reader->state_stack->sp; */
    struct cparse_reader_state *state_dst;
    state_dst = reader->state_stack->body + reader->state_stack->size;

    state_dst->ln = reader->ln;
    state_dst->col = reader->col;
    state_dst->cur = reader->cur;
    /* reader->state_stack->sp++; */
    reader->state_stack->size++;
}

void cparse_reader_load(struct cparse_reader *reader)
{
    struct cparse_reader_state *state_src;

    reader->state_stack->size--;
    /* reader->state_stack->sp--; */
    /* state_src = reader->state_stack->sp; */
    state_src = reader->state_stack->body + reader->state_stack->size;

    reader->ln = state_src->ln;
    reader->col = state_src->col;
    reader->cur = state_src->cur;
}

void cparse_reader_drop(struct cparse_reader *reader)
{
    struct cparse_reader_state *state_src;

    reader->state_stack->size--;
    /* reader->state_stack->sp--; */
    /* state_src = reader->state_stack->sp; */
    state_src = reader->state_stack->body + reader->state_stack->size - 1;

    reader->ln = state_src->ln;
    reader->col = state_src->col;
    reader->cur = state_src->cur;
}

void cparse_reader_erase(struct cparse_reader *reader)
{
    reader->state_stack->size--;
    /* reader->state_stack->sp--; */
}

/* EOF/BOF */
cparse_bool cparse_reader_eof(struct cparse_reader *reader)
{
    return (reader->cur == reader->s + reader->len) ? cparse_true : cparse_false;
}

cparse_bool cparse_reader_bof(struct cparse_reader *reader)
{
    return (reader->cur == reader->s) ? cparse_true : cparse_false;
}

/* Move */
int cparse_reader_forward(struct cparse_reader *reader)
{
    if (cparse_reader_eof(reader) == cparse_true) return -1;

    if (cparse_reader_peek_char(reader) == CPARSE_READER_CR)
    {
        reader->ln++;
        reader->col = 1;
        reader->cur++;
        if ((cparse_reader_eof(reader) == cparse_true) &&
                (cparse_reader_peek_char(reader) == CPARSE_READER_LF))
        {
            reader->cur++;
        }
    }
    else if (cparse_reader_peek_char(reader) == CPARSE_READER_LF)
    {
        reader->ln++;
        reader->col = 1;
        reader->cur++;
    }
    else
    {
        reader->cur++;
        reader->col++;
    }

    return 0;
}

int cparse_reader_forward_step(struct cparse_reader *reader, int step)
{
    while (step-- > 0)
    {
        if (cparse_reader_forward(reader) != 0) return -1;
    }

    return 0;
}

/* Peek Char */
int cparse_reader_peek_char(struct cparse_reader *reader)
{
    return (int)(*reader->cur);
}

/* Get Char */
int cparse_reader_get_char(struct cparse_reader *reader)
{
    int ch = cparse_reader_peek_char(reader);
    cparse_reader_forward(reader);

    return ch;
}

/* Get Position */
cparse_size_t cparse_reader_ln(struct cparse_reader *reader)
{
    return reader->ln;
}

cparse_size_t cparse_reader_col(struct cparse_reader *reader)
{
    return reader->col;
}

/* Ptr */
cparse_char_t *cparse_reader_ptr(struct cparse_reader *reader)
{
    return reader->cur;
}

