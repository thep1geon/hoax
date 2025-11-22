/*
 * Author: Maverick Haupt
 * Date: October, 2025
 *
 * This collection of data structures is meant to emulate typical generic data
 * structures you might find in a more featured language like Rust or C++. Most
 * of these data structures are just that, named data. But some (like the linked
 * list and dynamic array) do have associated "methods". One goal of this collection
 * is to keep the number of "methods" to a minimum. This library is supposed to
 * give you the data structures, it is up to you to put them to work. Dynamic
 * arrays and linked lists are different because they don't have a whole lot of
 * functionality without certain methods. A few handy, type agnostic macros have
 * also been provided, like the for-each macro for both the linked list and
 * dynamic array, as well as "constructors" for results and optionals.
 *
 * Another purpose of this library is just reduce boilerplate in my own code.
 * These are some of the most commonly used data structures, so I decided to
 * take the time and make them generic (but type-safe). Type safety is something
 * I really strive for in this implementation. If a data structure's "method"
 * needs a specificed type, I make the "method" an actual function to ensure
 * the type is checked by the compiler. I am aware of the _Generic macro feature
 * in C11, but I wanted to keep this compatible with C99.
 *
 * Because of the nature of C, a lot of this has been hacked together and
 * is probably not suitable for production code. Since we dealing with types
 * at the lexical level, we cannot really tell if the user passed in a type or
 * just gibberish -- this is caught by the compiler. But, this means if you use
 * a naming convention that prepends the struct keyword to your defined structs,
 * the majority of this code will not work since you cannot have spaces in C 
 * identifiers; this matters because we append the passed type onto the name
 * of the data structure; so, if a user passed in `struct point` into dynarray()
 * or something similar, it would not even compile. To get around this we have
 * to employ a little hack in the form of a suffix of the declaration of the
 * generic data structure (the _S prefix for struct). This works because, in the
 * case the user wants to use a struct type, we can just use the tag (or name) of
 * the struct and have the macro fill in the struct keyword for us when its needed.
 *
 * This is a little hacky, but the alternative would be forcing the user to typedef
 * all of their types which I do not fancy (mainly because I didn't want to have
 * to do that). While this addresses the problem of structs, it does not address
 * the problem of enums, unions, and pointers of all kinds.
 *
 * Let's start with enums and unions -- this is a tricky one for sure. Enumerations
 * are just named integer constants, so you don't really need to typedef these unless
 * that's your coding style. Unions are kind of similar, if you're going to be
 * using a union a data structure like this, you should be making it a tagged
 * union at the least; this will also let you use the _S declarations and it works
 * without a hitch.
 *
 * Now to talk about the elephant in the room: pointers of all kinds. With the
 * current implementation, you cannot just pass a pointer in the DECL macro, for
 * the same reason you couldn't use the struct keyword. This presents us with two
 * fixes, one, we add another prefix which denotes a pointer type. This will work with
 * pointers to types just fine, but will require the addition of many more macros
 * for declarations. The second option, and more feasible option, just require
 * a typedef for pointers only. This will let pointers work with the default
 * declaration macro for any data structure; as a bonus function pointers and
 * arrays would also work with this strategy. While I don't want to force typedefs
 * on every type the user wants to use with this little collection of data
 * structures, it is compromisable to force it for pointer types. This actually
 * has a few reasons of its own. One, it forces the user to acknowlege that they
 * are using pointer types in the data structure which is vital if they need to
 * free anything themselves since the free functions associated with the data
 * structures only free what they allocated. This means if you passed heap allocated
 * pointers into a dynamic array, you are responsible for freeing those heap
 * allocated objects, not the dynamic array. By forcing the user to typedef their
 * pointers, they are reminded to free them. Secondly, it makes the code easier
 * to write. I don't have to add a whole bunch of prefixes denoting pointer types
 * which makes my job easier, and the library easier to use - a win win for both
 * of us.
 *
 * Obviously, this library could be improved in many ways, but I think I have done
 * a decent job here.
 *
 * I am aware I am missing some other commonly used data
 * structures like hashmaps and trees of all sorts. But those are more complicated
 * and would require much more attention to detail. For example, a hashmap
 * not only requires two types, the key and value types, but a hash function
 * for the key and an equals function for the key. This is more cumbersome than
 * it needs to be for what I am going for, so I will be leaving it out for the
 * time being. If push comes to shove, I can implement a generic HashMap along
 * with other data structures that require additional functions to work. I will
 * also not be implementing stacks, queues, deques, doubly linked lists, or any
 * other data structure that can be emulated with the data structures currently
 * provided. This is for two reasons, firstly, it is just more work on me just
 * to rename existing data structures. And secondly, you can just use a linked
 * list or dynamic array for most of your use cases. In the event you cannot use
 * one of those two, you probably just need to roll your own anyway.
 *
 * With just a little bit of reading, I found another strategy for allowing any
 * type, regardless of the characters in the type (`struct`, pointers, etc.) that
 * relies on the user naming each data structure they declare instead of the macro
 * doing it for you. This has its own pros and cons. For pros, the types can be
 * anything, typedef'd or not. This allows for flexibility of the types and it keeps
 * the names of the declaration macros shorter and easier to read. But all great
 * things have their drawbacks. For starters, letting users define their own names
 * for types could become a bit of a foot-gun. By forcing the user onto a generated
 * name, it ensures there is consistency between types, keeping the code easier
 * to read and write. Another drawback would be added complexity the user wants
 * to declare a data structure instead of only when its actually necessary. An
 * additional parameter to each declaration macro would also make the code harder
 * to read. All in all, this could be a good idea, but for now, I want to stick
 * with what I have now. If I have any strong reasons to update it, I can do so
 * later.
 * */

