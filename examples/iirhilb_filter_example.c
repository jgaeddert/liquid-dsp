const char __docstr__[] =
"Hilbert transform example. This example demonstrates the"
" functionality of iirhilbf (infinite impulse response Hilbert transform)"
" as a filter to remove the negative half of the spectrum.";

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
    liquid_argparse_add(char*, filename, "iirhilb_filter_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, order,   7, 'O', "filter order", NULL);
    liquid_argparse_add(unsigned,     n, 128, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int num_samples = n + 50;

    // create Hilbert transform objects
    iirhilbf q0 = iirhilbf_create_default(order);
    iirhilbf q1 = iirhilbf_create_default(order);
    iirhilbf_print(q0);

    // data arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);     // complex input
    LIQUID_VLA(float, y, num_samples);     // real output
    LIQUID_VLA(liquid_float_complex, z, num_samples);     // complex output

    // run transform
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate input
        x[i]  = 1.0f*cexpf( 0.12f*_Complex_I*2*M_PI*i) + // primary tone
                0.1f*cexpf( 0.17f*_Complex_I*2*M_PI*i) + // secondary tone
                0.2f*cexpf(-0.40f*_Complex_I*2*M_PI*i);  // tone in negative spectrum
        x[i] *= (i < n) ? 1.855f*liquid_hamming(i,n) : 0.0f;

        // convert to real
        iirhilbf_c2r_execute(q0, x[i], &y[i]);

        // convert back to complex
        iirhilbf_r2c_execute(q1, y[i], &z[i]);
    }

    // destroy Hilbert transform object
    iirhilbf_destroy(q0);
    iirhilbf_destroy(q1);

    // 
    // export results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"t = 0:(num_samples-1);\n");

    for (i=0; i<num_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e + %12.4ej;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n",           i+1, y[i]);
        fprintf(fid,"z(%3u) = %12.4e + %12.4ej;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       t,imag(x),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,y,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  ylabel('original/real');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,real(z),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       t,imag(z),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");

    // plot results
    fprintf(fid,"nfft=4096;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/n,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y/n,nfft))));\n");
    fprintf(fid,"Z=20*log10(abs(fftshift(fft(z/n,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'LineWidth',1,'Color',[0.50 0.50 0.50],...\n");
    fprintf(fid,"             f,Y,'LineWidth',2,'Color',[0.00 0.50 0.25],...\n");
    fprintf(fid,"             f,Z,'LineWidth',1,'Color',[0.00 0.25 0.50]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/cplx','transformed/real','regenerated/cplx','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
