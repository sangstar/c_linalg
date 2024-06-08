#include <stdio.h>
#include "linalg.h"

int main() {
    float arr_a[3] = {1.f,2.f,3.f};
    struct vector a = TO_VECTOR(arr_a);
    print_vector(a);

    float arr_b[3] = {1.f,2.f,3.f};
    struct vector b = TO_VECTOR(arr_b);

    printf("%f",dot_product(a, b));
    return 0;
}