//
// filter_kaiser.c : demonstrate fir filter design using Kaiser window
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/filter_kaiser_time.gnu"
#define OUTPUT_FILENAME_FREQ "figures.gen/filter_kaiser_freq.gnu"

int main() {
    // options
    float fc=0.15f;         // filter cutoff frequency
    float ft=0.05f;         // filter transition
    float As=60.0f;         // stop-band attenuation [dB]
    float mu=0.0f;          // fractional timing offset

    // estimate required filter length and generate filter
    unsigned int h_len = estimate_req_filter_len(ft,As);
    float h[h_len];

    // Compute filter coefficients.
    // 
    // This is equivalent to
    //      liquid_firdes_kaiser(h_len,fc,As,mu,h);
    // but do it manually so we can extract the sinc and window for 
    // plotting
    float beta = kaiser_beta_As(As);
    float t;
    float h1[h_len];
    float h2[h_len];
    unsigned int i;
    for (i=0; i<h_len; i++) {
        t = (float)i - (float)(h_len-1)/2 + mu;
     
        // sinc prototype
        h1[i] = sincf(2.0f*fc*t);

        // kaiser window
        h2[i] = kaiser(i,h_len,beta,mu);

        // composite
        h[i] = h1[i] * h2[i];
    }   


    // compute filter power spectral density
    unsigned int nfft=512;
    float complex X[nfft];
    liquid_doc_compute_psdf(h,h_len,X,nfft,LIQUID_DOC_PSDWINDOW_NONE,1);

    // print results to file(s)
    FILE * fid = NULL;

    // 
    // generate time-domain plot
    //

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-0.4:1.1]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# prototype\n");
    fprintf(fid,"set ylabel 'sinc, window'\n");
    fprintf(fid,"set xrange [0:%u]\n", h_len-1);
    fprintf(fid,"plot '-' using 1:2 with impulses linetype 1 linewidth 3 linecolor rgb '%s' title 'sinc',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines    linetype 1 linewidth 3 linecolor rgb '%s' title 'Kaiser window'\n", LIQUID_DOC_COLOR_RED);
    // sinc
    for (i=0; i<h_len; i++)
        fprintf(fid,"%6u %12.4e\n", i, h1[i]);
    fprintf(fid,"e\n");

    // Kaiser window
    for (i=0; i<h_len; i++)
        fprintf(fid,"%6u %12.4e\n", i, h2[i]);
    fprintf(fid,"e\n");


    fprintf(fid,"# filter\n");
    fprintf(fid,"set ylabel 'filter'\n");
    fprintf(fid,"set xrange [0:%u]\n", h_len-1);
    fprintf(fid,"plot '-' using 1:2 with impulses linetype 1 linewidth 3 linecolor rgb '%s' title 'composite'\n", LIQUID_DOC_COLOR_GREEN);
    // export output
    for (i=0; i<h_len; i++) {
        fprintf(fid,"%6u %12.4e\n", i, h[i]);
    }
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TIME);


    // 
    // generate frequency-domain plot
    //

    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-100:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#004080'\n");

    // save array to output
    float f;
    unsigned int k;
    fprintf(fid,"# frequency psd\n");
    for (i=0; i<nfft; i++) {
        f = (float)(i) / (float)nfft - 0.5f;
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f, 20.0f*log10f(cabsf(X[k])));
    }

    fclose(fid);
    return 0;
}

