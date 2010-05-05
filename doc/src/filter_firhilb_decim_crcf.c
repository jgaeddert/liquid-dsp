//
// filter_firhilb_decim_crcf.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <liquid/liquid.h>
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_firhilb_decim_crcf.gnu"

int main() {
    unsigned int m=5;               // filter semi-length
    unsigned int h_len = 4*m+1;     // filter length
    float slsl=60.0f;               // filter sidelobe suppression level
    float fc=0.37f;                 // signal center frequency
    unsigned int num_samples=128;   // number of samples

    // allocate memory for arrays
    float x[2*num_samples];         // real input
    float complex y[num_samples];   // complex output

    // create Hilbert transform object
    firhilb f = firhilb_create(h_len,slsl);
    firhilb_print(f);

    // generate input sequence
    unsigned int i;
    float theta=0.0f, dtheta=2*M_PI*fc;
    for (i=0; i<2*num_samples; i++) {
        // compute real input signal
        x[i] = cosf(theta);// * hamming(i,2*num_samples);
        theta += dtheta;
    }

    // run Hilber transform
    for (i=0; i<num_samples; i++) {
        // execute transform (decimator) to compute complex signal
        firhilb_decim_execute(f, &x[2*i], &y[i]);
    }

    unsigned int nfft = 512;
    float complex X[nfft];
    float complex Y[nfft];
    liquid_doc_compute_psdf( x,2*num_samples,X,nfft,LIQUID_DOC_PSDWINDOW_HAMMING,0);
    liquid_doc_compute_psdcf(y,  num_samples,Y,nfft,LIQUID_DOC_PSDWINDOW_HAMMING,0);
    fft_shift(X,nfft);
    //fft_shift(Y,nfft);
    float scaling_factor = 20*log10f(num_samples);

    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");

#if 0

    // 
    // plot time-domain
    //

    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-1.5:1.5]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'real'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'real'\n", LIQUID_DOC_COLOR_RED);
    // export output
    for (i=0; i<2*num_samples; i++) {
        fprintf(fid,"%6u %12.4e\n", i, x[i]);
    }
    fprintf(fid,"e\n");


    fprintf(fid,"# complex\n");
    fprintf(fid,"set ylabel 'complex'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'real',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'imag'\n", LIQUID_DOC_COLOR_GREEN);
    // export output
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"%6u %12.4e\n", i, crealf(y[i]));
    }
    fprintf(fid,"e\n");

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"%6u %12.4e\n", i, cimagf(y[i]));
    }
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");



#else

    // 
    // plot frequency-domain
    //

    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-80:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n",LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"# real\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'original/real',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'transformed/decimated'\n",   LIQUID_DOC_COLOR_GREEN);
    // export output
    for (i=0; i<nfft; i++) {
        float fx = (float)(i) / (float)nfft - 0.5f;
        fprintf(fid,"%12.8f %12.4e\n", fx, 20*log10f(cabsf(X[i])) - scaling_factor);
    }
    fprintf(fid,"e\n");
    for (i=0; i<nfft; i++) {
        float fy = (float)(i) / (float)(2*nfft);
        fprintf(fid,"%12.8f %12.4e\n", fy, 20*log10f(cabsf(Y[i])) - scaling_factor);
    }
    fprintf(fid,"e\n");
#endif

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firhilb_destroy(f);
    printf("done.\n");
    return 0;

    return 0;
}
