//
// Floating-point matrix
// 

#include "matrix_internal.h"

#define MATRIX(name)    LIQUID_CONCAT(fmatrix, name)
#define MATRIX_NAME     "fmatrix"
#define T               float
#define MATRIX_PRINT_ELEMENT(X,R,C,r,c) \
    printf("%5.2f\t", matrix_access(X,R,C,r,c));

#include "matrix.c"

