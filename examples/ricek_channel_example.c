const char __docstr__[] =
"Rice-K fading generator example."
" This example generates correlated circular complex Gauss random variables"
" using an approximation to the ideal Doppler filter. The resulting Gauss"
" random variables are converted to Rice-K random variables using a simple"
" transformation. The resulting output file plots the filter's power"
" spectral density, the fading power envelope as a function of time, and the"
" distribution of Rice-K random variables alongside the theoretical PDF to"
" demonstrate that the technique is valid.";

#include <stdio.h>
#include <stdlib.h>
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
    liquid_argparse_add(char*, filename, "ricek_channel_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, h_len,        51, 'D', "Doppler filter length", NULL);
    liquid_argparse_add(float,    fd,          0.1, 'f', "maximum Doppler frequency", NULL);
    liquid_argparse_add(float,    K,             2, 'K', "Rice fading factor", NULL);
    liquid_argparse_add(float,    omega,         1, 'O', "mean power", NULL);
    liquid_argparse_add(float,    theta,         0, 't', "angle of arrival", NULL);
    liquid_argparse_add(unsigned, num_samples, 400, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (K < 0.0f)
        return liquid_error(LIQUID_EICONFIG,"fading factor K must be greater than zero");
    if (omega < 0.0f)
        return liquid_error(LIQUID_EICONFIG,"signal power Omega must be greater than zero");
    if (fd <= 0.0f || fd >= 0.5f)
        return liquid_error(LIQUID_EICONFIG,"Doppler frequency must be in (0,0.5)");
    if (h_len < 4)
        return liquid_error(LIQUID_EICONFIG,"Doppler filter length too small");
    if (num_samples == 0)
        return liquid_error(LIQUID_EICONFIG,"number of samples must be greater than zero");

    // allocate array for output samples
    liquid_float_complex * y = (liquid_float_complex*) malloc(num_samples*sizeof(liquid_float_complex));

    // generate Doppler filter coefficients
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_doppler(h_len, fd, K, theta, h);

    // normalize filter coefficients such that output Gauss random
    // variables have unity variance
    float std = 0.0f;
    unsigned int i;
    for (i=0; i<h_len; i++)
        std += h[i]*h[i];
    std = sqrtf(std);
    for (i=0; i<h_len; i++)
        h[i] /= std;

    // create Doppler filter from coefficients
    firfilt_crcf fdoppler = firfilt_crcf_create(h,h_len);

    // generate complex circular Gauss random variables
    liquid_float_complex v;    // circular Gauss random variable (uncorrelated)
    liquid_float_complex x;    // circular Gauss random variable (correlated w/ Doppler filter)
    float s   = sqrtf((omega*K)/(K+1.0));
    float sig = sqrtf(0.5f*omega/(K+1.0));
    for (i=0; i<num_samples; i++) {
        // generate complex Gauss random variable
        crandnf(&v);

        // push through Doppler filter
        firfilt_crcf_push(fdoppler, v);
        firfilt_crcf_execute(fdoppler, &x);

        // convert result to random variable with Rice-K distribution
        y[i] = _Complex_I*( crealf(x)*sig + s ) +
                          ( cimagf(x)*sig     );
    }

    // destroy filter object
    firfilt_crcf_destroy(fdoppler);

    // export results to file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"h_len       = %u;\n", h_len);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    // save filter coefficients
    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%6u) = %12.4e;\n", i+1, h[i]);

    // save samples
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // plot power spectral density of filter
    fprintf(fid,"nfft = min(1024, 2^(ceil(log2(h_len))+4));\n");
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H    = 20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H);\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Filter Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");

    // plot fading profile
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"plot(t,20*log10(abs(y)));\n");
    fprintf(fid,"xlabel('Normalized Time [t F_s]');\n");
    fprintf(fid,"ylabel('Fading Power Envelope [dB]');\n");
    fprintf(fid,"axis([0 num_samples -40 10]);\n");
    fprintf(fid,"grid on;\n");

    // plot distribution
    fprintf(fid,"[nn xx]   = hist(abs(y),15);\n");
    fprintf(fid,"bin_width = xx(2) - xx(1);\n");
    fprintf(fid,"ymax = max(abs(y));\n");
    fprintf(fid,"s    = %12.4e;\n", s);
    fprintf(fid,"sig  = %12.4e;\n", sig);
    fprintf(fid,"yp   = 1.1*ymax*[1:500]/500;\n");
    fprintf(fid,"pdf  = (yp/sig^2) .* exp(-(yp.^2+s^2)/(2*sig^2)) .* besseli(0,yp*s/sig^2);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(yp,pdf,'-', xx,nn/(num_samples*bin_width),'x');\n");
    fprintf(fid,"xlabel('Fading Magnitude');\n");
    fprintf(fid,"ylabel('Probability Density');\n");
    fprintf(fid,"legend('theory','data','location','northeast');\n");

    // close output file
    fclose(fid);
    printf("results written to %s\n", filename);

    // clean up allocated arrays
    free(y);
    return 0;
}

