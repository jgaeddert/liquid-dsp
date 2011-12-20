//
// agc_transient.c
//
// Automatic gain control example demonstrating the transient
// response of an input signal with dynamic level changing.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/agc_transient.gnu"

int main() {
    // options
    float bt=1e-2f;                     // agc loop bandwidth
    float fc=0.019f;                    // sinusoid frequency
    float gmin = 0.1f;                  // agc minimum gain
    float gmax = 2.0f;                  // agc maximum gain
    unsigned int num_samples = 2048;    // number of samples

    // create objects
    agc_crcf p = agc_crcf_create();
    agc_crcf_set_bandwidth(p, bt);
    agc_crcf_set_gain_limits(p, gmin, gmax);

    // squelch
    agc_crcf_squelch_deactivate(p);

    unsigned int i;
    float complex x[num_samples];
    float complex y[num_samples];
    float gain[num_samples];
    float rssi[num_samples];

    // print info
    printf("automatic gain control // loop bandwidth: %4.2e\n",bt);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = cexpf(_Complex_I*2*M_PI*fc*i);

    unsigned int n=0;
    unsigned int n0   = num_samples * 0.1;
    unsigned int ramp = num_samples * 0.03;
    unsigned int n1   = num_samples * 0.7f;
    for (i=0; i<n0; i++)    gain[n++] = 0.0f;
    for (i=0; i<ramp; i++)  gain[n++] = 0.5f - 0.5f*cosf(M_PI*i/(float)ramp);
    for (i=0; i<n1; i++)    gain[n++] = 1.0f;
    for (i=0; i<ramp; i++)  gain[n++] = 0.5f + 0.5f*cosf(M_PI*i/(float)ramp);
    while (n < num_samples) gain[n++] = 0.0f;

#if 0
    // exponential decay
    for (i=0; i<num_samples; i++)
        gain[i] *= (i < n0) ? 1.0f : exp(-0.001f*(i-n0));
#else
    // sinusoidal variation
    for (i=0; i<num_samples; i++)
        gain[i] *= 1.0f + 0.3f*cosf(2.0f*M_PI*i*0.0009f);
#endif

    // apply gain
    for (i=0; i<num_samples; i++)
        x[i] *= gain[i];

    // run agc
    agc_crcf_reset(p);
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y[i]);
        rssi[i] = agc_crcf_get_signal_level(p);
    }

    // destroy AGC object
    agc_crcf_destroy(p);

    // open/initialize output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
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

    //
    // input signal
    //
    fprintf(fid,"# input signal\n");
    fprintf(fid,"set ylabel 'input signal'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'real',\\\n", LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'imag'\n", LIQUID_DOC_COLOR_PURPLE);
    // export output
    fprintf(fid,"# real\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(x[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(x[i]));
    fprintf(fid,"e\n");

    //
    // output signal
    //
    fprintf(fid,"# output signal\n");
    fprintf(fid,"set ylabel 'output signal'\n");
    fprintf(fid,"set xrange [0:%u]\n", num_samples);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'real',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' title 'imag'\n", LIQUID_DOC_COLOR_GREEN);
    // export output
    fprintf(fid,"# real\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(y[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(y[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"unset multiplot\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);


    printf("done.\n");
    return 0;
}

