//
// agc_squelch.c : automatic gain control squelch plot
//

#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include <liquid/liquid.h>
#include "liquid.doc.h"

#define OUTPUT_FILENAME  "figures.gen/agc_squelch.gnu"

int main() {
    // options
    float etarget=1.0f;         // target level
    float noise_floor = -25.0f; // noise floor [dB]
    float threshold = noise_floor + 5.0f;
    float bt=0.10f;             // agc loop bandwidth
    unsigned int num_samples = 2048;
    unsigned int timeout = num_samples/8;

    // squelch status indices
    unsigned int index_rise     = 0;
    unsigned int index_fall     = 0;
    unsigned int index_timeout  = 0;

    // create objects
    agc_crcf p = agc_crcf_create();
    agc_crcf_set_target(p, etarget);
    agc_crcf_set_bandwidth(p, bt);

    // squelch
    agc_crcf_squelch_activate(p);
    agc_crcf_squelch_set_threshold(p,threshold);
    agc_crcf_squelch_set_timeout(p,timeout);

    unsigned int i;
    float complex x[num_samples];
    float complex y[num_samples];
    int squelch[num_samples];
    float rssi[num_samples];

    // prime agc with noise
    float noise_std = powf(10.0f, noise_floor / 10.0f) / sqrtf(2.0f);
    for (i=0; i<2000; i++) {
        float complex noise = noise_std*(randnf() + _Complex_I*randnf());
        float complex dummy;
        agc_crcf_execute(p, noise, &dummy);
    }

    for (i=0; i<num_samples; i++) {
        x[i] = cexpf(_Complex_I*2*M_PI*0.093f*i);

        // add ripple to amplitude
        //x[i] *= gamma*( 1.0f + 0.2f*cosf(2*M_PI*0.0037f*i) );
    }
    unsigned int n=0;
    unsigned int n0   = num_samples / 6;
    unsigned int ramp = num_samples / 10;
    unsigned int n1   = num_samples / 3;
    for (i=0; i<n0; i++)    x[n++] *= 0.0f;
    for (i=0; i<ramp; i++)  x[n++] *= 0.5f - 0.5f*cosf(M_PI*i/(float)ramp);
    for (i=0; i<n1; i++)    x[n++] *= 1.0f;
    for (i=0; i<ramp; i++)  x[n++] *= 0.5f + 0.5f*cosf(M_PI*i/(float)ramp);
    while (n < num_samples) x[n++] *= 0.0f;

    // add noise
    for (i=0; i<num_samples; i++)
        x[i] += noise_std*(randnf() + _Complex_I*randnf());

    // run agc
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y[i]);
        rssi[i] = agc_crcf_get_signal_level(p);
        squelch[i] = agc_crcf_squelch_get_status(p);

        if (squelch[i] == LIQUID_AGC_SQUELCH_RISE)
            index_rise = i;
        else if (squelch[i] == LIQUID_AGC_SQUELCH_FALL)
            index_fall = i;
        else if (squelch[i] == LIQUID_AGC_SQUELCH_TIMEOUT)
            index_timeout = i;
    }
    agc_crcf_destroy(p);


    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n",num_samples-1);
    fprintf(fid,"set yrange [0:%f]\n",-noise_floor + 10.0f);
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'time'\n");
    fprintf(fid,"set ylabel 'signal level'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"unset border\n");
    fprintf(fid,"unset xtics\n");
    fprintf(fid,"unset ytics\n");
    fprintf(fid,"set zeroaxis linetype 1 linewidth 2 linecolor rgb '#000000'\n");
    //fprintf(fid,"set grid xtics ytics\n");
    //fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#004080',\\\n");
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '#777777',\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 pointsize 1.2 linecolor rgb '#440000'\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"%4u %8.2f\n", i, 10*log10f(rssi[i]) - noise_floor + 5.0f);
    }
    fprintf(fid,"e\n");
    fprintf(fid,"# horizontal threshold line\n");
    fprintf(fid,"  0  10\n");
    fprintf(fid,"  %u 10\n", num_samples);
    fprintf(fid,"e\n");
    fprintf(fid,"# status points\n");
    fprintf(fid,"  %u 10\n", index_rise);
    fprintf(fid,"  %u 10\n", index_fall);
    fprintf(fid,"  %u 10\n", index_timeout);
    fprintf(fid,"e\n");
    fclose(fid);

    printf("done.\n");
    return 0;
}
