// file: doc/listings/cvsd.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int nbits=3;       // number of adjacent bits to observe
    float zeta=1.5f;            // slope adjustment multiplier
    float alpha = 0.95;         // pre-/post-filter coefficient

    // create cvsd encoder/decoder
    cvsd q = cvsd_create(nbits, zeta, alpha);

    float x;                    // input sample
    unsigned char b;            // encoded bit
    float y;                    // output sample

    // ...

    // repeat as necessary
    {
        b = cvsd_encode(q, x);  // encode sample

        y = cvsd_decode(q, b);  // decode sample
    }

    cvsd_destroy(q);            // destroy cvsd object
}
