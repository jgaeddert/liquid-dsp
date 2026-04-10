const char __docstr__[] =
"Hilbert transform: 1:2 complex-to-real interpolator.  This"
" example demonstrates the functionality of firhilb (finite"
" impulse response Hilbert transform) interpolator which converts"
" a complex time series into a real one with twice the number of"
" samples.  The input is a complex-valued sinusoid of N samples."
" The output is a real-valued sinusoid of 2*N samples.";

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
    liquid_argparse_add(char*,    filename, "firhilb_interp_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,             7, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    As,           60, 's', "filter stop-band suppression", NULL);
    liquid_argparse_add(float,    fc,         0.31, 'c', "signal center frequency", NULL);
    liquid_argparse_add(unsigned, num_samples, 120, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // data arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);   // complex input
    LIQUID_VLA(float, y, 2*num_samples);         // real output

    // initialize input array
    unsigned int i;
    for (i=0; i<num_samples; i++)
        x[i] = cexpf(_Complex_I*2*M_PI*fc*i) + 0.6f*cexpf(_Complex_I*2*M_PI*1.1*fc*i);

    // create Hilbert transform object
    firhilbf q = firhilbf_create(m,As);

    // execute transform (interpolator) to compute real signal
    firhilbf_interp_execute_block(q, x, num_samples, y);

    // destroy Hilbert transform object
    firhilbf_destroy(q);

    printf("firhilb interpolated %u complex samples to %u real samples\n",
            num_samples, 2*num_samples);

    // 
    // export results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n", 2*i+1, y[2*i+0]);
        fprintf(fid,"y(%3u) = %12.4e;\n", 2*i+2, y[2*i+1]);
    }

    // plot results
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"wx = 1.8534/num_samples*hamming(length(x)).'; %% x window\n");
    fprintf(fid,"wy = 1.8534/num_samples*hamming(length(y)).'; %% y window\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*wx,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*wy,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fi=[0:(nfft-1)]/(2*nfft);\n");
    fprintf(fid,"figure; plot(fi,X,'Color',[0.5 0.5 0.5],f,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/complex','transformed/interpolated','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
