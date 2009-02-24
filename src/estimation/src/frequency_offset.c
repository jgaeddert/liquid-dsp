//
// Estimate frequency offset
//

#include <math.h>
#include <complex.h>

float estimate_freqoffset(float complex * _x, unsigned int _n)
{
    float complex sum=0.0f;
    unsigned int i;
    for (i=0; i<_n-1; i++) {
        sum += _x[i+1] * conj(_x[i]);
    }
    return cargf(sum);
}

