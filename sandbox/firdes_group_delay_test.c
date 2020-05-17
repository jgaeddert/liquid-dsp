// firpf_rrrf_example.c - demonstrate poly-phase filter-bank as interpolator
#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"
#define OUTPUT_FILENAME "firpfb_rrrf_example.m"

int main(int argc, char*argv[]) {
    // options
    unsigned int m  =     8; // filter delay (samples)
    float        fc = 0.15f; // filter cut-off frequency

    // derived values and buffers
    unsigned int h_len = 2*m+1;
    float        h[h_len];  // filter impulse response

    unsigned int i;
    float dt = -2.0f;
    printf("# fractional sample offset: specified vs. actual\n");
    printf("#\n");
    printf("# %12s %12s %12s\n", "specified", "actual", "error");
    while (dt <= 2.0f) {
        // generate filter coefficients
        for (i=0; i<h_len; i++) {
            float t = (float)i - (float)m - dt;
            h[i] = sincf(fc*t) * liquid_hamming(i,h_len);
        }

        // evaluate group delay at DC and compensate for nominal delay
        float dt_actual = fir_group_delay(h,h_len,0.0f) - (float)m;
        printf("  %12.9f %12.9f %12.9f\n", dt, dt_actual, dt-dt_actual);

        // 
        dt += 0.125f;
    }
    return 0;
}
