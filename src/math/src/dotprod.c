//
// Generic dot product
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dotprod.h"

float dotprod_fff(float *_x, float *_y, unsigned int _n)
{
    float r=0.0f;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _x[i] * _y[i];
    return r;
}

float dotprod4_fff(float *_x, float *_y, unsigned int _n)
{
    // BUG: need to ensure length of _n is a multiple of 4
    float r=0.0f;
    unsigned int i;
    for (i=0; i<_n; i+=4) {
        r += _x[i]   * _y[i];
        r += _x[i+1] * _y[i+1];
        r += _x[i+2] * _y[i+2];
        r += _x[i+3] * _y[i+3];
    }
    return r;
}

