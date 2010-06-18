#include <liquid/liquid.h>
// ...
{
    // options
    unsigned int n=16;  // input data size
    int flags=0;        // fft flags

    // allocated memory arrays
    float complex * x = (float complex*) malloc(n * sizeof(float complex));
    float complex * y = (float complex*) malloc(n * sizeof(float complex));

    // ... initialize input ...

    // create fft plan
    fftplan q = fft_create_plan(n, x, y, FFT_FORWARD, flags);

    // execute fft (repeat as necessary)
    fft_execute(q);

    // destroy fft plan and free memory arrays
    fft_destroy_plan(q);
    free(x);
    free(y);
}
