//
//
//

#include <stdio.h>
#include "liquid.h"

int main() {
    unsigned int n=16;
    float complex x[n]; // time-domain 'signal'
    float complex y[n]; // fft(x)
    float complex z[n]; // ifft(y)

    unsigned int i;
    for (i=0; i<n; i++)
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
    printf("original signal, x[n]:\n");
    for (i=0; i<n; i++)
        printf("  x[%3u] = %8.4f + j%8.4f\n", i, crealf(x[i]), cimagf(x[i]));
    printf("y[n] = fft( x[n] ):\n");
    for (i=0; i<n; i++)
        printf("  y[%3u] = %8.4f + j%8.4f\n", i, crealf(y[i]), cimagf(y[i]));
    printf("z[n] = ifft( y[n] ):\n");
    for (i=0; i<n; i++)
        printf("  z[%3u] = %8.4f + j%8.4f\n", i, crealf(z[i]), cimagf(z[i]));

    fft_destroy_plan(pf);
    fft_destroy_plan(pr);

    printf("done.\n");
    return 0;
}

