#ifndef __NATIVE_H
#define __NATIVE_H

struct expr;

typedef struct expr(*native_fn)(struct expr args);

struct expr native_display(struct expr args);

#endif  /* __NATIVE_H */
