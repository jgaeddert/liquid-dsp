//
// Matrix method prototypes, internal
//

#ifndef __LIQUID_MATRIX_INTERNAL_H__
#define __LIQUID_MATRIX_INTERNAL_H__

#include <complex.h>
#include "liquid.h"

#define MATRIX_MAX_SIZE 1024

// fast access to matrix element, read/write
#define matrix_fast_access(X,r,c,R,C) ((X)[(r)*(C)+(c)])

#endif // __LIQUID_MATRIX_INTERNAL_H__
