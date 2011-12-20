//
// cvsd_sine_example.c
//
// Continuously-variable slope delta example, sinusoidal input.
// This example demonstrates the CVSD audio encoder interface, and
// its response to a sinusoidal input.  The output distortion
// ratio is computed, and the time-domain results are written to
// a file.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/audio_cvsd.gnu"

int main() {
    // options
    unsigned int num_samples=256;   // number of samples
    float fc = 0.04179f;            // input sine frequency
    unsigned int nbits=2;           // 
    float zeta=1.5f;                //
    float alpha=0.95f;              //
    int subplot_type = 0;           // 0:spectrum, 1:data series

    // derived values
    unsigned int i;

    // create cvsd codecs
    cvsd cvsd_encoder = cvsd_create(nbits, zeta, alpha);
    cvsd cvsd_decoder = cvsd_create(nbits, zeta, alpha);

    // data arrays
    float x[num_samples];
    float y[num_samples];
    unsigned char data_enc[num_samples];

    // generate input data: windowed sinusoid
    for (i=0; i<num_samples; i++) {
        x[i] = sinf(2*M_PI*i*fc) + 0.5f*sinf(2*M_PI*i*fc*1.8f);
        x[i] *= 0.8f*hamming(i,num_samples);
    }

    // run encoder
    for (i=0; i<num_samples; i++)
        data_enc[i] = cvsd_encode(cvsd_encoder, x[i]);

    // run decoder
    for (i=0; i<num_samples; i++)
        y[i] = cvsd_decode(cvsd_decoder, data_enc[i]);

    // generate plots
    FILE * fid = NULL;

    // 
    // generate time-domain plot
    //

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n",num_samples);
    fprintf(fid,"set yrange [-1.5:1.5]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# time series\n");
    fprintf(fid,"set xlabel 'sample index'\n");
    fprintf(fid,"set ylabel 'time series'\n");
    fprintf(fid,"set xrange [0:%u]\n", num_samples);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'audio input',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'cvsd output'\n", LIQUID_DOC_COLOR_BLUE);
    // export output
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, x[i]);
    fprintf(fid,"e\n");

    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, y[i]);
    fprintf(fid,"e\n");

    if (subplot_type == 0) {
        // spectrum plot

        unsigned int nfft = 512;
        float complex X[nfft];
        float complex Y[nfft];
        liquid_doc_compute_psdf(x, num_samples, X, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
        liquid_doc_compute_psdf(y, num_samples, Y, nfft, LIQUID_DOC_PSDWINDOW_NONE, 0);
        fft_shift(X,nfft);
        fft_shift(Y,nfft);
        float scaling_factor = 20*log10f(num_samples);

        fprintf(fid,"# spectrum\n");
        fprintf(fid,"set xrange [0:0.5];\n");
        fprintf(fid,"set yrange [-80:0]\n");
        fprintf(fid,"set xlabel 'Normalized Frequency'\n");
        fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
        fprintf(fid,"set key top right nobox\n");
        fprintf(fid,"set xtics -0.5,0.1,0.5\n");
        fprintf(fid,"set ytics -200,20,100\n");
        fprintf(fid,"set grid xtics ytics\n");
        fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'audio input',\\\n", LIQUID_DOC_COLOR_GRAY);
        fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' title 'cvsd output'\n",   LIQUID_DOC_COLOR_PURPLE);
        // export output
        for (i=nfft/2; i<nfft; i++) {
            float f = (float)(i) / (float)nfft - 0.5f;
            fprintf(fid,"%12.8f %12.4e\n", f, 20*log10f(cabsf(X[i])) - scaling_factor);
        }
        fprintf(fid,"e\n");
        for (i=nfft/2; i<nfft; i++) {
            float f = (float)(i) / (float)nfft - 0.5f;
            fprintf(fid,"%12.8f %12.4e\n", f, 20*log10f(cabsf(Y[i])) - scaling_factor);
        }
        fprintf(fid,"e\n");
    } else {
        // data series

        fprintf(fid,"# data series\n");
        fprintf(fid,"set xlabel 'sample index'\n");
        fprintf(fid,"set ylabel 'encoded data'\n");
        fprintf(fid,"set xrange [0:%u];\n",num_samples);
        fprintf(fid,"set yrange [-0.2:1.2]\n");
        fprintf(fid,"set key top right nobox\n");
        fprintf(fid,"set nokey # disable legend\n");
        fprintf(fid,"set grid xtics ytics\n");
        fprintf(fid,"plot '-' using 1:2 with steps linetype 1 linewidth 2 linecolor rgb '%s'\n", LIQUID_DOC_COLOR_GREEN);
        // export output
        for (i=0; i<num_samples; i++)
            fprintf(fid,"%6u %1u\n", i, data_enc[i]);
        fprintf(fid,"e\n");
    }

    fprintf(fid,"unset multiplot\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TIME);

    printf("done.\n");
    return 0;
}

