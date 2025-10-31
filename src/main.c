#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "expr.h"
#include "reader.h"
#include "module.h"
#include "vm.h"
#include "compiler.h"

i32 main(i32 argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    expr_new_nil();

    struct vm vm = vm_create();
    struct module module = {0};

    struct slice(char) src = STRING("(display)\n(display (display))\n(display (+ (+ 3 2) 7))\n(display 42)");
    struct expr_reader reader = reader_create(src);
    u32 ptr = 0;

    printf("-=-=-=-=-=-=-Source=-=-=-=-=-=-=\n%.*s\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n", (i32)reader.src.length, src.ptr);

    while ((ptr = read_expr(&reader)) != 0) {
        module.code.length = 0;
        module.constants.length = 0;

        compile(&module, EXPR(ptr));

        vm_load_module(&vm, &module);
        vm_run(&vm);
    }

    vm_destroy(&vm);
    module_destroy(&module);


    DYNARRAY_FREE(&exprs);

    return 0;
}
