//
// math_window.c : demonstrate windowing functions
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <liquid/liquid.h>

#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/math_window.gnu"

int main() {
    // options
    unsigned int n=51;      // window size
    unsigned int nfft=1024; // number of fft points

    float beta0 =  3.0f;
    float beta1 = 10.0f;

    // allocate memory for windows
    float w_kaiser_b0[n];
    float w_hamming[n];
    float w_hann[n];
    float w_kaiser_b1[n];
    float w_blackmanharris[n];

    // allocate memory for power spectral density arrays
    float complex W_kaiser_b0[nfft];
    float complex W_hamming[nfft];
    float complex W_hann[nfft];
    float complex W_kaiser_b1[nfft];
    float complex W_blackmanharris[nfft];

    // allocate memory for colors
    char color_kaiser_b0[]      = "#400040";
    char color_hamming[]        = "#004080";
    char color_hann[]           = "#008040";
    char color_kaiser_b1[]      = "#404000";
    char color_blackmanharris[] = "#800000";

    // generate windows
    unsigned int i;
    for (i=0; i<n; i++) {
        w_kaiser_b0[i]      = kaiser(i,n,beta0,0);
        w_hamming[i]        = hamming(i,n);
        w_hann[i]           = hann(i,n);
        w_kaiser_b1[i]      = kaiser(i,n,beta1,0);
        w_blackmanharris[i] = blackmanharris(i,n);
    }

    // compute power spectral density
    liquid_doc_compute_psdf(w_kaiser_b0,      n, W_kaiser_b0,      nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    liquid_doc_compute_psdf(w_hamming,        n, W_hamming,        nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    liquid_doc_compute_psdf(w_hann,           n, W_hann,           nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    liquid_doc_compute_psdf(w_kaiser_b1,      n, W_kaiser_b1,      nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    liquid_doc_compute_psdf(w_blackmanharris, n, W_blackmanharris, nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);

    // print results to file(s)
    FILE * fid = NULL;

    // 
    // generate time-domain plot
    //

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-0.1:1.1]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# prototype\n");
    fprintf(fid,"set ylabel 'temporal window'\n");
    fprintf(fid,"set xrange [0:%u]\n", n-1);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Kaiser, B=3',\\\n",  color_kaiser_b0);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Hamming',\\\n",      color_hamming);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Hann',\\\n",         color_hann);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Kaiser, B=10',\\\n", color_kaiser_b1);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Blackman-harris'\n", color_blackmanharris);

    // Kaiser[0]
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, w_kaiser_b0[i]);
    fprintf(fid,"e\n");

    // hamming
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, w_hamming[i]);
    fprintf(fid,"e\n");

    // hann
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, w_hann[i]);
    fprintf(fid,"e\n");

    // Kaiser[1]
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, w_kaiser_b1[i]);
    fprintf(fid,"e\n");

    // Blackamn-harris
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, w_blackmanharris[i]);
    fprintf(fid,"e\n");

    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set xrange [0:0.5]\n");
    fprintf(fid,"set yrange [-120:0]\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Kaiser, B=3',\\\n",  color_kaiser_b0);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Hamming',\\\n",      color_hamming);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Hann',\\\n",         color_hann);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Kaiser, B=10',\\\n", color_kaiser_b1);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'Blackman-harris'\n", color_blackmanharris);

    // save array to output
    unsigned int k;
    float f[nfft];
    for (i=0; i<nfft; i++)
        f[i] = (float)(i) / (float)nfft - 0.5f;

    fprintf(fid,"# frequency psd\n");

    // Kaiser[0]
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W_kaiser_b0[k])));
    }
    fprintf(fid,"e\n");

    // Hamming
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W_hamming[k])));
    }
    fprintf(fid,"e\n");

    // Hann
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W_hann[k])));
    }
    fprintf(fid,"e\n");

    // Kaiser[1]
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W_kaiser_b1[k])));
    }
    fprintf(fid,"e\n");

    // Blackman-harris
    for (i=0; i<nfft; i++) {
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(W_blackmanharris[k])));
    }
    fprintf(fid,"e\n");

    fclose(fid);
    return 0;
}

