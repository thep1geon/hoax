#ifndef __VM_H
#define __VM_H

#define STACK_MAX 128

#include "common.h"
#include "expr.h"
#include "module.h"

struct vm {
    struct expr stack[STACK_MAX];
    struct module* module;
    u8* ip;
    u32 sp;
    u8 running;
};

u8 vm_fetch_u8(struct vm* vm);

struct expr vm_push(struct vm* vm, struct expr expr);

struct expr vm_pop(struct vm* vm);

struct expr vm_get_const(struct vm* vm, u8 const_index);

struct expr vm_run(struct vm* vm, struct module* module);

#endif  /* __VM_H */
