#include <stdio.h>
#include <stdlib.h>
#include "linalg.h"


extern vector create_vector(float* elems, size_t size) {
    vector v;
    v.size = size;
    v.elements = elems;
    return v;
};

extern matrix create_matrix(vector* vectors, size_t num_vectors) {
    matrix mat;
    int num_rows = 0;
    mat.vectors = (vector *)malloc(num_vectors * sizeof(vector));
    mat.num_cols = vectors[0].size;
    for (size_t i = 0; i < num_vectors; ++i) {
        mat.vectors[i] = vectors[i];
        num_rows++;
    }
    mat.num_rows = num_rows;
    return mat;
}

extern void free_matrix(matrix* mat) {
    free(mat->vectors);
}


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

extern float dot_product(vector *a, vector *b) {
    float result = 0.f;
    if (a->size != b->size) {
        printf("Sizes are incompatible for dot products: (%i, %i)", a->size, b->size);
        exit(1);
    }
    for (int i = 0; i < a->size; ++i) {
        printf("Multipying %f with %f\n", a->elements[i], b->elements[i]);
        result += (a->elements[i] * b->elements[i]);
    }
    printf("Finished\n");
    return result;
};

extern matrix* matmul(matrix *a, matrix *b) {
    // Allocate enough row vectors to match num_cols of b
    matrix *c = (matrix *)malloc(sizeof(matrix) * b->num_cols);

    // Matrix product always has num_rows from LHS, num_cols from RHS
    c->num_cols = b->num_cols;
    c->num_rows = a->num_rows;
    c->vectors = (vector *)malloc(a->num_rows * sizeof(vector));
    for (int i = 0; i < a->num_cols; ++i) {
        // Initialize the first row vec to save to matrix C.
        // It'll have a value saved for each mat a row dotted with mat b column
        // So it'll have as many cols as b columns
        vector *current_vec = (vector *)malloc(sizeof(vector) * b->num_cols);
        current_vec->elements = (float *)malloc(sizeof(float) * b->num_cols);
        current_vec->size = a->num_cols;
        for (int j = 0; j < a->num_rows; ++j) {
            // Instantiate the b column vector to dot with the matrix a row vector
            // matrix a's vectors are given as row vectors, so already have it with
            // &a-vectors[i]. Need to manually construct the column vector for b.
            vector *b_col_vec = (vector *)malloc(sizeof(vector) * b->num_rows);
            b_col_vec->elements = (float *)malloc(sizeof(float) * b->num_rows);
            b_col_vec->size = b->num_rows;
            for (int k = 0; k < b->num_rows; ++k) {
                // The b columns are constructed from left to right (sequenced by j)
                // The column is therefore fixed while taking the j'th element per
                // row to construct the column vector. For example, for the very
                // first column, we take the 0'th element of each row.
                b_col_vec->elements[k] = b->vectors[k].elements[j];
            }
            // Finally, dot the matrix a row vector with the b column vector to get
            // the j'th element of the i'th row for matrix C.
            current_vec->elements[j] = dot_product(&a->vectors[i], b_col_vec);
            free(b_col_vec); // Free the memory allocated for b_col_vec
        }
        print_vector(*current_vec);
        c->vectors[i] = *current_vec;
        free(current_vec); // Free the memory allocated for current_vec
    }
    return c; // Return the result matrix
}