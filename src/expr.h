#ifndef __EXPR_H
#define __EXPR_H

#include "common.h"

/* ~TODO: Implement strings */

enum expr_type {
    E_NIL,
    E_INTEGER,
    E_CONS,
    E_SYMBOL,
    E_TYPE_COUNT,
};

struct expr {
    /* the goal is to keep this union to the max of 8 bytes */
    union {
        i64 integer;
        /* 
         * Pointer into the source string. This way we don't have to worry about
         * freeing the expression's string.
         *
         * The length of this string is stored outside of the union to bypass
         * the padding of a struct of a pointer and a u8 inside of the union.
         * */
        char* symbol;
        struct {
            u32 car;
            u32 cdr;
        };
    };

    struct file_location loc; /* only used for exprs that come from parsing */
    u8 type;
    u8 length; /* only used for strings and symbols */

    /* essentially 2 free bytes if we need to store more infomation here */
    u16 padding;
};

DYNARRAY_DECL_S(expr);

extern struct dynarray(expr) exprs;

u32 expr_new();
u32 expr_new_nil();
u32 expr_new_integer(i64 integer);
u32 expr_new_symbol(char* symbol, u8 length);
u32 expr_new_cons(u32 car, u32 cdr);

struct expr expr_create();
struct expr expr_create_nil();
struct expr expr_create_integer(i64 integer);
struct expr expr_create_symbol(char* symbol, u8 length);
struct expr expr_create_cons(u32 car, u32 cdr);

/* Takes an index (pointer) into the expr array and returns the associated expr */
#define EXPR(ptr) exprs.at[(ptr)]

#define CAR(e) EXPR((e).car)
#define CDR(e) EXPR((e).cdr)

u8 nilp(struct expr expr);
u8 integerp(struct expr expr);
u8 symbolp(struct expr expr);
u8 consp(struct expr expr);

void expr_print(struct expr expr);
void expr_println(struct expr expr);

u8 expr_cons_length(struct expr expr);

#endif  /*__EXPR_H*/
