//
// fft_example.c
//
// This example demonstrates the interface to the fast discrete Fourier
// transform (FFT).
// SEE ALSO: mdct_example.c
//           fct_example.c
//

#include <stdio.h>
#include "liquid.h"

int main() {
    // options
    unsigned int n=16;  // input data size
    int method=0;       // fft method (ignored)

    // allocated memory arrays
    float complex x[n]; // time-domain 'signal'
    float complex y[n]; // fft(x)
    float complex z[n]; // ifft(y)

    // initialize input
    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = (float)i - _Complex_I*(float)i;

#if 0
    // run transforms computing plans internally
    fft_run(n, x, y, FFT_FORWARD, method);
    fft_run(n, y, z, FFT_REVERSE, method);
#else
    // create fft plans
    fftplan pf = fft_create_plan(n, x, y, FFT_FORWARD, method);
    fftplan pr = fft_create_plan(n, y, z, FFT_REVERSE, method);

    // execute fft plans
    fft_execute(pf);
    fft_execute(pr);

    // destroy fft plans
    fft_destroy_plan(pf);
    fft_destroy_plan(pr);
#endif

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

    printf("done.\n");
    return 0;
}

