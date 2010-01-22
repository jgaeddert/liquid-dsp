#include <liquid/liquid.h>
// ...
{
    unsigned int m=5;           // filter semi-length
    unsigned int h_len = 4*m+1; // filter length
    float slsl=60.0f;           // filter sidelobe suppression level

    // create Hilbert transform object
    firhilb f = firhilb_create(h_len,slsl);

    float complex x;    // interpolator input
    float y[2];         // interpolator output

    // ...

    // execute transform (interpolator) to compute real signal
    firhilb_interp_execute(f, x, y);

    // clean up allocated memory
    firhilb_destroy(f);
}
