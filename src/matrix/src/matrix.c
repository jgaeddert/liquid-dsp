//
// Matrix method definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void MATRIX(_print)(T * _X, unsigned int _R, unsigned int _C)
{
    printf("matrix [%u x %u] : \n", _R, _C);
    unsigned int r,c;
    for (r=0; r<_R; r++) {
        for (c=0; c<_C; c++) {
            MATRIX_PRINT_ELEMENT(_X,_R,_C,r,c);
        }
        printf("\n");
    }
}

void MATRIX(_add)(unsigned int _R, unsigned int _C,
                  T * _X, T * _Y, T * _Z)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++)
        _Z[i] = _X[i] + _Y[i];
}

void MATRIX(_sub)(unsigned int _R, unsigned int _C,
                  T * _X, T * _Y, T * _Z)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++)
        _Z[i] = _X[i] - _Y[i];
}

void MATRIX(_mul)(T * _X, unsigned int _XR, unsigned int _XC,
                  T * _Y, unsigned int _YR, unsigned int _YC,
                  T * _Z, unsigned int _ZR, unsigned int _ZC)
{
    // ensure lengths are valid
    if (_ZR != _XR || _ZC != _YC) {
        printf("error: matrix_multiply(), invalid dimensions\n");
        exit(0);
    }

    unsigned int r, c, i;
    for (r=0; r<_ZR; r++) {
        for (c=0; c<_ZC; c++) {
            // z(i,j) = dotprod( x(i,:), y(:,j) )
            T sum=0.0f;
            for (i=0; i<_ZC; i++) {
                sum += matrix_access(_X,_XR,_XC,r,i) *
                       matrix_access(_Y,_YR,_YC,i,c);
            }
            matrix_access(_Z,_ZR,_ZC,r,c) = sum;
#ifdef DEBUG
            printf("z(%u,%u) = ", r, c);
            MATRIX_PRINT_ELEMENT(_Z,_ZR,_ZC,r,c);
            printf("\n");
#endif
        }
    }
}


// compute matrix transpose
void MATRIX(_trans)(T * _X, unsigned int _XR, unsigned int _XC)
{
    T y[_XR*_XC];
    memmove(y,_X,_XR*_XC*sizeof(T));

    unsigned int r,c;
    for (r=0; r<_XR; r++) {
        for (c=0; c<_XC; c++) {
            matrix_access(_X,_XC,_XR,c,r) = matrix_access(y,_XR,_XC,r,c);
        }
    }
}


