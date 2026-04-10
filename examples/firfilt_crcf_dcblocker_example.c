const char __docstr__[] =
"This example demonstrates how to create a DC-blocking non-recursive"
" (finite impulse response) filter.";

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
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firfilt_crcf_dcblocker_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_samples,1200, 'n', "number of samples", NULL);
    liquid_argparse_add(unsigned, m,            25, 'm', "prototype filter semi-length", NULL);
    liquid_argparse_add(float,    As,           30, 's', "prototype filter stop-band suppression", NULL);
    liquid_argparse_parse(argc,argv);

    // design filter from prototype
    firfilt_crcf q = firfilt_crcf_create_dc_blocker(m,As);
    firfilt_crcf_print(q);

    // allocate memory for data arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);   // original input
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // input with DC offset
    LIQUID_VLA(liquid_float_complex, z, num_samples);   // DC-blocked result

    // generate signals
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // original input signal
        x[i] = cexpf( (0.170f*i + 1e-4f*i*i)*_Complex_I );

        // add DC offset
        y[i] = x[i] + 2.0f*cexpf( 0.007f*_Complex_I*i );

        // run filter to try to remove DC offset
        firfilt_crcf_push   (q, y[i]);
        firfilt_crcf_execute(q, &z[i]);
    }

    // destroy filter object
    firfilt_crcf_destroy(q);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"num_samples=%u;\n",num_samples);
    fprintf(fid,"x=zeros(1,num_samples);\n");
    fprintf(fid,"y=zeros(1,num_samples);\n");

    // save input, output arrays
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }

    // plot output
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(x),'-','Color',[0 0.2 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('original input');\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -3 3]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,real(y),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y),'-','Color',[0 0.5 0.2],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('input with DC offset');\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -3 3]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,real(z),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(z),'-','Color',[0 0.2 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('DC-blocked output');\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -3 3]);\n");
    fprintf(fid,"  grid on;\n");

    // close output file
    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}

