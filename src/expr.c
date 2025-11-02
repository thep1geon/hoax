#include <stdio.h>

#include "expr.h"

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

    exprs.at[ptr].type = E_NIL;

    return ptr;
}

u32 expr_new_boolean(bool boolean) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = E_BOOLEAN;
    exprs.at[ptr].boolean = boolean;

    return ptr;
}

u32 expr_new_integer(i64 integer) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = E_INTEGER;
    exprs.at[ptr].integer = integer;

    return ptr;
}

u32 expr_new_symbol(char* symbol, u8 length) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = E_SYMBOL;
    exprs.at[ptr].symbol = symbol;
    exprs.at[ptr].length = length;

    return ptr;
}

u32 expr_new_cons(u32 car, u32 cdr) {
    u32 ptr = expr_new();

    exprs.at[ptr].type = E_CONS;
    exprs.at[ptr].car = car;
    exprs.at[ptr].cdr = cdr;

    return ptr;
}

struct expr expr_create() {
    return (struct expr){0};
}

struct expr expr_create_nil() {
    struct expr expr = expr_create();
    expr.type = E_NIL;

    return expr;
}

struct expr expr_create_boolean(bool boolean) {
    struct expr expr = expr_create();

    expr.type = E_BOOLEAN;
    expr.boolean = boolean;

    return expr;
}

struct expr expr_create_integer(i64 integer) {
    struct expr expr = expr_create();
    expr.type = E_INTEGER;
    expr.integer = integer;

    return expr;
}

struct expr expr_create_symbol(char* symbol, u8 length) {
    struct expr expr = expr_create();
    expr.type = E_SYMBOL;
    expr.symbol = symbol;
    expr.length = length;

    return expr;
}

struct expr expr_create_cons(u32 car, u32 cdr) {
    struct expr expr = expr_create();
    expr.type = E_CONS;
    expr.car = car;
    expr.cdr = cdr;

    return expr;
}

/* Takes an index (pointer) into the expr array and returns the associated expr */
#define EXPR(ptr) exprs.at[(ptr)]

#define CAR(e) EXPR((e).car)
#define CDR(e) EXPR((e).cdr)

u8 nilp(struct expr expr) { return expr.type == E_NIL; }
u8 boolp(struct expr expr) { return expr.type == E_BOOLEAN; }
u8 integerp(struct expr expr) { return expr.type == E_INTEGER; }
u8 symbolp(struct expr expr) { return expr.type == E_SYMBOL; }
u8 consp(struct expr expr) { return expr.type == E_CONS; }

void expr_print(struct expr expr) {
    switch ((enum expr_type) expr.type) {
        case E_NIL:
            printf("nil");
            break;
        case E_INTEGER:
            printf("%ld", expr.integer);
            break;
        case E_SYMBOL:
            printf("%.*s", (i32)expr.length, expr.symbol);
            break;
        case E_CONS:
            printf("(");
            expr_print(CAR(expr));
            printf(" . ");
            expr_print(CDR(expr));
            printf(")");
            break;
        case E_BOOLEAN:
            printf(expr.boolean ? "T" : "F");
            break;
    }
}

void expr_println(struct expr expr) {
    expr_print(expr);
    putchar('\n');
}

static u8 __expr_cons_length(struct expr expr, u8 acc) {
    if (!consp(CDR(expr))) return acc;

    return __expr_cons_length(CDR(expr), acc+1);
}

u8 expr_cons_length(struct expr expr) {
    return __expr_cons_length(expr, 1);
}
