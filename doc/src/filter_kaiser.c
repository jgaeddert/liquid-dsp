//
// filter_kaiser.c : demonstrate fir filter design using Kaiser window
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <liquid/liquid.h>

#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_kaiser.gnu"

int main() {
    // options
    float fc=0.4f;          // filter cutoff frequency
    float ft=0.1f;          // filter transition
    float slsl=60.0f;       // sidelobe suppression level
    float mu=0.0f;          // fractional timing offset

    // estimate required filter length and generate filter
    unsigned int h_len=(unsigned int)num_fir_filter_taps(slsl,ft);
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,mu,h);

    // compute filter power spectral density
    unsigned int nfft=512;
    float complex X[nfft];
    liquid_doc_compute_psdf(h,h_len,X,nfft,LIQUID_DOC_PSDWINDOW_NONE,1);

    // open output file and print header
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-100:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '#999999' lw 1\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#004080'\n");

    // save array to output
    float f;
    unsigned int i, k;
    fprintf(fid,"# frequency psd\n");
    for (i=0; i<nfft; i++) {
        f = (float)(i) / (float)nfft - 0.5f;
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.4e\n", f, 20.0f*log10f(cabsf(X[k])));
    }

    fclose(fid);
    return 0;
}

