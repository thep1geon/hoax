#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "expr.h"
#include "reader.h"

i32 main(i32 argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct expr expr;
    struct slice(char) src = STRING("(42)\n(6 7)\n(= 42 (* 6 7))\n3\n(cons 6 7)");
    struct expr_reader reader = reader_create(src);
    u32 ptr = 0;

    printf("-=-=-=-=-=-=-Source=-=-=-=-=-=-=\n%.*s\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n", (i32)reader.src.length, src.ptr);

    expr_new_nil();

    while ((ptr = read_expr(&reader)) != 0) {
        expr = EXPR(ptr);
        printf("(location %d %d) ", expr.loc.line, expr.loc.column);
        expr_print(expr);
        putchar('\n');
    }

    DYNARRAY_FREE(&exprs);

    return 0;
}
