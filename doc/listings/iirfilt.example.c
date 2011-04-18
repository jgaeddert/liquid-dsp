// file: doc/listings/iirfilt.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int order=4;   // filter order

    unsigned int n = order+1;
    float b[n], a[n];

    // ... initialize filter coefficients ...

    // create filter object
    iirfilt_crcf q = iirfilt_crcf_create(b,n,a,n);

    float complex x;    // input sample
    float complex y;    // output sample
    
    // execute filter (repeat as necessary)
    iirfilt_crcf_execute(q,x,&y);

    // destroy filter object
    iirfilt_crcf_destroy(q);
}
