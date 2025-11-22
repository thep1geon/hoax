#include <stdio.h>

#include "common.h"

#include "expr.h"
#include "reader.h"

struct reader reader_create(struct slice(char) src) {
    return (struct reader){ src, (struct file_location){ 1, 1 }, 0, 0 };
}

static inline void advance(struct reader* reader) {
    reader->current_location.column += 1;

    if (reader->src.ptr[reader->cursor] == '\n') {
        reader->current_location.line += 1;
        reader->current_location.column = 1;
    }

    reader->cursor += 1;
}

static inline bool is_space(char c) {
    switch (c) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            return true;
    }

    return false;
}

static inline bool is_digit(char c) { return '0' <= c && c <= '9'; }

static inline bool is_alpha(char c) {
    return (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_');
}

static inline bool is_special_char(char c) {
    switch (c) {
        case '?':
        case '!':
        case '.':
        case '+':
        case '-':
        case '*':
        case '/':
        case '<':
        case '=':
        case '>':
        case ':':
        case '$':
        case '%':
        case '^':
        case '&':
        case '_':
        case '~':
        case '#':
            return true;
        default:
            return false;
    }
}

static inline bool is_symbol(char c) {
    return is_digit(c) || is_alpha(c) || is_special_char(c);
}

static inline u8 bound(struct reader* reader) {
    return reader->cursor < reader->src.length;
}

static inline void skip_space(struct reader* reader) {
    while (bound(reader) && is_space(reader->src.ptr[reader->cursor]))
        advance(reader);
}

static inline char char_at(struct reader* reader) {
    return reader->src.ptr[reader->cursor];
}

static inline char char_peek(struct reader* reader) {
    if (reader->cursor + 1 >= reader->src.length) return '\0';
    return reader->src.ptr[reader->cursor + 1];
}

static inline void skip_comment(struct reader* reader) {
    while (bound(reader) && char_at(reader) != '\n') {
        advance(reader);
    }

    advance(reader);
}

u32 read_expr(struct reader* reader) {
read_expr_begin:
    struct file_location loc;
    u32 ptr;

    skip_space(reader);

    /* we've reached the end of the string */
    if (!bound(reader)) return 0;

    loc = reader->current_location;

    if (char_at(reader) == ';' && char_peek(reader) == ';') {
        advance(reader);
        advance(reader);
        skip_comment(reader);
        goto read_expr_begin;
    }

    if (char_at(reader) == '(') {
        advance(reader);
        ptr = read_cons(reader);
    } else {
        ptr = read_atom(reader);
    }

    if (ptr != READER_ERROR) {
        EXPR(ptr).loc = loc;
    }
    return ptr;
}

u32 read_atom(struct reader* reader) {
    if (is_digit(char_at(reader))) {
        return read_integer(reader);
    }

    if (is_symbol(char_at(reader))) {
        return read_symbol(reader);
    }

    if (char_at(reader) == ')') {
        fprintf(stderr, "(%d:%d) error: unexpected ')'\n", 
                reader->current_location.line, reader->current_location.column);
        reader->error_code = READER_ERROR_UNEXPECTED_CLOSING_PAREN;
    }
    else {
        fprintf(stderr, "(%d:%d) error: unknown character: '%c'\n",
                reader->current_location.line, reader->current_location.column, char_at(reader));
        reader->error_code = READER_ERROR_UNEXPECTED_CHARACTER;
    }

    advance(reader);

    return READER_ERROR;
}

u32 read_integer(struct reader* reader) {
    char buf[32] = {0};
    usize buf_ptr = 0;
    i64 integer;

    buf[buf_ptr++] = char_at(reader);
    advance(reader);

    while (is_digit(char_at(reader)) && bound(reader)) {
        buf[buf_ptr++] = char_at(reader);
        advance(reader);
    }

    integer = atoll(buf);

    return expr_new_integer(integer);
}

u32 read_symbol(struct reader* reader) {
    u8 length = 1;
    char* symbol = reader->src.ptr + reader->cursor;
    advance(reader);

    while (is_symbol(char_at(reader)) && bound(reader)) {
        advance(reader);
        length += 1;
    }

    return expr_new_symbol(symbol, length);
}

/* 
 * @TODO: See if there is a better way to calculate the length of a list without
 *  having to traverse the list multiple times.
 * */
u32 read_cons(struct reader* reader) {
    u32 car;
    u32 cons;

    skip_space(reader);

    if (char_at(reader) == ')') {
        advance(reader);
        return 0;
    }

    /* 
     * @TODO: Somehow figure out where the missing closing paren is supossed to
     *       go.
     * */
    if (!bound(reader)) {
        fprintf(stderr, "(%d:%d): error: expected ')', found EOF instead\n",
                reader->current_location.line,
                reader->current_location.column);
        reader->error_code = READER_ERROR_UNEXPECTED_EOF;
        return READER_ERROR;
    }

    car = read_expr(reader);

    if (car == READER_ERROR) return car;

    cons = expr_new_cons(car, read_cons(reader));

    if (reader->error_code != 0) return READER_ERROR;

    EXPR(cons).length = expr_cons_length(EXPR(cons));

    return cons;
}
