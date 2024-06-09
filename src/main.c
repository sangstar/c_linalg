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