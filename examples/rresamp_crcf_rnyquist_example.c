const char __docstr__[] =
"Demonstration of matched filter interpolator and decimator running at"
" rational rate that is only slightly higher than occupied bandwidth."
" The resulting constellation has minimal inter-symbol interference and"
" is normalized to unity gain.";

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "rresamp_crcf_rnyquist_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, interp,        5, 'i', "output rate (interpolation factor)", NULL);
    liquid_argparse_add(unsigned, decim,         4, 'd', "input rate (decimation factor)", NULL);
    liquid_argparse_add(unsigned, m,            15, 'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(float,    beta,       0.15, 'w', "filter excess bandwidth", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800, 'n', "number of symbols to generate", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (interp == 0 || interp > 1000)
        return liquid_error(LIQUID_EICONFIG,"interpolation rate must be in [1,1000]");
    if (decim == 0 || decim > 1000)
        return liquid_error(LIQUID_EICONFIG,"decimation rate must be in [1,1000]");
    if (decim > interp)
        return liquid_error(LIQUID_EICONFIG,"this example requires interp > decim");

    // create resampler objects
    rresamp_crcf q0 = rresamp_crcf_create_prototype(LIQUID_FIRFILT_ARKAISER,interp,decim,m,beta);
    rresamp_crcf q1 = rresamp_crcf_create_prototype(LIQUID_FIRFILT_ARKAISER,decim,interp,m,beta);
    rresamp_crcf_print(q0);

    // input/output buffers
    LIQUID_VLA(liquid_float_complex, buf_x, decim); // input
    LIQUID_VLA(liquid_float_complex, buf_y, interp); // interp
    LIQUID_VLA(liquid_float_complex, buf_z, decim); // decim

    // generate input symbols
    unsigned int i, n = 0;
    FILE * fid = fopen(filename, "w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"x = []; y = []; z = [];\n");
    float g = M_SQRT1_2;
    while (n < num_symbols) {
        n += decim;
        if (n > num_symbols-2*m)
            g = 0;
        // generate symbols
        for (i=0; i<decim; i++)
            buf_x[i] = g * ( (rand() & 1 ? 1 : -1 ) + ( rand() & 1 ? 1 : -1 )*_Complex_I);

        // run resampler and write interp output samples
        rresamp_crcf_execute(q0, buf_x, buf_y);

        // run through matched filter
        rresamp_crcf_execute(q1, buf_y, buf_z);

        // write results to file
        for (i=0; i<decim; i++) fprintf(fid,"x(end+1) = %12.4e + 1i*%12.4e;\n", crealf(buf_x[i]), cimagf(buf_x[i]));
        for (i=0; i<interp; i++) fprintf(fid,"y(end+1) = %12.4e + 1i*%12.4e;\n", crealf(buf_y[i]), cimagf(buf_y[i]));
        for (i=0; i<decim; i++) fprintf(fid,"z(end+1) = %12.4e + 1i*%12.4e;\n", crealf(buf_z[i]), cimagf(buf_z[i]));
    }
    fprintf(fid,"ny = length(y);\n");
    fprintf(fid,"ty = 0:(ny-1);\n");
    fprintf(fid,"nfft = 2^nextpow2(ny);\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure('position',[1 1 800 600]);\n");
    fprintf(fid,"subplot(2,2,1:2)\n");
    fprintf(fid,"  plot(ty,real(y),ty,imag(y));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([0 ny -1.6 1.6]);\n");
    fprintf(fid,"  xlabel('output sample index');\n");
    fprintf(fid,"  ylabel('signal');\n");
    fprintf(fid,"  legend('real','imag');\n");
    fprintf(fid,"subplot(2,2,3)\n");
    fprintf(fid,"  plot(z,'x');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.2);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"subplot(2,2,4)\n");
    fprintf(fid,"  plot(f,20*log10(abs(fftshift(fft(y/sqrt(ny),nfft)))));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-0.5 0.5 -100 20]);\n");
    fprintf(fid,"  xlabel('Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fclose(fid);

    // clean up allocated objects
    rresamp_crcf_destroy(q0);
    rresamp_crcf_destroy(q1);
    printf("results written to %s\n",filename);
    return 0;
}
