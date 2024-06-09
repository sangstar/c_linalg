#include <stdio.h>
#include <stdlib.h>
#include "math.h"


vector create_vector(float* elems, size_t size) {
    vector v;
    v.size = size;
    v.elements = elems;
    return v;
};


void print_vector(vector v) {
    printf("[");
    for (int i = 0; i < v.size; ++i) {
        printf("%f", v.elements[i]);
        if (i != v.size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

float dot_product(vector a, vector b) {
    float result = 0.f;
    if (a.size != b.size) {
        printf("Sizes are incompatible for dot products: (%i, %i)", a.size, b.size);
        exit(1);
    }
    for (int i = 0; i <= a.size; ++i) {
        result += (a.elements[i] * b.elements[i]);
    }
    return result;
};
