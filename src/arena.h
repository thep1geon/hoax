#ifndef __ARENA_H
#define __ARENA_H

#include "common.h"

#define ARENA_DEFAULT_CAP (KILOBYTES(4))

struct arena {
    void* mem_start;
    void* mem_cursor;
    usize capacity; /**/
};

struct arena arena_create(usize capacity);

void arena_destroy(struct arena* arena);
void arena_clear(struct arena* arena);

void* arena_alloc(struct arena* arena, usize size);
usize arena_used(const struct arena* arena);

#endif  /*__ARENA_H*/
