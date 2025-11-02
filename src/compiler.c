#include <stdio.h>

#include "compiler.h"

u8 compile(struct module* module, struct expr expr) {
    u8 ret;
    
    ret = compile_expr(module, expr);
    module_write_byte(module, OP_RETURN);

    return ret;
}

u8 compile_expr(struct module* module, struct expr expr) {
    switch ((enum expr_type)expr.type) {
        case E_INTEGER:
            module_write_byte(module, OP_CONSTANT);
            module_write_byte(module, module_write_const(module, expr));
            break;
        case E_CONS:
            return compile_list(module, expr);
        case E_SYMBOL:
            fprintf(stderr, "Compilation of symbols is not implemented yet\n");
            return 1;
        case E_NIL:
            break;
        case E_TYPE_COUNT:
        default:
            assert(0 && "How did we get here?");
    }

    return 0;
}

u8 compile_list(struct module* module, struct expr expr) {
    assert(symbolp(CAR(expr)) && "The first element of a list must be a symbol");

    return compile_function(module, expr);
}

u8 compile_function(struct module* module, struct expr expr) {
    /* 
     * We already know the first element of the list is a symbol.
     *
     * What we need to do now is compile the arguments and then emit the bytes
     * for the given function.
     * */

    struct expr car = CAR(expr);

    compile_args(module, CDR(expr));

    /* ~TODO: we need a better way to check against builtin functions */
    if (memcmp(car.symbol, "+", 1) == 0) {
        module_write_byte(module, OP_ADD);
    } else if (memcmp(car.symbol, "-", 1) == 0) {
        module_write_byte(module, OP_SUB);
    } else if (memcmp(car.symbol, "*", 1) == 0) {
        module_write_byte(module, OP_MUL);
    } else if (memcmp(car.symbol, "/", 1) == 0) {
        module_write_byte(module, OP_DIV);
    } else if (memcmp(car.symbol, "quit", 4) == 0) {
        module_write_byte(module, OP_HALT);
    } else if (memcmp(car.symbol, "display", 7) == 0) {
        module_write_byte(module, OP_DISPLAY);
    }else {
        fprintf(stderr, "(%d:%d) error: unknown builtin function: %.*s\n", 
                car.loc.line, car.loc.column, car.length, car.symbol);
        return 2;
    }

    return 0;
}

u8 compile_args(struct module* module, struct expr expr) {
    if (!consp(expr)) return 0;

    compile_expr(module, CAR(expr));

    return compile_args(module, CDR(expr));
}
