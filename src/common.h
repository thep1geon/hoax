#ifndef __COMMON_H
#define __COMMON_H

#include <stdlib.h>
#include <stdio.h>

#include "typedef.h"

/* Gives us dynamic arrays, linked lists, slices, and optionals */
#include "generics.h"
#include "string.h"

#define UNUSED(v) (void)v
#define UNIMPLEMENTED() assert(0 && "Unimplemented");

/* 
 * @TODO: See about encoding a file location in a 32 bit integer with the
 * first 8 bits being the column and the last 24 being the line.
 *
 * Realistically, your source code should not be exceeding 256 columns, that's
 * just bad practice at that point.  With the current scheme you can have a max
 * of 65k lines in a single file, which is pretty good and very unlikely to be
 * surpassed any time soon.
 * */
struct file_location {
    u16 line;
    u16 column;
};

#define KILOBYTES(n) (n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

#define ARRAY_LENGTH(xs) (sizeof(xs)/sizeof(*xs))
#define ARRAY_FOR_EACH(arr, e) \
    for (usize __iter = 0; __iter < ARRAY_LENGTH((arr)) && ((e = (arr)[__iter]) || true); ++__iter)

#endif  /*__COMMON_H*/
