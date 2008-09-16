//
// Half-band decimator
//

#include <math.h>
#include <stdlib.h>

#include "decim2.h"
#include "firdes.h"

// additional function prototypes
void decim2_dotprod(float * _h, unsigned int _h_len, float *_x, float *_y);

struct decim2_s {
    float * h;
    unsigned int h_len;

    float fc;
    float b;
};

// Create half-band decimator object
//   n  : filter length
//   fc : carrier frequency (-1 < fc < 1)
//   b  : bandwidth (0 < b < 0.5)
decim2 decim2_create(unsigned int _n, float _fc, float _b)
{
    decim2 d = (decim2) malloc(sizeof(struct decim2_s));
    d->h = (float*) malloc(_n*sizeof(float));
    d->h_len = _n;

    // use windowed half-band sinc fir filter design
    fir_design_halfband_windowed_sinc(d->h, d->h_len);

    d->fc = _fc;
    d->b = _b;

    return d;
}

// Destroy half-band decimator object
void decim2_destroy(decim2 _d)
{
    free(_d->h);
    free(_d);
}

// Execute
void decim2_execute(decim2 _d, float * _x, unsigned int _x_len, float * _y, unsigned int _y_len)
{
    unsigned int i, n=0;
    for (i=0; i<(_x_len-_d->h_len); i+=2) {
        decim2_dotprod(_d->h, _d->h_len, &_x[i], &_y[n]);
        n++;
    }
}

void decim2_dotprod(float * _h, unsigned int _h_len, float *_x, float *_y)
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

