// 
// equalizer_cccf.c
//  
// Generates four files:
//  * figures.gen/equalizer_cccf_const.gnu  : signal constellation
//  * figures.gen/equalizer_cccf_mse.gnu    : mean-square error
//  * figures.gen/equalizer_cccf_psd.gnu    : power spectral density
//  * figures.gen/equalizer_cccf_taps.gnu   : equalizer taps

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include <liquid/liquid.h>
#include "liquid.doc.h"

#define OUTPUT_FILENAME_CONST   "figures.gen/equalizer_cccf_const.gnu"
#define OUTPUT_FILENAME_MSE     "figures.gen/equalizer_cccf_mse.gnu"
#define OUTPUT_FILENAME_PSD     "figures.gen/equalizer_cccf_psd.gnu"
#define OUTPUT_FILENAME_TAPS    "figures.gen/equalizer_cccf_taps.gnu"

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

    // plotting options
    unsigned int nfft = 512;    // fft size

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
    fir_filter_cccf_print(f);

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
    float mse;
    float zeta=0.1; // smoothing factor (small zeta -> smooth MSE)
    for (i=0; i<n; i++) {
        // update LMS EQ and compute smoothed mean-squared error
        eqlms_cccf_execute(eqlms, y[i], d[i], &z_lms);
        mse = cabsf(d[i] - z_lms);
        mse_lms[i] = (i == 0) ? mse : mse_lms[i-1]*(1-zeta) + mse * zeta;

        // update RLS EQ and compute smoothed mean-squared error
        eqrls_cccf_execute(eqrls, y[i], d[i], &z_rls);
        mse = cabsf(d[i] - z_rls);
        mse_rls[i] = (i == 0) ? mse : mse_rls[i-1]*(1-zeta) + mse * zeta;
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
    fprintf(fid,"set xrange [-1.6:1.6];\n");
    fprintf(fid,"set yrange [-1.6:1.6];\n");
    fprintf(fid,"set xlabel 'I'\n");
    fprintf(fid,"set ylabel 'Q'\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 0.5\n");
    fprintf(fid,"plot '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'received',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'LMS EQ',\\\n",   LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'RLS EQ'\n",      LIQUID_DOC_COLOR_BLUE);
    // received
    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(y[i]), cimagf(y[i]));
    fprintf(fid,"e\n");

    // LMS
    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(d_hat_lms[i]), cimagf(d_hat_lms[i]));
    fprintf(fid,"e\n");

    // RLS
    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(d_hat_rls[i]), cimagf(d_hat_rls[i]));
    fprintf(fid,"e\n");

    fclose(fid);


    // 
    // mse : mean-squared error
    //
    fid = fopen(OUTPUT_FILENAME_MSE,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_MSE);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [0:%u];\n", n);
    fprintf(fid,"set yrange [1e-3:10];\n");
    fprintf(fid,"set log y\n");
    fprintf(fid,"set xlabel 'sample index'\n");
    fprintf(fid,"set ylabel 'mean-squared error'\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linewidth 2 linetype 1 linecolor rgb '%s' title 'LMS',\\\n", LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with lines linewidth 2 linetype 1 linecolor rgb '%s' title 'RLS'\n",    LIQUID_DOC_COLOR_BLUE);
    // LMS
    for (i=0; i<n; i++)
        fprintf(fid,"  %4u %16.8e\n", i, mse_lms[i]);
    fprintf(fid,"e\n");

    // RLS
    for (i=0; i<n; i++)
        fprintf(fid,"  %4u %16.8e\n", i, mse_rls[i]);
    fprintf(fid,"e\n");

    fclose(fid);



    // 
    // psd : power spectral density
    //
    float complex H[nfft];
    float complex H_LMS[nfft];
    float complex H_RLS[nfft];
    liquid_doc_compute_psdcf(h,h_len, H,     nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
    liquid_doc_compute_psdcf(w_lms,p, H_LMS, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
    liquid_doc_compute_psdcf(w_rls,p, H_RLS, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
    fft_shift(H,    nfft);
    fft_shift(H_LMS,nfft);
    fft_shift(H_RLS,nfft);
    float freq[nfft];
    for (i=0; i<nfft; i++)
        freq[i] = (float)(i) / (float)nfft - 0.5f;

    fid = fopen(OUTPUT_FILENAME_PSD,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_PSD);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-5:5]\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'received',\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'LMS',\\\n",     LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'RLS'\n",        LIQUID_DOC_COLOR_BLUE);
    // received signal
    for (i=0; i<nfft; i++)
        fprintf(fid,"%12.8f %12.4e\n", freq[i], 20*log10f(cabsf(H[i])) );
    fprintf(fid,"e\n");

    // LMS equalizer
    for (i=0; i<nfft; i++)
        fprintf(fid,"%12.8f %12.4e\n", freq[i], 20*log10f(cabsf(H_LMS[i])) + 20*log10f(H[i]));
    fprintf(fid,"e\n");

    // RLS equalizer
    for (i=0; i<nfft; i++)
        fprintf(fid,"%12.8f %12.4e\n", freq[i], 20*log10f(cabsf(H_RLS[i])) + 20*log10f(H[i]));
    fprintf(fid,"e\n");

    fclose(fid);


    // 
    // taps : equalizer taps
    //
    fid = fopen(OUTPUT_FILENAME_TAPS,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TAPS);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 0.5\n");
    fprintf(fid,"set xrange [-1:%u];\n", h_len > p ? h_len : p);
    fprintf(fid,"set yrange [-1.8:1.8]\n");
    fprintf(fid,"set xlabel 'filter index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 0.8\n");

    // real
    fprintf(fid,"set ylabel 'real'\n");
    fprintf(fid,"plot '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'channel',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'LMS',\\\n",     LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'RLS'\n",        LIQUID_DOC_COLOR_BLUE);
    // channel
    for (i=0; i<h_len; i++)
        fprintf(fid,"  %4u %12.4e\n", i, crealf(h[i]));
    fprintf(fid,"e\n");
    // LMS
    for (i=0; i<p; i++)
        fprintf(fid,"  %4u %12.4e\n", i, crealf(w_lms[i]));
    fprintf(fid,"e\n");
    // RLS
    for (i=0; i<p; i++)
        fprintf(fid,"  %4u %12.4e\n", i, crealf(w_rls[i]));
    fprintf(fid,"e\n");

    // imag
    fprintf(fid,"set ylabel 'imag'\n");
    fprintf(fid,"plot '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'channel',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'LMS',\\\n",     LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with points pointtype 12 linecolor rgb '%s' title 'RLS'\n",        LIQUID_DOC_COLOR_BLUE);
    // channel
    for (i=0; i<h_len; i++)
        fprintf(fid,"  %4u %12.4e\n", i, cimagf(h[i]));
    fprintf(fid,"e\n");
    // LMS
    for (i=0; i<p; i++)
        fprintf(fid,"  %4u %12.4e\n", i, cimagf(w_lms[i]));
    fprintf(fid,"e\n");
    // RLS
    for (i=0; i<p; i++)
        fprintf(fid,"  %4u %12.4e\n", i, cimagf(w_rls[i]));
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");

    fclose(fid);

    return 0;
}
