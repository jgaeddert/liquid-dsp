/
// This example demonstrates how the use the nco/pll object
// (numerically-controlled oscillator with phase-locked loop) interface for
// tracking to a complex sinusoid.  The loop bandwidth, phase offset, and
// other parameter can be specified via the command-line interface.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "nco_crcq16_pll_example.m"

// print usage/help message
void usage()
{
    printf("nco_pll_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  b     : pll bandwidth, default: 0.01\n");
    printf("  n     : number of samples, default: 512\n");
    printf("  p     : phase offset (radians), default: pi/4\n");
    printf("  f     : frequency offset (radians), default: 0.2\n");
}

int main(int argc, char*argv[])
{
    srand( time(NULL) );

    // parameters
    float phase_offset     = M_PI / 4.0f; // initial phase offset
    float frequency_offset = 0.2f;        // initial freq. offset
    float pll_bandwidth    = 0.01f;       // initial PLL bandwidth
    unsigned int n         = 512;         // number of iterations

    int dopt;
    while ((dopt = getopt(argc,argv,"uhb:n:p:f:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();    return 0;
        case 'b':   pll_bandwidth = atof(optarg);   break;
        case 'n':   n = atoi(optarg);               break;
        case 'p':   phase_offset = atof(optarg);    break;
        case 'f':   frequency_offset= atof(optarg); break;
        default:
            exit(1);
        }
    }

    // objects
    nco_crcq16 nco_tx = nco_crcq16_create(LIQUID_VCO);
    nco_crcq16 nco_rx = nco_crcq16_create(LIQUID_VCO);

    // initialize objects
    nco_crcq16_set_phase(nco_tx,         q16_angle_float_to_fixed(phase_offset));
    nco_crcq16_set_frequency(nco_tx,     q16_angle_float_to_fixed(frequency_offset));
    nco_crcq16_pll_set_bandwidth(nco_rx, pll_bandwidth);

    // generate input
    cq16_t x[n];
    cq16_t y[n];
    q16_t phase_error[n];

    unsigned int i;
    for (i=0; i<n; i++) {
        // generate complex sinusoid
        nco_crcq16_cexpf(nco_tx, &x[i]);

        // update nco
        nco_crcq16_step(nco_tx);
    }

    // run loop
    for (i=0; i<n; i++) {
#if 0
        // test resetting bandwidth in middle of acquisition
        if (i == 100) nco_pll_set_bandwidth(nco_rx, pll_bandwidth*0.2f);
#endif

        // generate complex sinusoid
        nco_crcq16_cexpf(nco_rx, &y[i]);

        // compute phase error
        // phase_error[i] = cargf(x[i]*conjf(y[i]));
        phase_error[i] = cq16_carg( cq16_mul(x[i], cq16_conj(y[i])) );

        // update pll
        nco_crcq16_pll_step(nco_rx, phase_error[i]);

        // update rx nco object
        nco_crcq16_step(nco_rx);

        // print phase error
        if ( (i+1)%50 == 0 || i==n-1 || i==0)
            printf("%4u : phase error = %12.8f\n", i+1, q16_angle_fixed_to_float(phase_error[i]));
    }
    nco_crcq16_destroy(nco_tx);
    nco_crcq16_destroy(nco_rx);

    // write output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"x = zeros(1,n);\n");
    fprintf(fid,"y = zeros(1,n);\n");
    for (i=0; i<n; i++) {
        float complex xf = cq16_fixed_to_float(x[i]);
        float complex yf = cq16_fixed_to_float(y[i]);
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(xf), cimagf(xf));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(yf), cimagf(yf));
        fprintf(fid,"e(%4u) = %12.4e;\n", i+1, q16_angle_fixed_to_float(phase_error[i]));
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,real(y));\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),t,imag(y));\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,e);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('phase error');\n");
    fprintf(fid,"  axis([0 n -pi pi]);\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
