#ifndef __MODULE_H
#define __MODULE_H

#include "common.h"
#include "expr.h"

DYNARRAY_DECL(u8);

enum op_code {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_CONS,
    OP_CAR,
    OP_CDR,

    OP_CONSTANT,
    OP_RETURN,
    OP_HALT,

    OP_DISPLAY,
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
