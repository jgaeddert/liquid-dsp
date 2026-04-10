const char __docstr__[] =
"Hilbert transform example.  This example demonstrates the"
" functionality of iirhilbf (finite impulse response Hilbert transform)"
" which converts a complex time series into a real one and then back.";

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
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirhilb_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,ftype_str, "butter", 't', "filter type: butter, cheby1, cheby2, ellip, bessel", NULL);
    liquid_argparse_add(unsigned,     n,        3, 'O', "filter order", NULL);
    liquid_argparse_add(float,       fc,    0.20f, 'c', "pass-band cutoff frequency (low-pass prototype)", NULL);
    liquid_argparse_add(float,       As,    60.0f, 's', "stopband attenuation [dB", NULL);
    liquid_argparse_add(float,       Ap,     1.0f, 'p', "passband ripple [dB]", NULL);
    liquid_argparse_add(unsigned, num_samples,180, 'n', "number of samples", NULL);
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

    // validate input
    if (fc <= 0 || fc >= 0.5)
        return liquid_error(LIQUID_EICONFIG,"cutoff frequency out of range");
    if (Ap <= 0)
        return liquid_error(LIQUID_EICONFIG,"pass-band ripple out of range");
    if (As <= 0)
        return liquid_error(LIQUID_EICONFIG,"stop-band ripple out of range");

    // derived values
    unsigned int num_total_samples = num_samples + 50; // allow for filter settling

    // create Hilbert transform objects
    iirhilbf qi = iirhilbf_create(ftype,n,Ap,As);    // interpolator
    iirhilbf qd = iirhilbf_create(ftype,n,Ap,As);    // decimator
    iirhilbf_print(qi);

    // data arrays
    LIQUID_VLA(liquid_float_complex, x, num_total_samples);   // complex input
    LIQUID_VLA(float, y, 2*num_total_samples);   // real output
    LIQUID_VLA(liquid_float_complex, z, num_total_samples);   // complex output

    // initialize input array
    unsigned int i;
    for (i=0; i<num_total_samples; i++) {
        x[i]  = cexpf(_Complex_I*2*M_PI*fc*i) +
                cexpf(_Complex_I*2*M_PI*fc*i*1.3f)*0.1f;
        x[i] *= (i < num_samples) ? 1.855f*liquid_hamming(i,num_samples) : 0.0f;
    }

    // execute interpolator (complex to real conversion)
    iirhilbf_interp_execute_block(qi, x, num_total_samples, y);
    
    // execute decimator (real to complex conversion)
    iirhilbf_decim_execute_block(qd, y, num_total_samples, z);

    // destroy Hilbert transform object
    iirhilbf_destroy(qi);
    iirhilbf_destroy(qd);

    // 
    // export results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"num_total_samples=%u;\n", num_total_samples);
    fprintf(fid,"tx = 0:(num_total_samples-1);\n");
    fprintf(fid,"ty = [0:(2*num_total_samples-1)]/2;\n");
    fprintf(fid,"tz = tx;\n");

    for (i=0; i<num_total_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e + %12.4ej;\n",   i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n",           2*i+1, y[2*i+0]);
        fprintf(fid,"y(%3u) = %12.4e;\n",           2*i+2, y[2*i+1]);
        fprintf(fid,"z(%3u) = %12.4e + %12.4ej;\n",   i+1, crealf(z[i]), cimagf(z[i]));
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       tx,imag(x),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(ty,y,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  ylabel('original/real');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(tz,real(z),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       tz,imag(z),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");

    // plot results
    fprintf(fid,"nfft=4096;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/num_samples,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y/num_samples,nfft))));\n");
    fprintf(fid,"Z=20*log10(abs(fftshift(fft(z/num_samples,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f+0.5,X,'LineWidth',1,'Color',[0.50 0.50 0.50],...\n");
    fprintf(fid,"             f*2,  Y,'LineWidth',2,'Color',[0.00 0.50 0.25],...\n");
    fprintf(fid,"             f+0.5,Z,'LineWidth',1,'Color',[0.00 0.25 0.50]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1.0 1.0 -80 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/cplx','transformed/real','regenerated/cplx','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
