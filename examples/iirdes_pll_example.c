const char __docstr__[] =
"This example demonstrates 2nd-order IIR phase-locked loop filter"
" design with a practical simulation.";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirdes_pll_example.m",  'o', "output filename", NULL);
    liquid_argparse_add(float,   pll_bandwidth,    0.01f;,        'b', "PLL bandwidth", NULL);
    liquid_argparse_add(float,   zeta,             1/sqrtf(2.0f), 'z', "PLL damping factor", NULL);
    liquid_argparse_add(float,   K,                1000.0f,       'K', "PLL loop gain", NULL);
    liquid_argparse_add(unsigned,n,                800,           'n', "number of iterations", NULL);
    liquid_argparse_add(float,   phase_offset,     M_PI / 4.0f,   'p', "phase offset", NULL);
    liquid_argparse_add(float,   frequency_offset, 0.3f,          'f', "frequency offset", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (pll_bandwidth <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth must be greater than 0");
    if (zeta <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"damping factor must be greater than 0");
    if (K <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"loop gain must be greater than 0");

    // data arrays
    LIQUID_VLA(liquid_float_complex, x, n);         // input complex sinusoid
    LIQUID_VLA(liquid_float_complex, y, n);         // output complex sinusoid
    LIQUID_VLA(float, phase_error, n);       // output phase error

    // generate PLL filter
    LIQUID_VLA(float, b, 3);
    LIQUID_VLA(float, a, 3);
    iirdes_pll_active_lag(pll_bandwidth, zeta, K, b, a);
    iirfilt_rrrf pll = iirfilt_rrrf_create(b,3,a,3);
    iirfilt_rrrf_print(pll);

    unsigned int d=n/32;      // print every "d" lines
    unsigned int i;
    float phi;
    for (i=0; i<n; i++) {
        phi = phase_offset + i*frequency_offset;
        x[i] = cexpf(_Complex_I*phi);
    }

    // run loop
    float theta = 0.0f;
    y[0] = 1.0f;
    for (i=0; i<n; i++) {

        // generate complex sinusoid
        y[i] = cexpf(_Complex_I*theta);

        // compute phase error
        phase_error[i] = cargf(x[i]*conjf(y[i]));

        // update pll
        iirfilt_rrrf_execute(pll, phase_error[i], &theta);

        // print phase error
        if ((i)%d == 0 || i==n-1 || i==0)
            printf("%4u : phase error = %12.8f\n", i, phase_error[i]);
    }

    // destroy filter object
    iirfilt_rrrf_destroy(pll);

    // write output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"x = zeros(1,n);\n");
    fprintf(fid,"y = zeros(1,n);\n");
    for (i=0; i<n; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"e(%4u) = %12.4e;\n", i+1, phase_error[i]);
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,real(y));\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),t,imag(y));\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,e);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('phase error');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
