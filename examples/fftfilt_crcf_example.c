const char __docstr__[] =
"Complex FFT-based finite impulse response filter example. This example"
" demonstrates the functionality of firfilt by designing a low-order"
" prototype and using it to filter a noisy signal.  The filter coefficients"
" are real, but the input and output arrays are complex. The filter order"
" and cutoff frequency are specified at the beginning, and the result is"
" compared to the regular corresponding firfilt_crcf output.";

#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,        filename, "fftfilt_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned int, h_len,      57,    'N', "filter length", NULL);
    liquid_argparse_add(float,        fc,         0.10f, 'f', "cutoff frequency", NULL);
    liquid_argparse_add(float,        As,         60.0f, 'a', "stop-band attenuation", NULL);
    liquid_argparse_add(unsigned int, n,          64,    'n', "number of samples per block", NULL);
    liquid_argparse_add(unsigned int, num_blocks, 6,     'b', "total number of blocks", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int num_samples = n * num_blocks;

    // design filter
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_kaiser(h_len, fc, As, 0, h);

    // design FFT-based filter and scale to bandwidth
    fftfilt_crcf q0 = fftfilt_crcf_create(h, h_len, n);
    fftfilt_crcf_set_scale(q0, 2.0f*fc);

    // design regular FIR filter
    firfilt_crcf q1 = firfilt_crcf_create(h, h_len);
    firfilt_crcf_set_scale(q1, 2.0f*fc);

    unsigned int i;

    // allocate memory for data arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);   // input
    LIQUID_VLA(liquid_float_complex, y0, num_samples);  // output (fftfilt)
    LIQUID_VLA(liquid_float_complex, y1, num_samples);  // output (firfilt)

    // generate input signal (noise)
    for (i=0; i<num_samples; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // run signal through fft-based filter in blocks
    for (i=0; i<num_blocks; i++)
        fftfilt_crcf_execute(q0, &x[i*n], &y1[i*n]);

    // run signal through regular filter one sample at a time
    for (i=0; i<num_samples; i++) {
        // run filter
        firfilt_crcf_push(q1, x[i]);
        firfilt_crcf_execute(q1, &y0[i]);
    }

    // destroy filter objects
    fftfilt_crcf_destroy(q0);
    firfilt_crcf_destroy(q1);

    // compute error norm
    float rmse = 0.0f;
    printf("  %6s : %8s : %8s (%8s), %8s : %8s (%8s)\n",
            "index",
            "re{fir}", "re{fft}", "re{err}",
            "im{fir}", "im{fft}", "im{err}");
    for (i=0; i<num_samples; i++) {
        liquid_float_complex e = y0[i] - y1[i];
        printf("  %6u : %8.5f : %8.5f (%8.5f), %8.5f : %8.5f (%8.5f)\n",
                i,
                crealf(y0[i]), crealf(y1[i]), crealf(e),
                cimagf(y0[i]), cimagf(y1[i]), cimagf(e));

        // accumulate error
        rmse += crealf( e*conjf(e) );
    }
    // normalize RMS error
    rmse = sqrtf( rmse/(float)num_samples );
    printf("  rmse : %12.4e\n", rmse);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"h_len=%u;\n", h_len);
    fprintf(fid,"n=%u;\n",num_samples);
    fprintf(fid,"x =zeros(1,n);\n");
    fprintf(fid,"y0=zeros(1,n);\n");
    fprintf(fid,"y1=zeros(1,n);\n");

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x( %4u) = %12.4e + j*%12.4e;\n", i+1, crealf( x[i]), cimagf( x[i]));
        fprintf(fid,"y0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y0[i]), cimagf(y0[i]));
        fprintf(fid,"y1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y1[i]), cimagf(y1[i]));
    }

    // plot output
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(y0),'-','Color',[0 0.5 0.2],'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y1),'-','Color',[0 0.2 0.5],'LineWidth',1);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('fftfilt','firfilt','location','northeast');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(y0),'-','Color',[0 0.5 0.2],'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y1),'-','Color',[0 0.2 0.5],'LineWidth',1);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('fftfilt','firfilt','location','northeast');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

