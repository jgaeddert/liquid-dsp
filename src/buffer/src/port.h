//
//
//

#ifndef __LIQUID_PORT_H__
#define __LIQUID_PORT_H__

#include <complex.h>

#define LIQUID_CONCAT(prefix,name)  prefix ## name

#define PORT_MANGLE_FLOAT(name)     LIQUID_CONCAT(fport,name)
#define PORT_MANGLE_CFLOAT(name)    LIQUID_CONCAT(cfport,name)
#define PORT_MANGLE_UINT(name)      LIQUID_CONCAT(uiport,name)

// Macro
//  X: name-mangling macro
//  T: data type
#define LIQUID_PORT_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _n); \
void X(_destroy)(X() _p); \
void X(_print)(X() _p); \
void X(_produce)(X() _p, float * _v, unsigned int _n); \
void X(_consume)(X() _p, float ** _v, unsigned int _n); \
void X(_release)(X() _p, unsigned int _n);

LIQUID_PORT_DEFINE_API(PORT_MANGLE_FLOAT, float)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_CFLOAT, float complex)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_UINT, unsigned int)

#endif // __LIQUID_PORT_H__

