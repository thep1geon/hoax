#include <stdio.h>

#include "builtin.h"
#include "reader.h"
#include "compiler.h"
#include "generics.h"

SMAP_IMPL_S(builtin_function_info);

void compiler_init(struct compiler* compiler, struct slice(char) src, struct module* module) {
    compiler->reader = reader_create(src);
    compiler->module = module;

    smap__builtin_function_info_put(&compiler->builtins, STRING("+"),
                                    (struct builtin_function_info){2, OP_ADD});

    smap__builtin_function_info_put(&compiler->builtins, STRING("-"),
                                    (struct builtin_function_info){2, OP_SUB});
    
    smap__builtin_function_info_put(&compiler->builtins, STRING("*"),
                                    (struct builtin_function_info){2, OP_MUL});

    smap__builtin_function_info_put(&compiler->builtins, STRING("/"),
                                    (struct builtin_function_info){2, OP_DIV});

    smap__builtin_function_info_put(&compiler->builtins, STRING("car"),
                                    (struct builtin_function_info){1, OP_CAR});

    smap__builtin_function_info_put(&compiler->builtins, STRING("cdr"),
                                    (struct builtin_function_info){1, OP_CDR});

    smap__builtin_function_info_put(&compiler->builtins, STRING("cons"),
                                    (struct builtin_function_info){1, OP_CONS});

    smap__builtin_function_info_put(&compiler->builtins, STRING("quit"),
                                    (struct builtin_function_info){0, OP_HALT});

    smap__builtin_function_info_put(&compiler->builtins, STRING("toggle-debug"),
                                    (struct builtin_function_info){0, OP_TOGGLE_DEBUG});
}

void compiler_destroy(struct compiler* compiler) {
    SMAP_DESTROY(&compiler->builtins);
}

static inline u8 emit_byte(struct compiler* compiler, u8 byte) {
    module_write_byte(compiler->module, byte);
    return compiler->module->code.length;
}

static inline u8 emit_constant(struct compiler* compiler, struct expr expr) {
    emit_byte(compiler, OP_CONSTANT);
    return emit_byte(compiler, module_write_const(compiler->module, expr));
}

static inline u8 emit_jmp(struct compiler* compiler, u8 jmp) {
    emit_byte(compiler, jmp);
    emit_byte(compiler, 0x00);
    return emit_byte(compiler, 0x00);
}

static inline void patch_jmp(struct compiler* compiler, u32 jmp_save) {
    u16 jmp_offset;

    jmp_offset = compiler->module->code.length - jmp_save;
    compiler->module->code.at[jmp_save-2] = (u8)(((u16)jmp_offset >> 8) & 0xFF);
    compiler->module->code.at[jmp_save-1] = ((u8) jmp_offset) & 0xFF;
}

u8 compile(struct compiler* compiler) {
    u8 ret;
    u32 ptr;

    ret = COMPILE_OK;

    while ((ptr = read_expr(&compiler->reader)) != 0) {

        /* If we failed to read an expression we can propagate that up */
        if (ptr == READER_ERROR) {
            return COMPILE_READER_ERROR;
        }

        ret = compile_expr(compiler, EXPR(ptr));

        if (ret != COMPILE_OK) break;
    }

    emit_byte(compiler, OP_RETURN);

    return ret;
}

u8 compile_expr(struct compiler* compiler, struct expr expr) {
    switch ((enum expr_type)expr.type) {
        case EXPR_INTEGER:
            emit_constant(compiler, expr);
            break;
        case EXPR_CONS:
            return compile_list(compiler, expr);
        case EXPR_SYMBOL:
            return compile_symbol(compiler, expr);
        case EXPR_NIL:
        case EXPR_BOOLEAN:
        case EXPR_NATIVE:
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
        emit_byte(compiler, OP_TRUE);
    } else if (memcmp(expr.symbol, "f", 1) == 0) {
        emit_byte(compiler, OP_FALSE);
    } else if (memcmp(expr.symbol, "nil", 3) == 0) {
        emit_byte(compiler, OP_NIL);
    } else {
        emit_constant(compiler, expr);
        emit_byte(compiler, OP_LOAD_VAR);
    }

    return COMPILE_OK;
}

u8 compile_list(struct compiler* compiler, struct expr expr) {
    if (!symbolp(CAR(expr))) {
        fprintf(stderr, "(%d:%d) error: the first element of a list must be a symbol:\n\t'",
                expr.loc.line, expr.loc.column);
        expr_fprint(stderr, expr);
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
    u8 ret;

    if (expr.length != 4) {
        fprintf(stderr, "(%d:%d) error: if expressions must have 4 parts:\n\t'",
                expr.loc.line, expr.loc.column);
        expr_fprint(stderr, expr);
        fprintf(stderr, "'\n");
        return COMPILE_EXPECTED_ARGS;
    }

    condition = CAR(CDR(expr));
    then_branch = CAR(CDR(CDR(expr)));
    else_branch = CAR(CDR(CDR(CDR(expr))));

    ret = compile_expr(compiler, condition);
    if (ret != COMPILE_OK) return ret;

    jmf_save = emit_jmp(compiler, OP_JMF);

    ret = compile_expr(compiler, then_branch);
    if (ret != COMPILE_OK) return ret;

    jmp_save = emit_jmp(compiler, OP_JMP);

    patch_jmp(compiler, jmf_save);

    ret = compile_expr(compiler, else_branch);
    if (ret != COMPILE_OK) return ret;

    patch_jmp(compiler, jmp_save);

    return COMPILE_OK;
}

u8 compile_function(struct compiler* compiler, struct expr expr) {
    u8 ret;

    if ((ret = compile_builtin_function(compiler, expr)) != COMPILE_UNKOWN_FUNCTION) {
        return ret;
    }

    ret = compile_args(compiler, CDR(expr));
    if (ret != COMPILE_OK) return ret;
    emit_constant(compiler, CAR(expr));
    emit_byte(compiler, OP_CALL);

    return COMPILE_OK;
}

u8 compile_builtin_function(struct compiler* compiler, struct expr expr) {
    u8 ret;

    struct option(builtin_function_info) fn = {0};

    struct expr car = CAR(expr);
    struct expr args = CDR(expr);

    struct slice(char) fn_name = {car.symbol, car.length};

    fn = smap__builtin_function_info_get(&compiler->builtins, fn_name);

    if (!fn.is_some) {
        return COMPILE_UNKOWN_FUNCTION;
    }

    /* do a compile time check of the number of arguments required by that function */
    if (args.length != fn.item.arity) {
        fprintf(stderr, "(%d:%d) error: '%.*s' takes %d arguments but only %d were provided\n", 
                car.loc.line, car.loc.column, car.length, car.symbol, fn.item.arity, args.length);
        return COMPILE_MISSING_FUNCTION_ARGS;
    }

    ret = compile_args(compiler, args);
    if (ret != COMPILE_OK) return ret;

    emit_byte(compiler, fn.item.op_code);

    return COMPILE_OK;
}

u8 compile_args(struct compiler* compiler, struct expr expr) {
    u8 ret;

    if (!consp(expr)) return COMPILE_OK;

    ret = compile_expr(compiler, CAR(expr));
    if (ret != COMPILE_OK) return ret;

    return compile_args(compiler, CDR(expr));
}
