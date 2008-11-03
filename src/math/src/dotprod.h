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
#define LIQUID_DOTPROD_DEFINE_API(X,T)                          \
T X()(T *_x, T *_y, unsigned int _n);   \
T X(4)(T *_x, T *_y, unsigned int _n);

// Define APIs
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_FLOAT,  float)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_DOTPROD_H__
