#ifndef __BUILTIN_H
#define __BUILTIN_H

struct builtin_symbol {
    char* strlit;
    u8 length; /* They must also adhere to the 256 character limit */
};

/*
 * The builtin functions/symbols 
 *
 * Literals:
 *      t => OP_TRUE
 *        - Pushes the value true onto the stack
 *      f => OP_FALSE
 *        - Pushes the value false onto the stack
 * Maths:
 *      + => OP_ADD
 *        - Takes two elements off the stack and adds them together if they are 
 *          both numbers
 *      - => OP_SUB
 *        - Takes two elements off the stack and subtracts them if they are both
 *          numbers
 *      * => OP_MUL
 *        - Takes two elements off the stack and multiplies them together if they
 *          are both numbers
 *      / => OP_DIV
 *        - Takes two elements off the stack and divides them if they are both
 *          numbers
 *
 * Cons Manipulation:
 *      cons => OP_CONS
 *        - Takes two elements off the top of the stack, pairs them together,
 *          and puts the result on the stack
 *      car => OP_CAR
 *        - If the element on the top of the stack is of type cons, put the CAR
 *          of cons cell on the stack without touching the cons cell internally
 *      cdr => OP_CDR
 *        - If the element on the top of the stack is of type cons, put the CDR
 *          of cons cell on the stack without touching the cons cell internally
 *
 * VM Related Functionality:
 *      display => OP_DISPLAY
 *        - Pops the element on the top of the stack and prints it to stdout
 *      quit => OP_RETURN
 *        - Quits the execution of the VM. More useful in the REPL
 * */

#endif  /*__BUILTIN_H*/
