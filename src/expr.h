#ifndef __EXPR_H
#define __EXPR_H

#include "arena.h"
#include "common.h"
#include "native.h"

/* @TODO: Implement dynamic symbols */
/* @TODO: Implement strings */
/* @TODO: Implement floating point numbers */
/* @TODO: Implement some sort of garbage collection for the "heap" */

enum expr_type {
    EXPR_NIL,
    EXPR_BOOLEAN,
    EXPR_INTEGER,
    EXPR_CONS,
    EXPR_SYMBOL,
    EXPR_NATIVE,
};

struct expr {
    /* the goal is to keep this union to the max of 8 bytes */
    union {
        bool boolean;
        i64 integer;
        /* 
         * Pointer to the string stored in the expr arena allocator.
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
SMAP_DECL_S(expr);

extern struct dynarray(expr) exprs;
extern struct arena expr_arena;

u32 expr_box(struct expr expr);

u32 expr_new();
u32 expr_new_nil();
u32 expr_new_boolean(bool boolean);
u32 expr_new_integer(i64 integer);
u32 expr_new_symbol(char* symbol, u8 length);
u32 expr_new_cons(u32 car, u32 cdr);
u32 expr_new_native(native_fn fn, u8 arity);

struct expr expr_create();
struct expr expr_create_nil();
struct expr expr_create_boolean(bool boolean);
struct expr expr_create_integer(i64 integer);
struct expr expr_create_symbol(char* symbol, u8 length);
struct expr expr_create_cons(u32 car, u32 cdr);
struct expr expr_create_native(native_fn fn, u8 arity);

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

void expr_fprint(FILE* stream, struct expr expr);
void expr_fprintln(FILE* stream, struct expr expr);
void expr_print(struct expr expr);
void expr_println(struct expr expr);

bool expr_is_truthy(struct expr expr);

u8 expr_cons_length(struct expr expr);
u32 expr_cons_append(u32 list, struct expr expr);
u32 expr_cons_reverse(u32 list);

struct expr expr_native_call(struct expr func, struct expr args);

#endif  /*__EXPR_H*/
