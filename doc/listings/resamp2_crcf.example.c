// file: doc/listings/resamp2_crcf.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int m = 7;         // filter semi-length
    float As=-60.0f;            // resampling filter stop-band attenuation

    // create half-band resampler
    resamp2_crcf q = resamp2_crcf_create(m,0.0f,As);

    float complex x;            // complex input
    float complex y[2];         // output buffer

    // ... initialize input ...
    {
        // execute half-band resampler as interpolator
        resamp2_crcf_interp_execute(q, x, y);
    }

    // ... repeat as necessary ...

    // clean up allocated objects
    resamp2_crcf_destroy(q);
}
