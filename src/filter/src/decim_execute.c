//
// Decimator: execute
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "decim_internal.h"

void decim_dotprod(float * _h, unsigned int _h_len, float *_x, float *_y)
{
    float s=0;
    unsigned int i;

    // compute dot product, ignoring every other tap
    for (i=0; i<_h_len; i+=2)
        s += _h[i]*_x[i];

    // compute output at center
    s += _h[(_h_len-1)/2]*_x[(_h_len-1)/2];

    // return result
    *_y = s;
}

float dotprod(float * _x, float * _y, unsigned int _n)
{
    unsigned int i;
    float s=0.0f;
    for (i=0; i<_n; i++)
        s += _x[i] * _y[i];
    return s;
}

// Decimate
void decim_execute(
    decim _d,
    float * _x,
    unsigned int _x_len,
    unsigned int * _nr,
    float * _y,
    unsigned int _y_len,
    unsigned int * _nw)
{
    // if halfband
    //   ...
    // else
    decim_execute_generic(_d, _x, _x_len, _nr, _y, _y_len, _nw);

}

// Decimate
void decim_execute_generic(
    decim _d,
    float * _x,
    unsigned int _x_len,
    unsigned int * _nr,
    float * _y,
    unsigned int _y_len,
    unsigned int * _nw)
{
    unsigned int nr=0, nw=0;
    float * r;
    fwindow b = _d->buffers[0];
    
    // consume entire input or entire output
    while ((_x_len >= _d->D) && (_y_len > 0)) {
        // 
        fwindow_write(b, &_x[nr], _d->D);
        fwindow_read(b, &r);

        // compute result
        _y[nw] = dotprod(_d->h, r, _d->h_len);

        // bookkeeping
        _x_len -= _d->D;
        nr += _d->D;
        _y_len--;
        nw++;
    }
    *_nr = nr;
    *_nw = nw;
}

