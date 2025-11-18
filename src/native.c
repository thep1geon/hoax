#include "common.h"
#include "native.h"
#include "expr.h"

void expr_println(FILE*, struct expr);

struct expr native_display(struct expr args) {
    expr_println(stdout, args);
    return expr_create_nil();
}
