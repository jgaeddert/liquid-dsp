//
// Matrix method definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct X(_s) {
    unsigned int M; // number of rows
    unsigned int N; // number of columns
    unsigned int L; // size, M*N
    T * v;          // memory
};

X() X(_create)(unsigned int _M, unsigned int _N)
{
    X() x = (X()) malloc(sizeof(struct X(_s)));
    x->M = _M;
    x->N = _N;
    x->L = (x->M) * (x->N);
    if (x->L > MATRIX_MAX_SIZE) {
        printf("error: matrix_create(%u, %u), maximum size (%u) exceeded\n",
            _M, _N, MATRIX_MAX_SIZE);
        exit(0);
    }
    x->v = (T*) malloc((x->L)*sizeof(T));
    return x;
}

void X(_destroy)(X() _x)
{
    // TODO: ensure free operation is done safely
    free(_x->v);
    free(_x);
}

X() X(_copy)(X() _x)
{
    //X() y = matrix_create(_x->M, _x->N);
    //memcpy(y->v,_x->v,(_x->M)*(_x->N)*sizeof(T));
    //return y;

    // same as above, but copies all internal variables by default
    X() y = (X()) malloc(sizeof(struct X(_s)));
    memcpy(y, _x, sizeof(struct X(_s)));
    y->v = (T*) malloc((y->M)*(y->N)*sizeof(T));
    memcpy(y->v, _x->v, (y->M)*(y->N)*sizeof(T));
    return y;
}

void X(_print)(X() _x)
{
    unsigned int m, n;
    //for (n=0; n<_x->N; n++)
    //    printf("\t%u", n);
    printf("matrix [%ux%u] :\n",_x->M,_x->N);
    for (m=0; m<_x->M; m++) {
        //printf("%u:\t", m);
        printf("\t");
        for (n=0; n<_x->N; n++) {
            MATRIX_PRINT_ELEMENT(_x,m,n)
            printf("  ");
        }
        printf("\n");
    }
    printf("\n");
}

void X(_clear)(X() _x)
{
    memset(_x->v, 0x00, (_x->M)*(_x->N)*sizeof(T));
}

void X(_dim)(X() _x, unsigned int *_M, unsigned int *_N)
{
    *_M = _x->M;
    *_N = _x->N;
}

void X(_assign)(X() _x, unsigned int _m, unsigned int _n, T _value)
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

void X(_access)(X() _x, unsigned int _m, unsigned int _n, T * _y)
{
    if (_m >= _x->M) {
        printf("error: matrix_access(), row index out of range\n");
        return 0.0f;
    } else if (_n >= _x->N) {
        printf("error: matrix_access(), column index out of range\n");
        return 0.0f;
    }

    //return _x->v[_m*(_x->N) + _n];
    *_y = matrix_fast_access(_x,_m,_n);
}

void X(_multiply)(X() _x, X() _y, X() _z)
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
            T sum=0.0f;
#ifdef DEBUG
                printf("z(%u,%u) = ", m, n);
#endif
            for (i=0; i<_z->M; i++) {
                sum += matrix_fast_access(_x,m,i) *
                       matrix_fast_access(_y,i,n);
            }
            matrix_fast_access(_z,m,n) = sum;
        }
    }
}

void X(_transpose)(X() _x)
{
    // quick and dirty implementation:
    //   - create new matrix
    //   - copy values
    X() t = X(_copy)(_x);

    unsigned int tmp = _x->N;
    _x->N = _x->M;
    _x->M = tmp;

    unsigned int m, n;
    for (m=0; m<_x->M; m++) {
        for (n=0; n<_x->N; n++) {
            matrix_fast_access(_x,m,n) = matrix_fast_access(t,n,m);
        }
    }

    X(_destroy)(t);
}

void X(_invert)(X() _x)
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
void X(_lu_decompose)(X() _x, X() L, X() U)
{
    // test that matrices are square and of proper size
    if (!matrix_is_square(_x) || !matrix_is_square(L) || !matrix_is_square(U)) {
        printf("error: matrix_lu_decompose(), matrices must be square\n");
        return;
    } else if (!matrix_valid_size(L,_x->M,_x->N) || !matrix_valid_size(U,_x->M,_x->N)) {
        printf("error: matrix_lu_decompose(), L, U matrices must match input matrix\n");
        return;
    }

    X(_clear)(L);
    X(_clear)(U);

    unsigned int N = _x->N;

    X() a = L;
    X() b = U;

    unsigned int i;
    for (i=0; i<N; i++)
        matrix_fast_access(a,i,i) = 1.0f;

    unsigned int j, k;
    for (j=0; j<N; j++) {
        for (i=0; i<j; i++) {
            T sum=0.0f;
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

