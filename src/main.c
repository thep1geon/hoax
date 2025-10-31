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

#define INPUT_BUFFER_CAP 128

void repl() {
    struct vm vm;
    struct module module;
    struct expr_reader reader;
    struct slice(char) input;
    struct expr expr;
    u32 ptr;
    char input_buffer[INPUT_BUFFER_CAP];

    expr_new_nil();

    vm = vm_create();
    module = (struct module){0};

    printf("(hoax)>> ");
    while (fgets(input_buffer, INPUT_BUFFER_CAP, stdin)) {
        input = STRING(input_buffer);
        if (input.length <= 1) {
            printf("(hoax)>> ");
            continue;
        }

        reader = reader_create(input);
        ptr = read_expr(&reader);

        module.code.length = 0;
        module.constants.length = 0;

        if (compile(&module, EXPR(ptr))) {
            printf("(hoax)>> ");
            continue;
        }

        vm_load_module(&vm, &module);

        expr = vm_run(&vm);

        if (!nilp(expr)) expr_println(expr);

        printf("(hoax)>> ");
    }

    putchar('\n');

    module_destroy(&module);

    vm_destroy(&vm);

    DYNARRAY_FREE(&exprs);
}

i32 main(i32 argc, char** argv) {
    
    /* Fire up the repl */
    if (argc == 1) {
        repl();
        return 0;
    }

    file(argc-1, argv+1);

    return 0;
}
