#ifndef __COMPILER_H
#define __COMPILER_H

#include "module.h"
#include "expr.h"
#include "reader.h"

enum compiler_status {
    COMPILE_OK,
    COMPILE_UNKOWN_FUNCTION,
    COMPILE_UNKOWN_SYMBOL,
};

struct compiler {
    struct expr_reader reader;
    struct module* module;
};

/* 
 * ~TODO: Hide some of thse functions, they don't all need to be public for the
 *       users of the "library" to see.
 * */

void compiler_init(struct compiler* compiler, struct slice(char) src, struct module* module);

u8 compile(struct compiler* compiler);
u8 compile_expr(struct compiler* compiler, struct expr expr);
u8 compile_symbol(struct compiler* compiler, struct expr expr);
u8 compile_list(struct compiler* compiler, struct expr expr);
u8 compile_if(struct compiler* compiler, struct expr expr);
u8 compile_function(struct compiler* compiler, struct expr expr);
u8 compile_args(struct compiler* compiler, struct expr expr);

#endif  /*__COMPILER_H*/
