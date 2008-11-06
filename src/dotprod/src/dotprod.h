//
// Dot product
//

#ifndef __LIQUID_DOTPROD_H__
#define __LIQUID_DOTPROD_H__

#include <complex.h>

#define DOTPROD_CONCAT(prefix,name) prefix ## name
#define DOTPROD_MANGLE_FLOAT(name)  DOTPROD_CONCAT(fdotprod,name)
#define DOTPROD_MANGLE_CFLOAT(name) DOTPROD_CONCAT(cfdotprod,name)

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_DOTPROD_DEFINE_API(X,T)              \
T X(_run)(T *_x, T *_y, unsigned int _n);           \
T X(_run4)(T *_x, T *_y, unsigned int _n);          \
                                                    \
typedef struct X(_s) * X();                         \
X() X(_create)(T * _v, unsigned int _n);            \
void X(_destroy)(X() _q);                           \
T X(_execute)(X() _q, T * _v);

// Define APIs
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_FLOAT,  float)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_DOTPROD_H__
