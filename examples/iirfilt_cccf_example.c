const char __docstr__[] =
"Complex infinite impulse response filter example. Demonstrates the"
" functionality of iirfilt with complex coefficients by designing a"
" filter with specified parameters and then filters noise.";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    liquid_argparse_add(char*, filename, "iirfilt_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,ftype_str, "butter", 't', "filter type: butter, cheby1, cheby2, ellip, bessel", NULL);
    liquid_argparse_add(char*,btype_str,     "LP", 'b', "filter transformation: LP, HP, BP, BS", NULL);
    liquid_argparse_add(unsigned, order,        3, 'O', "filter order", NULL);
    liquid_argparse_add(float,       fc,    0.20f, 'c', "pass-band cutoff frequency (low-pass prototype)", NULL);
    liquid_argparse_add(float,       f0,    0.25f, 'f', "center frequency (band-pass, band-stop)", NULL);
    liquid_argparse_add(float,       As,    60.0f, 's', "stopband attenuation [dB", NULL);
    liquid_argparse_add(float,       Ap,     1.0f, 'p', "passband ripple [dB]", NULL);
    liquid_argparse_add(bool,    use_tf,    false, 'T', "use direct transfer function rather than second-order sections", NULL);
    liquid_argparse_add(unsigned,     n,      128, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // filter type
    liquid_iirdes_filtertype ftype = LIQUID_IIRDES_BUTTER;
    if (strcmp(ftype_str,"butter")==0)
        ftype = LIQUID_IIRDES_BUTTER;
    else if (strcmp(ftype_str,"cheby1")==0)
        ftype = LIQUID_IIRDES_CHEBY1;
    else if (strcmp(ftype_str,"cheby2")==0)
        ftype = LIQUID_IIRDES_CHEBY2;
    else if (strcmp(ftype_str,"ellip")==0)
        ftype = LIQUID_IIRDES_ELLIP;
    else if (strcmp(ftype_str,"bessel")==0)
        ftype = LIQUID_IIRDES_BESSEL;
    else
        return liquid_error(LIQUID_EICONFIG,"unknown filter type '%s'", ftype_str);

    // band type
    liquid_iirdes_bandtype btype = LIQUID_IIRDES_LOWPASS;
    if (strcmp(btype_str,"LP")==0)
        btype = LIQUID_IIRDES_LOWPASS;
    else if (strcmp(btype_str,"HP")==0)
        btype = LIQUID_IIRDES_HIGHPASS;
    else if (strcmp(btype_str,"BP")==0)
        btype = LIQUID_IIRDES_BANDPASS;
    else if (strcmp(btype_str,"BS")==0)
        btype = LIQUID_IIRDES_BANDSTOP;
    else
        return liquid_error(LIQUID_EICONFIG,"iirdes_example, unknown band type '%s'", btype_str);

    // output format: second-order sections or transfer function
    liquid_iirdes_format format = use_tf ? LIQUID_IIRDES_TF : LIQUID_IIRDES_SOS;

    // validate input
    if (fc <= 0 || fc >= 0.5)
        fprintf(stderr,"error: cutoff frequency out of range\n");
    if (f0 < 0 || f0 > 0.5)
        fprintf(stderr,"error: center frequency out of range\n");
    if (Ap <= 0)
        return liquid_error(LIQUID_EICONFIG,"pass-band ripple out of range");
    if (As <= 0)
        return liquid_error(LIQUID_EICONFIG,"stop-band ripple out of range");

    // design filter from prototype
    iirfilt_cccf q = iirfilt_cccf_create_prototype(ftype, btype, format, order, fc, f0, Ap, As);
    iirfilt_cccf_print(q);

    unsigned int i;

    // allocate memory for data arrays
    LIQUID_VLA(liquid_float_complex, x, n);
    LIQUID_VLA(liquid_float_complex, y, n);

    // generate input signal (noisy sine wave with decaying amplitude)
    unsigned int wlen = (3*n)/4;
    for (i=0; i<n; i++) {
        // input signal (windowed noise)
        x[i]  = randnf() + _Complex_I*randnf();
        x[i] *= i < wlen ? liquid_hamming(i,wlen) : 0.0f;

        // run filter
        iirfilt_cccf_execute(q, x[i], &y[i]);
    }

    // compute two-sided frequency response
    unsigned int nfft=512;
    LIQUID_VLA(liquid_float_complex, H, nfft);
    for (i=0; i<nfft; i++) {
        float freq = (float)i / (float)nfft - 0.5f;
        iirfilt_cccf_freqresponse(q, freq, &H[i]);
    }

    // destroy filter object
    iirfilt_cccf_destroy(q);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);

    // save input, output arrays
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");
    for (i=0; i<n; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // save frequency response
    fprintf(fid,"nfft=%u;\n",nfft);
    fprintf(fid,"H=zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(H[i]), cimagf(H[i]));

    // plot time-domain output
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y),'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','filtered output');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y),'-','Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('input','filtered output');\n");
    fprintf(fid,"  grid on;\n");

    // plot spectral output
    fprintf(fid,"X = 20*log10(abs(fftshift(fft(x))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot([0:(n-1)]/n-0.5, X, 'Color', [1 1 1]*0.5,\n");
    fprintf(fid,"     [0:(n-1)]/n-0.5, Y, 'Color', [0 0.25 0.50]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('input','filtered output');\n");
    fprintf(fid,"grid on;\n");

    // plot ideal frequency response
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)));\n");
    fprintf(fid,"  axis([-0.5 0.5 -3 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Pass band (dB)');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)));\n");
    fprintf(fid,"  axis([-0.5 0.5 -100 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Stop band (dB)');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,180/pi*arg(H));\n");
    fprintf(fid,"  axis([-0.5 0.5 -180 180]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Phase (degrees)');\n");
    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

