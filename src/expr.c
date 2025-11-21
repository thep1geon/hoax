#include <stdio.h>

#include "expr.h"
#include "native.h"

DYNARRAY_IMPL_S(expr);

struct dynarray(expr) exprs = {0};

u32 expr_box(struct expr expr) {
    /* The dynarray is too full for our needs */
    /* We have to be careful of overflowing our u32 */
    if (exprs.length >= (usize)(1 << 31)) {
        assert(0 && "We need to fix this asap!");
        return 0;
    }

    dynarray__expr_push(&exprs, expr);

    return exprs.length - 1;
}

u32 expr_new() {
    struct expr expr = {0};
    return expr_box(expr);
}

u32 expr_new_nil() {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_NIL;

    return ptr;
}

u32 expr_new_boolean(bool boolean) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_BOOLEAN;
    exprs.at[ptr].boolean = boolean;

    return ptr;
}

u32 expr_new_integer(i64 integer) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_INTEGER;
    exprs.at[ptr].integer = integer;

    return ptr;
}

u32 expr_new_symbol(char* symbol, u8 length) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_SYMBOL;
    exprs.at[ptr].symbol = symbol;
    exprs.at[ptr].length = length;

    return ptr;
}

u32 expr_new_cons(u32 car, u32 cdr) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_CONS;
    exprs.at[ptr].car = car;
    exprs.at[ptr].cdr = cdr;

    return ptr;
}

u32 expr_new_native(native_fn fn, u8 arity) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = EXPR_NATIVE;
    exprs.at[ptr].native = fn;
    exprs.at[ptr].arity = arity;

    return ptr;
}

struct expr expr_create() {
    return (struct expr){0};
}

struct expr expr_create_nil() {
    struct expr expr = expr_create();
    expr.type = EXPR_NIL;

    return expr;
}

struct expr expr_create_boolean(bool boolean) {
    struct expr expr = expr_create();

    expr.type = EXPR_BOOLEAN;
    expr.boolean = boolean;

    return expr;
}

struct expr expr_create_integer(i64 integer) {
    struct expr expr = expr_create();
    expr.type = EXPR_INTEGER;
    expr.integer = integer;

    return expr;
}

struct expr expr_create_symbol(char* symbol, u8 length) {
    struct expr expr = expr_create();
    expr.type = EXPR_SYMBOL;
    expr.symbol = symbol;
    expr.length = length;

    return expr;
}

struct expr expr_create_cons(u32 car, u32 cdr) {
    struct expr expr = expr_create();
    expr.type = EXPR_CONS;
    expr.car = car;
    expr.cdr = cdr;

    return expr;
}

struct expr expr_create_native(native_fn fn, u8 arity) {
    struct expr expr = expr_create();
    expr.type = EXPR_NATIVE;
    expr.native = fn;
    expr.arity = arity;

    return expr;
}

/* Takes an index (pointer) into the expr array and returns the associated expr */
#define EXPR(ptr) exprs.at[(ptr)]

#define CAR(e) EXPR((e).car)
#define CDR(e) EXPR((e).cdr)

u8 nilp(struct expr expr) { return expr.type == EXPR_NIL; }
u8 boolp(struct expr expr) { return expr.type == EXPR_BOOLEAN; }
u8 integerp(struct expr expr) { return expr.type == EXPR_INTEGER; }
u8 symbolp(struct expr expr) { return expr.type == EXPR_SYMBOL; }
u8 consp(struct expr expr) { return expr.type == EXPR_CONS; }
u8 nativep(struct expr expr) { return expr.type == EXPR_NATIVE; }

void expr_print(struct expr expr) {
    expr_fprint(stdout, expr);
}

void expr_println(struct expr expr) {
    expr_fprintln(stdout, expr);
}

void expr_fprint(FILE* stream, struct expr expr) {
    switch ((enum expr_type) expr.type) {
        case EXPR_NIL:
            fprintf(stream, "nil");
            break;
        case EXPR_INTEGER:
            fprintf(stream, "%ld", expr.integer);
            break;
        case EXPR_SYMBOL:
            fprintf(stream, "%.*s", (i32)expr.length, expr.symbol);
            break;
        case EXPR_CONS:
            fprintf(stream, "(");
            expr_fprint(stream, CAR(expr));
            fprintf(stream, " . ");
            expr_fprint(stream, CDR(expr));
            fprintf(stream, ")");
            break;
        case EXPR_BOOLEAN:
            fprintf(stream, expr.boolean ? "T" : "F");
            break;
        case EXPR_NATIVE:
            fprintf(stream, "<native fn>");
            break;
    }
}

void expr_fprintln(FILE* stream, struct expr expr) {
    expr_fprint(stream, expr);
    putchar('\n');
}

bool expr_is_truthy(struct expr expr) {
    switch (expr.type) {
        case EXPR_NIL:
            return false;
        case EXPR_BOOLEAN:
            return expr.boolean;
        case EXPR_INTEGER:
            return expr.integer != 0;
        case EXPR_CONS:
            return expr.length != 0;
        case EXPR_NATIVE:
            return expr.native != NULL;
        case EXPR_SYMBOL:
            UNIMPLEMENTED();
   }

    return false;
}

static u8 __expr_cons_length(struct expr expr, u8 acc) {
    if (!consp(CDR(expr))) return acc;

    return __expr_cons_length(CDR(expr), acc+1);
}

u8 expr_cons_length(struct expr expr) {
    if (!consp(expr)) return 0;
    return __expr_cons_length(expr, 1);
}

u32 expr_cons_append(u32 list, struct expr expr) {
    u32 cdr;
    if (list == 0) return expr_new_cons(expr_box(expr), 0);

    cdr = EXPR(list).cdr;


    EXPR(list).cdr = expr_cons_append(cdr, expr);
    return list;
}

/*
  (define .rev (lambda (xs acc)
                (cond
                    ((empty? xs) acc)
                    (else (let ( (x (car xs)) 
                               (.rev (cdr xs) (cons x) acc))))))))
*/

u32 __expr_cons_reverse(u32 list, u32 acc) {
    if (expr_cons_length(EXPR(list)) == 0) return acc;
    return __expr_cons_reverse(EXPR(list).cdr, expr_new_cons(EXPR(list).car, acc));
}

u32 expr_cons_reverse(u32 list) {
    return __expr_cons_reverse(list, 0);
}

struct expr expr_native_call(struct expr func, struct expr args) {
    assert(nativep(func));

    return func.native(args);
}
