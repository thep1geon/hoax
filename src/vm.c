#include <stdio.h>

#include "vm.h"
#include "module.h"

struct vm vm_create() {
    struct expr* stack = malloc(sizeof(struct expr) * STACK_MAX);

    return (struct vm){
        .stack = stack,
        .module = 0,
        .sp = 0,
        .ip = 0,
    };
}

void vm_destroy(struct vm* vm) {
    free(vm->stack);
}

void vm_load_module(struct vm* vm, struct module* module) {
    vm->module = module; 
    vm->ip = 0;
    vm->sp = 0;
}

u8 vm_fetch_u8(struct vm* vm) {
    return vm->module->code.at[vm->ip];
}

struct expr vm_push(struct vm* vm, struct expr expr) {
    return vm->stack[vm->sp++] = expr;
}

struct expr vm_pop(struct vm* vm) {
    if (vm->sp == 0) return expr_create_nil();
    return vm->stack[--vm->sp];
}

struct expr vm_get_const(struct vm* vm, u8 const_index) {
    return vm->module->constants.at[const_index];
}

struct expr vm_run(struct vm* vm) {
    struct expr a, b;
    struct expr expr;
    u8 inst;

    if (!vm->module) {
        fprintf(stderr, "[vm_run] Error: No module loaded. Aborting.\n");
        return expr_create_nil();
    }

    inst = vm_fetch_u8(vm);
    while (vm->ip < vm->module->code.length) {
        vm->ip += 1;

        switch ((enum op_code)inst) {
            case OP_CONSTANT:
                expr = vm_get_const(vm, vm_fetch_u8(vm));
                vm->ip += 1;
                vm_push(vm, expr);
                break;
            case OP_ADD:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "a and b must both be integers");
                vm_push(vm, expr_create_integer(a.integer + b.integer));
                break;
            case OP_DISPLAY:
                expr = vm_pop(vm);
                expr_println(expr);
                break;
            case OP_RETURN:
                expr = vm_pop(vm);
                return expr;
        }

        inst = vm_fetch_u8(vm);
    }

    return expr_create_nil();
}

