#ifndef __COMPILER_H
#define __COMPILER_H

#include "module.h"
#include "expr.h"
#include "reader.h"
#include "generics.h"
#include "builtin.h"

/* @TODO: Implement compilation of symbols */
/* @TODO: Implement global variables */
/* @TODO: Implement let expressions */
/* @TODO: Implement user defined functions */

enum compiler_status {
    COMPILE_OK,
    COMPILE_UNKOWN_FUNCTION,
    COMPILE_UNKOWN_SYMBOL,
    COMPILE_EXPECTED_ARGS,
    COMPILE_EXPECTED_SYMBOL,
    COMPILE_MISSING_FUNCTION_ARGS,
    COMPILE_READER_ERROR,
};

struct compiler {
    struct module* module;
    struct reader reader;
    struct smap(builtin_function_info) builtins;
};

/* 
 * @TODO: Hide some of thse functions, they don't all need to be public for the
 *       users of the "library" to see.
 * */

void compiler_init(struct compiler* compiler, struct slice(char) src, struct module* module);

void compiler_destroy(struct compiler* compiler);

u8 compile(struct compiler* compiler);
u8 compile_expr(struct compiler* compiler, struct expr expr);
u8 compile_symbol(struct compiler* compiler, struct expr expr);
u8 compile_list(struct compiler* compiler, struct expr expr);
u8 compile_if(struct compiler* compiler, struct expr expr);
u8 compile_builtin_function(struct compiler* compiler, struct expr expr);
u8 compile_function(struct compiler* compiler, struct expr expr);
u8 compile_args(struct compiler* compiler, struct expr expr);

#endif  /*__COMPILER_H*/
