// file: doc/listings/firdecim.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int M = 4;         // decimation factor
    unsigned int h_len = 21;    // filter length

    // design filter and create decimator object
    float h[h_len];             // filter coefficients
    firdecim_crcf q = firdecim_crcf_create(M,h,h_len);

    // generate input signal and decimate
    float complex x[M];         // input samples
    float complex y;            // output sample

    // run decimator (repeat as necessary)
    {
        firdecim_crcf_execute(q, x, &y, 0);
    }

    // destroy decimator object
    firdecim_crcf_destroy(q);
}
