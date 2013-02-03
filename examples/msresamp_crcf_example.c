//
// msresamp_crcf_example.c
//
// Demonstration of the multi-stage arbitrary resampler
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "msresamp_crcf_example.m"

// print usage/help message
void usage()
{
    printf("Usage: %s [OPTION]\n", __FILE__);
    printf("  h     : print help\n");
    printf("  r     : resampling rate (output/input), default: 0.23175\n");
    printf("  s     : stop-band attenuation [dB],     default: 60\n");
    printf("  n     : number of input samples,        default: 400\n");
    printf("  f     : input signal frequency,         default: 0.017\n");
}

int main(int argc, char*argv[])
{
    // options
    float r=0.23175f;       // resampling rate (output/input)
    float As=60.0f;         // resampling filter stop-band attenuation [dB]
    unsigned int n=400;     // number of input samples
    float fc=0.017f;        // complex sinusoid frequency

    int dopt;
    while ((dopt = getopt(argc,argv,"hr:s:n:f:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();            return 0;
        case 'r':   r   = atof(optarg); break;
        case 's':   As  = atof(optarg); break;
        case 'n':   n   = atoi(optarg); break;
        case 'f':   fc  = atof(optarg); break;
        default:
            exit(1);
        }
    }

    // validate input
    if (n == 0) {
        fprintf(stderr,"error: %s, number of input samples must be greater than zero\n", argv[0]);
        exit(1);
    } else if (r <= 0.0f) {
        fprintf(stderr,"error: %s, resampling rate must be greater than zero\n", argv[0]);
        exit(1);
    } else if ( fabsf(log2f(r)) > 10 ) {
        fprintf(stderr,"error: %s, resampling rate unreasonable\n", argv[0]);
        exit(1);
    }

    unsigned int i;

    // create multi-stage arbitrary resampler object
    msresamp_crcf q = msresamp_crcf_create(r,As);
    msresamp_crcf_print(q);
    float delay = msresamp_crcf_get_delay(q);

    // number of input samples (zero-padded)
    unsigned int nx = n + (int)ceilf(delay) + 10;

    // output buffer with extra padding for good measure
    unsigned int ny_alloc = (unsigned int) (2*(float)nx * r);  // allocation for output

    // allocate memory for arrays
    float complex x[nx];
    float complex y[ny_alloc];

    // generate input signal
    float wsum = 0.0f;
    for (i=0; i<nx; i++) {
        // compute window
        float w = i < n ? kaiser(i, n, 10.0f, 0.0f) : 0.0f;

        // apply window to complex sinusoid
        x[i] = cexpf(_Complex_I*2*M_PI*fc*i) * w;

        // accumulate window
        wsum += w;
    }

    // run resampler
    unsigned int ny;
    msresamp_crcf_execute(q, x, nx, y, &ny);

    // print basic results
    printf("input samples   : %u\n", nx);
    printf("output samples  : %u\n", ny);
    printf("delay           : %f samples\n", delay);

    // clean up allocated objects
    msresamp_crcf_destroy(q);

    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"delay=%f;\n", delay);
    fprintf(fid,"r=%12.8f;\n", r);

    fprintf(fid,"nx = %u;\n", nx);
    fprintf(fid,"x = zeros(1,nx);\n");
    for (i=0; i<nx; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    fprintf(fid,"ny = %u;\n", ny);
    fprintf(fid,"y = zeros(1,ny);\n");
    for (i=0; i<ny; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"nfft=2^nextpow2(max(nx,ny));\n");
    fprintf(fid,"%% estimate PSD, normalize by array length\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x,nfft)/length(x))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y,nfft)/length(y))));\n");
    fprintf(fid,"G=max(X);\n");
    fprintf(fid,"X=X-G;\n");
    fprintf(fid,"Y=Y-G;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"if r>1, fx = f/r; fy = f;   %% interpolated\n");
    fprintf(fid,"else,   fx = f;   fy = f*r; %% decimated\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"plot(fx,X,'Color',[0.5 0.5 0.5],fy,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','resampled','location','northeast');");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"tx=[0:(length(x)-1)];\n");
    fprintf(fid,"ty=[0:(length(y)-1)]/r-delay;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,real(y),'-s','Color',[0.5 0 0],    'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,imag(y),'-s','Color',[0 0.5 0],    'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");

    fclose(fid);
    printf("results written to %s\n",OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
