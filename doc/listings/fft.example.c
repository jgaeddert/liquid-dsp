// file: doc/listings/fft.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int n=16;  // input data size
    int flags=0;        // fft flags (typically ignored)

    // allocated memory arrays
    float complex * x = (float complex*) malloc(n * sizeof(float complex));
    float complex * y = (float complex*) malloc(n * sizeof(float complex));

    // create fft plan
    fftplan q = fft_create_plan(n, x, y, FFT_FORWARD, flags);

    // ... initialize input ...

    // execute fft (repeat as necessary)
    fft_execute(q);

    // destroy fft plan and free memory arrays
    fft_destroy_plan(q);
    free(x);
    free(y);
}
