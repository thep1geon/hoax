#include <stdio.h>

#include "builtin.h"
#include "reader.h"
#include "compiler.h"
#include "generics.h"

/* @TODO: See if I can abstract away the direct calls to module_write_byte */
/* @TODO: Come up with an interface to make patching jumps simpler */

static struct builtin_function functions[] = {
    { { "+", 1 },             2, OP_ADD },
    { { "-", 1 },             2, OP_SUB },
    { { "*", 1 },             2, OP_MUL },
    { { "/", 1 },             2, OP_DIV },
    { { "car", 3 },           1, OP_CAR },
    { { "cdr", 3 },           1, OP_CDR },
    { { "cons", 4 },          2, OP_CONS },
    { { "quit", 4 },          0, OP_HALT },
    { { "display", 7 },       1, OP_DISPLAY },
    { { "toggle-debug", 12 }, 0, OP_TOGGLE_DEBUG },
};

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
        case E_NATIVE:
            break;
    }

    return COMPILE_OK;
}

u8 compile_symbol(struct compiler* compiler, struct expr expr) {
    /* 
     * We probably won't have to do the whole table thing for symbols because
     * I don't foresee having to lookup that many more symbols
     * */
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

    /* 
     * But I can foresee the need to do a table for special forms like 'if',
     * 'let', 'defun', 'define', etc.
     *
     * @TODO: Create another table of special forms and their respective
     *        compilation function.
     * */
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
    u8 ret;

    if ((ret = compile_builtin_function(compiler, expr)) != COMPILE_UNKOWN_FUNCTION) {
        return ret;
    }

    module_write_byte(compiler->module, OP_CONSTANT);
    module_write_byte(compiler->module, module_write_const(compiler->module, CDR(expr)));
    module_write_byte(compiler->module, OP_CONSTANT);
    module_write_byte(compiler->module, module_write_const(compiler->module, CAR(expr)));
    module_write_byte(compiler->module, OP_CALL);

    return COMPILE_OK;
}

u8 compile_builtin_function(struct compiler* compiler, struct expr expr) {
    u8 ret;
    u32 i;

    struct builtin_function* fn = 0;

    struct expr car = CAR(expr);
    struct expr args = CDR(expr);

    /* Search for the function in the table of functions */
    for (i = 0; i < ARRAY_LENGTH(functions); ++i) {
        fn = functions + i;

        if (fn->name.length == car.length) {
            if (memcmp(fn->name.ptr, car.symbol, car.length) == 0) {
                break;
            }
        }

        fn = 0;
    }

    /* check to make sure we got a function */
    if (fn == 0) {
        return COMPILE_UNKOWN_FUNCTION;
    }

    /* do a compile time check of the number of arguments required by that function */
    if (args.length != fn->arity) {
        fprintf(stderr, "(%d:%d) error: '%.*s' takes %d arguments but only %d were provided\n", 
                car.loc.line, car.loc.column, car.length, car.symbol, fn->arity, args.length);
        return COMPILE_MISSING_FUNCTION_ARGS;
    }

    ret = compile_args(compiler, args);
    if (ret != COMPILE_OK) return ret;

    module_write_byte(compiler->module, fn->op_code);

    return COMPILE_OK;
}

u8 compile_args(struct compiler* compiler, struct expr expr) {
    u8 ret;

    if (!consp(expr)) return COMPILE_OK;

    ret = compile_expr(compiler, CAR(expr));
    if (ret != COMPILE_OK) return ret;

    return compile_args(compiler, CDR(expr));
}
