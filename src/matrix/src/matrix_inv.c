//
// Matrix inverse method definitions
//

#include "liquid.internal.h"

void MATRIX(_gjelim)(T * _X, unsigned int _XR, unsigned int _XC);

void MATRIX(_inv)(T * _X, unsigned int _XR, unsigned int _XC)
{
    // ensure lengths are valid
    if (_XR != _XC ) {
        printf("error: matrix_inv(), invalid dimensions\n");
        exit(0);
    }

    printf("\n\nmatrix inversion on :\n");
    MATRIX(_print)(_X,_XR,_XC);

    // X:
    //  x11 x12 ... x1n
    //  x21 x22 ... x2n
    //  ...
    //  xn1 xn2 ... xnn

    // allocate temporary memory
    T x[2*_XR*_XC];
    unsigned int xr = _XR;
    unsigned int xc = _XC*2;

    // x:
    //  x11 x12 ... x1n 1   0   ... 0
    //  x21 x22 ... x2n 0   1   ... 0
    //  ...
    //  xn1 xn2 ... xnn 0   0   ... 1
    unsigned int r,c;
    for (r=0; r<_XR; r++) {
        // copy matrix elements
        for (c=0; c<_XC; c++)
            matrix_access(x,xr,xc,r,c) = matrix_access(_X,_XR,_XC,r,c);

        // append identity matrix
        for (c=0; c<_XC; c++)
            matrix_access(x,xr,xc,r,_XC+c) = (r==c) ? 1 : 0;
    }

    MATRIX(_print)(x,xr,xc);

    // perform Gauss-Jordan elimination on x
    // x:
    //  1   0   ... 0   y11 y12 ... y1n
    //  0   1   ... 0   y21 y22 ... y2n
    //  ...
    //  0   0   ... 1   yn1 yn2 ... ynn
    MATRIX(_gjelim)(x,xr,xc);

    // copy result from right half of x
    for (r=0; r<_XR; r++) {
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) = matrix_access(x,xr,xc,r,_XC+c);
    }
}

// Gauss-Jordan elmination
void MATRIX(_gjelim)(T * _X, unsigned int _XR, unsigned int _XC)
{
    unsigned int r, c, i;
    for (r=0; r<_XR; r++) {
        for (c=0; c<_XC; c++) {
        }
    }
}

