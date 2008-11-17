//
// fft_shift
//

//#include <math.h>
#include "fft_internal.h"

void fft_shift(float complex *_x, unsigned int _n)
{
    unsigned int i, n2;
    if (_n%2)
        n2 = (_n-1)/2;
    else
        n2 = _n/2;

    float complex tmp;
    for (i=0; i<n2; i++) {
        tmp = _x[i];
        _x[i] = _x[i+n2];
        _x[i+n2] = tmp;
    }
}
