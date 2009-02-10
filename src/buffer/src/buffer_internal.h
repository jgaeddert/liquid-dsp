//
// Buffers, internal functions
//

#ifndef __LIQUID_BUFFER_INTERNAL_H__
#define __LIQUID_BUFFER_INTERNAL_H__

#include "liquid.composite.h"

//
// Buffers
//

#define buffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_BUFFER_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);                             \
void X(_c_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_s_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_c_write)(X() _b, T * _v, unsigned int _n);      \
void X(_s_write)(X() _b, T * _v, unsigned int _n);      \
void X(_c_release)(X() _b, unsigned int _n);            \
void X(_s_release)(X() _b, unsigned int _n);            \
void X(_c_push)(X() _b, T _v);                          \
void X(_s_push)(X() _b, T _v);

LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_UINT, unsigned int)


//
// Windows
//

#define window_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_WINDOW_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);

LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_CFLOAT, float complex)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_UINT, unsigned int)


#endif // __LIQUID_BUFFER_INTERNAL_H__

