// file: doc/listings/firhilb.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int m=5;           // filter semi-length
    float slsl=60.0f;           // filter sidelobe suppression level

    // create Hilbert transform objects
    firhilbf q0 = firhilbf_create(m,slsl);
    firhilbf q1 = firhilbf_create(m,slsl);

    float complex x;    // interpolator input
    float y[2];         // interpolator output
    float complex z;    // decimator output

    // ...

    // execute transforms
    firhilbf_interp_execute(q0, x, y);   // interpolator
    firhilbf_decim_execute(q1, y, &z);   // decimator

    // clean up allocated memory
    firhilbf_destroy(q0);
    firhilbf_destroy(q1);
}
