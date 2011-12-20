//
// filter_iirfilt_crcf.c
//
// Complex infinite impulse response filter example. Demonstrates
// the functionality of iirfilt by designing a low-order
// prototype (e.g. Butterworth) and using it to filter a noisy
// signal.  The filter coefficients are real, but the input and
// output arrays are complex.  The filter order and cutoff
// frequency are specified at the beginning.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/filter_iirfilt_crcf_time.gnu"

int main() {
    // options
    unsigned int order=4;   // filter order
    float fc=0.1f;          // cutoff frequency
    float f0=0.0f;          // center frequency
    float Ap=1.0f;          // pass-band ripple
    float As=40.0f;         // stop-band attenuation
    unsigned int n=128;     // number of samples
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_ELLIP;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;

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
    unsigned int r = N%2;       // odd/even order
    unsigned int L = (N-r)/2;   // filter semi-length

    // allocate memory for filter coefficients
    unsigned int h_len = (format == LIQUID_IIRDES_SOS) ? 3*(L+r) : N+1;
    float b[h_len];
    float a[h_len];

    // design filter
    liquid_iirdes(ftype, btype, format, order, fc, f0, Ap, As, b, a);

    // create filter object
    iirfilt_crcf f = NULL;
    if (format == LIQUID_IIRDES_SOS)
        f = iirfilt_crcf_create_sos(b,a,L+r);
    else
        f = iirfilt_crcf_create(b,h_len, a,h_len);
    iirfilt_crcf_print(f);

    unsigned int i;

    // allocate memory for data arrays
    float complex x[n];
    float complex y[n];

    // generate input signal (noisy sine wave with decaying amplitude)
    for (i=0; i<n; i++) {
#if 0
        x[i] = cexpf((2*M_PI*0.057f*_Complex_I - 0.04f)*i);
        x[i] += 0.1f*(randnf() + _Complex_I*randnf());
#else
        x[i] = 1.0f*cexpf(_Complex_I*2*M_PI*0.057f*i) + 
               0.5f*cexpf(_Complex_I*2*M_PI*0.192f*i);
        x[i] *= expf(-(float)i*0.014f);
#endif
    }

    // run filter
    for (i=0; i<n; i++)
        iirfilt_crcf_execute(f, x[i], &y[i]);

    // destroy filter object
    iirfilt_crcf_destroy(f);


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
    fprintf(fid,"set xrange [0:%u]\n", n);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'input',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'filtered'\n", LIQUID_DOC_COLOR_BLUE);
    // export output
    for (i=0; i<n; i++) {
        fprintf(fid,"%6u %12.4e\n", i, crealf(x[i]));
    }
    fprintf(fid,"e\n");

    for (i=0; i<n; i++) {
        fprintf(fid,"%6u %12.4e\n", i, crealf(y[i]));
    }
    fprintf(fid,"e\n");


    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'imag'\n");
    fprintf(fid,"set xrange [0:%u]\n", n);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'input',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'filtered'\n", LIQUID_DOC_COLOR_GREEN);
    // export output
    for (i=0; i<n; i++) {
        fprintf(fid,"%6u %12.4e\n", i, cimagf(x[i]));
    }
    fprintf(fid,"e\n");

    for (i=0; i<n; i++) {
        fprintf(fid,"%6u %12.4e\n", i, cimagf(y[i]));
    }
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TIME);

    printf("done.\n");
    return 0;
}

