// file: doc/listings/agc.example.c
#include <liquid/liquid.h>

int main() {
    agc_rrrf q = agc_rrrf_create();     // create object
    agc_rrrf_set_bandwidth(q,1e-3f);    // set loop filter bandwidth

    float x;                            // input sample
    float y;                            // output sample

    // ...

    agc_rrrf_execute(q, x, &y);         // repeat as necessary

    agc_rrrf_destroy(q);                // clean it up
}
