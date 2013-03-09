// iirfilt_intdiff_test
//
// Tests infinite impulse reponse (IIR) integrator and differentiator
// filters.
//
// References:
//  [Pintelon:1990] Rik Pintelon and Johan Schoukens, "Real-Time
//      Integration and Differentiation of Analog Signals by Means of
//      Digital Filtering," IEEE Transactions on Instrumentation and
//      Measurement, vol 39 no. 6, December 1990.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "iirfilt_intdiff_test.m"

int main(int argc, char*argv[]) {
    // options
    unsigned int num_samples = 100;

    // integrator second-order sections
    float Bi[12] = {
        1.0f, -4.26790400000000f,  -6.400965325377000,
        1.0f,  3.87791172132679f,   11.36377584040000,
        1.0f, -1.42910328910057f,   20.69986108410000,
        1.0f, -7.99261372097011f,   27.28982076915599,};
    float Ai[12] = {
        1.0f, -0.4194765000000000f, -0.5805235000000000f,
        1.0f,  0.1904232630641466f,  0.0543832194444100f,
        1.0f, -0.1456570930905469f,  0.0329333571002500f,
        1.0f, -0.3046105139587997f,  0.0269438108284900f,};

    // differentiator second-order sections
    float Bd[12] = {
        1.0f,  0.70257500000000f,  -1.702575000000000f,
        1.0f,  8.81652829611905f,   34.54365443822500f,
        1.0f,  3.49423846094481f,   17.61834305124100f,
        1.0f, -4.20028789957191f,   28.62553888065601f,};
    float Ad[12] = {
        1.0f,  0.6590848000000000f,  -0.1598824726636800f,
        1.0f,  0.3474605342253413f,   0.0894477098996100f,
        1.0f, -0.0658154727551225f,   0.0498373031032900f,
        1.0f, -0.2978098115406486f,   0.0383638816890000f,};

    // allocate arrays
    float complex x[num_samples];
    float complex y[num_samples];
    float complex z[num_samples];

    // create filter objects
    iirfilt_crcf integrator     = iirfilt_crcf_create_sos(Bi,Ai,4);
    iirfilt_crcf differentiator = iirfilt_crcf_create_sos(Bd,Ad,4);

    float tmin = -5.0f;
    float tmax =  5.0f;
    float dt   = (tmax-tmin)/(float)(num_samples-1);
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // time vector
        float t = tmin + dt*i;

        // generate input signal
        x[i] = sincf(2.0f*t) + _Complex_I*cosf(M_PI*t);

        // run integrator
        iirfilt_crcf_execute(integrator, x[i], &y[i]);

        // run differentitator
        iirfilt_crcf_execute(differentiator, y[i], &z[i]);
    }

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // save
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"x = zeros(1,n);\n");
    fprintf(fid,"y = zeros(1,n);\n");
    fprintf(fid,"z = zeros(1,n);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%5u)=%12.4e+%12.4ej; ", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%5u)=%12.4e+%12.4ej; ", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%5u)=%12.4e+%12.4ej; ", i+1, crealf(z[i]), cimagf(z[i]));
        fprintf(fid,"\n");
    }

    // plot results
    fprintf(fid,"tmin = %f;\n", tmin);
    fprintf(fid,"tmax = %f;\n", tmax);
    fprintf(fid,"dt = (tmax-tmin)/(n-1);\n");
    fprintf(fid,"t = tmin + [0:(n-1)]*dt;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t, real(x), t, imag(x));\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t, real(y), t, imag(y));\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t, real(z), t, imag(z));\n");

    // compute frequency response
    unsigned int nfft = 256;
    float complex Hi[nfft], Hd[nfft];
    float gdi[nfft], gdd[nfft];
    for (i=0; i<nfft; i++) {
        float f = 1e-4f + 0.5f * (float)i / (float)nfft;

        // compute magnitude/phase response
        iirfilt_crcf_freqresponse(integrator,     f, &Hi[i]);
        iirfilt_crcf_freqresponse(differentiator, f, &Hd[i]);

        // compute group delay
        gdi[i] = iirfilt_crcf_groupdelay(integrator, f);
        gdd[i] = iirfilt_crcf_groupdelay(differentiator, f);
    }

    // save frequency response
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"Hi  = zeros(1,nfft);\n");
    fprintf(fid,"Hd  = zeros(1,nfft);\n");
    fprintf(fid,"gdi = zeros(1,nfft);\n");
    fprintf(fid,"gdd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"Hi(%5u)=%12.4e+%12.4ej; ", i+1, crealf(Hi[i]), cimagf(Hi[i]));
        fprintf(fid,"Hd(%5u)=%12.4e+%12.4ej; ", i+1, crealf(Hd[i]), cimagf(Hd[i]));
        fprintf(fid,"gdi(%5u)=%12.4e; ",        i+1, gdi[i]);
        fprintf(fid,"gdd(%5u)=%12.4e; ",        i+1, gdd[i]);
        fprintf(fid,"\n");
    }

    // plot magnitude response
    fprintf(fid,"f = 0.5*[0:(nfft-1)]/nfft;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1),\n");
    fprintf(fid,"  plot(f,20*log10(Hi),'-','Color',[0 0.5 0.0],'LineWidth',2,...\n");
    fprintf(fid,"       f,20*log10(Hd),'-','Color',[0 0.0 0.5],'LineWidth',2);\n");
    fprintf(fid,"  %%axis([0.0 0.5 0 ceil(1.1*max([gdi gdd]))]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Integrator','Differentiator',0);\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Filter PSD [dB]');\n");

    // plot group delay
    fprintf(fid,"subplot(2,1,2),\n");
    fprintf(fid,"  plot(f,gdi,'-','Color',[0 0.5 0.0],'LineWidth',2,...\n");
    fprintf(fid,"       f,gdd,'-','Color',[0 0.0 0.5],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 0 ceil(1.1*max([gdi gdd]))]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Integrator','Differentiator',0);\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Group delay [samples]');\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // destroy filter objects
    iirfilt_crcf_destroy(integrator);
    iirfilt_crcf_destroy(differentiator);

    printf("done.\n");
    return 0;
}

