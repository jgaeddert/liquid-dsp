//
// iirhilb_filter_example.c
//
// Hilbert transform example. This example demonstrates the
// functionality of iirhilbf (infinite impulse response Hilbert transform)
// as a filter to remove the negative half of the spectrum.
//
// SEE ALSO: iirhilb_interp_example.c
//           iirhilb_example.c
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirhilb_filter_example.m"

int main() {
    unsigned int n  = 7;                // Hilbert filter order
    float        fc = 0.123456;         // signal center frequency
    unsigned int num_input_samples=128; // number of samples

    // derived values
    unsigned int num_total_samples = num_input_samples + 50; // allow for filter settling

    // create Hilbert transform object
    iirhilbf q0 = iirhilbf_create_default(n); // complex to real
    iirhilbf q1 = iirhilbf_create_default(n); // real to complex
    iirhilbf_print(q0);

    // data arrays
    float complex x[num_total_samples];     // complex input
    float         y[num_total_samples];     // real output
    float complex z[num_total_samples];     // complex output

    // run transform
    unsigned int i;
    for (i=0; i<num_total_samples; i++) {
        // generate input
        x[i]  = cexpf(_Complex_I*2*M_PI*fc*i) +
                cexpf(_Complex_I*2*M_PI*fc*i*1.3f)*0.1f;
        x[i] *= (i < num_input_samples) ? 1.855f*hamming(i,num_input_samples) : 0.0f;

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
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_input_samples=%u;\n", num_input_samples);
    fprintf(fid,"num_total_samples=%u;\n", num_total_samples);
    fprintf(fid,"t = 0:(num_total_samples-1);\n");

    for (i=0; i<num_total_samples; i++) {
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
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,y,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  ylabel('original/real');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,real(z),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       t,imag(z),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");

    // plot results
    fprintf(fid,"nfft=4096;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/num_input_samples,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y/num_input_samples,nfft))));\n");
    fprintf(fid,"Z=20*log10(abs(fftshift(fft(z/num_input_samples,nfft))));\n");
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
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
