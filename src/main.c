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

void file(char* filename) {
    struct vm vm;
    struct module module;
    struct slice(char) src;
    struct expr_reader reader;
    char* file_contents;
    FILE* fp;
    usize file_size;
    u32 ptr;

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "[file] error: failed to open file %s\n", filename);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    file_contents = malloc(file_size);
    assert(file_contents && "OOM or something");
    
    assert(file_size == fread(file_contents, 1, file_size, fp));

    src = (struct slice(char)){.ptr = file_contents, .length = file_size};

    expr_new_nil();

    vm = vm_create();
    module = (struct module){0};

    reader = reader_create(src);
    while ((ptr = read_expr(&reader)) != 0) {
        compile_expr(&module, EXPR(ptr));
    }

    compile(&module, EXPR(0));

    vm_load_module(&vm, &module);

    vm_run(&vm);

    module_destroy(&module);
    vm_destroy(&vm);
    DYNARRAY_FREE(&exprs);
    free(file_contents);
}

i32 main(i32 argc, char** argv) {
    
    /* Fire up the repl */
    if (argc == 1) {
        repl();
        return 0;
    }

    file(argv[1]);

    return 0;
}
