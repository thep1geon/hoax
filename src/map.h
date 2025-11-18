#ifndef __MAP_H
#define __MAP_H

#include "common.h"
#include "generics.h"
#include "expr.h"

#define DEFAULT_MAP_SIZE (1024)

struct map_slot {
    struct slice(char) key;
    struct expr value;
    bool occupied;
};

struct map {
    struct map_slot* slots;
    u64 size;
};

u64 slice__char_hash(struct slice(char) key);

struct map map_create(u64 map_size);
void map_destroy(struct map* map);

u64 map_compute_hash(struct map* map, struct slice(char) key);

struct expr map_get(struct map* map, struct slice(char) key);
struct expr map_set(struct map* map, struct slice(char) key, struct expr expr);

#endif  /*__MAP_H*/
