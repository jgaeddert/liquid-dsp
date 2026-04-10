const char __docstr__[] =
"Demonstrates a basic phase-locked loop to track the phase of a"
" complex sinusoid.";

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>
#include <time.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"pll_example.m",'o', "output filename", NULL);
    liquid_argparse_add(float,    phase_offset,     0.800, 'p', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    frequency_offset, 0.010, 'f', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    wn,               0.030, 'w', "phase-locked loop (PLL) bandwidth", NULL);
    liquid_argparse_add(float,    zeta,             0.707, 'z', "PLL damping factor", NULL);
    liquid_argparse_add(float,    K,                 1000, 'K', "PLL loop gain", NULL);
    liquid_argparse_add(unsigned, num_samples,        240, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // buffers for storing sample data
    LIQUID_VLA(float, theta, num_samples); // input phase
    LIQUID_VLA(liquid_float_complex, x, num_samples); // input sinusoid
    LIQUID_VLA(float, phi, num_samples); // output phase
    LIQUID_VLA(liquid_float_complex, y, num_samples); // output sinusoid
    LIQUID_VLA(float, err, num_samples); // phase error

    // generate iir loop filter object
    iirfilt_rrrf H = iirfilt_rrrf_create_pll(wn, zeta, K);
    iirfilt_rrrf_print(H);

    unsigned int i;

    // generate input
    float t=phase_offset;
    float dt = frequency_offset;
    for (i=0; i<num_samples; i++)
    {
        theta[i] = t;
        x[i] = cexpf(_Complex_I*theta[i]);
        t += dt;
    }

    // run loop
    unsigned int d = num_samples/24; // print every "d" lines
    float phi_hat=0.0f;
    for (i=0; i<num_samples; i++)
    {
        y[i] = cexpf(_Complex_I*phi_hat);

        // compute error
        err[i] = cargf(x[i]*conjf(y[i]));

        if ( (i%d)==0 )
            printf("e(%3u) = %12.8f;\n", i, err[i]);

        // filter error
        iirfilt_rrrf_execute(H,err[i],&phi_hat);

        phi[i] = phi_hat;
    }

    // destroy filter
    iirfilt_rrrf_destroy(H);

    // open output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"theta(%3u) = %16.8e;\n", i+1, theta[i]);
        fprintf(fid,"  phi(%3u) = %16.8e;\n", i+1, phi[i]);
        fprintf(fid,"  err(%3u) = %16.8e;\n", i+1, err[i]);
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,theta,t,phi);\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('phase');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,err);\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('phase error');\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("output written to %s.\n", filename);
    return 0;
}
