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
#include "builtin.h"
#include "map.h"

#define INPUT_BUFFER_CAP KILOBYTES(1)

SLICE_DECL_S(slice__char);

/* @TODO: Implement readline functionality into the repl for a better experience */
void repl() {
    struct vm vm;
    struct module module;
    struct slice(char) input;
    struct expr expr;
    struct compiler compiler;
    char input_buffer[INPUT_BUFFER_CAP];

    expr_new_nil();

    vm = (struct vm){0};
    vm_init(&vm);
    module = (struct module){0};

    compiler = (struct compiler){0};

    printf("(hoax)>> ");
    while (vm.running && fgets(input_buffer, INPUT_BUFFER_CAP, stdin)) {
        input = STRING(input_buffer);
        if (input.length <= 1) {
            printf("(hoax)>> ");
            continue;
        }

        DYNARRAY_CLEAR(&module.code);
        DYNARRAY_CLEAR(&module.constants);

        compiler_init(&compiler, input, &module);

        if (compile(&compiler) == COMPILE_OK) {
            if (vm.debug)
                module_disassemble(compiler.module);

            expr = vm_run(&vm, compiler.module);

            if (!nilp(expr)) expr_println(stdout, expr);
        }

        if (vm.running)
            printf("(hoax)>> ");

    }

    if (vm.running)
        putchar('\n');

    if (compiler.module)
        module_destroy(compiler.module);
    vm_destroy(&vm);
    DYNARRAY_FREE(&exprs);
}

void file(char* filename) {
    struct vm vm;
    struct module module;
    struct slice(char) src;
    struct compiler compiler;
    char* file_contents;
    FILE* fp;
    usize file_size;

    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "[file] error: failed to open file %s\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    file_contents = malloc(file_size);
    assert(file_contents && "OOM or something");
    
    assert(file_size == fread(file_contents, 1, file_size, fp));

    src = (struct slice(char)){.ptr = file_contents, .length = file_size};

    expr_new_nil();

    vm = (struct vm){0};
    vm_init(&vm);
    module = (struct module){0};

    compiler = (struct compiler){0};

    compiler_init(&compiler, src, &module);

    if (compile(&compiler) == COMPILE_OK)
        vm_run(&vm, compiler.module);

    module_destroy(compiler.module);
    DYNARRAY_FREE(&exprs);
    vm_destroy(&vm);
    free(file_contents);
}

i32 main(i32 argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    /* Fire up the repl */
    if (argc == 1) {
        repl();
        return 0;
    }

    file(argv[1]);

    return 0;
}
