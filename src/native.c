#include "common.h"
#include "native.h"
#include "expr.h"

struct expr native_display(struct expr args) {
    expr_println(CAR(args));
    return expr_create_nil();
}

struct expr native_hello(struct expr args) {
    UNUSED(args);
    printf("Hello from the C language!\n");
    return expr_create_nil();
}

struct expr native_add(struct expr args) {
    struct expr a, b;

    assert(integerp(CAR(args)) && integerp(CAR(CDR(args))));

    a = CAR(args);
    b = CAR(CDR(args));

    return expr_create_integer(a.integer + b.integer);
}
