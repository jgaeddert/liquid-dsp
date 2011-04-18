// file: doc/listings/resamp_crcf.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int h_len = 13;    // filter semi-length (filter delay)
    float r=0.9f;               // resampling rate (output/input)
    float bw=0.5f;              // resampling filter bandwidth
    float slsl=-60.0f;          // resampling filter sidelobe suppression level
    unsigned int npfb=32;       // number of filters in bank (timing resolution)

    // create resampler
    resamp_crcf q = resamp_crcf_create(r,h_len,bw,slsl,npfb);

    unsigned int n = (unsigned int)ceilf(r);
    float complex x;            // complex input
    float complex y[n];         // output buffer
    unsigned int num_written;   // number of values written to buffer

    // ... initialize input ...

    // execute resampler, storing result in output buffer
    resamp_crcf_execute(q, x, y, &num_written);

    // ... repeat as necessary ...

    // clean up allocated objects
    resamp_crcf_destroy(q);
}
