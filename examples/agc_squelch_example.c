//
// agc_squelch_example.c
//
// Automatic gain control with squelch example.  This example demonstrates
// the squelch control functionality of the AGC module.  Squelch is used
// to suppress the output of the AGC when the signal level drops below a
// certain threshold.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include "liquid.h"

#define OUTPUT_FILENAME "agc_squelch_example.m"

int main() {
    // options
    float gamma=1.0f;           // channel gain
    float noise_floor = -25.0f; // noise floor [dB]
    float bt=0.10f;             // agc loop bandwidth
    unsigned int num_samples = 500;
    unsigned int d=num_samples/25;  // print every d iterations

    // create objects
    agc_crcf p = agc_crcf_create();
    agc_crcf_set_bandwidth(p, bt);

    // squelch
    agc_crcf_squelch_activate(p);
    agc_crcf_squelch_set_threshold(p,noise_floor+10.0f);
    agc_crcf_squelch_set_timeout(p,16);
    agc_crcf_squelch_enable_auto(p);

    unsigned int i;
    float complex x[num_samples];
    float complex y[num_samples];
    int squelch[num_samples];
    int noise_floor_est[num_samples];
    float rssi[num_samples];

    // print info
    printf("automatic gain control // loop bandwidth: %4.2e\n",bt);

    for (i=0; i<num_samples; i++) {
        x[i] = gamma * cexpf(_Complex_I*2*M_PI*0.093f*i);

        // add ripple to amplitude
        //x[i] *= 1.0f + 0.2f*cosf(2*M_PI*0.0037f*i);
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
    float noise_std = powf(10.0f, noise_floor / 10.0f) / sqrtf(2.0f);
    for (i=0; i<num_samples; i++)
        x[i] += noise_std*(randnf() + _Complex_I*randnf());

    // run agc
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(p, x[i], &y[i]);
        rssi[i] = agc_crcf_get_signal_level(p);
        squelch[i] = agc_crcf_squelch_get_status(p);
        noise_floor_est[i] = agc_crcf_squelch_get_threshold(p) - 4.0f;

        if ( ((i+1)%d) == 0 )
            printf("%4u: %12.8f %c\n", i+1,
                                       10*log10f(rssi[i]),
                                       squelch[i]==LIQUID_AGC_SQUELCH_ENABLED ? '*' : ' ');

    }
    agc_crcf_destroy(p);

    // open output file
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    fprintf(fid,"rssi = zeros(1,num_samples);\n");
    fprintf(fid,"squelch = zeros(1,num_samples);\n");
    fprintf(fid,"noise_floor_est = zeros(1,num_samples);\n");

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"      x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"      y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"   rssi(%4u) = %12.4e;\n", i+1, rssi[i]);
        fprintf(fid,"squelch(%4u) = %d;\n", i+1, squelch[i]);
        fprintf(fid,"noise_floor_est(%4u) = %d;\n", i+1, noise_floor_est[i]);
    }

    fprintf(fid,"squelch_enabled = (squelch == %u);\n", LIQUID_AGC_SQUELCH_ENABLED);

    fprintf(fid,"\n\n");
    fprintf(fid,"n = length(x);\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,imag(x));\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('input');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,10*log10(rssi),'-k','LineWidth',2, t,noise_floor_est,'-b');\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('rssi [dB]');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,real(y),t,imag(y),t,squelch_enabled,'-r');\n");
    //fprintf(fid,"  plot(t,real(y).*(1-squelch_enabled),t,imag(y).*(1-squelch_enabled));\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('output');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

