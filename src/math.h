#define ARRLEN(x)  (sizeof(x) / sizeof((x)[0]))
#define TO_VECTOR(x)  create_vector(x, ARRLEN(x))

#ifndef MATH_H
#define MATH_H

#include <stddef.h>


typedef struct {
    int size;

    // We declare a float* to store array elements. This is
    // because array lend themselves quite well to pointers.

    // C effectively uses pointer arithmetic to make array indexing
    // fairly natural. We can happily do elements[0] = 2 and then
    // do elements[1] = 3, because each time we index, we're actually
    // implicitly doing pointer arithmetic. When we do elements[i] = 3,
    // we're actually saying *(elements + i) = 3, in other words, 
    // "take the memory address initially allocated for elements, 
    // move it i, and assign 3 to that." 
    float* elements;
} vector;

vector create_vector(float* elems, size_t size);
void print_vector(vector v);
float dot_product(vector a, vector b);

#endif /* MATH_H */
