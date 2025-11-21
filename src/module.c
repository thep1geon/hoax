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

static inline u16 __module_get_u16(struct module* module, u32 offset) {
    return ((u16)module->code.at[offset] << 8) | module->code.at[offset + 1];
}

void module_disassemble(struct module* module) {
    u8 const_index;
    u32 offset = 0;

    fprintf(stderr, "Module Bytecode:\n");
    while (offset < module->code.length) {
        printf("%04X\t", offset);
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
            case OP_JMP:
                offset += 1;
                printf("OP_JMP %d\n", __module_get_u16(module, offset));
                offset += 1;
                break;
            case OP_JMF:
                offset += 1;
                printf("OP_JMF %d\n", __module_get_u16(module, offset));
                offset += 1;
                break;
            case OP_CALL:
                puts("OP_CALL");
                break;
            case OP_TRUE:
                puts("OP_TRUE");
                break;
            case OP_FALSE:
                puts("OP_FALSE");
                break;
            case OP_NIL:
                puts("OP_NIL");
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
            case OP_TOGGLE_DEBUG:
                puts("OP_TOGGLE_DEBUG");
                break;
            case OP_CONSTANT:
                offset += 1;
                const_index = module->code.at[offset];
                printf("OP_CONSTANT %d (", const_index);
                expr_print(stdout, module->constants.at[const_index]);
                printf(")\n");
                break;
        }

        offset += 1;
    }
}
