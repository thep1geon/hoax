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

    ret = COMPILE_OK;

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
    if (!symbolp(CAR(expr))) {
        fprintf(stderr, "(%d:%d) error: the first element of a list must be a symbol:\n\t'",
                expr.loc.line, expr.loc.column);
        expr_print(stderr, expr);
        fprintf(stderr, "'\n");
        return COMPILE_EXPECTED_SYMBOL;
    }

    if (memcmp(CAR(expr).symbol, "if", 2) == 0)
        return compile_if(compiler, expr);

    return compile_function(compiler, expr);
}

u8 compile_if(struct compiler* compiler, struct expr expr) {
    struct expr condition;
    struct expr then_branch;
    struct expr else_branch;
    u32 jmf_save, jmp_save;
    u16 jmf_offset, jmp_offset;
    u8 ret;

    if (expr.length != 4) {
        fprintf(stderr, "(%d:%d) error: if expressions must have 4 parts:\n\t'",
                expr.loc.line, expr.loc.column);
        expr_print(stderr, expr);
        fprintf(stderr, "'\n");
        return COMPILE_EXPECTED_ARGS;
    }

    condition = CAR(CDR(expr));
    then_branch = CAR(CDR(CDR(expr)));
    else_branch = CAR(CDR(CDR(CDR(expr))));

    ret = compile_expr(compiler, condition);
    if (ret != COMPILE_OK) return ret;

    module_write_byte(compiler->module, OP_JMF);
    module_write_byte(compiler->module, 0x00);
    module_write_byte(compiler->module, 0x00);
    jmf_save = compiler->module->code.length;

    ret = compile_expr(compiler, then_branch);
    if (ret != COMPILE_OK) return ret;

    module_write_byte(compiler->module, OP_JMP);
    module_write_byte(compiler->module, 0x00);
    module_write_byte(compiler->module, 0x00);
    jmp_save = compiler->module->code.length;

    jmf_offset = compiler->module->code.length - jmf_save;
    compiler->module->code.at[jmf_save-2] = (u8)(((u16)jmf_offset >> 8) & 0xFF);
    compiler->module->code.at[jmf_save-1] = ((u8) jmf_offset) & 0xFF;

    ret = compile_expr(compiler, else_branch);
    if (ret != COMPILE_OK) return ret;

    jmp_offset = compiler->module->code.length - jmp_save;
    compiler->module->code.at[jmp_save-2] = (u8)(((u16)jmp_offset >> 8) & 0xFF);
    compiler->module->code.at[jmp_save-1] = ((u8) jmp_offset) & 0xFF;

    return COMPILE_OK;
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
