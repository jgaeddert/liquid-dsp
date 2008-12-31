//
// Dot product
//

#ifndef __LIQUID_DOTPROD_H__
#define __LIQUID_DOTPROD_H__

#include <complex.h>

#define DOTPROD_CONCAT(prefix,name) prefix ## name
#define DOTPROD_MANGLE_RRRF(name)   DOTPROD_CONCAT(dotprod_rrrf,name)
#define DOTPROD_MANGLE_CCCF(name)   DOTPROD_CONCAT(dotprod_cccf,name)
#define DOTPROD_MANGLE_CRCF(name)   DOTPROD_CONCAT(dotprod_crcf,name)

// large macro
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_DOTPROD_DEFINE_API(X,TO,TC,TI)       \
TO X(_run)(TC *_h, TI *_x, unsigned int _n);        \
TO X(_run4)(TC *_h, TI *_x, unsigned int _n);       \
                                                    \
typedef struct X(_s) * X();                         \
X() X(_create)(TC * _v, unsigned int _n);           \
void X(_destroy)(X() _q);                           \
TO X(_execute)(X() _q, TI * _v);

// Define APIs
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_RRRF, float, float, float)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CCCF, float complex, float complex, float complex)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CRCF, float complex, float, float complex)

#endif // __LIQUID_DOTPROD_H__
