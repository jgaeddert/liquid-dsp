// 
// eqlms_cccf.c
//  
// Generates four files:
//  * figures.gen/eqlms_cccf_const.gnu  : signal constellation
//  * figures.gen/eqlms_cccf_mse.gnu    : mean-square error
//  * figures.gen/eqlms_cccf_psd.gnu    : power spectral density
//  * figures.gen/eqlms_cccf_taps.gnu   : equalizer taps

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>

#include <liquid/liquid.h>
#include "liquid.doc.h"

#define OUTPUT_FILENAME_CONST   "figures.gen/eqlms_cccf_const.gnu"
#define OUTPUT_FILENAME_MSE     "figures.gen/eqlms_cccf_mse.gnu"
#define OUTPUT_FILENAME_PSD     "figures.gen/eqlms_cccf_psd.gnu"
#define OUTPUT_FILENAME_TAPS    "figures.gen/eqlms_cccf_taps.gnu"

// print macro for complex numbers
//  F   :   output file
//  S   :   variable name (string)
//  I   :   index
//  V   :   value
#define PRINT_COMPLEX(F,S,I,V) fprintf(F,"%s(%4u) = %5.2f+j*%5.2f;",S,I,crealf(V),cimagf(V));

int main() {
    // options
    unsigned int n=512;     // number of symbols to observe
    unsigned int ntrain=256;// number of training symbols
    unsigned int h_len=4;   // channel filter length
    unsigned int p=8;       // equalizer order

    // bookkeeping variables
    float complex d[n];     // data sequence
    float complex y[n];     // received data sequence (filtered by channel)
    float complex d_hat[n]; // recovered data sequence
    float complex h[h_len]; // channel filter coefficients
    float complex w[p];     // equalizer filter coefficients
    float mse[n];
    unsigned int i;

    // create equalizer
    eqlms_cccf eq = eqlms_cccf_create(p);

    // create channel filter (random delay taps)
    h[0] = 1.0f;
    for (i=1; i<h_len; i++)
        h[i] = (randnf() + randnf()*_Complex_I) * 0.1f;
    fir_filter_cccf f = fir_filter_cccf_create(h,h_len);

    // generate random data signal
    for (i=0; i<n; i++)
        d[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;

    // filter data signal through channel
    for (i=0; i<n; i++) {
        fir_filter_cccf_push(f,d[i]);
        fir_filter_cccf_execute(f,&y[i]);
    }

    // run equalizer
    for (i=0; i<p; i++)
        w[i] = 0;
    float complex z;
    for (i=0; i<n; i++) {
        eqlms_cccf_execute(eq, y[i], d[i], &z);
        mse[i] = cabsf(d[i] - z);
    }
    eqlms_cccf_get_weights(eq, w);
    //eqlms_cccf_train(eq, w, y, d, ntrain);

    // create filter from equalizer output
    fir_filter_cccf feq = fir_filter_cccf_create(w,p);

    // run equalizer filter
    for (i=0; i<n; i++) {
        fir_filter_cccf_push(feq,y[i]);
        fir_filter_cccf_execute(feq,&d_hat[i]);
    }

    //
    // print results
    //
#if 0
    printf("channel:\n");
    for (i=0; i<h_len; i++) {
        PRINT_COMPLEX(stdout,"h",i+1,h[i]); printf("\n");
        PRINT_COMPLEX(fid,   "h",i+1,h[i]); fprintf(fid,"\n");
    }

    printf("equalizer:\n");
    for (i=0; i<p; i++) {
        PRINT_COMPLEX(stdout,"w",i+1,w[i]); printf("\n");
        PRINT_COMPLEX(fid,   "w",i+1,w[i]); fprintf(fid,"\n");
    }
#endif

#if 0
    float complex e;
    float mse=0.0f;
    for (i=0; i<n; i++) {
        if (i==ntrain)
            printf("----------\n");

        /*
        PRINT_COMPLEX(stdout,"d",   i+1,    d[i]);
        PRINT_COMPLEX(stdout,"y",   i+1,    y[i]);
        PRINT_COMPLEX(stdout,"dhat",i+1,    d_hat[i]);
        printf("\n");
        */

        PRINT_COMPLEX(fid,  "d",    i+1,    d[i]);
        PRINT_COMPLEX(fid,  "y",    i+1,    y[i]);
        PRINT_COMPLEX(fid,  "d_hat",i+1,    d_hat[i]);
        fprintf(fid, "\n");

        // compute mse
        e = d[i] - d_hat[i];
        mse += crealf(e*conj(e));
    }
    mse /= n;
    printf("mse: %12.8f\n", mse);
#endif

    // clean up allocated memory
    fir_filter_cccf_destroy(f);
    eqlms_cccf_destroy(eq);
    fir_filter_cccf_destroy(feq);

    // 
    // generate plots
    //
    FILE * fid = NULL;

    // 
    // const: constellation
    //
    fid = fopen(OUTPUT_FILENAME_CONST,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_CONST);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set samples 128\n");
    fprintf(fid,"plot [-10:10] sin(x),atan(x),cos(atan(x))\n");
    fclose(fid);


    // 
    // mse : mean-squared error
    //
    fid = fopen(OUTPUT_FILENAME_MSE,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_MSE);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set samples 128\n");
    fprintf(fid,"plot [-10:10] sin(x),atan(x),cos(atan(x))\n");
    fclose(fid);


    // 
    // psd : power spectral density
    //
    fid = fopen(OUTPUT_FILENAME_PSD,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_PSD);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set samples 128\n");
    fprintf(fid,"plot [-10:10] sin(x),atan(x),cos(atan(x))\n");
    fclose(fid);


    // 
    // taps : equalizer taps
    //
    fid = fopen(OUTPUT_FILENAME_TAPS,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TAPS);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set samples 128\n");
    fprintf(fid,"plot [-10:10] sin(x),atan(x),cos(atan(x))\n");
    fclose(fid);

    return 0;
}
