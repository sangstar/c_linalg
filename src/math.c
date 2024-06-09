#include <stdio.h>
#include <stdlib.h>
#include "math.h"


extern vector create_vector(float* elems, size_t size) {
    vector v;
    v.size = size;
    v.elements = elems;
    return v;
};


extern void print_vector(vector v) {
    printf("[");
    for (int i = 0; i < v.size; ++i) {
        printf("%f", v.elements[i]);
        if (i != v.size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

extern float dot_product(vector a, vector b) {
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

/*
Example usage:

#include <stdio.h>
#include "math.h"

int main() {
    float arr_a[3] = {1.f,2.f,3.f};
    vector a = TO_VECTOR(arr_a);
    print_vector(a);

    float arr_b[3] = {1.f,3.f,3.f};
    vector b = TO_VECTOR(arr_b);

    printf("%f",dot_product(a, b));
    return 0;
}
*/