#ifndef __GENERICS_H
#define __GENERICS_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "typedef.h"

/* @TODO: Think about making a generic string-hashmap */

/* Dynamic Arrays */

#define dynarray(T) dynarray__##T

#define DYNARRAY_DECL_S(T)                                                          \
    struct dynarray(T) {                                                            \
        struct T* at;                                                               \
        usize length;                                                               \
        usize capacity;                                                             \
    };                                                                              \
                                                                                    \
    void dynarray__##T##_push(struct dynarray(T)* dynarray, struct T element);      \
    struct T dynarray__##T##_pop(struct dynarray(T)* dynarray)

#define DYNARRAY_IMPL_S(T)                                                          \
    void dynarray__##T##_push(struct dynarray(T)* dynarray, struct T element) {     \
        DYNARRAY_RESIZE(struct T, dynarray);                                        \
        dynarray->at[dynarray->length++] = element;                                 \
    }                                                                               \
                                                                                    \
    struct T dynarray__##T##_pop(struct dynarray(T)* dynarray) {                    \
        return DYNARRAY_POP(dynarray);                                              \
    }

#define DYNARRAY_DECL(T)                                                    \
    struct dynarray(T) {                                                    \
        T* at;                                                              \
        usize length;                                                       \
        usize capacity;                                                     \
    };                                                                      \
                                                                            \
    void dynarray__##T##_push(struct dynarray(T)* dynarray, T element);     \
    T dynarray__##T##_pop(struct dynarray(T)* dynarray)

#define DYNARRAY_IMPL(T)                                                    \
    void dynarray__##T##_push(struct dynarray(T)* dynarray, T element) {    \
        DYNARRAY_RESIZE(T, dynarray);                                       \
        dynarray->at[dynarray->length++] = element;                         \
    }                                                                       \
                                                                            \
    T dynarray__##T##_pop(struct dynarray(T)* dynarray) {                   \
        return DYNARRAY_POP(dynarray);                                      \
    }

#define DYNARRAY_RESIZE(T, da)                                          \
    do {                                                                \
        if ((da)->length >= (da)->capacity) {                           \
            if ((da)->capacity == 0) {                                  \
                (da)->capacity = 4;                                     \
            } else {                                                    \
                (da)->capacity *= 2;                                    \
            }                                                           \
            (da)->at = realloc((da)->at, sizeof(T) * (da)->capacity);   \
            assert((da)->at);                                           \
        }                                                               \
    } while (0)

