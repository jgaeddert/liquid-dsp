const char __docstr__[] = "Simulation of a real-valued phase-locked loop";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"nco_pll_real_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    type_str,         "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    phase_offset,     3.000, 'p', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    frequency_offset, 0.100, 'f', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    pll_bandwidth,    0.002, 'w', "phase-locked loop bandwidth", NULL);
    liquid_argparse_add(unsigned, n,                  512, 'n', "number of samples", NULL);
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
    nco_crcf_set_frequency(nco_tx, 0.3);
    nco_crcf_set_frequency(nco_rx, 0.3 + frequency_offset);
    nco_crcf_pll_set_bandwidth(nco_rx, pll_bandwidth);

    // write output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"n = %u; x = zeros(1,n); y = zeros(1,n); e = zeros(1,n);\n", n);

    // run basic simulation
    unsigned int i;
    float x, y, e; // input, output, phase error
    for (i=0; i<n; i++) {
        // compute input, output signals
        x = nco_crcf_sin(nco_tx);
        y = nco_crcf_sin(nco_rx);

        // compute phase error estimate
        e = 2 * nco_crcf_sin(nco_tx) * nco_crcf_cos(nco_rx);

        // compute true phase and frequency errors
        float err_phase = cargf(cexpf(_Complex_I*(nco_crcf_get_phase(nco_rx)-nco_crcf_get_phase(nco_tx))));
        float err_freq  = nco_crcf_get_frequency(nco_rx) - nco_crcf_get_frequency(nco_tx);

        // update receive pll
        nco_crcf_pll_step(nco_rx, e);

        // update nco objects
        nco_crcf_step(nco_tx);
        nco_crcf_step(nco_rx);

        // print phase error
        if ( (i+1)%50 == 0 || i==n-1 || i==0)
            printf("%4u : phase error = %12.8f, freq. error = %12.8f\n", i+1, err_phase, err_freq);
        // save output
        fprintf(fid,"x(%4u)=%12.4e;\n", i+1, x);
        fprintf(fid,"y(%4u)=%12.4e;\n", i+1, y);
        fprintf(fid,"e(%4u)=%12.4e;\n", i+1, err_phase);
        fprintf(fid,"f(%4u)=%12.4e;\n", i+1, err_freq);
    }
    nco_crcf_destroy(nco_tx);
    nco_crcf_destroy(nco_rx);

    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure('position',[100 100 600 800]);\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,x,'Color',[1 1 1]*0.8,...;\n");
    fprintf(fid,"       t,y,'Color',[0 0.2 0.5]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('NCO output');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,e,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('Phase error');\n");
    fprintf(fid,"  axis([0 n -pi pi]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,f,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('freq. error');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
