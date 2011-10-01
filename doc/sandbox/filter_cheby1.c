//
// filter_cheby1.c : demonstrate iir filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <liquid/liquid.h>

#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_cheby1.gnu"

int main() {
    // options
    unsigned int num_filters=3;
    float fc = 0.4f;        // cutoff
    float ripple = 1.0f;    // passband ripple [dB]
    int color_output = 1;

    // epsilon
    float epsilon = sqrtf( powf(10.0f, ripple / 10.0f) - 1.0f );

    unsigned int nfft=512;
    float Hmag[nfft];
    float Harg[nfft];

    // open output file and print header
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:0.5];\n");
    fprintf(fid,"set yrange [-6:0.5]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '#999999' lw 1\n");
    unsigned int i;
    for (i=0; i<num_filters; i++) {
        unsigned int r, g, b;   // plot colors
        unsigned int shade = 192 - (unsigned int) ( (float)i / (float)num_filters * 128 );
        r = color_output ? 0  : shade;
        g = color_output ? 64 : shade;
        b = shade;
        fprintf(fid,"%s '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#%.2x%.2x%.2x'%s\n",
                i==0 ? "plot" : "    ",
                r, g, b,
                i==(num_filters-1) ? "" : ",\\");
    }

    // generate filters, compute spectrum, save array to output
    float f;
    unsigned int k;
    fprintf(fid,"# frequency psd phase\n");
    for (i=0; i<num_filters; i++) {
        unsigned int n=2*i + 2; // filter order
        float b[n+1];           // numerator
        float a[n+1];           // denominator

        cheby1f(n,fc,epsilon,b,a);

        // compute filter power spectral density
        liquid_doc_freqz(b,n+1,a,n+1,nfft,Hmag,Harg);

        for (k=0; k<nfft/2; k++) {
            f = (float)(k) / (float)nfft;
            fprintf(fid,"%12.8f %12.4e %12.4e\n", f, 20.0f*log10f(cabsf(Hmag[k])), Harg[k]);
        }
        fprintf(fid,"e\n");
    }

    fclose(fid);
    return 0;

}

