// file: doc/listings/firfilt.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int h_len=21;  // filter order
    float h[h_len];         // filter coefficients

    // ... initialize filter coefficients ...

    // create filter object
    firfilt_crcf q = firfilt_crcf_create(h,h_len);

    float complex x;    // input sample
    float complex y;    // output sample
    
    // execute filter (repeat as necessary)
    {
        firfilt_crcf_push(q, x);    // push input sample
        firfilt_crcf_execute(q,&y); // compute output
    }

    // destroy filter object
    firfilt_crcf_destroy(q);
}
