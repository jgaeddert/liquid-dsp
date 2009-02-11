//
//
//

#include <stdio.h>
#include "liquid.h"

int main() {
    unsigned int n=16;
    float complex x[n];
    float complex y[n];
    float complex z[n];

    unsigned int i;
    for (i=0; i<n; i++)
        //x[i] = (float)i;
        x[i] = (float)i - _Complex_I*(float)i;

    fftplan pf = fft_create_plan(n, x, y, FFT_FORWARD);
    fftplan pr = fft_create_plan(n, y, z, FFT_REVERSE);

    // execute ffts
    fft_execute(pf);
    fft_execute(pr);

    // normalize inverse
    for (i=0; i<n; i++)
        z[i] /= (float) n;

    // print results
    for (i=0; i<n; i++)
        printf("y[%u] =\t%8.2f +\tj%8.2f\n", i, crealf(y[i]), cimagf(y[i]));
    printf("==========\n");
    for (i=0; i<n; i++)
        printf("z[%u] =\t%8.2f +\tj%8.2f\n", i, crealf(z[i]), cimagf(z[i]));

    fft_destroy_plan(pf);
    fft_destroy_plan(pr);

    printf("done.\n");
    return 0;
}

