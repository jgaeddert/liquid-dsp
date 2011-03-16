// file: doc/listings/decim.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int D = 4;         // decimation factor
    unsigned int h_len = 21;    // filter length
    float h[h_len];             // filter coefficients

    // create decimator object
    decim_crcf q = decim_crcf_create(D,h,h_len);

    float x[D];                 // input samples
    float y;                    // output sample

    // run decimator (repeat as necessary)
    {
        decim_crcf_execute(q, x, &y, 0);
    }

    // destroy decimator object
    decim_crcf_destroy(q);
}