#define DYNARRAY_CLEAR(da) (da)->length = 0

#define DYNARRAY_FREE(da) free((da)->at);

#define DYNARRAY_POP(da) (da)->at[--(da)->length]

#define DYNARRAY_FOR_EACH(da, e) \
    for (usize __iter = 0; __iter < (da)->length && (e = &(da)->at[__iter], true); ++__iter)

/* Linked Lists */

#define linked_list(T) linked_list__##T
#define linked_list_node(T) linked_list_node__##T

#define LINKED_LIST_DECL(T) \
    OPTION_DECL(T);\
    struct linked_list_node(T) {\
        T item;\
        struct linked_list_node(T)* next;\
    };\
\
    struct linked_list(T) {\
        struct linked_list_node(T)* head;\
        struct linked_list_node(T)* tail;\
        usize length;\
    };\
\
    void linked_list__##T##_push_back(struct linked_list(T)* list, T e);\
    void linked_list__##T##_push_front(struct linked_list(T)* list, T e);\
    struct option(T) linked_list__##T##_pop_back(struct linked_list(T)* list);\
    struct option(T) linked_list__##T##_pop_front(struct linked_list(T)* list);\
    void linked_list__##T##_free(struct linked_list(T)* list)

#define LINKED_LIST_IMPL(T) \
    void linked_list__##T##_push_back(struct linked_list(T)* list, T e) {\
        struct linked_list_node(T)* new_node = malloc(sizeof(struct linked_list_node(T)));\
        new_node->item = e;\
        new_node->next = 0;\
        list->length += 1;\
        if (!list->tail) {\
            list->head = new_node;\
            list->tail = new_node;\
            return;\
        }\
        list->tail->next = new_node;\
        list->tail = new_node;\
        return;\
    }\
\
    void linked_list__##T##_push_front(struct linked_list(T)* list, T e) {\
        struct linked_list_node(T)* new_node = malloc(sizeof(struct linked_list_node(T)));\
        new_node->item = e;\
        new_node->next = 0;\
        list->length += 1;\
        if (!list->head) {\
            list->head = new_node;\
            list->tail = new_node;\
            return;\
        }\
        new_node->next = list->head;\
        list->head = new_node;\
        return;\
    }\
\
    struct option(T) linked_list__##T##_pop_back(struct linked_list(T)* list) {\
        struct linked_list_node(T)* curr_node;\
        T item;\
        usize i;\
        curr_node = (list)->head;\
        if (list->length <= 0) return (struct option(T))OPTION_NONE();\
        list->length -= 1;\
        for (i = 0; i < list->length; ++i) {\
            curr_node = curr_node->next;\
        }\
        item = curr_node->next->item;\
        free(curr_node->next);\
        curr_node->next = 0;\
        return (struct option(T))OPTION_SOME(item);\
    }\
\
    struct option(T) linked_list__##T##_pop_front(struct linked_list(T)* list) {\
        struct linked_list_node(T)* old_head;\
        T item;\
        if (list->length <= 0) return (struct option(T)) OPTION_NONE();\
        list->length -= 1;\
        old_head = list->head;\
        item = old_head->item;\
        list->head = old_head->next;\
        free(old_head);\
        return (struct option(T))OPTION_SOME(item);\
    }\
\
    void linked_list__##T##_free(struct linked_list(T)* list) {\
        do {\
            linked_list__##T##_pop_front(list);\
        } while (list->length > 0);\
    }

#define LINKED_LIST_DECL_S(T) \
    OPTION_DECL_S(T);\
    struct linked_list_node(T) {\
        struct T item;\
        struct linked_list_node(T)* next;\
    };\
\
    struct linked_list(T) {\
        struct linked_list_node(T)* head;\
        struct linked_list_node(T)* tail;\
        usize length;\
    };\
\
    void linked_list__##T##_push_back(struct linked_list(T)* list, struct T e);\
    void linked_list__##T##_push_front(struct linked_list(T)* list, struct T e);\
    struct option(T) linked_list__##T##_pop_back(struct linked_list(T)* list);\
    struct option(T) linked_list__##T##_pop_front(struct linked_list(T)* list);\
    void linked_list__##T##_free(struct linked_list(T)* list)

