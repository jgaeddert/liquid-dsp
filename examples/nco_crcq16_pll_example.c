char __docstr__[] =
"This example demonstrates how the use the nco/pll object"
" (numerically-controlled oscillator with phase-locked loop) interface for"
" tracking to a complex sinusoid.  The loop bandwidth, phase offset, and"
" other parameter can be specified via the command-line interface."
" All operations are in 16-bit fixed-point format";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"nco_crcq16_pll_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    type_str,         "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    phase_offset,     0.700, 'p', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    frequency_offset, 0.200, 'f', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    pll_bandwidth,    0.010, 'w', "phase-locked loop bandwidth", NULL);
    liquid_argparse_add(unsigned, num_samples,        512, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (strcmp(type_str,"nco") && strcmp(type_str,"vco"))
        return liquid_error(LIQUID_EICONFIG,"invalid nco type '%s' (must be either 'nco' or 'vco')", type_str);

    // objects
    int type = strcmp(type_str,"nco")==0 ? LIQUID_NCO : LIQUID_VCO;
    nco_crcq16 nco_tx = nco_crcq16_create(type);
    nco_crcq16 nco_rx = nco_crcq16_create(type);

    // initialize objects
    nco_crcq16_set_phase(nco_tx,         q16_angle_float_to_fixed(phase_offset));
    nco_crcq16_set_frequency(nco_tx,     q16_angle_float_to_fixed(frequency_offset));
    nco_crcq16_pll_set_bandwidth(nco_rx, pll_bandwidth);

    // generate input
    cq16_t x[num_samples];
    cq16_t y[num_samples];
    q16_t phase_error[num_samples];

    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate complex sinusoid
        nco_crcq16_cexpf(nco_tx, &x[i]);

        // update nco
        nco_crcq16_step(nco_tx);
    }

    // run loop
    for (i=0; i<num_samples; i++)
    {
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
        if ( (i+1)%50 == 0 || i==num_samples-1 || i==0)
            printf("%4u : phase error = %12.8f\n", i+1, q16_angle_fixed_to_float(phase_error[i]));
    }
    nco_crcq16_destroy(nco_tx);
    nco_crcq16_destroy(nco_rx);

    // write output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"x = zeros(1,n);\n");
    fprintf(fid,"y = zeros(1,n);\n");
    for (i=0; i<num_samples; i++) {
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
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
