#define ARRLEN(x)  (sizeof(x) / sizeof((x)[0]))
#define TO_VECTOR(x)  create_vector(x, ARRLEN(x))

#ifndef MATH_H
#define MATH_H

#include <stddef.h>


typedef struct {
    int size;
    float* elements;
} vector;

vector create_vector(float* elems, size_t size);
void print_vector(vector v);
float dot_product(vector a, vector b);

#endif /* MATH_H */
