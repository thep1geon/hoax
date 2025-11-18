#ifndef __EXPR_H
#define __EXPR_H

#include "common.h"
#include "native.h"

/* @TODO: Implement native functions */
/* @TODO: Implement strings */
/* @TODO: Implement dynamic symbols */
/* @TODO: Update the enum prefix to EXPR from E */

enum expr_type {
    E_NIL,
    E_BOOLEAN,
    E_INTEGER,
    E_CONS,
    E_SYMBOL,
    E_NATIVE,
};

struct expr {
    /* the goal is to keep this union to the max of 8 bytes */
    union {
        bool boolean;
        i64 integer;
        /* 
         * Pointer into the source string. This way we don't have to worry about
         * freeing the expression's string.
         *
         * The length of this string is stored outside of the union to bypass
         * the padding of a struct of a pointer and a u8 inside of the union.
         * */
        char* symbol;
        native_fn native;
        struct {
            u32 car;
            u32 cdr;
        };
    };

    struct file_location loc; /* only used for exprs that come from parsing */
    u8 type;

    union {
        u8 length; /* used for the length of strings, symbols, and lists */
        u8 arity; /* used for the number of arguments a function or closure take */
    };

    /* essentially 2 free bytes if we need to store more infomation here */
    u16 padding;
};

DYNARRAY_DECL_S(expr);

extern struct dynarray(expr) exprs;

u32 expr_box(struct expr expr);

u32 expr_new();
u32 expr_new_nil();
u32 expr_new_boolean(bool boolean);
u32 expr_new_integer(i64 integer);
u32 expr_new_symbol(char* symbol, u8 length);
u32 expr_new_cons(u32 car, u32 cdr);
u32 expr_new_native(native_fn fn);

struct expr expr_create();
struct expr expr_create_nil();
struct expr expr_create_boolean(bool boolean);
struct expr expr_create_integer(i64 integer);
struct expr expr_create_symbol(char* symbol, u8 length);
struct expr expr_create_cons(u32 car, u32 cdr);
struct expr expr_create_native(native_fn fn);

/* Takes an index (pointer) into the expr array and returns the associated expr */
#define EXPR(ptr) exprs.at[(ptr)]

#define CAR(e) EXPR((e).car)
#define CDR(e) EXPR((e).cdr)

u8 nilp(struct expr expr);
u8 booleanp(struct expr expr);
u8 integerp(struct expr expr);
u8 symbolp(struct expr expr);
u8 consp(struct expr expr);
u8 nativep(struct expr expr);

/* 
 * @TODO: Have a default function which prints to stdout and have a different
 *        set of functions to specify the FILE to print to.
 * */
void expr_print(FILE* stream, struct expr expr);
void expr_println(FILE* stream, struct expr expr);

bool expr_is_truthy(struct expr expr);

u8 expr_cons_length(struct expr expr);

#endif  /*__EXPR_H*/
