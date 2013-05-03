//
// firhilb_example.c
//
// Hilbert transform example.  This example demonstrates the
// functionality of firhilbf (finite impulse response Hilbert transform)
// which converts a real time series into a complex one and then back.
//
// SEE ALSO: firhilb_interp_example.c
//           firhilb_example.c
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firhilb_example.m"

int main() {
    unsigned int m = 5;                 // Hilbert filter semi-length
    float As       = 60.0f;             // stop-band attenuation [dB]
    float fc       = 0.123456;          // signal center frequency
    unsigned int num_input_samples=128; // number of samples

    // derived values
    unsigned int h_len = 4*m+1;         // filter length
    unsigned int num_total_samples = num_input_samples + h_len;

    // create Hilbert transform object
    firhilbf q = firhilbf_create(m,As);
    firhilbf_print(q);

    // data arrays
    float         x[num_total_samples]; // real input
    float complex y[num_total_samples]; // complex output
    float         z[num_total_samples]; // real output

    // initialize input array
    unsigned int i;
    for (i=0; i<num_total_samples; i++) {
        x[i] = cosf(2*M_PI*fc*i);
        x[i] *= (i < num_input_samples) ? 1.855f*hamming(i,num_input_samples) : 0.0f;
    }

    for (i=0; i<num_total_samples; i++) {
        // execute real-to-complex conversion
        firhilbf_r2c_execute(q, x[i], &y[i]);

        // execute complex-to-real conversion
        firhilbf_c2r_execute(q, y[i], &z[i]);

        //printf("y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // destroy Hilbert transform object
    firhilbf_destroy(q);

    // 
    // export results to file
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_input_samples=%u;\n", num_input_samples);
    fprintf(fid,"num_total_samples=%u;\n", num_total_samples);
    fprintf(fid,"t = 0:(num_total_samples-1);\n");

    for (i=0; i<num_total_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e;\n",           i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e + %12.4ej;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u) = %12.4e;\n",           i+1, z[i]);
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,x,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  ylabel('original/real');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,real(y),'Color',[0.00 0.25 0.50],'LineWidth',1.3,...\n");
    fprintf(fid,"       t,imag(y),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  ylabel('transformed/complex');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,z,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('regenerated/real');\n");
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
    fprintf(fid,"legend('original/real','transformed/complex','regenerated/real','location','northeast');");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
