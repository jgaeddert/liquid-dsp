//
// Matrix method prototypes, internal
//

#ifndef __LIQUID_MATRIX_INTERNAL_H__
#define __LIQUID_MATRIX_INTERNAL_H__

#include "matrix.h"

#define MATRIX_MAX_SIZE 1024

struct matrix_s {
    unsigned int M; // number of rows
    unsigned int N; // number of columns
    unsigned int L; // size, M*N
    float * v;      // memory
};

// fast access to matrix element, read/write
#define matrix_fast_access(x,m,n) (x->v[m*(x->N)+n])

// is matrix square?
#define matrix_is_square(x) ( ((x->M)==(x->N)) ? true : false )

// is matrix valid size?
#define matrix_valid_size(x,m,n) ( ((x->M)==m && (x->N)==n) ? true : false )


#endif // __LIQUID_MATRIX_INTERNAL_H__
