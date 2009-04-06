//
// Complex floating-point matrix
// 

#include "matrix_internal.h"

#define MATRIX(name)    LIQUID_CONCAT(cfmatrix, name)
#define MATRIX_NAME     "cfmatrix"
#define T               float complex
#define MATRIX_PRINT_ELEMENT(X,R,C,r,c) \
    printf("%4.2f+j%4.2f\t", \
        crealf(matrix_access(X,R,C,r,c)), \
        cimagf(matrix_access(X,R,C,r,c)));

#include "matrix.c"

