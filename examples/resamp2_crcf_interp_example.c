//
// resamp2_crcf_interp_example.c
//
// Halfband interpolator.  This example demonstrates the interface to the
// interpolating halfband resampler.  A low-frequency input sinusoid is
// generated and fed into the interpolator one sample at a time,
// producing two outputs at each iteration.  The results are written to
// an output file.
//
// SEE ALSO: resamp2_crcf_interp_example.c
//           interp_crcf_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcf_interp_example.m"

// print usage/help message
void usage()
{
    printf("%s [options]\n", __FILE__);
    printf("  h     :   print help\n");
    printf("  n     :   number of input samples,      default: 64\n");
    printf("  m     :   filter semi-length,           default: 5\n");
    printf("  s     :   filter stop-band attenuation, default: 60 dB\n");
}

int main(int argc, char*argv[])
{
    unsigned int m=5;               // filter semi-length
    float fc=0.074f;                // input tone frequency
    unsigned int num_samples = 64;  // number of input samples
    float As=60.0f;                 // stop-band attenuation [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:m:s:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                    return 0;
        case 'n':   num_samples = atoi(optarg); break;
        case 'm':   m           = atoi(optarg); break;
        case 's':   As          = atof(optarg); break;
        default:
            exit(1);
        }
    }
    unsigned int i;

    // allocate arrays
    float complex x[  num_samples]; // input array
    float complex y[2*num_samples]; // output array

    // generate input
    unsigned int w_len = num_samples - 2*m; // window length
    float beta = 8.0f;                      // kaiser window factor
    float w_sum = 0.0f;                     // gain due to window
    for (i=0; i<num_samples; i++) {
        // compute windowing function and keep track of gain
        float w = (i < w_len ? kaiser(i,w_len,beta,0) : 0.0f);
        w_sum += w;

        // compute windowed complex sinusoid
        x[i] = w * cexpf(_Complex_I*2*M_PI*fc*i);
    }

    // create/print the half-band resampler
    resamp2_crcf q = resamp2_crcf_create(m,0,As);
    resamp2_crcf_print(q);
    unsigned int delay = resamp2_crcf_get_delay(q);

    // run the resampler
    for (i=0; i<num_samples; i++) {
        // execute the interpolator
        resamp2_crcf_interp_execute(q, x[i], &y[2*i]);

        // print results to screen
        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // destroy half-band resampler
    resamp2_crcf_destroy(q);

    // 
    // export results
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"delay      =%u;\n", delay);
    fprintf(fid,"w_sum     =%12.8f;\n", w_sum);
        
    for (i=0; i<num_samples; i++) {
        // save results to output file
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", i+1,   crealf(x[i    ]), cimagf(x[i    ]));
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", 2*i+1,      crealf(y[2*i+0]),      cimagf(y[2*i+0]));
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", 2*i+2,      crealf(y[2*i+1]),      cimagf(y[2*i+1]));
    }

    // plot time series
    fprintf(fid,"tx =  0:(  num_samples-1);\n");
    fprintf(fid,"ty = [0:(2*num_samples-1)]/2 - (delay+1)/2;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,real(y),'-s','Color',[0.5 0.0 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','interpolated','location','northeast');");
    fprintf(fid,"  axis([-delay num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,imag(y),'-s','Color',[0.0 0.5 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','interpolated','location','northeast');");
    fprintf(fid,"  axis([-delay num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('imag');\n");

    // plot spectrum
    fprintf(fid,"nfft=max([1024, 2^(1+nextpow2(num_samples))]);\n");
    fprintf(fid,"g = 1/w_sum;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x*g,  nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y*g/2,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(  f,X,'LineWidth',1,  'Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"     2*f,Y,'LineWidth',1.5,'Color',[0.1 0.3 0.5]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','interpolated','location','northeast');");
    fprintf(fid,"axis([-1 1 -100 10]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
