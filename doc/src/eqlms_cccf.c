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
#include <math.h>
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
    unsigned int h_len=6;   // channel filter length
    unsigned int p=10;      // equalizer order
    float mu=0.500f;        // LMS learning rate
    float lambda=0.999f;    // RLS learning rate
    float SNRdB = 65.0f;    // signal-to-noise ratio

    // bookkeeping variables
    float complex d[n];     // data sequence
    float complex h[h_len]; // channel filter coefficients
    float complex y[n];     // received data sequence (filtered by channel)

    // LMS
    float complex d_hat_lms[n]; // recovered data sequence
    float complex w_lms[p];     // equalizer filter coefficients
    float mse_lms[n];           // equalizer mean-squared error

    // RLS
    float complex d_hat_rls[n]; // recovered data sequence
    float complex w_rls[p];     // equalizer filter coefficients
    float mse_rls[n];           // equalizer mean-squared error
    unsigned int i;

    // create LMS equalizer
    eqlms_cccf eqlms = eqlms_cccf_create(p);
    eqlms_cccf_set_bw(eqlms, mu);

    // create RLS equalizer
    eqrls_cccf eqrls = eqrls_cccf_create(p);
    eqrls_cccf_set_bw(eqrls, lambda);

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
    float complex noise;
    float gamma = powf(10.0f, -SNRdB/10.0f); // * sqrtf(2.0f);
    //float gamma = 0.0f;
    for (i=0; i<n; i++) {
        fir_filter_cccf_push(f,d[i]);
        fir_filter_cccf_execute(f,&y[i]);

        // add noise
        crandnf(&noise);
        y[i] += noise*gamma;
    }

    // intialize equalizer coefficients
    for (i=0; i<p; i++) {
        w_lms[i] = 0;
        w_rls[i] = w_lms[i];
    }

    // train equalizers
    float complex z_lms, z_rls;
    for (i=0; i<n; i++) {
        eqlms_cccf_execute(eqlms, y[i], d[i], &z_lms);
        mse_lms[i] = cabsf(d[i] - z_lms);

        eqrls_cccf_execute(eqrls, y[i], d[i], &z_rls);
        mse_rls[i] = cabsf(d[i] - z_rls);
    }

    // create filter from equalizer output
    eqlms_cccf_get_weights(eqlms, w_lms);
    fir_filter_cccf feqlms = fir_filter_cccf_create(w_lms,p);

    // create filter from equalizer output
    eqrls_cccf_get_weights(eqrls, w_rls);
    fir_filter_cccf feqrls = fir_filter_cccf_create(w_rls,p);

    // run equalizer filter
    for (i=0; i<n; i++) {
        fir_filter_cccf_push(feqlms,y[i]);
        fir_filter_cccf_execute(feqlms,&d_hat_lms[i]);

        fir_filter_cccf_push(feqrls,y[i]);
        fir_filter_cccf_execute(feqrls,&d_hat_rls[i]);
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
    eqlms_cccf_destroy(eqlms);
    eqrls_cccf_destroy(eqrls);
    fir_filter_cccf_destroy(feqlms);
    fir_filter_cccf_destroy(feqrls);

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
