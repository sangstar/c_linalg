#define ARRLEN(x)  (sizeof(x) / sizeof((x)[0]))

#ifndef LINALG_H
#define LINALG_H

#include <stddef.h>

struct vector {
    int size;
    float* elements;
};

struct vector create_vector(float* elems, size_t size);
void print_vector(struct vector v);
float dot_product(struct vector a, struct vector b);

#endif /* LINALG_H */
