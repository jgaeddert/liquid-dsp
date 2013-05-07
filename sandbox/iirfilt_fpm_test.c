//
// iirfilt_fpm_test.c
//
// Compare fixed-point and floating-point implementations of the
// recursive (infinite impulse response) filters.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirfilt_fpm_test.m"

int main() {
    // options
    unsigned int order=3;   // filter order
    float fc=0.1f;          // cutoff frequency
    float f0=0.0f;          // center frequency
    float Ap=1.0f;          // pass-band ripple
    float As=40.0f;         // stop-band attenuation
    unsigned int n=50;      // number of samples
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_BUTTER;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_TF;

    // design filter objects from prototype
    iirfilt_crcf   filtf = iirfilt_crcf_create_prototype(  ftype, btype, format, order, fc, f0, Ap, As);
    iirfilt_crcq16 filtq = iirfilt_crcq16_create_prototype(ftype, btype, format, order, fc, f0, Ap, As);
    iirfilt_crcf_print(filtf);
    iirfilt_crcq16_print(filtq);

    unsigned int i;

    // allocate memory for data arrays
    float complex xf[n];
    float complex yf[n];
    cq16_t        xq[n];
    cq16_t        yq[n];

    // generate input signal (noisy sine wave with decaying amplitude)
    for (i=0; i<n; i++) {
#if 0
        xf[i] = cexpf((2*M_PI*0.057f*_Complex_I - 0.04f)*i);
        xf[i] += 0.1f*(randnf() + _Complex_I*randnf());
#else
        xf[i] = 1.0f;
#endif
        xq[i] = cq16_float_to_fixed(xf[i]);

        // run filter
        iirfilt_crcf_execute(filtf, xf[i], &yf[i]);
        iirfilt_crcq16_execute(filtq, xq[i], &yq[i]);
    }

    // destroy filter objects
    iirfilt_crcf_destroy(filtf);
    iirfilt_crcq16_destroy(filtq);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"n=%u;\n",n);
    //fprintf(fid,"nfft=%u;\n",nfft);
    fprintf(fid,"xf=zeros(1,n);\n");
    fprintf(fid,"xq=zeros(1,n);\n");
    fprintf(fid,"yf=zeros(1,n);\n");
    fprintf(fid,"yq=zeros(1,n);\n");
    //fprintf(fid,"H=zeros(1,nfft);\n");

    for (i=0; i<n; i++) {
        // save floating point values
        fprintf(fid,"xf(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(xf[i]), cimagf(xf[i]));
        fprintf(fid,"yf(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(yf[i]), cimagf(yf[i]));

        // save fixed-point values
        float complex x = cq16_fixed_to_float(xq[i]);
        float complex y = cq16_fixed_to_float(yq[i]);
        fprintf(fid,"xq(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x), cimagf(x));
        fprintf(fid,"yq(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y), cimagf(y));
    }
#if 0
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(H[i]), cimagf(H[i]));
#endif
    // plot output
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(yf),'-','Color',[0.5 0.5 0.5 ],'LineWidth',1,...\n");
    fprintf(fid,"       t,real(yq),'-','Color',[0   0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('output/real');\n");
    fprintf(fid,"  legend('float','fixed-point','position','northeast');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(yf),'-','Color',[0.5 0.5  0.5],'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(yq),'-','Color',[0   0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('output/imag');\n");
    fprintf(fid,"  legend('float','fixed-point','position','northeast');\n");
    fprintf(fid,"  grid on;\n");
#if 0
    // plot frequency response
    fprintf(fid,"f=0.5*[0:(nfft-1)]/nfft;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)));\n");
    fprintf(fid,"  axis([0 0.5 -3 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Pass band (dB)',0);\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)));\n");
    fprintf(fid,"  axis([0 0.5 -100 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Stop band (dB)',0);\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,180/pi*arg(H));\n");
    //fprintf(fid,"  axis([0 0.5 -100 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Phase (degrees)',0);\n");
#endif
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

