//
// Complex floating-point matrix
// 

#include "matrix_internal.h"

#define MATRIX(name)    LIQUID_CONCAT(cfmatrix, name)
#define MATRIX_NAME     "cfmatrix"
#define T               float complex
#define MATRIX_PRINT_ELEMENT(x,m,n) \
    printf("%4.2f+j%4.2f\t", \
        crealf(matrix_fast_access(x,m,n)), \
        cimagf(matrix_fast_access(x,m,n)));

#include "matrix.c"

