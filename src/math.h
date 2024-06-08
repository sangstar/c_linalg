#define ARRLEN(x)  (sizeof(x) / sizeof((x)[0]))
#define TO_VECTOR(x)  create_vector(x, ARRLEN(x))

#ifndef MATH_H
#define MATH_H

#include <stddef.h>

struct vector {
    int size;
    float* elements;
};

struct vector create_vector(float* elems, size_t size);
void print_vector(struct vector v);
float dot_product(struct vector a, struct vector b);

#endif /* MATH_H */
