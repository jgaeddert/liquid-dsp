//
// iir_filter_crcf_example.c
//
// Complex infinite impulse response filter example. Demonstrates
// the functionality of iir_filter by designing a low-order
// prototype (e.g. Butterworth) and using it to filter a noisy
// signal.  The filter coefficients are real, but the input and
// output arrays are complex.  The filter order and cutoff
// frequency are specified at the beginning.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iir_filter_crcf_example.m"

int main() {
    // options
    unsigned int order=4;   // filter order
    float fc=0.1f;          // cutoff frequency
    float f0=0.0f;          // center frequency
    float Ap=1.0f;          // pass-band ripple
    float As=40.0f;         // stop-band attenuation
    unsigned int n=128;     // number of samples
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_ELLIP;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;

    // derived values : compute filter length
    unsigned int N = order; // effective order
    // filter order effectively doubles for band-pass, band-stop
    // filters due to doubling the number of poles and zeros as
    // a result of filter transformation
    if (btype == LIQUID_IIRDES_BANDPASS ||
        btype == LIQUID_IIRDES_BANDSTOP)
    {
        N *= 2;
    }
    unsigned int r = N%2;       // odd/even order
    unsigned int L = (N-r)/2;   // filter semi-length

    // allocate memory for filter coefficients
    unsigned int h_len = (format == LIQUID_IIRDES_SOS) ? 3*(L+r) : N+1;
    float b[h_len];
    float a[h_len];

    // design filter
    iirdes(ftype, btype, format, order, fc, f0, Ap, As, b, a);

    // create filter object
    iir_filter_crcf f = NULL;
    if (format == LIQUID_IIRDES_SOS)
        f = iir_filter_crcf_create_sos(b,a,L+r);
    else
        f = iir_filter_crcf_create(b,h_len, a,h_len);
    iir_filter_crcf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    unsigned int i;
    float complex x;
    float complex y;
    for (i=0; i<n; i++) {
        // generate input signal (noisy sine wave with decaying amplitude)
        x = cexpf((2*M_PI*0.057f*_Complex_I - 0.04f)*i);
        cawgn(&x,0.1f);

        // run filter
        iir_filter_crcf_execute(f, x, &y);

        //printf("%4u : %12.8f + j*%12.8f\n", i, crealf(y), cimagf(y));
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x), cimagf(x));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y), cimagf(y));
    }

    // output filter coefficients using extra precision
    if (format == LIQUID_IIRDES_SOS) {
        // convert to transfer function form by convolving second-order
        // sections (effectively this expands the polynomial)
        fprintf(fid,"b = [1];\n");
        fprintf(fid,"a = [1];\n");
        for (i=0; i<L+r; i++) {
            fprintf(fid,"b = conv(b,[%16.8e %16.8e %16.8e]);\n", b[3*i+0], b[3*i+1], b[3*i+2]);
            fprintf(fid,"a = conv(a,[%16.8e %16.8e %16.8e]);\n", a[3*i+0], a[3*i+1], a[3*i+2]);
        }

    } else {
        for (i=0; i<h_len; i++) {
            fprintf(fid,"b(%3u) = %16.8e;\n", i+1, b[i]);
            fprintf(fid,"a(%3u) = %16.8e;\n", i+1, a[i]);
        }
    }

    // plot filter response
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"freqz(b,a);\n");

    // plot output
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y),'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','filtered output',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y),'-','Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('input','filtered output',1);\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // destroy filter object
    iir_filter_crcf_destroy(f);

    printf("done.\n");
    return 0;
}

