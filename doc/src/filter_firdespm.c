//
// firdespm_example.c
//
// This example demonstrates finite impulse response filter design
// using the Parks-McClellan algorithm.
// SEE ALSO: firdes_kaiser_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_firdespm.gnu"

int main(int argc, char*argv[]) {
    // options
    unsigned int n=55;  // filter length
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int nfft = 1024;

    //float fp = 0.08f;   // pass-band cutoff frequency
    //float fs = 0.16f;   // stop-band cutoff frequency

    // derived values
    unsigned int num_bands = 4;
    float bands[8]   = {0.0f,   0.1f,   // 1
                        0.15f,  0.3f,   // 0
                        0.33f,  0.4f,   // 0.1
                        0.42f,  0.5f};  // 0
    float des[4]     = {1.0f, 0.0f, 0.1f, 0.0f};
    float weights[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    liquid_firdespm_wtype wtype[4] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};
    unsigned int i;
    float h[n];
#if 0
    firdespm q = firdespm_create(n,bands,des,weights,num_bands,btype);
    firdespm_print(q);
    firdespm_execute(q,h);
    firdespm_destroy(q);
#else
    firdespm_run(n,num_bands,bands,des,weights,wtype,btype,h);
#endif

    // 
    // generate plots
    //
    FILE * fid = NULL;

    // 
    // generate time-domain plot
    //

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file '%s' for writing.\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    //fprintf(fid,"set yrange [-0.1:0.35]\n");
    fprintf(fid,"set ytics -1,0.1,1\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# taps\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set ylabel 'Impulse Response'\n");
    //fprintf(fid,"set xrange [0:%u]\n", n);
    fprintf(fid,"plot '-' using 1:2 with impulses linetype 1 linewidth 3 linecolor rgb '%s' notitle\n", LIQUID_DOC_COLOR_RED);
    // export output
    for (i=0; i<n; i++) {
        fprintf(fid,"%12.4e %12.4e\n", (float)(i) - 0.5*n + 0.5, h[i]);
    }
    fprintf(fid,"e\n");

    // 
    // generate frequency-domain plot
    //
 
    float complex H[nfft];
    liquid_doc_compute_psdf(h, n, H, nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    fft_shift(H,nfft);

    fprintf(fid,"# spectrum\n");
    fprintf(fid,"set xrange [0:0.5];\n");
    fprintf(fid,"set yrange [-80:20];\n");
    fprintf(fid,"set xtics -0.5,0.1,0.5\n");
    fprintf(fid,"set ytics -200,20,100\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' notitle\n",   LIQUID_DOC_COLOR_BLUE);
    // export output
    for (i=nfft/2; i<nfft; i++) {
        float f = (float)(i) / (float)nfft - 0.5f;
        fprintf(fid,"%12.8f %12.4e\n", f, 20*log10f(cabsf(H[i])) );
    }
    fprintf(fid,"e\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);


    return 0;
}

