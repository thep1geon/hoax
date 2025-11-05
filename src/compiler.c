#include <stdio.h>

#include "reader.h"
#include "compiler.h"
#include "generics.h"

void compiler_init(struct compiler* compiler, struct slice(char) src, struct module* module) {
    compiler->reader = reader_create(src);
    compiler->module = module;
}

u8 compile(struct compiler* compiler) {
    u8 ret;
    u32 ptr;

    // ptr = read_expr(&compiler->reader);
    while ((ptr = read_expr(&compiler->reader)) != 0) {
        ret = compile_expr(compiler, EXPR(ptr));

        if (ret != COMPILE_OK) break;
    }

    module_write_byte(compiler->module, OP_RETURN);

    return ret;
}

u8 compile_expr(struct compiler* compiler, struct expr expr) {
    switch ((enum expr_type)expr.type) {
        case E_INTEGER:
            module_write_byte(compiler->module, OP_CONSTANT);
            module_write_byte(compiler->module, module_write_const(compiler->module, expr));
            break;
        case E_CONS:
            return compile_list(compiler, expr);
        case E_SYMBOL:
            return compile_symbol(compiler, expr);
        case E_NIL:
        case E_BOOLEAN:
            break;
    }

    return COMPILE_OK;
}

u8 compile_symbol(struct compiler* compiler, struct expr expr) {
    if (memcmp(expr.symbol, "t", 1) == 0) {
        module_write_byte(compiler->module, OP_TRUE);
    } else if (memcmp(expr.symbol, "f", 1) == 0) {
        module_write_byte(compiler->module, OP_FALSE);
    } else if (memcmp(expr.symbol, "nil", 3) == 0) {
        module_write_byte(compiler->module, OP_NIL);
    } else {
        fprintf(stderr, "(%d:%d) error: unknown builtin symbol: %.*s\n", 
                expr.loc.line, expr.loc.column, expr.length, expr.symbol);
        return COMPILE_UNKOWN_SYMBOL;
    }

    return COMPILE_OK;
}

u8 compile_list(struct compiler* compiler, struct expr expr) {
    assert(symbolp(CAR(expr)) && "The first element of a list must be a symbol");

    return compile_function(compiler, expr);
}

u8 compile_function(struct compiler* compiler, struct expr expr) {
    /* 
     * We already know the first element of the list is a symbol.
     *
     * What we need to do now is compile the arguments and then emit the bytes
     * for the given function.
     * */
    u8 ret;

    struct expr car = CAR(expr);

    ret = compile_args(compiler, CDR(expr));
    if (ret != COMPILE_OK) return ret;

    /*
     * ~TODO: we need a better way to check against builtin functions
     *
     * ~TODO: We can actually do type checking and arity checking at compile time
     *        for all functions.  Once a function has been registered into the
     *        current environment, we can know how many arguments it takes and
     *        the types of each (if the type was provided).
     *  */
    if (memcmp(car.symbol, "+", 1) == 0) {
        module_write_byte(compiler->module, OP_ADD);
    } else if (memcmp(car.symbol, "-", 1) == 0) {
        module_write_byte(compiler->module, OP_SUB);
    } else if (memcmp(car.symbol, "*", 1) == 0) {
        module_write_byte(compiler->module, OP_MUL);
    } else if (memcmp(car.symbol, "/", 1) == 0) {
        module_write_byte(compiler->module, OP_DIV);
    } else if (memcmp(car.symbol, "car", 3) == 0) {
        module_write_byte(compiler->module, OP_CAR);
    } else if (memcmp(car.symbol, "cdr", 3) == 0) {
        module_write_byte(compiler->module, OP_CDR);
    } else if (memcmp(car.symbol, "cons", 4) == 0) {
        module_write_byte(compiler->module, OP_CONS);
    } else if (memcmp(car.symbol, "quit", 4) == 0) {
        module_write_byte(compiler->module, OP_HALT);
    } else if (memcmp(car.symbol, "display", 7) == 0) {
        module_write_byte(compiler->module, OP_DISPLAY);
    } else if (memcmp(car.symbol, "toggle-debug", 12) == 0) {
        module_write_byte(compiler->module, OP_TOGGLE_DEBUG);
    }else {
        fprintf(stderr, "(%d:%d) error: unknown builtin function: %.*s\n", 
                car.loc.line, car.loc.column, car.length, car.symbol);
        return COMPILE_UNKOWN_FUNCTION;
    }

    return COMPILE_OK;
}

u8 compile_args(struct compiler* compiler, struct expr expr) {
    u8 ret;

    if (!consp(expr)) return COMPILE_OK;

    ret = compile_expr(compiler, CAR(expr));
    if (ret != COMPILE_OK) return ret;

    return compile_args(compiler, CDR(expr));
}
