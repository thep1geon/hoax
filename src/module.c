#include <stdio.h>
#include <stdio.h>

#include "common.h"
#include "module.h"

DYNARRAY_IMPL(u8);

void module_destroy(struct module* module) {
    DYNARRAY_FREE(&module->code);
    DYNARRAY_FREE(&module->constants);
}

void module_write_byte(struct module* module, u8 byte) {
    dynarray__u8_push(&module->code, byte);
}

u8 module_write_const(struct module* module, struct expr expr) {
    dynarray__expr_push(&module->constants, expr);

    return (u8) (module->constants.length - 1);
}

void module_disassemble(struct module* module) {
    u8 const_index;
    u32 offset = 0;

    while (offset < module->code.length) {
        switch ((enum op_code)module->code.at[offset]) {
            case OP_RETURN:
                puts("OP_RETURN");
                break;
            case OP_HALT:
                puts("OP_HALT");
                break;
            case OP_ADD:
                puts("OP_ADD");
                break;
            case OP_SUB:
                puts("OP_SUB");
                break;
            case OP_MUL:
                puts("OP_MUL");
                break;
            case OP_DIV:
                puts("OP_DIV");
                break;
            case OP_DISPLAY:
                puts("OP_DISPLAY");
                break;
            case OP_CONS:
                puts("OP_CONS");
                break;
            case OP_CAR:
                puts("OP_CAR");
                break;
            case OP_CDR:
                puts("OP_CDR");
                break;
            case OP_CONSTANT:
                offset += 1;
                const_index = module->code.at[offset];
                printf("OP_CONSTANT %d (", const_index);
                expr_print(module->constants.at[const_index]);
                printf(")\n");
                break;
        }

        offset += 1;
    }
}
