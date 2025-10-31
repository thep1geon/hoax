#include <stdio.h>

#include "compiler.h"

void compile(struct module* module, struct expr expr) {
    compile_expr(module, expr);
    module_write_byte(module, OP_RETURN);
}

void compile_expr(struct module* module, struct expr expr) {
    switch ((enum expr_type)expr.type) {
        case E_INTEGER:
            module_write_byte(module, OP_CONSTANT);
            module_write_byte(module, module_write_const(module, expr));
            break;
        case E_CONS:
            compile_list(module, expr);
            break;
        case E_SYMBOL:
            assert(0 && "Compilation of symbols is not implemented yet");
        case E_NIL:
            return;
        case E_TYPE_COUNT:
        default:
            assert(0 && "How did we get here?");
    }
}

void compile_list(struct module* module, struct expr expr) {
    UNUSED(module);
    
    if (symbolp(CAR(expr))) compile_function(module, expr);

    else {
        compile_expr(module, CAR(expr));
        compile_expr(module, CDR(expr));
    }
}

void compile_function(struct module* module, struct expr expr) {
    /* 
     * We already know the first element of the list is a symbol.
     *
     * What we need to do now is compile the arguments and then 'call' the function.
     * */

    struct expr car = CAR(expr);

    compile_list(module, CDR(expr));

    if (memcmp(car.symbol, "+", 1) == 0) {
        module_write_byte(module, OP_ADD);
    } else if (memcmp(car.symbol, "display", 7) == 0){
        module_write_byte(module, OP_DISPLAY);
    }else {
        fprintf(stderr, "[compile_function] Error: Unknown builtin function: %.*s\n", car.length, car.symbol);
        exit(1);
    }
}

