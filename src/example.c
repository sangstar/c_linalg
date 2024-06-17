#include <stdio.h>
#include <stdlib.h>
#include "linalg.h"

int main() {
    float arr_a[2] = {1.f,2.f};
    vector a = TO_VECTOR(arr_a);
    print_vector(a);

    float arr_b[2] = {1.f,3.f};
    vector b = TO_VECTOR(arr_b);


    size_t num_vectors = 2;
    vector *vectors = (vector *)malloc(sizeof(vector) * num_vectors);
    vectors[0] = a;
    vectors[1] = b;
    matrix mat_a = create_matrix(vectors, num_vectors);


    float arr_c[2] = {1.f,2.f};
    vector c = TO_VECTOR(arr_c);
    print_vector(a);

    float arr_d[2] = {1.f,3.f};
    vector d = TO_VECTOR(arr_d);
    vector *vectors_b = (vector *)malloc(sizeof(vector) * num_vectors);
    vectors_b[0] = c;
    vectors_b[1] = d;
    matrix mat_b = create_matrix(vectors_b, num_vectors);

    matrix *mat_c = matmul(&mat_a, &mat_b);
//    free(mat_a.vectors);
//    free(mat_b.vectors);
//    free(mat_c.vectors);
}