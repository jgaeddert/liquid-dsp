//
// Windows, internal functions
//

#ifndef __LIQUID_WINDOW_INTERNAL_H__
#define __LIQUID_WINDOW_INTERNAL_H__

#include "window.h"

#define window_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_WINDOW_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);

LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_CFLOAT, float complex)

#endif // __LIQUID_WINDOW_INTERNAL_H__

