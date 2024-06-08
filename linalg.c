#include <stdio.h>
#include <stdlib.h>
#include "linalg.h"

struct vector {
    int size;
    float* elements;
};

struct vector create_vector(float* elems, size_t size) {
    struct vector v;
    v.size = size;
    v.elements = elems;
    return v;
};


void print_vector(struct vector v) {
    printf("[");
    for (int i = 0; i < v.size; ++i) {
        printf("%f", v.elements[i]);
        if (i != v.size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

float dot_product(struct vector a, struct vector b) {
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

int main() {
    float arr_a[3] = {1.f,2.f,3.f};
    struct vector a = create_vector(arr_a, ARRLEN(arr_a));
    print_vector(a);

    float arr_b[3] = {1.f,2.f,3.f};
    struct vector b = create_vector(arr_b, ARRLEN(arr_b));

    printf("%f",dot_product(a, b));
    return 0;
}