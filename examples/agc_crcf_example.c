const char __docstr__[] =
"Automatic gain control example demonstrating its transient response.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char *,   filename,    "agc_crcf_example.m",'o', "output filename",NULL);
    liquid_argparse_add(float,    bt,          0.01f,               'b', "agc loop bandwidth", NULL);
    liquid_argparse_add(float,    gamma,       0.001f,              'g', "initial signal level", NULL);
    liquid_argparse_add(unsigned, num_samples, 2000,                'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (bt < 0.0f)
        return liquid_error(LIQUID_EICONFIG,"bandwidth must be positive");
    if (num_samples == 0)
        return liquid_error(LIQUID_EICONFIG,"number of samples must be greater than zero");
    
    unsigned int i;

    // create objects
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);
    //agc_crcf_set_scale(q, 0.5f);

    LIQUID_VLA(liquid_float_complex, x, num_samples);   // input
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // output
    LIQUID_VLA(float, rssi, num_samples);        // received signal strength

    // print info
    agc_crcf_print(q);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = gamma * cexpf(_Complex_I*2*M_PI*0.0193f*i);

    // run agc
    for (i=0; i<num_samples; i++) {
        // apply gain
        agc_crcf_execute(q, x[i], &y[i]);

        // retrieve signal level [dB]
        rssi[i] = agc_crcf_get_rssi(q);
    }

    // destroy AGC object
    agc_crcf_destroy(q);

    // 
    // export results
    //
    FILE* fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid,"%% %s: auto-generated file\n\n",filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"rssi(%4u)  = %12.4e;\n", i+1, rssi[i]);
    }

    // plot results
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t, real(x), '-', 'Color',[0 0.2 0.5],...\n");
    fprintf(fid,"       t, imag(x), '-', 'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('input');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t, real(y), '-', 'Color',[0 0.2 0.5],...\n");
    fprintf(fid,"       t, imag(y), '-', 'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('output');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,rssi,'-','LineWidth',1.2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('rssi [dB]');\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

