//
// Matrix method prototypes
//

#ifndef __LIQUID_MATRIX_H__
#define __LIQUID_MATRIX_H__

#include <complex.h>

#define LIQUID_CONCAT(prefix, name) prefix ## name
#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(fmatrix, name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(cfmatrix, name)

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_MATRIX_DEFINE_API(X,T)                       \
typedef struct X(_s) * X();                                 \
X() X(_create)(unsigned int _M, unsigned int _N);           \
X() X(_copy)(X() _x);                                       \
void X(_destroy)(X() _x);                                   \
void X(_print)(X() _x);                                     \
void X(_clear)(X() _x);                                     \
void X(_dim)(X() _x, unsigned int *_M, unsigned int *_N);   \
void X(_assign)(X() _x, unsigned int _m, unsigned int _n,   \
    T _value);                                              \
T X(_access)(X() _x, unsigned int _m, unsigned int _n);     \
void X(_multiply)(X() _x, X() _y, X() _z);                  \
void X(_transpose)(X() _x);                                 \
void X(_invert)(X() _x);                                    \
void X(_lu_decompose)(X() _x, X() _lower, X() _upper);
//void X(_add)(X() _x, X() _y, X() _z);

LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_FLOAT, float)
LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_MATRIX_H__
