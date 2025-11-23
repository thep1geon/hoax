#ifndef __STRING_H
#define __STRING_H

#include <string.h>

#include "common.h"

SLICE_DECL(char);

/* 
 * Useful little string function for turning string literals into slices of 
 * characters
 * */
#define STRING(strlit) (struct slice(char)){.ptr = (strlit), .length = strlen((strlit))}

#define STRINGF(s) (i32)s.length, s.ptr

struct slice(char) string_from_parts(char* ptr, usize length);

bool string_equal(struct slice(char) a, struct slice(char) b);

u64 string_hash(struct slice(char) key);

#endif  /*__STRING_H*/
