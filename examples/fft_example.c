const char __docstr__[] =
"This example demonstrates the interface to the fast discrete Fourier"
" transform (FFT).";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

// print usage/help message
void usage()
{
    printf("fft_example [options]\n");
    printf("  h     : print help\n");
    printf("  v/q   : verbose/quiet\n");
    printf("  n     : fft size, default: 16\n");
}

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(unsigned, nfft,    16, 'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, method,   0, 'm', "FFT method (ignored)", NULL);
    liquid_argparse_add(bool,     verbose,  0, 'v', "enable verbose output", NULL);
    liquid_argparse_parse(argc,argv);

    // allocate memory arrays
    liquid_float_complex * x = (liquid_float_complex*) fft_malloc(nfft*sizeof(liquid_float_complex));
    liquid_float_complex * y = (liquid_float_complex*) fft_malloc(nfft*sizeof(liquid_float_complex));
    liquid_float_complex * z = (liquid_float_complex*) fft_malloc(nfft*sizeof(liquid_float_complex));

    // initialize input
    unsigned int i;
    for (i=0; i<nfft; i++)
        x[i] = (float)i - _Complex_I*(float)i;

    // create fft plans
    fftplan pf = fft_create_plan(nfft, x, y, LIQUID_FFT_FORWARD,  method);
    fftplan pr = fft_create_plan(nfft, y, z, LIQUID_FFT_BACKWARD, method);

    // print fft plans
    fft_print_plan(pf);
    //fft_print_plan(pr);

    // execute fft plans
    fft_execute(pf);
    fft_execute(pr);

    // destroy fft plans
    fft_destroy_plan(pf);
    fft_destroy_plan(pr);

    // normalize inverse
    for (i=0; i<nfft; i++)
        z[i] /= (float) nfft;

    if (verbose) {
        // print results
        printf("original signal, x[n]:\n");
        for (i=0; i<nfft; i++)
            printf("  x[%3u] = %8.4f + j%8.4f\n", i, crealf(x[i]), cimagf(x[i]));
        printf("y[n] = fft( x[n] ):\n");
        for (i=0; i<nfft; i++)
            printf("  y[%3u] = %8.4f + j%8.4f\n", i, crealf(y[i]), cimagf(y[i]));
        printf("z[n] = ifft( y[n] ):\n");
        for (i=0; i<nfft; i++)
            printf("  z[%3u] = %8.4f + j%8.4f\n", i, crealf(z[i]), cimagf(z[i]));
    }

    // compute RMSE between original and result
    float rmse = 0.0f;
    for (i=0; i<nfft; i++) {
        liquid_float_complex d = x[i] - z[i];
        rmse += crealf(d * conjf(d));
    }
    rmse = sqrtf( rmse / (float)nfft );
    printf("rmse = %12.4e\n", rmse);

    // free allocated memory
    fft_free(x);
    fft_free(y);
    fft_free(z);

    printf("done.\n");
    return 0;
}

