//
// resamp2_crcf_filter_example.c
//
// Halfband filter example.
//
// SEE ALSO: resamp2_crcf_decim_example.c
//           resamp2_crcf_interp_example.c
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcf_filter_example.m"

int main() {
    unsigned int m=9;               // filter semi-length
    unsigned int num_samples=128;   // number of input samples
    float As=60.0f;                 // stop-band attenuation [dB]

    // derived values
    unsigned int h_len = 4*m+1;     // half-band filter length
    unsigned int N = num_samples + h_len;

    // arrays
    float complex x[N];             // input time series
    float complex y[N];             // output time series

    // generate input sequence
    unsigned int i;
    for (i=0; i<N; i++) {
        //x[i] = randnf() * cexpf(_Complex_I*M_PI*randf());
        x[i] = cexpf(_Complex_I*2*M_PI*0.072f*i) + 0.6f*cexpf(_Complex_I*2*M_PI*0.37f*i);
        x[i] *= (i < num_samples) ? hamming(i,num_samples) : 0.0f;
    }

    // create/print the half-band filter with a specified
    // stop-band attenuation
    resamp2_crcf q = resamp2_crcf_create(m,0.0f,As);
    resamp2_crcf_print(q);

    for (i=0; i<N; i++) {
        // run the filter
        resamp2_crcf_filter_execute(q, x[i], &y[i]);

        //printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    // clean up allocated objects
    resamp2_crcf_destroy(q);

    // 
    // export output file
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"N = %u;\n", N);
    fprintf(fid,"t = 0:(N-1);\n");

    // save results to output file
    for (i=0; i<N; i++) {
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'Color',[0.5 0.5 0.5],t-2*m,real(y),'Color',[0.0 0.5 0.25]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('original','filtered',1);");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),'Color',[0.5 0.5 0.5],t-2*m,imag(y),'Color',[0.0 0.25 0.5]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('original','filtered',1);");

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"g = 1 / sqrt( real(x*x') );\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x*g,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y*g,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,X,'Color',[0.5 0.5 0.5],f,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','filtered',1);");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
