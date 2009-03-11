// file: eqrls_rrrf_example.c
//
// Tests simple modulation/demodulation without noise or phase
// offset, counting the number of resulting symbol errors.
//
// Complile and run:
//   $ gcc eqrls_rrrf_example.c -lliquid -o eqrls_rrrf_example
//   $ ./eqrls_rrrf_example

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

int main() {
    unsigned int n=16;
    unsigned int p=6;
    eqrls_rrrf eq = eqrls_rrrf_create(p);

    eqrls_rrrf_print(eq);

    unsigned int h_len = 4;
    float h[4] = {1.0f, -0.1f, 0.2f, 0.05f};
    fir_filter_rrrf f = fir_filter_rrrf_create(h,h_len);

    float d[n];
    float y[n];

    unsigned int i;
    for (i=0; i<n; i++) {
        d[i] = rand() % 2 ? 1.0f : -1.0f;
        //d[i] = (i%2) ? 1.0f : -1.0f;
        fir_filter_rrrf_push(f,d[i]);
        fir_filter_rrrf_execute(f,&y[i]);
    }

    float w[n];
    memset(w, 0x00, p*sizeof(float));
    eqrls_rrrf_train(eq, w, y, d, n);

    for (i=0; i<p; i++)
        printf("w[%3u] = %8.4f\n", i, w[i]);

    // create filter
    fir_filter_rrrf feq = fir_filter_rrrf_create(w,p);

    // run equalizer filter
    float d_hat[n];
    float mse=0.0f;
    for (i=0; i<n; i++) {
        fir_filter_rrrf_push(feq,y[i]);
        fir_filter_rrrf_execute(feq,&d_hat[i]);

        mse += powf(d[i]-d_hat[i], 2.0f);
    }
    mse /= n;

    // print results
    for (i=0; i<n; i++) {
        printf("d(%3u) = %8.4f; ", i, d[i]);
        printf("y(%3u) = %8.4f; ", i, y[i]);
        printf("d_hat(%3u) = %8.4f; ", i, d_hat[i]);
        printf("\n");
    }
    printf("mse: %12.8f\n", mse);

    fir_filter_rrrf_destroy(f);
    eqrls_rrrf_destroy(eq);
    return 0;
}
