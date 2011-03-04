#include <liquid/liquid.h>
// ...
{
    // options
    unsigned int h_len = 21;

    float h[h_len];
    // initialize filter coeffiecients
    firfilt_rrrf f = firfilt_rrrf_create(h,h_len);

    float complex x;    // input sample
    float complex y;    // output sample
    
    {
        firfilt_rrrf_push(f, x);
        firfilt_rrrf_execute(f, &y); 
    }

    // destroy object
    firfilt_rrrf_destroy(f);
}
