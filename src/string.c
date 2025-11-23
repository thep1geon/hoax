#include "string.h"

struct slice(char) string_from_parts(char* ptr, usize length) {
    return (struct slice(char)){ptr, length};
}

bool string_equal(struct slice(char) a, struct slice(char) b) {
    usize i;

    if (a.length != b.length) return false;

    for (i = 0; i < a.length; ++i) {
        if (a.ptr[i] != b.ptr[i]) return false;
    }

    return true;
}

u64 string_hash(struct slice(char) key) {
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

