#include <stdio.h>
#include "typedef.h"

#define UNUSED(v) (void)v

// We'll start small now and scale as we need
#define STACK_CAP (128)

struct hoax_vm_stack {
    isize pointer; /* Points to the next empty space in the stack */
    i32 stack[STACK_CAP]; /* Backing [static] array for the stack */
};

enum hoax_vm_stack_op {
    HOAX_STACK_PUSH,
    HOAX_STACK_POP,
    HOAX_STACK_ADD_IMM,
};

struct hoax_vm_inst {
    i32 op;  /* Operation */
    i32 operand; /* Data associated with the instruction */
};

#define HOAX_VM_PUSH(v) (struct hoax_vm_inst){.op = HOAX_STACK_PUSH, .operand = (v)}
#define HOAX_VM_POP() (struct hoax_vm_inst){.op = HOAX_STACK_POP}

#define HOAX_VM_ADD_IMM() (struct hoax_vm_inst){.op = HOAX_STACK_ADD_IMM}

i32 hoax_vm_run_inst(struct hoax_vm_stack* stack, struct hoax_vm_inst inst) {
    i32 ret = 0;
    switch (inst.op) {
        case HOAX_STACK_PUSH:
            /* Stack overflow */
            if (stack->pointer >= STACK_CAP - 1) return 0;
            stack->stack[stack->pointer++] = inst.operand;
            break;
        case HOAX_STACK_POP:
            /* Stack underflow */
            if (stack->pointer == 0) return 0;
            ret = stack->stack[--stack->pointer];
            break;
        case HOAX_STACK_ADD_IMM:
            /* There are not enough arguments on the stack */
            if (stack->pointer < 2) return 0;
            /* We can just do some "advanced techniques" to
             * avoid popping and pushing as they can add up overtime*/
            stack->pointer -= 1;
            ret = stack->stack[stack->pointer-1] + stack->stack[stack->pointer];
            stack->stack[stack->pointer-1] = ret;
            break;
    }

    return ret;
}

i32 main(i32 argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    i32 ret;
    struct hoax_vm_stack stack = {0};

    hoax_vm_run_inst(&stack, HOAX_VM_PUSH(3));
    hoax_vm_run_inst(&stack, HOAX_VM_PUSH(4));
    hoax_vm_run_inst(&stack, HOAX_VM_ADD_IMM());
    ret = hoax_vm_run_inst(&stack, HOAX_VM_POP());

    return ret;
}
