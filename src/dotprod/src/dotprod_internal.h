//
// Dot product internal header
//

#ifndef __LIQUID_DOTPROD_INTERNAL_H__
#define __LIQUID_DOTPROD_INTERNAL_H__

#include "dotprod.h"

#define LIQUID_DOTPROD_INTERNAL_DEFINE_API(X,T) \
struct X(_s) { \
    T * v; \
    unsigned int n; \
};

LIQUID_DOTPROD_INTERNAL_DEFINE_API(DOTPROD_MANGLE_FLOAT, float)
LIQUID_DOTPROD_INTERNAL_DEFINE_API(DOTPROD_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_DOTPROD_INTERNAL_H__
