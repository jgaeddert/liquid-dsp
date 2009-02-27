//
// Floating-point matrix
// 

#include "matrix_internal.h"

#define X(name) LIQUID_CONCAT(fmatrix, name)
#define T float
#define MATRIX_PRINT_ELEMENT(x,m,n) \
    printf("%4.2f\t", matrix_fast_access(x,m,n));

#include "matrix.c"

