//
// resamp_crcf_example.c
//
// Demonstration of arbitrary resampler object whereby an input signal
// is resampled at an arbitrary rate.
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp_crcf_example.m"

// print usage/help message
void usage()
{
    printf("Usage: %s [OPTION]\n", __FILE__);
    printf("  h     : print help\n");
    printf("  r     : resampling rate (output/input),    default: 1.1\n");
    printf("  m     : filter semi-length (delay),        default: 13\n");
    printf("  b     : filter bandwidth, 0 < b < 0.5,     default: 0.4\n");
    printf("  s     : filter stop-band attenuation [dB], default: 60\n");
    printf("  p     : filter bank size,                  default: 64\n");
    printf("  n     : number of input samples,           default: 400\n");
    printf("  f     : input signal frequency,            default: 0.044\n");
}

int main(int argc, char*argv[])
{
    // options
    float r           = 1.1f;   // resampling rate (output/input)
    unsigned int m    = 13;     // resampling filter semi-length (filter delay)
    float As          = 60.0f;  // resampling filter stop-band attenuation [dB]
    float bw          = 0.45f;  // resampling filter bandwidth
    unsigned int npfb = 64;     // number of filters in bank (timing resolution)
    unsigned int n    = 400;    // number of input samples
    float fc          = 0.044f; // complex sinusoid frequency

    int dopt;
    while ((dopt = getopt(argc,argv,"hr:m:b:s:p:n:f:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();            return 0;
        case 'r':   r    = atof(optarg); break;
        case 'm':   m    = atoi(optarg); break;
        case 'b':   bw   = atof(optarg); break;
        case 's':   As   = atof(optarg); break;
        case 'p':   npfb = atoi(optarg); break;
        case 'n':   n    = atoi(optarg); break;
        case 'f':   fc   = atof(optarg); break;
        default:
            exit(1);
        }
    }

    // validate input
    if (r <= 0.0f) {
        fprintf(stderr,"error: %s, resampling rate must be greater than zero\n", argv[0]);
        exit(1);
    } else if (m == 0) {
        fprintf(stderr,"error: %s, filter semi-length must be greater than zero\n", argv[0]);
        exit(1);
    } else if (bw == 0.0f || bw >= 0.5f) {
        fprintf(stderr,"error: %s, filter bandwidth must be in (0,0.5)\n", argv[0]);
        exit(1);
    } else if (As < 0.0f) {
        fprintf(stderr,"error: %s, filter stop-band attenuation must be greater than zero\n", argv[0]);
        exit(1);
    } else if (npfb == 0) {
        fprintf(stderr,"error: %s, filter bank size must be greater than zero\n", argv[0]);
        exit(1);
    } else if (n == 0) {
        fprintf(stderr,"error: %s, number of input samples must be greater than zero\n", argv[0]);
        exit(1);
    }

    unsigned int i;

    // number of input samples (zero-padded)
    unsigned int nx = n + 2*m + 1;

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1 * nx * r) + 4;

    // arrays
    float complex x[nx];
    float complex y[y_len];

    // create resampler
    resamp_crcf q = resamp_crcf_create(r,m,bw,As,npfb);

    // generate input signal
    for (i=0; i<nx; i++)
        x[i] = (i < n) ? cexpf(_Complex_I*2*M_PI*fc*i) *kaiser(i,nx,10.0f,0.0f) : 0.0f;

    // resample
    unsigned int ny=0;
    unsigned int nw;
    for (i=0; i<nx; i++) {
        // execute resampler, storing in output buffer
        resamp_crcf_execute(q, x[i], &y[ny], &nw);

        // increment output size
        ny += nw;
    }

    // clean up allocated objects
    resamp_crcf_destroy(q);

    // print results
    printf("desired resampling rate :   %12.8f\n", r);
    printf("measured resampling rate:   %12.8f (%u/%u)\n", (float)ny/(float)nx, ny, nx);


    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"m=%u;\n", m);
    fprintf(fid,"npfb=%u;\n",  npfb);
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
    fprintf(fid,"nfft=512;\n");
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
    fprintf(fid,"axis([-0.5 0.5 -100 20]);\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"tx=[0:(length(x)-1)];\n");
    fprintf(fid,"ty=[0:(length(y)-1)]/r-m;\n");
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
