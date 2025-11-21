# Hoax

A LISP-like language inspired by [Ruse](https://github.com/thep1geon/ruse) which
is inspired by Scheme.

## A Little Tour

```
(hoax)>> (+ 3 (+ 2 2))
2
```
```
(hoax)>> (cons 1 (cons 2 (cons 3)))
(1 . (2 . (3 . nil)))
```
```
(hoax)>> (car (cons 1 (cons 2 (cons 3))))
1
```
```
(hoax)>> (cdr (cons 1 (cons 2 (cons 3))))
(2 . (3 . nil))
```
```
(hoax)>> (if t 3 9)
3
```
```
(hoax)>> (if () 3 9)
9
```

Development is still a work in progress, and as such, there are many features
still missing. As of right now these are some of the main features in the
current version:

- Integers
- 'cons'
- 'car' and 'cdr'
- 'display'
- Basic arthimetic (excluding /)
- 'if'
- Builtin symbols (t, f, nil)
- Comments
- Builtin functions from C (#display, #hello)

## The Motivation

I want to improve on the ideas of Ruse but make it better in almost every way.
Even the name, Hoax, is a synonym of Ruse which is a synonym for Scheme.

I want to keep the simplicity of Ruse but make the language more usuable. As a
proof of concept, I was able to implement a small subset of Ruse in Ruse, but
it was not very capable.

One of the biggest faults of Ruse was that it was just a simple tree-walk
interpreter which means the speed of execution was almost none existient. Hoax,
on the other hand, will compile to a virtual machine byte code. This means Hoax
will still be intrepreted, but it will be much faster than Ruse ever was.

The second shortcoming of Ruse was how clunky it was on the inside; not to
mention all the source code was in one file, split up only by comments. All of
the native functions were a mess of type checking and validating arguments which
took extra-long because of the tree-walk nature of the language. Hoax will solve
this by having a better system of native functions from the interpreter itself.

The third shortcoming was the memory management (or there lackof).
Nothing was ever freed in Ruse, everything was allocated on one arena which
would only be freed at the end of execution. Hoax will have a runtime garbage
collector, which will hinder speed, but Hoax is not going to be written for
systems level programming anyway. The memory model for Hoax will be more
efficient too; Ruse had everything stuffed into a large tagged union and
allocated all willy nilly without a care in the world.

Another shortcoming was that Ruse could only technically intrepret one expression
per file or REPL input. This means you would have to wrap everything in a
file in a 'do' function which acted as a compound statement of sorts. It was
clunky, half-assed, and hacked together instead of having proper support for
multiple expressions in a file.

In Ruse's defense, it was cobbled together in approximately one night without a
real plan of action. But it was never fixed later, features were only added on
to the pile of garbage that was Ruse.

Ruse was a neat project for a prototype of sorts, but I want to create something
better for the fun of it.

### TL;DR

Ruse was more or less hacked together and never cleaned up after more features
were added. This resulted in a heaping pile of garbgae I unfortunately have to
call mine.

Hoax will serve as an improvement to Ruse, fixing all the issues which were
present in Ruse.


### Crafting Interpreters

I've read [Crafting Interpreters](https://craftinginterpreters.com/) a few
times actually, but I have never made a interpreted language which compiles to
a virtual machine's byte code. This will be a good exercise.

If you have not read the book yet, you should; it is a great introduction to the
world of programming language development. There is a free (and legal) online
version of the book on the website. If you want to support Robert Nystrom, consider
purchasing a copy of the book.

## Roadmap

A little road map to keep me on track. This will me modified as I go along of course.

The idea here is to get the Hoax Virtual Machine (HXVM) up and running the bare
minimum before we start tackling actual Hoax source code. We are *not* trying
to make the next JVM by any means, but we still want our VM to be useful.

- [ ] Hoax Virtual Machine
    - [x] Interpret hand-made VM chunks of code
    - [ ] Call C functions from the HXVM
        - [x] Native functions built into the HXVM
- [x] Start compiling Hoax to HVM byte code
    - [x] Parsing
    - [ ] Other semantic analysis

## Features I want in Hoax

A list of features I want in Hoax that were or were not in Ruse.

- C Interop
    - The idea here is to have an API of sorts to interact with Hoax from C and
    then be able to dynamically link libraries into an instance of the HXVM and
    call the functions.
    - Another idea is to have some sort of C FFI within the Hoax compiler to
    call out to externally defined functions and symbols when linked against the
    library.
        - An example here would be to link against libc, and then write the
        function signatures for various libc functions in Hoax
- Macros
- Type system of sorts
- Garbage collection
- User defined data-types
- A virtual machine interpreter instead of a tree-walker interpreter

## The HXVM (Hoax Virtual Machine)

The HXVM will have an instruction set very particular to the
semantics of LISP. Various instructions will only make sense in the realm of a 
LISP, this set includes, but is not limited to:

- OP\_CAR
    - Pushes the car (head) of the element at the top of the stack if it is of
    type cons
- OP\_CDR
    - Pushes the cdr (tail) of the element at the top of the stack if it is of
    type cons
- OP\_CONS
    - Pops the first two elements on the stack, boxes them, and places them in
    a cons cell on the top of the stack

But the HXVM will also feature more generalized instructions for working with
all types of data, such as math operations, control flow instructions, etc.

## Hoax as a language

Hoax as a language is meant to be an interpreted scripting language with a focus
on extensibility. There will be many options in the future to interop with C at
different levels depending on your use case. Aside from that, I plan on adding
a macro system which will run at compile time, along with a module/namespace
system which will also be computed at compile time. It might seem like not a lot
now, but this is all the foundational code being written to set up for a good
time moving forward.
