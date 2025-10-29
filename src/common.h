#ifndef __COMMON_H
#define __COMMON_H

#include "typedef.h"

/* Gives us dynamic arrays, linked lists, slices, and optionals */
#include "generics.h"
SLICE_DECL(char); /* basically a string */

#define UNUSED(v) (void)v


struct file_location {
    u16 line;
    u16 column;
};

#endif  /*__COMMON_H*/
