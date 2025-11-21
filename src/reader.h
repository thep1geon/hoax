#ifndef __READER_H
#define __READER_H

#include "common.h"

/* @TODO: Implement a way to convey errors while reading */

struct reader {
    struct slice(char) src;
    u32 cursor;
    struct file_location current_location;
};

struct reader reader_create(struct slice(char) src);

u32 read_expr(struct reader* reader);
u32 read_atom(struct reader* reader);
u32 read_integer(struct reader* reader);
u32 read_symbol(struct reader* reader);
u32 read_cons(struct reader* reader);

#endif  /* __READER_H */
