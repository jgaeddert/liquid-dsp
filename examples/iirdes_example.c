const char __docstr__[] =
"Tests infinite impulse response (IIR) digital filter design.";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirdes_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,ftype_str, "butter", 't', "filter type: butter, cheby1, cheby2, ellip, bessel", NULL);
    liquid_argparse_add(char*,btype_str,     "LP", 'b', "filter transformation: LP, HP, BP, BS", NULL);
    liquid_argparse_add(unsigned, order,        3, 'n', "filter order", NULL);
    liquid_argparse_add(float,       fc,    0.20f, 'c', "pass-band cutoff frequency (low-pass prototype)", NULL);
    liquid_argparse_add(float,       f0,    0.25f, 'f', "center frequency (band-pass, band-stop)", NULL);
    liquid_argparse_add(float,       As,    60.0f, 's', "stopband attenuation [dB", NULL);
    liquid_argparse_add(float,       Ap,     1.0f, 'p', "passband ripple [dB]", NULL);
    liquid_argparse_add(bool,    use_tf,    false, 'T', "use direct transfer function rather than second-order sections", NULL);
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
    unsigned int r = N % 2;     // odd/even order
    unsigned int L = (N-r)/2;   // filter semi-length

    // allocate memory for filter coefficients
    unsigned int h_len = (format == LIQUID_IIRDES_SOS) ? 3*(L+r) : N+1;
    LIQUID_VLA(float, b, h_len);
    LIQUID_VLA(float, a, h_len);

    // design filter
    liquid_iirdes(ftype, btype, format, order, fc, f0, Ap, As, b, a);

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    fprintf(fid,"n=%u;\n", order);
    fprintf(fid,"r=%u;\n", r);
    fprintf(fid,"L=%u;\n", L);
    fprintf(fid,"nfft=1024;\n");

    unsigned int i;
    if (format == LIQUID_IIRDES_TF) {
        // print coefficients
        for (i=0; i<=N; i++) printf("a[%3u] = %12.8f;\n", i, a[i]);
        for (i=0; i<=N; i++) printf("b[%3u] = %12.8f;\n", i, b[i]);

        fprintf(fid,"a = zeros(1,n+1);\n");
        fprintf(fid,"b = zeros(1,n+1);\n");
        for (i=0; i<=N; i++) {
            fprintf(fid,"a(%3u) = %12.4e;\n", i+1, a[i]);
            fprintf(fid,"b(%3u) = %12.4e;\n", i+1, b[i]);
        }
        fprintf(fid,"\n");
        fprintf(fid,"H = fft(b,nfft)./fft(a,nfft);\n");
        fprintf(fid,"H = fftshift(H);\n");
        fprintf(fid,"%% group delay\n");
        fprintf(fid,"c = conv(b,fliplr(conj(a)));\n");
        fprintf(fid,"cr = c.*[0:(length(c)-1)];\n");
        fprintf(fid,"t0 = fftshift(fft(cr,nfft));\n");
        fprintf(fid,"t1 = fftshift(fft(c, nfft));\n");
        fprintf(fid,"polebins = find(abs(t1)<1e-6);\n");
        fprintf(fid,"t0(polebins)=0;\n");
        fprintf(fid,"t1(polebins)=1;\n");
        fprintf(fid,"gd = real(t0./t1) - length(a) + 1;\n");

    } else {
        float * B = b;
        float * A = a;
        // print coefficients
        printf("B [%u x 3] :\n", L+r);
        for (i=0; i<L+r; i++)
            printf("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);
        printf("A [%u x 3] :\n", L+r);
        for (i=0; i<L+r; i++)
            printf("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);

        unsigned int j;
        for (i=0; i<L+r; i++) {
            for (j=0; j<3; j++) {
                fprintf(fid,"B(%3u,%3u) = %16.8e;\n", i+1, j+1, B[3*i+j]);
                fprintf(fid,"A(%3u,%3u) = %16.8e;\n", i+1, j+1, A[3*i+j]);
            }
        }
        fprintf(fid,"\n");
        fprintf(fid,"H = ones(1,nfft);\n");
        fprintf(fid,"gd = zeros(1,nfft);\n");
        fprintf(fid,"t0 = zeros(1,nfft);\n");
        fprintf(fid,"t1 = zeros(1,nfft);\n");
        fprintf(fid,"for i=1:(L+r),\n");
        fprintf(fid,"    H = H .* fft(B(i,:),nfft)./fft(A(i,:),nfft);\n");
        fprintf(fid,"    %% group delay\n");
        fprintf(fid,"    c = conv(B(i,:),fliplr(conj(A(i,:))));\n");
        fprintf(fid,"    cr = c.*[0:4];\n");
        fprintf(fid,"    t0 = fftshift(fft(cr,nfft));\n");
        fprintf(fid,"    t1 = fftshift(fft(c, nfft));\n");
        fprintf(fid,"    polebins = find(abs(t1)<1e-6);\n");
        fprintf(fid,"    t0(polebins)=0;\n");
        fprintf(fid,"    t1(polebins)=1;\n");
        fprintf(fid,"    gd = gd + real(t0./t1) - 2;\n");
        fprintf(fid,"end;\n");
        fprintf(fid,"H = fftshift(H);\n");
    }

    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure;\n");

    // plot magnitude response (detail)
    fprintf(fid,"subplot(3,1,1),\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)),'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 -4 1]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Filter PSD [dB]');\n");

    // plot magnitude response (full range)
    fprintf(fid,"subplot(3,1,2),\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)),'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 -100 10]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Filter PSD [dB]');\n");

    // plot group delay
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,gd,'-','Color',[0 0.5 0],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 0 ceil(1.1*max(gd))]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Group delay [samples]');\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

