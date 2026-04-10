const char __docstr__[] =
"This example demonstrates the iirinterp object (IIR interpolator) interface.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirinterp_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned,           M,   4, 'M', "interpolation factor", NULL);
    liquid_argparse_add(unsigned,       order,   8, 'O', "filter order", NULL);
    liquid_argparse_add(unsigned, num_samples,  64, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // create interpolator from prototype
    iirinterp_crcf q = iirinterp_crcf_create_default(M,order);

    // derived values
    float delay = iirinterp_crcf_groupdelay(q,0.0f);

    // generate input signal and interpolate
    LIQUID_VLA(liquid_float_complex, x, num_samples); // input samples
    LIQUID_VLA(liquid_float_complex, y, M*num_samples); // output samples
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // input signal (sinusoidal chirp)
        x[i] = cexpf(_Complex_I*(-0.17f*i + 0.9*i*i/(float)num_samples));

        // apply window
        x[i] *= (i < num_samples-5) ? liquid_hamming(i,num_samples) : 0.0f;

        // push through interpolator
        iirinterp_crcf_execute(q, x[i], &y[M*i]);
    }

    // destroy interpolator object
    iirinterp_crcf_destroy(q);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"delay = %f;\n", delay);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"x = zeros(1,  num_samples);\n");
    fprintf(fid,"y = zeros(1,M*num_samples);\n");

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<M*num_samples; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(  num_samples-1)];\n");
    fprintf(fid,"ty = [0:(M*num_samples-1)]/M - delay;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(tx,real(x),'-s','MarkerSize',3,ty,real(y),'-s','MarkerSize',1);\n");
    fprintf(fid,"    legend('input','interp','location','northeast');\n");
    fprintf(fid,"    axis([0 num_samples -1.2 1.2]);\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(tx,imag(x),'-s','MarkerSize',3,ty,imag(y),'-s','MarkerSize',1);\n");
    fprintf(fid,"    legend('input','interp','location','northeast');\n");
    fprintf(fid,"    axis([0 num_samples -1.2 1.2]);\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    grid on;\n");

    // power spectral density
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"fx   = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"fy   = M*fx;\n");
    fprintf(fid,"X    = 20*log10(abs(fftshift(fft(x  ,nfft))));\n");
    fprintf(fid,"Y    = 20*log10(abs(fftshift(fft(y/M,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(fx,X,'LineWidth',2, fy,Y,'LineWidth',1);\n");
    fprintf(fid,"legend('input','interp','location','northeast');\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
