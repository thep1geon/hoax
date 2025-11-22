#ifndef __READER_H
#define __READER_H

#include "common.h"

#define READER_ERROR UINT32_MAX

enum reader_error_code {
    READER_ERROR_UNEXPECTED_CLOSING_PAREN = 1,
    READER_ERROR_UNEXPECTED_CHARACTER,
    READER_ERROR_UNEXPECTED_EOF,
};

struct reader {
    struct slice(char) src;
    struct file_location current_location;
    u32 cursor;
    u32 error_code;
};

struct reader reader_create(struct slice(char) src);

u32 read_expr(struct reader* reader);
u32 read_atom(struct reader* reader);
u32 read_integer(struct reader* reader);
u32 read_symbol(struct reader* reader);
u32 read_cons(struct reader* reader);

#endif  /* __READER_H */
