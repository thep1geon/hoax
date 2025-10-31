#ifndef __COMPILER_H
#define __COMPILER_H

#include "module.h"
#include "expr.h"

void compile(struct module* module, struct expr expr);
void compile_expr(struct module* module, struct expr expr);
void compile_list(struct module* module, struct expr expr);
void compile_function(struct module* module, struct expr expr);

#endif  /*__COMPILER_H*/
