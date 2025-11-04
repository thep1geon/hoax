#include <stdio.h>

#include "vm.h"
#include "module.h"

u8 vm_fetch_u8(struct vm* vm) {
    return *vm->ip;
}

struct expr vm_push(struct vm* vm, struct expr expr) {
    return vm->stack[vm->sp++] = expr;
}

struct expr vm_pop(struct vm* vm) {
    if (vm->sp == 0) return expr_create_nil();
    return vm->stack[--vm->sp];
}

struct expr vm_peek(struct vm* vm) {
    if (vm->sp == 0) return expr_create_nil();
    return vm->stack[vm->sp - 1];
}

struct expr vm_get_const(struct vm* vm, u8 const_index) {
    return vm->module->constants.at[const_index];
}

struct expr vm_run(struct vm* vm, struct module* module) {
    struct expr expr, a, b;
    u32 a_ptr, b_ptr;
    u8 inst;

    vm->module = module; 
    vm->ip = module->code.at;

    inst = vm_fetch_u8(vm);
    while (vm->running) {
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
                assert(integerp(a) && integerp(b) && "Both operands must be integers");
                vm_push(vm, expr_create_integer(a.integer + b.integer));
                break;
            case OP_SUB:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "Both operands must be integers");
                vm_push(vm, expr_create_integer(b.integer - a.integer));
                break;
            case OP_MUL:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "Both operands must be integers");
                vm_push(vm, expr_create_integer(a.integer * b.integer));
                break;
            case OP_DIV:
                UNIMPLEMENTED();
            case OP_TRUE:
                vm_push(vm, expr_create_boolean(true));
                break;
            case OP_FALSE:
                vm_push(vm, expr_create_boolean(false));
                break;
            case OP_NIL:
                vm_push(vm, expr_create_nil());
                break;
            case OP_CONS:
                a_ptr = expr_box(vm_pop(vm));
                b_ptr = expr_box(vm_pop(vm));
                vm_push(vm, expr_create_cons(b_ptr, a_ptr));
                break;
            case OP_CAR:
                expr = vm_peek(vm);
                assert(consp(expr));
                vm_push(vm, CAR(vm_peek(vm)));
                break;
            case OP_CDR:
                expr = vm_peek(vm);
                assert(consp(expr));
                vm_push(vm, CDR(vm_peek(vm)));
                break;
            case OP_DISPLAY:
                expr = vm_pop(vm);
                expr_println(expr);
                break;
            case OP_TOGGLE_DEBUG:
                vm->debug = !vm->debug;
                break;
            case OP_HALT:
                vm->running = false;
                expr = vm_pop(vm);
                return expr;
            case OP_RETURN:
                expr = vm_pop(vm);
                return expr;
        }

        inst = vm_fetch_u8(vm);
    }

    return expr_create_nil();
}

