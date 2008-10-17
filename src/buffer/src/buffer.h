//
// Buffers
//

#ifndef __MODULE_BUFFER_H__
#define __MODULE_BUFFER_H__

#include <complex.h>

typedef enum {
    CIRCULAR=0,
    STATIC
} buffer_type;

#define LIQUID_CONCAT(prefix, name) prefix ## name
#define BUFFER_MANGLE_FLOAT(name)  LIQUID_CONCAT(buffer, name)
#define BUFFER_MANGLE_CFLOAT(name) LIQUID_CONCAT(cbuffer, name)

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_BUFFER_DEFINE_API(X,T)                   \
                                                        \
typedef struct X(_s) * X();                             \
X() X(_create)(buffer_type _type, unsigned int _n);     \
void X(_destroy)(X() _b);                               \
void X(_print)(X() _b);                                 \
void X(_debug_print)(X() _b);                           \
void X(_clear)(X() _b);                                 \
void X(_read)(X() _b, T ** _v, unsigned int *_n);       \
void X(_release)(X() _b, unsigned int _n);              \
void X(_write)(X() _b, T * _v, unsigned int _n);
//void X(_force_write)(X() _b, T * _v, unsigned int _n);

// Define APIs
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_CFLOAT, float complex)

#endif // __MODULE_BUFFER_H__

