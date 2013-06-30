//
// resamp2_crcq16_decim_example.c
//
// Halfband decimator.  This example demonstrates the interface to the
// decimating halfband resampler in fixed-point math.
//
// SEE ALSO: resamp2_crcf_decim_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcq16_decim_example.m"

// print usage/help message
void usage()
{
    printf("%s [options]\n", __FILE__);
    printf("  h     :   print help\n");
    printf("  n     :   number of output samples,     default: 64\n");
    printf("  m     :   filter semi-length,           default: 5\n");
    printf("  s     :   filter stop-band attenuation, default: 60 dB\n");
}

int main(int argc, char*argv[])
{
    unsigned int m=5;               // filter semi-length
    float fc=0.037f;                // input tone frequency
    unsigned int num_samples = 64;  // number of output samples
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
    cq16_t x[2*num_samples]; // input array
    cq16_t y[  num_samples]; // output array

    // generate input
    unsigned int w_len = 2*num_samples - 4*m;   // window length
    float beta = 8.0f;                          // kaiser window factor
    float w_sum = 0.0f;                         // gain due to window
    for (i=0; i<2*num_samples; i++) {
        // compute windowing function and keep track of gain
        float w = (i < w_len ? kaiser(i,w_len,beta,0) : 0.0f);
        w_sum += w;

        // compute windowed complex sinusoid
        x[i] = cq16_float_to_fixed( w*cexpf(_Complex_I*2*M_PI*fc*i) );
    }

    // create/print the half-band resampler
    resamp2_crcq16 q = resamp2_crcq16_create(m,0,As);
    resamp2_crcq16_print(q);
    unsigned int delay = resamp2_crcq16_get_delay(q);

    // run the resampler
    for (i=0; i<num_samples; i++) {
        // execute the decimator
        resamp2_crcq16_decim_execute(q, &x[2*i], &y[i]);

        // print results to screen
        printf("y(%3u) = %8.4f + j*%8.4f;\n",
                i+1,
                q16_fixed_to_float(y[i].real),
                q16_fixed_to_float(y[i].imag));
    }

    // destroy half-band resampler
    resamp2_crcq16_destroy(q);

    // 
    // export results
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"delay      =%u;\n", delay);
    fprintf(fid,"w_sum      =%12.8f;\n", w_sum);
        
    // save results to output file
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.8f + 1j*%12.8f;\n", 2*i+1,      q16_fixed_to_float(x[2*i+0].real),      q16_fixed_to_float(x[2*i+0].imag));
        fprintf(fid,"x(%3u) = %12.8f + 1j*%12.8f;\n", 2*i+2,      q16_fixed_to_float(x[2*i+1].real),      q16_fixed_to_float(x[2*i+1].imag));
        fprintf(fid,"y(%3u) = %12.8f + 1j*%12.8f;\n", i+1,   0.5f*q16_fixed_to_float(y[i    ].real), 0.5f*q16_fixed_to_float(y[i    ].imag));
    }

    // plot time series
    fprintf(fid,"tx =  0:(2*num_samples-1);\n");
    fprintf(fid,"ty = [0:(  num_samples-1)]*2 - delay;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,real(y),'-s','Color',[0.5 0.0 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"  axis([-delay 2*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,imag(y),'-s','Color',[0.0 0.5 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"  axis([-delay 2*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('imag');\n");

    // plot spectrum
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"g = 1/w_sum;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(  x*g,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(2*y*g,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,  X,'LineWidth',1,  'Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"     f/2,Y,'LineWidth',1.5,'Color',[0.1 0.3 0.5]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated','location','northeast');");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
