//
// fft : inefficient but effective
//

#include <math.h>
#include "fft_internal.h"

void fft_execute(fftplan _p)
{
    unsigned int i, j;
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->y[i] = 0.0f;
        for (j=0; j<_p->n; j++) {
            phi = 2*M_PI*i*j / (float) (_p->n);
            if (_p->direction == FFT_FORWARD)
                _p->y[i] += _p->x[j] * cexpf(_Complex_I*phi);
            else
                _p->y[i] += _p->x[j] * cexpf(-_Complex_I*phi);
        }
    }
}

