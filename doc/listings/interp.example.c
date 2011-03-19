// file: doc/listings/interp.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int M=4;       // interpolation factor
    unsigned int h_len;     // interpolation filter length

    // design filter and create interpolator
    float h[h_len];         // filter coefficients
    interp_crcf q = interp_crcf_create(M,h,h_len);

    // generate input signal and interpolate
    float complex x;        // input sample
    float complex y[M];     // output samples

    // run interpolator (repeat as necessary)
    {
        interp_crcf_execute(q, x, y);
    }

    // destroy the interpolator object
    interp_crcf_destroy(q);
}
