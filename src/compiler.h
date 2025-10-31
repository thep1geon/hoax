#ifndef __COMPILER_H
#define __COMPILER_H

#include "module.h"
#include "expr.h"

u8 compile(struct module* module, struct expr expr);
u8 compile_expr(struct module* module, struct expr expr);
u8 compile_list(struct module* module, struct expr expr);
u8 compile_function(struct module* module, struct expr expr);
u8 compile_args(struct module* module, struct expr expr);

#endif  /*__COMPILER_H*/
