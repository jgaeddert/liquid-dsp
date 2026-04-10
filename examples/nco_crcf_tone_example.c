const char __docstr__[] = "Generate tone at low frequency and test phase response";

#include <stdio.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "nco_crcf_tone_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    type_str,      "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    fc,   0.000241852307, 'r', "center frequency", NULL);
    liquid_argparse_add(unsigned, num_samples,    2400, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (strcmp(type_str,"nco") && strcmp(type_str,"vco"))
        return liquid_error(LIQUID_EICONFIG,"invalid nco type '%s' (must be either 'nco' or 'vco')", type_str);

    // create the NCO object
    nco_crcf q = nco_crcf_create(strcmp(type_str,"nco")==0 ? LIQUID_NCO : LIQUID_VCO);
    nco_crcf_set_frequency(q, 2*M_PI*fc);
    nco_crcf_print(q);

    unsigned int i;
    LIQUID_VLA(liquid_float_complex, x, num_samples);
    for (i=0; i<num_samples; i++) {
        nco_crcf_cexpf(q, &x[i]);
        nco_crcf_step(q);
    }

    // destroy objects
    nco_crcf_destroy(q);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"x = zeros(1,n);\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%6u) = %12.9f + %12.9fj;\n", i+1, crealf(x[i]), cimagf(x[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, real(x), '-', 'LineWidth',1.5,...\n");
    fprintf(fid,"     t, imag(x), '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Signal');\n");
    fprintf(fid,"ylabel('Time [sample index]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([0 n -1 1]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}
