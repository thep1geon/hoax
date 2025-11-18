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