/* 
 * @TODO: I'm pretty sure these linked list implementations are broken. This is
 * not of utmost importance as I'm not using linked lists right now, but I might.
 * */

#define LINKED_LIST_IMPL_S(T) \
    void linked_list__##T##_push_back(struct linked_list(T)* list, struct T e) {\
        struct linked_list_node(T)* new_node = malloc(sizeof(struct linked_list_node(T)));\
        new_node->item = e;\
        new_node->next = 0;\
        list->length += 1;\
        if (!list->tail) {\
            list->head = new_node;\
            list->tail = new_node;\
            return;\
        }\
        list->tail->next = new_node;\
        list->tail = new_node;\
        return;\
    }\
\
    void linked_list__##T##_push_front(struct linked_list(T)* list, struct T e) {\
        struct linked_list_node(T)* new_node = malloc(sizeof(struct linked_list_node(T)));\
        new_node->item = e;\
        new_node->next = 0;\
        list->length += 1;\
        if (!list->head) {\
            list->head = new_node;\
            list->tail = new_node;\
            return;\
        }\
        new_node->next = list->head;\
        list->head = new_node;\
        return;\
    }\
\
    struct option(T) linked_list__##T##_pop_back(struct linked_list(T)* list) {\
        struct linked_list_node(T)* curr_node;\
        struct T item;\
        usize i;\
        struct linked_list_node(T) curr_node = (this)->head;\
        if (list->length <= 0) return (struct option(T))OPTION_NONE();\
        list->length -= 1;\
        for (i = 0; i < list->length; ++i) {\
            curr_node = curr_node->next;\
        }\
        item = curr_node->next->item;\
        free(curr_node->next);\
        curr_node->next = 0;\
        return (struct option(T))OPTION_SOME(item);\
    }\
\
    struct option(T) linked_list__##T##_pop_front(struct linked_list(T)* list) {\
        struct linked_list_node(T)* old_head;\
        struct T item;\
        if (list->length <= 0) return (struct option(T)) OPTION_NONE();\
        list->length -= 1;\
        old_head = list->head;\
        item = old_head->item;\
        list->head = old_head->next;\
        free(old_head);\
        return (struct option(T))OPTION_SOME(item);\
    }\
\
    void linked_list__##T##_free(struct linked_list(T)* list) {\
        do {\
            linked_list__##T##_pop_front(list);\
        } while (list->length > 0);\
    }

#define LINKED_LIST_FOR_EACH(list, e) \
    for ((e) = (list)->head; (e); (e) = (e)->next)

/* Optionals */

#define option(T) option__##T

#define OPTION_DECL(T)  \
    struct option(T) {  \
        T item;         \
        bool is_some;   \
    }

#define OPTION_DECL_S(T)    \
    struct option(T) {      \
        struct T item;      \
        bool is_some;       \
    }

#define OPTION_SOME(T, d) (struct option(T)){.item = (d), .is_some = true}
#define OPTION_NONE(T) (struct option(T)){.is_some = false}

/* Slices */

#define slice(T) slice__##T

#define SLICE_DECL(T)   \
    struct slice(T) {   \
        T* ptr;         \
        usize length;   \
    }

#define SLICE_DECL_S(T) \
    struct slice(T) {   \
        struct T* ptr;  \
        usize length;   \
    }

#define SLICE_FROM_ARR(arr, start, end) {.ptr = (arr)+(start), .length = (end)-(start)}
#define SUBSLICE(slice, start, end) {.ptr = (slice).ptr+(start), .length = (end)-(start)}

/* 
 * WARN: This macro will evaulate the input the slice multiple times, so
 * make sure you know of that before using.
 * */
#define SLICE_FOR_EACH(slice, e)\
    for (usize __iter = 0; __iter < (slice).length && ((e = (slice).ptr[__iter]), true); ++__iter)


#endif  /*__GENERICS_H*/
