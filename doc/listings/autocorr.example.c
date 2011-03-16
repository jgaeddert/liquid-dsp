// file: doc/listings/autocorr.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int n = 60;        // autocorr window length
    unsigned int delay = 20;    // autocorr overlap delay

    // create autocorrelator object
    autocorr_cccf q = autocorr_cccf_create(n,delay);

    float complex x;            // input sample
    float complex rxx;          // output auto-correlation

    // compute auto-correlation (repeat as necessary)
    {
        autocorr_cccf_push(q, x);
        autocorr_cccf_execute(q, &rxx);
    }

    // destroy autocorrelator object
    autocorr_cccf_destroy(q);
}
