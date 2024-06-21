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

vector get_row_vector(matrix *mat, int row_index) {
    return mat->vectors[row_index];
}

vector get_column_vector(matrix *mat, int col_index) {
    vector col;
    col.size = mat->num_cols;
    col.elements = (float *)malloc(mat->num_cols * sizeof(float));
    if (!col.elements) {
        printf("Memory allocation failed for column vector creation");
        exit(1);
    }
    for (int i = 0; i < mat->num_rows; ++i) {
        col.elements[i] = mat->vectors[i].elements[col_index];
    }
    return col;
}

extern matrix* matmul(matrix *a, matrix *b) {

    if (a->num_rows != b->num_cols) {
        printf("Matrices are incompatible for multiplication");
        exit(1);
    }

    matrix *c = (matrix *)malloc(sizeof(matrix));
    if (!c) {
        printf("Memory allocation failed for matrix product");
        exit(1);
    }

    // Matrix product always has num_rows from LHS, num_cols from RHS
    c->num_cols = b->num_cols;
    c->num_rows = a->num_rows;

    c->vectors = (vector *)malloc(a->num_rows * sizeof(vector));
    if (!c->vectors) {
        printf("Memory allocation failed for matrix product vectors");
        free(c);
        exit(1);
    }

    for (int i = 0; i < a->num_cols; ++i) {

        // Initialize the first row vec to save to matrix C.
        // It'll have a value saved for each mat a row dotted with mat b column
        // So it'll have as many cols as b columns

        c->vectors[i].elements = (float *)malloc(c->num_cols * sizeof(float));
        if (!c->vectors[i].elements) {
            printf("Memory allocation failed for matrix product vector elements");
            for (int k = 0; k < i; ++k) {
                free(c->vectors[k].elements);
            }
            free(c->vectors);
            free(c);
            exit(1);
        }

        // This is a column vector, so its length will the number of rows
        c->vectors[i].size = c->num_rows;
        vector a_row_vector = get_row_vector(a, i);

        for (int j = 0; j < a->num_rows; ++j) {
            // Instantiate the b column vector to dot with the matrix a row vector
            // matrix a's vectors are given as row vectors, so already have it with
            // &a-vectors[i]. Need to manually construct the column vector for b.

            // This variable's lifetime willl be just in this loop, so declare it as
            // a variable on the stack
            vector b_col_vec = get_column_vector(b, j);
            if (b_col_vec.size == 0 && b->num_rows != 0) {
                printf("Column vector size corrupted.");
                exit(1);
            }
            for (int k = 0; k < b->num_rows; ++k) {
                // The b columns are constructed from left to right (sequenced by j)
                // The column is therefore fixed while taking the j'th element per
                // row to construct the column vector. For example, for the very
                // first column, we take the 0'th element of each row.
                b_col_vec.elements[k] = b->vectors[k].elements[j];
            }
            // Finally, dot the matrix a row vector with the b column vector to get
            // the j'th element of the i'th row for matrix C.

            c->vectors[i].elements[j] = dot_product(&a_row_vector, &b_col_vec);
            free(b_col_vec.elements);
        }
        print_vector(c->vectors[i]);
    }
    return c; // Return the result matrix
}