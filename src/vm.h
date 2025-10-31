#ifndef __VM_H
#define __VM_H

#define STACK_MAX 128

#include "common.h"
#include "expr.h"
#include "module.h"

struct vm {
    struct expr* stack;
    struct module* module;
    u32 sp;
    u32 ip;
};

struct vm vm_create();

void vm_destroy(struct vm* vm);

void vm_load_module(struct vm* vm, struct module* module);

u8 vm_fetch_u8(struct vm* vm);

struct expr vm_push(struct vm* vm, struct expr expr);

struct expr vm_pop(struct vm* vm);

struct expr vm_get_const(struct vm* vm, u8 const_index);

struct expr vm_run(struct vm* vm);

#endif  /* __VM_H */
