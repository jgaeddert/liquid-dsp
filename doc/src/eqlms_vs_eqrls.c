// 
// eqlms_vs_eqrls.c
//  
// Generates four files:
//  * filename_const.gnu    : signal constellation
//  * filename_mse.gnu      : mean-square error
//  * filename_freq.gnu     : power spectral density
//  * filename_taps.gnu     : equalizer taps

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("equalizer_cccf [options]\n");
    printf("  u/h   : print usage\n");
    printf("  f     : file base name (required)\n");
    printf("  g     : specify gnuplot version <4.2>\n");
    printf("  n     : number of symbols <512>\n");
    printf("  c     : channel filter length <6>\n");
    printf("  p     : equalizer filter length <10>\n");
    printf("  m     : LMS equalizer learning rate <0.5>\n");
    printf("  l     : RLS equalizer learning rate <0.999>\n");
    printf("  s     : signal-to-noise ratio [db] <40>\n");
    printf("  t     : channel phase offset <0>\n");
}


int main(int argc, char*argv[])
{
    // options
    unsigned int n=512;         // number of symbols to observe
    unsigned int h_len=6;       // channel filter length
    unsigned int p=10;          // equalizer order
    float mu=0.500f;            // LMS learning rate
    float lambda=0.999f;        // RLS learning rate
    float SNRdB = 40.0f;        // signal-to-noise ratio
    float theta = 0.0f;         // channel phase offset

    // plotting options
    unsigned int nfft = 512;    // fft size
    float gnuplot_version = 4.2;
    char filename_base[256] = "figures.gen/eqlms_vs_eqrls";

    int dopt;
    while ((dopt = getopt(argc,argv,"uhf:g:n:c:p:m:l:s:t:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                              return 0;
        case 'f': strncpy(filename_base,optarg,256);    break;
        case 'g': gnuplot_version = atoi(optarg);       break;
        case 'n': n = atoi(optarg);                     break;
        case 'c': h_len = atoi(optarg);                 break;
        case 'p': p = atoi(optarg);                     break;
        case 'm': mu = atof(optarg);                    break;
        case 'l': lambda = atof(optarg);                break;
        case 's': SNRdB = atof(optarg);                 break;
        case 't': theta = atof(optarg);                 break;
        default:
            exit(1);
        }
    }

    // set 'random' seed on options
    srand( h_len + p + nfft );

    if (strcmp(filename_base,"")==0) {
        fprintf(stderr,"error: %s, invalid or unspecified file base name\n", argv[0]);
        usage();
        return 1;
    }

    // bookkeeping variables
    float complex d[n];     // data sequence
    float complex h[h_len]; // channel filter coefficients
    float complex y[n];     // received data sequence (filtered by channel)

    // least mean-squares (LMS) equalizer
    float complex d_hat_lms[n]; // recovered data sequence
    float complex w_lms[p];     // equalizer filter coefficients
    float mse_lms[n];           // equalizer mean-squared error

    // recursive least-squares (RLS) equalizer
    float complex d_hat_rls[n]; // recovered data sequence
    float complex w_rls[p];     // equalizer filter coefficients
    float mse_rls[n];           // equalizer mean-squared error

    unsigned int i;

    // create LMS equalizer
    eqlms_cccf eqlms = eqlms_cccf_create(NULL,p);
    eqlms_cccf_set_bw(eqlms, mu);

    // create RLS equalizer
    eqrls_cccf eqrls = eqrls_cccf_create(NULL,p);
    eqrls_cccf_set_bw(eqrls, lambda);

    // create channel filter (random delay taps)
    h[0] = 1.0f;
    for (i=1; i<h_len; i++)
        h[i] = (randnf() + randnf()*_Complex_I) * 0.1f;
    // apply channel phase offset
    for (i=0; i<h_len; i++)
        h[i] *= cexpf(_Complex_I*theta);
    firfilt_cccf f = firfilt_cccf_create(h,h_len);

    // generate random data signal
    for (i=0; i<n; i++)
        d[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;

    // filter data signal through channel
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<n; i++) {
        firfilt_cccf_push(f,d[i]);
        firfilt_cccf_execute(f,&y[i]);

        // add noise
        y[i] += nstd * (randnf() + randnf()*_Complex_I) * M_SQRT1_2;
    }

    // intialize equalizer coefficients
    for (i=0; i<p; i++) {
        w_lms[i] = (i==0) ? 1.0f : 0.0f;
        w_rls[i] = (i==0) ? 1.0f : 0.0f;
    }

    // train equalizers
    float complex z_lms, z_rls;
    float mse;
    float zeta=0.1f; // smoothing factor (small zeta -> smooth MSE)
    for (i=0; i<n; i++) {
        // update LMS EQ and compute smoothed mean-squared error
        eqlms_cccf_push(eqlms, y[i]);
        eqlms_cccf_execute(eqlms, &z_lms);
        eqlms_cccf_step(eqlms, d[i], z_lms);
        mse = cabsf(d[i] - z_lms);
        mse_lms[i] = (i == 0) ? mse : mse_lms[i-1]*(1-zeta) + mse * zeta;

        // update RLS EQ and compute smoothed mean-squared error
        eqrls_cccf_push(eqrls, y[i]);
        eqrls_cccf_execute(eqrls, &z_rls);
        eqrls_cccf_step(eqrls, d[i], z_rls);
        mse = cabsf(d[i] - z_rls);
        mse_rls[i] = (i == 0) ? mse : mse_rls[i-1]*(1-zeta) + mse * zeta;
    }

    // create filter from equalizer output
    eqlms_cccf_get_weights(eqlms, w_lms);
    firfilt_cccf feqlms = firfilt_cccf_create(w_lms,p);

    // create filter from equalizer output
    eqrls_cccf_get_weights(eqrls, w_rls);
    firfilt_cccf feqrls = firfilt_cccf_create(w_rls,p);

    // run equalizer filter
    for (i=0; i<n; i++) {
        firfilt_cccf_push(feqlms,y[i]);
        firfilt_cccf_execute(feqlms,&d_hat_lms[i]);

        firfilt_cccf_push(feqrls,y[i]);
        firfilt_cccf_execute(feqrls,&d_hat_rls[i]);
    }

    // clean up allocated memory
    firfilt_cccf_destroy(f);
    eqlms_cccf_destroy(eqlms);
    eqrls_cccf_destroy(eqrls);
    firfilt_cccf_destroy(feqlms);
    firfilt_cccf_destroy(feqrls);

    // 
    // generate plots
    //
    FILE * fid = NULL;
    char filename[300];

    // 
    // const: constellation
    //
    strncpy(filename, filename_base, 256);
    strcat(filename, "_const.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [-2:2];\n");
    fprintf(fid,"set yrange [-2:2];\n");
    fprintf(fid,"set xlabel 'In-phase'\n");
    fprintf(fid,"set ylabel 'Quadrature phase'\n");
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
    strncpy(filename, filename_base, 256);
    strcat(filename, "_mse.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [0:%u];\n", n);
    fprintf(fid,"set yrange [1e-3:10];\n");
    fprintf(fid,"set log y\n");
    fprintf(fid,"set format y '10^{%%L}'\n");
    fprintf(fid,"set xlabel 'Sample index'\n");
    fprintf(fid,"set ylabel 'Mean-squared error'\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linewidth 4 linetype 1 linecolor rgb '%s' title 'LMS',\\\n", LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with lines linewidth 4 linetype 1 linecolor rgb '%s' title 'RLS'\n",    LIQUID_DOC_COLOR_BLUE);
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

    strncpy(filename, filename_base, 256);
    strcat(filename, "_freq.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-10:10]\n");
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
    strncpy(filename, filename_base, 256);
    strcat(filename, "_taps.gnu");
    fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing\n", argv[0], filename);
        return 1;
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set size ratio 0.5\n");
    fprintf(fid,"set xrange [-1:%u];\n", h_len > p ? h_len : p);
    fprintf(fid,"set yrange [-1.2:1.2]\n");
    fprintf(fid,"set xlabel 'Filter index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 0.8\n");

    // real
    fprintf(fid,"set ylabel 'Real'\n");
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
    fprintf(fid,"set ylabel 'Imag'\n");
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
