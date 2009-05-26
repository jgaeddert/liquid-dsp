//
// Matrix inverse method definitions
//

#include "liquid.internal.h"

void MATRIX(_inv)(T * _X, unsigned int _XR, unsigned int _XC)
{
    // ensure lengths are valid
    if (_XR != _XC ) {
        printf("error: matrix_inv(), invalid dimensions\n");
        exit(0);
    }

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
    unsigned int r, c;

    // TODO: choose pivot rows carefully
    for (r=0; r<_XR; r++)
        MATRIX(_pivot)(_X,_XR,_XC,r,r);

    // scale by diagonal
    T g;
    for (r=0; r<_XR; r++) {
        g = 1 / matrix_access(_X,_XR,_XC,r,r);
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) *= g;
    }
}

// pivot on element _r, _c
void MATRIX(_pivot)(T * _X, unsigned int _XR, unsigned int _XC, unsigned int _r, unsigned int _c)
{
    T v = matrix_access(_X,_XR,_XC,_r,_c);
    if (v==0) {
        printf("warning: pivoting on zero\n");
        return;
    }
    unsigned int r,c;

    // pivot using back-substitution
    T g;    // multiplier
    for (r=0; r<_XR; r++) {

        // skip over pivot row
        if (r == _r)
            continue;

        // compute multiplier
        g = matrix_access(_X,_XR,_XC,r,_c) / v;

        // back-substitution
        for (c=0; c<_XC; c++) {
            matrix_access(_X,_XR,_XC,r,c) = g*matrix_access(_X,_XR,_XC,_r,c) -
                                              matrix_access(_X,_XR,_XC, r,c);
        }
    }
}

void MATRIX(_swaprows)(T * _X, unsigned int _XR, unsigned int _XC, unsigned int _r1, unsigned int _r2)
{
    unsigned int c;
    T v_tmp;
    for (c=0; c<_XC; c++) {
        v_tmp = matrix_access(_X,_XR,_XC,_r1,c);
        matrix_access(_X,_XR,_XC,_r1,c) = matrix_access(_X,_XR,_XC,_r2,c);
        matrix_access(_X,_XR,_XC,_r2,c) = v_tmp;
    }
}
