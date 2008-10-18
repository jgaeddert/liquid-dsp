//
// Buffers, internal functions
//

#ifndef __LIQUID_BUFFER_INTERNAL_H__
#define __LIQUID_BUFFER_INTERNAL_H__

#include "buffer.h"

#define buffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_BUFFER_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);                             \
void X(_c_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_s_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_c_write)(X() _b, T * _v, unsigned int _n);      \
void X(_s_write)(X() _b, T * _v, unsigned int _n);      \
void X(_c_release)(X() _b, unsigned int _n);            \
void X(_s_release)(X() _b, unsigned int _n);

LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_UINT, unsigned int)

#endif // __LIQUID_BUFFER_INTERNAL_H__

