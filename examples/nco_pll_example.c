const char __docstr__[] =
"This example demonstrates how the use the nco/pll object"
" (numerically-controlled oscillator with phase-locked loop) interface for"
" tracking to a complex sinusoid.  The loop bandwidth, phase offset, and"
" other parameter can be specified via the command-line interface.";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"nco_pll_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    type_str,         "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    phase_offset,     0.000, 'p', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    frequency_offset, 0.400, 'f', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    pll_bandwidth,    0.003, 'w', "phase-locked loop bandwidth", NULL);
    liquid_argparse_add(unsigned, num_samples,        512, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (strcmp(type_str,"nco") && strcmp(type_str,"vco"))
        return liquid_error(LIQUID_EICONFIG,"invalid nco type '%s' (must be either 'nco' or 'vco')", type_str);

    // objects
    liquid_ncotype type = strcmp(type_str,"nco")==0 ? LIQUID_NCO : LIQUID_VCO;
    nco_crcf nco_tx = nco_crcf_create(type);
    nco_crcf nco_rx = nco_crcf_create(type);

    // initialize objects
    nco_crcf_set_phase(nco_tx, phase_offset);
    nco_crcf_set_frequency(nco_tx, frequency_offset);
    nco_crcf_pll_set_bandwidth(nco_rx, pll_bandwidth);

    // generate input
    LIQUID_VLA(liquid_float_complex, x, num_samples);
    LIQUID_VLA(liquid_float_complex, y, num_samples);
    LIQUID_VLA(float, phase_error, num_samples);

    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate complex sinusoid
        nco_crcf_cexpf(nco_tx, &x[i]);

        // update nco
        nco_crcf_step(nco_tx);
    }

    // run loop
    for (i=0; i<num_samples; i++)
    {
        // generate input
        nco_crcf_cexpf(nco_rx, &y[i]);

        // update rx nco object
        nco_crcf_step(nco_rx);

        // compute phase error
        phase_error[i] = cargf(x[i]*conjf(y[i]));

        // update pll
        nco_crcf_pll_step(nco_rx, phase_error[i]);

        // print phase error
        if ( (i+1)%50 == 0 || i==num_samples-1 || i==0)
            printf("%4u : phase error = %12.8f\n", i+1, phase_error[i]);
    }
    nco_crcf_destroy(nco_tx);
    nco_crcf_destroy(nco_rx);

    // write output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"x = zeros(1,n);\n");
    fprintf(fid,"y = zeros(1,n);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"e(%4u) = %12.4e;\n", i+1, phase_error[i]);
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure('color','white','position',[100 100 1200 600]);\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,real(x),'Color',[1 1 1]*0.8);\n");
    fprintf(fid,"  plot(t,real(y),'Color',[0 0.2 0.5]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,imag(x),'Color',[1 1 1]*0.8);\n");
    fprintf(fid,"  plot(t,imag(y),'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,e,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('phase error');\n");
    fprintf(fid,"  axis([0 n -pi pi]);\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
