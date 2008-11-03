//
// Generic dot product
//

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>

T X()(T *_x, T *_y, unsigned int _n)
{
    T r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _x[i] * _y[i];
    return r;
}

T X(4)(T *_x, T *_y, unsigned int _n)
{
    // BUG: need to ensure length of _n is a multiple of 4
    T r=0;
    unsigned int i;
    for (i=0; i<_n; i+=4) {
        r += _x[i]   * _y[i];
        r += _x[i+1] * _y[i+1];
        r += _x[i+2] * _y[i+2];
        r += _x[i+3] * _y[i+3];
    }
    return r;
}

