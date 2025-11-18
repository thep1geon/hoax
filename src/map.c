#include <stdlib.h>

#include "string.h"
#include "expr.h"
#include "map.h"

u64 slice__char_hash(struct slice(char) key) {
    u64 P = 1571;
    u64 hash = 0; 
    u64 p = P;

    for (u64 i = 0; i < key.length; ++i) {
        for (u64 j = 0; j < i; ++j) {
            p *= P;
        }

        hash = (hash*(key.length-i)*i + ((u64)*key.ptr+i)*i) << (i);
        hash += key.length * i * p * (key.ptr[i] * key.ptr[i]);
        hash *= P*p;
    }

    return hash;
}

u64 map_compute_hash(struct map* map, struct slice(char) key) {
    return slice__char_hash(key) % map->size;
}

static void map_init(struct map* map, u64 map_size) {
    map->size = map_size;
    map->slots = calloc(map->size, sizeof(struct map_slot));
    assert(map->slots);
}

struct map map_create(u64 map_size) {
    struct map map = {0};

    map_init(&map, map_size);

    return map;
}

void map_destroy(struct map* map) {
    free(map->slots);
}

struct expr map_get(struct map* map, struct slice(char) key) {
    u64 hash;
    u64 initial_hash;
    struct map_slot slot;

    if (map->size == 0) return expr_create_nil();

    hash = map_compute_hash(map, key);
    initial_hash = hash;

    while (map->slots[hash].occupied && !string_equal(key, map->slots[hash].key)) {
        hash += 1;
        hash %= (map->size);

        if (hash == initial_hash) return expr_create_nil();
    }

    slot = map->slots[hash];
    if (!slot.occupied) return expr_create_nil();

    return slot.value;
}

struct expr map_set(struct map* map, struct slice(char) key, struct expr expr) {
    u64 hash;
    struct expr old_expr;

    if (map->size == 0) map_init(map, DEFAULT_MAP_SIZE);

    old_expr = expr_create_nil();

    hash = map_compute_hash(map, key);

    if (!map->slots[hash].occupied) {
        map->slots[hash].key = key;
        map->slots[hash].value = expr;
        map->slots[hash].occupied = true;
    } else {
        while (map->slots[hash].occupied && !string_equal(key, map->slots[hash].key)) {
            hash += 1;
            hash %= (map->size);
        }

        if (!map->slots[hash].occupied) {
            map->slots[hash].key = key;
            map->slots[hash].value = expr;
            map->slots[hash].occupied = true;
        } else {
            old_expr = map->slots[hash].value;
            map->slots[hash].value = expr;
        }
    }

    return old_expr;
}
