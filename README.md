# Hoax

A LISP-like language inspired by [Ruse](https://github.com/thep1geon/ruse) which
is inspired by Scheme.

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

The idea here is to get the Hoax Virtual Machine (HVM) up and running the bare
minimum before we start tackling actual Hoax source code. We are *not* trying
to make the next JVM by any means, but we still want our VM to be useful.

- [ ] Hoax Virtual Machine
    - [ ] Interpret hand-made VM chunks of code
    - [ ] Call C functions from the HVM
- [ ] Start compiling Hoax to HVM byte code
    - [ ] Lexing
    - [ ] Parsing
    - [ ] Other semantic analysis

## Features I want in Hoax

A list of features I want in Hoax that were or were not in Ruse.

- C Interop
- Macros
- Type system of sorts
- Garbage collection
- User defined data-types
