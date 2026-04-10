const char __docstr__[] =
"Hilbert transform: 2:1 real-to-complex decimator.  This example"
" demonstrates the functionality of firhilb (finite impulse response"
" Hilbert transform) decimator which converts a real time series"
" into a complex one with half the number of samples.  The input"
" is a real-valued sinusoid of N samples. The output is a complex-"
"valued sinusoid of N/2 samples.";

#include <stdio.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firhilb_decim_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,             5, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    As,           60, 's', "filter stop-band suppression", NULL);
    liquid_argparse_add(float,    fc,          0.1, 'c', "signal center frequency", NULL);
    liquid_argparse_add(unsigned, num_samples, 240, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // data arrays
    LIQUID_VLA(float, x, 2*num_samples);         // real input
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // complex output

    // initialize input array
    unsigned int i;
    for (i=0; i<2*num_samples; i++)
        x[i] = cosf(2*M_PI*fc*i) + 0.6f*sinf(2*M_PI*1.1*fc*i);

    // create Hilbert transform object
    firhilbf q = firhilbf_create(m,As);

    // execute transform (decimator) to compute complex signal
    firhilbf_decim_execute_block(q, x, num_samples, y);

    // destroy Hilbert transform object
    firhilbf_destroy(q);

    printf("firhilb decimated %u real samples to %u complex samples\n",
            2*num_samples, num_samples);

    // export results to file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e;\n", 2*i+1, x[2*i+0]);
        fprintf(fid,"x(%3u) = %12.4e;\n", 2*i+2, x[2*i+1]);
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // plot results
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"wx = 1.8534/num_samples*hamming(length(x)).'; %% x window\n");
    fprintf(fid,"wy = 1.8534/num_samples*hamming(length(y)).'; %% y window\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*wx,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*wy,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fd=0.5*f + 0.25;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],fd,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/real','transformed/decimated','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
