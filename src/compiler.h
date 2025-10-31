#ifndef __COMPILER_H
#define __COMPILER_H

#include "module.h"
#include "expr.h"

/* 
 * ~TODO: Make some sort of state to keep track of compilation.
 *
 * ~TODO: Create a better interface for compiling expressions into the current
 *       module and then have specified compiler_end and compiler_begin functions.
 *
 * ~TODO: Hide some of thse functions, they don't all need to be public for the
 *       users of the "library" to see.
 * */

u8 compile(struct module* module, struct expr expr);
u8 compile_expr(struct module* module, struct expr expr);
u8 compile_list(struct module* module, struct expr expr);
u8 compile_function(struct module* module, struct expr expr);
u8 compile_args(struct module* module, struct expr expr);

#endif  /*__COMPILER_H*/
