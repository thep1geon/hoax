#ifndef __READER_H
#define __READER_H

#include "common.h"

/* ~TODO: Implement a way to convey errors while reading */

struct expr_reader {
    struct slice(char) src;
    u32 cursor;
    struct file_location current_location;
};

struct expr_reader reader_create(struct slice(char) src);

u32 read_expr(struct expr_reader* reader);
u32 read_atom(struct expr_reader* reader);
u32 read_integer(struct expr_reader* reader);
u32 read_symbol(struct expr_reader* reader);
u32 read_cons(struct expr_reader* reader);

#endif  /* __READER_H */
