//
// Matrix method prototypes, internal
//

#ifndef __LIQUID_MATRIX_INTERNAL_H__
#define __LIQUID_MATRIX_INTERNAL_H__

#include <complex.h>
#include "liquid.h"

#define MATRIX_MAX_SIZE 1024

// fast access to matrix element, read/write
#define matrix_fast_access(x,m,n) (x->v[m*(x->N)+n])

// is matrix square?
#define matrix_is_square(x) ( ((x->M)==(x->N)) ? true : false )

// is matrix valid size?
#define matrix_valid_size(x,m,n) ( ((x->M)==m && (x->N)==n) ? true : false )

#define MATRIX_VALIDATE_INPUT(f,x,m,n)                      \
    if (m >= x->M) {                                        \
        printf("error: %s,%s, invalid row index: %u\n",     \
            MATRIX_NAME,f,m);                               \
        exit(0);                                            \
    } else if (n >= x->N) {                                 \
        printf("error: %s,%s, invalid col index: %u\n",     \
            MATRIX_NAME,f,n);                               \
        exit(0);                                            \
    }
        

#endif // __LIQUID_MATRIX_INTERNAL_H__
