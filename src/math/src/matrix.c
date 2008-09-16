//
// Matrix method definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "matrix.h"

#define DEBUG 0

struct matrix_s {
    unsigned int M;
    unsigned int N;
    float * v;
};

#define matrix_fast_access(x,m,n) (x->v[m*(x->N)+n])

#define matrix_is_square(x) ( ((x->M)==(x->N)) ? true : false )
#define matrix_valid_size(x,m,n) ( ((x->M)==m && (x->N)==n) ? true : false )

matrix matrix_create(unsigned int _M, unsigned int _N)
{
    matrix x = (matrix) malloc(sizeof(struct matrix_s));
    x->M = _M;
    x->N = _N;
    x->v = (float*) malloc(_M*_N*sizeof(float));
    return x;
}

matrix matrix_copy(matrix _x)
{
    //matrix y = matrix_create(_x->M, _x->N);
    //memcpy(y->v,_x->v,(_x->M)*(_x->N)*sizeof(float));
    //return y;

    // same as above, but copies all internal variables by default
    matrix y = (matrix) malloc(sizeof(struct matrix_s));
    memcpy(y, _x, sizeof(struct matrix_s));
    y->v = (float*) malloc((y->M)*(y->N)*sizeof(float));
    memcpy(y->v, _x->v, (y->M)*(y->N)*sizeof(float));
    return y;
}

void matrix_destroy(matrix _x)
{
    free(_x->v);
    free(_x);
}

void matrix_print(matrix _x)
{
    unsigned int m, n;
    //for (n=0; n<_x->N; n++)
    //    printf("\t%u", n);
    printf("matrix [%ux%u] :\n",_x->M,_x->N);
    for (m=0; m<_x->M; m++) {
        //printf("%u:\t", m);
        printf("\t");
        for (n=0; n<_x->N; n++) {
            printf("%4.2f\t", matrix_fast_access(_x,m,n));
        }
        printf("\n");
    }
    printf("\n");
}

void matrix_clear(matrix _x)
{
    memset(_x->v, 0x00, (_x->M)*(_x->N)*sizeof(float));
}

void matrix_dim(matrix _x, unsigned int *_M, unsigned int *_N)
{
    *_M = _x->M;
    *_N = _x->N;
}

void matrix_assign(matrix _x, unsigned int _m, unsigned int _n, float _value)
{
    if (_m >= _x->M) {
        printf("error: matrix_assign(), row index out of range\n");
        return;
    } else if (_n >= _x->N) {
        printf("error: matrix_assign(), column index out of range\n");
        return;
    }

    //_x->v[_m*(_x->N) + _n] = _value;
    matrix_fast_access(_x,_m,_n) = _value;
}

float matrix_access(matrix _x, unsigned int _m, unsigned int _n)
{
    if (_m >= _x->M) {
        printf("error: matrix_access(), row index out of range\n");
        return 0.0f;
    } else if (_n >= _x->N) {
        printf("error: matrix_access(), column index out of range\n");
        return 0.0f;
    }

    //return _x->v[_m*(_x->N) + _n];
    return matrix_fast_access(_x,_m,_n);
}

void matrix_multiply(matrix _x, matrix _y, matrix _z)
{
    // ensure lengths are valid
    if (!matrix_valid_size(_z,_x->M,_y->N)) {
        printf("error: matrix_multiply(), invalid dimensions\n");
        return;
    }

    unsigned int m, n, i;
    for (m=0; m<_z->M; m++) {
        for (n=0; n<_z->N; n++) {
            // z(i,j) = dotprod( x(i,:), y(:,j) )
            float sum=0.0f;
#if DEBUG
                printf("z(%u,%u) = ", m, n);
#endif
            for (i=0; i<_z->M; i++) {
                sum += matrix_fast_access(_x,m,i) *
                       matrix_fast_access(_y,i,n);
#if DEBUG
                printf("(%4.2f*%4.2f)+",matrix_fast_access(_x,m,i),matrix_fast_access(_y,i,n));
#endif
            }
#if DEBUG
            printf(" = %4.2f\n",sum);
#endif
            matrix_fast_access(_z,m,n) = sum;
        }
    }
}

void matrix_transpose(matrix _x)
{
    // quick and dirty implementation:
    //   - create new matrix
    //   - copy values
    matrix t = matrix_copy(_x);

    unsigned int tmp = _x->N;
    _x->N = _x->M;
    _x->M = tmp;

    unsigned int m, n;
    for (m=0; m<_x->M; m++) {
        for (n=0; n<_x->N; n++) {
            matrix_fast_access(_x,m,n) = matrix_fast_access(t,n,m);
        }
    }

    matrix_destroy(t);
}

void matrix_invert(matrix _x)
{
    // test that matrix is square
    if (!matrix_is_square(_x)) {
        printf("error: matrix_invert(), matrix is not square\n");
        return;
    }

    // LU decomposition
}

// decompose matrix into product of  lower/upper triangular matrices
// using Crout's algorithm
void matrix_lu_decompose(matrix _x, matrix L, matrix U)
{
    // test that matrices are square and of proper size
    if (!matrix_is_square(_x) || !matrix_is_square(L) || !matrix_is_square(U)) {
        printf("error: matrix_lu_decompose(), matrices must be square\n");
        return;
    } else if (!matrix_valid_size(L,_x->M,_x->N) || !matrix_valid_size(U,_x->M,_x->N)) {
        printf("error: matrix_lu_decompose(), L, U matrices must match input matrix\n");
        return;
    }

    matrix_clear(L);
    matrix_clear(U);

    unsigned int N = _x->N;

    matrix a = L;
    matrix b = U;

    unsigned int i;
    for (i=0; i<N; i++)
        matrix_fast_access(a,i,i) = 1.0f;

    unsigned int j, k;
    for (j=0; j<N; j++) {
        for (i=0; i<j; i++) {
            float sum=0.0f;
            if (i>0) {
                for (k=0; k<i-1; k++) {
                    sum += matrix_fast_access(a,i,k)*matrix_fast_access(b,k,j);
                }
            }
            matrix_fast_access(b,i,j) = matrix_fast_access(a,i,j) - sum;
        }

        for (i=j; i<N; i++) {
            //
        }
    }
}

