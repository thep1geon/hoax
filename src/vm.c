#include <stdio.h>

#include "vm.h"
#include "module.h"
void vm_init(struct vm* vm) {
    vm_set_global(vm, STRING("#display"), expr_create_native(native_display, 1));
    vm_set_global(vm, STRING("#hello"), expr_create_native(native_hello, 0));
    vm_set_global(vm, STRING("#+"), expr_create_native(native_add, 2));

    vm->running = true;
}

void vm_destroy(struct vm* vm) {
    SMAP_DESTROY(&vm->global_map);
}

u8 vm_fetch_u8(struct vm* vm) {
    vm->ip += 1;
    return *(vm->ip - 1);
}

u16 vm_fetch_u16(struct vm* vm) {
    vm->ip += 2;
    return ((u16)(*(vm->ip - 2)) << 8) | *(vm->ip - 1);
}

struct expr vm_get_const(struct vm* vm, u8 const_index) {
    return vm->module->constants.at[const_index];
}

void vm_dump_globals(struct vm* vm) {
    u64 i = 0;
    struct smap_slot(expr) slot;

    fprintf(stderr, "Virtual Machine Variables:\n");
    for (i = 0; i < vm->global_map.size; ++i) {
        slot = vm->global_map.slots[i];

        if (!slot.occupied) continue;

        fprintf(stderr, "Key: %.*s, Value: ", STRINGF(slot.key));
        expr_fprintln(stderr, slot.value);
    }
}

// struct option(expr) -> struct option__expr

struct option(expr) __vm_get_global(struct vm* vm, struct slice(char) name) {
    return smap__expr_get(&vm->global_map, name) ;
}

struct expr vm_get_global(struct vm* vm, struct slice(char) name) {
    struct option(expr) expr;
    if ((expr = __vm_get_global(vm, name)).is_some) {
        return expr.item;
    } else {
        return expr_create_nil();
    }
}

struct expr vm_set_global(struct vm* vm, struct slice(char) name, struct expr expr) {
    struct option(expr) expr_opt;
    if ((expr_opt = smap__expr_put(&vm->global_map, name, expr)).is_some) {
        return expr_opt.item;
    } else {
        return expr_create_nil();
    }
}

struct expr vm_load_var(struct vm* vm, struct slice(char) name) {
    struct expr expr = expr_create_nil();
    struct option(expr) opt_expr;

    if ((opt_expr = __vm_get_global(vm, name)).is_some) {
        expr = opt_expr.item;
    } else {
        fprintf(stderr, "%.*s is not defined\n", STRINGF(name));
    }

    return expr;
}

struct expr vm_store_var(struct vm* vm, struct slice(char) name) {
    struct expr expr = vm_pop(vm);

    vm_set_global(vm, name, expr);

    return expr;
}

/* 
 * @TODO: Figure out if there is a better way of passing arguments on the stack.
 *
 * Right now we are passing them in reverse order.  This is a problem if a function
 * expects multiple arguments and more were provided than needed.  With the way
 * it works now, the last arguments are on the ones passed into the function.
 * */
struct expr vm_function_call(struct vm* vm, struct slice(char) name) {
    u8 arity;
    struct expr arg, func;
    u32 args;


    func = vm_load_var(vm, name);

    /* the function was not found */
    if (nilp(func)) {
        return expr_create_nil();
    }

    assert(nativep(func));

    arity = func.arity;

    args = 0;

    while (arity) {
        arg = vm_pop(vm);
        assert(!nilp(arg));
        args = expr_cons_append(args, arg);
        arity--;
    }

    args = expr_cons_reverse(args);

    return expr_native_call(func, EXPR(args));
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

struct expr vm_run(struct vm* vm, struct module* module) {
    struct expr expr, a, b;
    u32 a_ptr, b_ptr;
    u8 inst;
    u16 jump_offset;

    vm->module = module; 
    vm->ip = module->code.at;

    inst = vm_fetch_u8(vm);
    while (vm->running) {
        switch ((enum op_code)inst) {
            case OP_CONSTANT:
                expr = vm_get_const(vm, vm_fetch_u8(vm));
                vm_push(vm, expr);
                break;
            case OP_LOAD_VAR:
                expr = vm_pop(vm);
                assert(symbolp(expr));
                vm_push(vm, vm_load_var(
                    vm,
                    (struct slice(char)){.ptr = expr.symbol, .length = expr.length}
                ));
                break;
            case OP_STORE_VAR:
                expr = vm_pop(vm);
                assert(symbolp(expr));
                vm_push(vm, vm_store_var(
                    vm,
                    (struct slice(char)){.ptr = expr.symbol, .length = expr.length}
                ));
                break;
            case OP_ADD:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "Both operands must be integers for OP_ADD");
                vm_push(vm, expr_create_integer(a.integer + b.integer));
                break;
            case OP_SUB:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "Both operands must be integers for OP_SUB");
                vm_push(vm, expr_create_integer(b.integer - a.integer));
                break;
            case OP_MUL:
                a = vm_pop(vm);
                b = vm_pop(vm);
                assert(integerp(a) && integerp(b) && "Both operands must be integers for OP_MUL");
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
            case OP_JMP:
                vm->ip += vm_fetch_u16(vm);
                break;
            case OP_JMF:
                jump_offset = vm_fetch_u16(vm);
                expr = vm_pop(vm);
                if (!expr_is_truthy(expr)) {
                    vm->ip += jump_offset;
                }
                break;
            case OP_CALL:
                /* the name of the function */
                expr = vm_pop(vm);
                assert(symbolp(expr));
                vm_push(vm, vm_function_call(
                    vm,
                    (struct slice(char)){.ptr = expr.symbol, .length = expr.length}
                ));
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

