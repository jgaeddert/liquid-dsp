// file: doc/listings/fft_dct.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int n=16;      // input data size
    int type = FFT_REDFT01; // DCT-III
    int flags=0;            // FFT flags (typically ignored)

    // allocated memory arrays
    float * x = (float*) malloc(n * sizeof(float));
    float * y = (float*) malloc(n * sizeof(float));

    // create FFT plan
    fftplan q = fft_create_plan_r2r_1d(n, x, y, type, flags);

    // ... initialize input ...

    // execute FFT (repeat as necessary)
    fft_execute(q);

    // destroy FFT plan and free memory arrays
    fft_destroy_plan(q);
    free(x);
    free(y);
}
