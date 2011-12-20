//
// fft_example.c : demonstrate FFT
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/fft_example_time.gnu"
#define OUTPUT_FILENAME_FREQ "figures.gen/fft_example_freq.gnu"

int main() {
    // options
    unsigned int order=3;   // filter order
    float fc=0.1f;          // filter cutoff frequency
    float f0=0.2f;          // filter center frequency
    float Ap= 0.5f;         // filter pass-band ripple [dB]
    float As=60.0f;         // filter stop-band ripple [dB]
    unsigned int nfft=201;  // input length and fft size

    unsigned int i;

    // allocated arrays
    float complex y[nfft];

    // generate input sequence (filtered noise)
    iirfilt_crcf q = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER,
                                                   LIQUID_IIRDES_LOWPASS,
                                                   LIQUID_IIRDES_SOS,
                                                   order,
                                                   fc, f0,
                                                   Ap, As);
    for (i=0; i<nfft; i++)
        iirfilt_crcf_execute(q, (randnf() + randnf()*_Complex_I)*M_SQRT1_2, &y[i]);
    iirfilt_crcf_destroy(q);
    
    // compute fft
    float complex Y[nfft];
    fft_run(nfft, y, Y, FFT_FORWARD, 0);

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
    fprintf(fid,"set xrange [0:%u];\n",nfft-1);
    fprintf(fid,"set yrange [-1.0:1.0]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set ylabel 'Time Series'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    //fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"set xrange [0:%u]\n", nfft-1);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'real',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'imag'\n",    LIQUID_DOC_COLOR_GREEN);

    // real
    for (i=0; i<nfft; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(y[i]));
    fprintf(fid,"e\n");

    // imag
    for (i=0; i<nfft; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(y[i]));
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TIME);


    // 
    // generate frequency-domain plot
    //

    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n", nfft-1);
    //fprintf(fid,"set yrange [-100:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set ylabel 'Frequency Response'\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'real',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'imag'\n",    LIQUID_DOC_COLOR_GREEN);

    // real
    for (i=0; i<nfft; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(Y[i]));
    fprintf(fid,"e\n");

    // imag
    for (i=0; i<nfft; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(Y[i]));
    fprintf(fid,"e\n");

    fclose(fid);
    return 0;
}

