//
// filter_msresamp_crcf.c
//

#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/filter_msresamp_crcf_time.gnu"
#define OUTPUT_FILENAME_FREQ "figures.gen/filter_msresamp_crcf_freq.gnu"

int main() {
    float r=sqrtf(19);  // resampling rate (output/input)
    unsigned int n=37;  // number of input samples
    float As=60.0f;     // stop-band attenuation [dB]

    unsigned int i;

    // derived values: number of input, output samples (adjusted for filter delay)
    unsigned int nx = 1.4*n;
    unsigned int ny_alloc = (unsigned int) (2*(float)nx * r);  // allocation for output

    // allocate memory for arrays
    float complex x[nx];
    float complex y[ny_alloc];

    // generate input signal (filter pulse with frequency offset)
    float hf[n];
    liquid_firdes_kaiser(n, 0.08f, 80.0f, 0, hf);
    for (i=0; i<nx; i++)
        x[i] = (i < n) ? hf[i]*cexpf(_Complex_I*1.17f*i) : 0.0f;

    // create resampler
    msresamp_crcf q = msresamp_crcf_create(r,As);
    float delay = msresamp_crcf_get_delay(q);

    // execute resampler, storing in output buffer
    unsigned int ny;
    msresamp_crcf_execute(q, x, nx, y, &ny);

    // print basic results
    printf("input samples   : %u\n", nx);
    printf("output samples  : %u\n", ny);
    printf("delay           : %f samples\n", delay);

    // clean up allocated objects
    msresamp_crcf_destroy(q);


    // 
    // export output files
    //
    FILE * fid;

    // 
    // export time plot
    //
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-1.2:1.2]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Input Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'Real'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 6 pointsize 0.9 linetype 1 linewidth 1 linecolor rgb '#666666' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 pointsize 0.6 linecolor rgb '#008000' title 'resampled'\n");
    // export input signal
    for (i=0; i<nx; i++)
        fprintf(fid,"%12.4e %12.4e %12.4e\n", (float)i, crealf(x[i]), cimagf(x[i]));
    fprintf(fid,"e\n");

    // export output signal
    for (i=0; i<ny; i++)
        fprintf(fid,"%12.4e %12.4e %12.4e\n", (float)(i)/r - delay, crealf(y[i]), cimagf(y[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:3 with linespoints pointtype 6 pointsize 0.9 linetype 1 linewidth 1 linecolor rgb '#666666' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:3 with points pointtype 7 pointsize 0.6 linecolor rgb '#800000' title 'resampled'\n");

    // export input signal
    for (i=0; i<nx; i++)
        fprintf(fid,"%12.4e %12.4e %12.4e\n", (float)i, crealf(x[i]), cimagf(x[i]));
    fprintf(fid,"e\n");

    // export output signal
    for (i=0; i<ny; i++)
        fprintf(fid,"%12.4e %12.4e %12.4e\n", (float)(i)/r - delay, crealf(y[i]), cimagf(y[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);


    // 
    // export spectrum plot
    //
    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    unsigned int nfft = 512;
    float complex X[nfft];
    float complex Y[nfft];
    liquid_doc_compute_psdcf(x, nx, X, nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    liquid_doc_compute_psdcf(y, ny, Y, nfft, LIQUID_DOC_PSDWINDOW_NONE, 1);
    fft_shift(X,nfft);
    fft_shift(Y,nfft);

    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-120:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Normalized Output Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n",LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"# real\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#999999' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#004080' title 'resampled'\n");
    // export output
    for (i=0; i<nfft; i++) {
        float fx = ((float)(i) / (float)nfft - 0.5f) / r;
        fprintf(fid,"%12.8f %12.4e\n", fx, 20*log10f(cabsf(X[i])));
    }
    fprintf(fid,"e\n");
    for (i=0; i<nfft; i++) {
        float fy = ((float)(i) / (float)nfft - 0.5f);
        fprintf(fid,"%12.8f %12.4e\n", fy, 20*log10f(cabsf(Y[i])));
    }
    fprintf(fid,"e\n");

    fclose(fid);

    printf("done.\n");
    return 0;
}
