#include <liquid/liquid.h>
// ...
{
    agc p = agc_create();       // create object
    agc_set_target(p,1.0f);     // set target energy level
    agc_set_bandwidth(p,1e-3f); // set loop filter bandwidth

    float x;                    // input
    float y;                    // output

    // ...

    agc_execute(p, x, &y);      // repeat as necessary

    agc_destroy(p);             // clean it up
}
