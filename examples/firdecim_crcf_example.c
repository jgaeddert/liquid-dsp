const char __docstr__[] =
"This example demonstrates the interface to the firdecim (finite"
" impulse response decimator) family of objects.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firdecim_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,             6, 'M', "decimation factor", NULL);
    liquid_argparse_add(unsigned, m,             8, 'm', "filter delay", NULL);
    liquid_argparse_add(float,    As,           60, 'a', "filter stop-band attenuation", NULL);
    liquid_argparse_add(unsigned, num_samples, 120, 'n', "number of samples (after decim)", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    if (M < 2)
        return liquid_error(LIQUID_EICONFIG,"decim factor must be greater than 1");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"filter delay must be greater than 0");
    if (As <= 0.0)
        return liquid_error(LIQUID_EICONFIG,"stop-band attenuation must be greater than zero");
    if (num_samples < 1)
        return liquid_error(LIQUID_EICONFIG,"must have at least one sample");

    // data arrays
    LIQUID_VLA(liquid_float_complex, x, M*num_samples); // number of samples before decimation
    LIQUID_VLA(liquid_float_complex, y, num_samples); // number of samples after decimation

    // initialize input array
    unsigned int i;
    unsigned int w_len = (unsigned int)(0.9*M*num_samples);
    float f0 = 0.017f;
    float f1 = 0.021f;
    for (i=0; i<M*num_samples; i++) {
        x[i]  = 0.6f*cexpf(_Complex_I*2*M_PI*f0*i);
        x[i] += 0.4f*cexpf(_Complex_I*2*M_PI*f1*i);
        x[i] *= (i < w_len) ? liquid_hamming(i,w_len) : 0;
    }

    // create decimator object and set scale
    firdecim_crcf decim = firdecim_crcf_create_kaiser(M, m, As);
    firdecim_crcf_set_scale(decim, 1.0f/(float)M);

    // execute decimator
    firdecim_crcf_execute_block(decim, x, num_samples, y);

    // destroy decimator object
    firdecim_crcf_destroy(decim);

    // 
    // export results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"M  = %u;\n", M);
    fprintf(fid,"m  = %u;\n", m);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    // inputs
    for (i=0; i<M*num_samples; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    // outputs
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // plot results
    fprintf(fid,"figure('position',[100 100 600 800]);\n");
    fprintf(fid,"tx = [0:(M*num_samples-1)];\n");
    fprintf(fid,"ty = [0:(  num_samples-1)]*M - M*m;\n");
    fprintf(fid,"nfft=3*2^nextpow2(M*num_samples);\n");
    fprintf(fid,"fx = [0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fy = fx/M;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(  x,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(M*y,nfft))));\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-',ty,real(y),'s');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-M*m M*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Input sample index');\n");
    fprintf(fid,"  ylabel('Real');\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-',ty,imag(y),'s');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-M*m M*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Input sample index');\n");
    fprintf(fid,"  ylabel('Imag');\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(fx,X,'Color',[0.5 0.5 0.5],fy,Y,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-0.5 0.5 -40 60]);\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  legend('original/real','transformed/decimated','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
