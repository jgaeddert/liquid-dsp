//
//
//

#include <stdio.h>

#include "liquid.h"

int main() {
    // initialize filter with 2nd-order low-pass butterworth filter
    float a[3] = { 
        1.000000000000000,
       -0.942809041582063,
        0.333333333333333};

    float b[3] = { 
        0.0976310729378175,
        0.1952621458756350,
        0.0976310729378175};

    iir_filter_rrrf f = iir_filter_rrrf_create(b,3,a,3);
    iir_filter_rrrf_print(f);

    unsigned int i;
    float y;
    for (i=0; i<15; i++) {
        if (i==0)   iir_filter_rrrf_execute(f, 1, &y);
        else        iir_filter_rrrf_execute(f, 0, &y);

        printf("%12.8f\n", y);
    }

    iir_filter_rrrf_destroy(f);

    return 0;
}

