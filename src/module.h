#ifndef __MODULE_H
#define __MODULE_H

#include "common.h"
#include "expr.h"

DYNARRAY_DECL(u8);

enum op_code {
    /* maths stuff */
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    /* control flow */
    OP_JMP, /* unconditional jump */
    OP_JMF, /* jump if the top of the stack is falsy */

    /* cons / heap stuff */
    OP_CONS,
    OP_CAR,
    OP_CDR,
    
    /* constant values */
    OP_TRUE,
    OP_FALSE,
    OP_NIL,

    /* loading a constant value */
    OP_CONSTANT,

    /* Stopping the vm in some way */
    OP_RETURN,
    OP_HALT,

    /* virtual machine builtins */
    OP_DISPLAY,
    OP_TOGGLE_DEBUG,
};

struct module {
    struct dynarray(u8) code;
    struct dynarray(expr) constants;
};

void module_destroy(struct module* module);

void module_write_byte(struct module* module, u8 byte);
u8 module_write_const(struct module* module, struct expr expr);

void module_disassemble(struct module* module);

#endif  /* __MODULE_H */
