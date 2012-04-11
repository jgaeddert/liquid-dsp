// 
// filter_iirdes.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_iirdes_example.gnu"

int main(int argc, char*argv[])
{
    // options
    unsigned int order=5;   // filter order
    float fc = 0.20f;       // cutoff frequency (low-pass prototype)
    float f0 = 0.25f;       // center frequency (band-pass, band-stop)
    float As = 60.0f;       // stopband attenuation [dB]
    float Ap = 1.0f;        // passband ripple [dB]
    unsigned int nfft=1024; // fft size

    // filter type
    liquid_iirdes_filtertype ftype = LIQUID_IIRDES_ELLIP;

    // band type
    liquid_iirdes_bandtype btype = LIQUID_IIRDES_BANDPASS;

    // output format: second-order sections or transfer function
    liquid_iirdes_format format = LIQUID_IIRDES_SOS;

    // derived values : compute filter length
    unsigned int N = order; // effective order
    // filter order effectively doubles for band-pass, band-stop
    // filters due to doubling the number of poles and zeros as
    // a result of filter transformation
    if (btype == LIQUID_IIRDES_BANDPASS ||
        btype == LIQUID_IIRDES_BANDSTOP)
    {
        N *= 2;
    }
    unsigned int r = N % 2;     // odd/even order
    unsigned int L = (N-r)/2;   // filter semi-length

    // allocate memory for filter coefficients
    unsigned int h_len = (format == LIQUID_IIRDES_SOS) ? 3*(L+r) : N+1;
    float B[h_len];
    float A[h_len];

    // design filter
    liquid_iirdes(ftype, btype, format, order, fc, f0, Ap, As, B, A);
    
    unsigned int i;

    // compute spectrum
    float complex H[nfft];
    float complex H0[nfft];
    for (i=0; i<nfft; i++)
        H[i] = 1.;

    unsigned int j;
    for (i=0; i<L+r; i++) {
        liquid_doc_freqz(&B[3*i], 3, &A[3*i], 3, nfft, H0);
        for (j=0; j<nfft; j++)
            H[j] *= H0[j];
    }
    // ensure response is not zero
    float eps = 1e-12f;
    for (j=0; j<nfft; j++) {
        if(cabsf(H[j]) < eps)
            H[j] = eps;
    }

    // 
    // generate spectrum plot
    //

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing.\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", OUTPUT_FILENAME);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:0.5];\n");
    fprintf(fid,"set yrange [-80:5]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 2:3 with lines linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"#    i   freq         PSD  \n");

    for (i=0; i<nfft/2; i++) {
        float f = (float)i/(float)nfft;
        fprintf(fid,"  %4u %12.4e %12.4e\n", i, f, 20*log10(cabsf(H[i])));
    }
    fclose(fid);
    printf("results written to '%s'.\n", OUTPUT_FILENAME);

    return 0;
}

