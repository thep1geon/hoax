#include <stdio.h>

#include "common.h"

#include "expr.h"
#include "reader.h"

struct expr_reader reader_create(struct slice(char) src) {
    return (struct expr_reader){ src, 0, (struct file_location){ 1, 1 } };
}

static inline void advance(struct expr_reader* reader) {
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
            return true;
        default:
            return false;
    }
}

static inline bool is_symbol(char c) {
    return is_digit(c) || is_alpha(c) || is_special_char(c);
}

static inline u8 bound(struct expr_reader* reader) {
    return reader->cursor < reader->src.length;
}

static inline void skip_space(struct expr_reader* reader) {
    while (bound(reader) && is_space(reader->src.ptr[reader->cursor]))
        advance(reader);
}

static inline char char_at(struct expr_reader* reader) {
    return reader->src.ptr[reader->cursor];
}

/* 
 * ~TODO: Add more error handling and actually handle the case of invalid
 * characters
 * */

u32 read_expr(struct expr_reader* reader) {
    struct file_location loc;
    u32 ptr;

    skip_space(reader);

    /* we've reached the end of the string */
    if (!bound(reader)) return 0;

    loc = reader->current_location;

    if (char_at(reader) == '(') {
        advance(reader);
        ptr = read_cons(reader);
        EXPR(ptr).loc = loc;
        EXPR(ptr).length = expr_cons_length(EXPR(ptr));
        return ptr;
    }

    ptr = read_atom(reader);
    EXPR(ptr).loc = loc;
    return ptr;
}

u32 read_atom(struct expr_reader* reader) {
    if (is_digit(char_at(reader))) {
        return read_integer(reader);
    }

    if (is_symbol(char_at(reader))) {
        return read_symbol(reader);
    }

    return 0;
}

u32 read_integer(struct expr_reader* reader) {
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

u32 read_symbol(struct expr_reader* reader) {
    u8 length = 1;
    char* symbol = reader->src.ptr + reader->cursor;
    advance(reader);

    while (is_symbol(char_at(reader)) && bound(reader)) {
        advance(reader);
        length += 1;
    }

    return expr_new_symbol(symbol, length);
}

u32 read_cons(struct expr_reader* reader) {
    u32 car;

    skip_space(reader);

    if (char_at(reader) == ')') {
        advance(reader);
        return 0;
    }

    /* 
     * ~TODO: Somehow figure out where the missing closing paren is supossed to
     *       go.
     * */
    if (!bound(reader)) {
        fprintf(stderr, "%d:%d: error: expected ')' found EOF instead\n",
                reader->current_location.line,
                reader->current_location.column);
        /* ~TODO: Find a better way to catch the error */
        exit(1);
    }

    car = read_expr(reader);

    return expr_new_cons(car, read_cons(reader));
}
