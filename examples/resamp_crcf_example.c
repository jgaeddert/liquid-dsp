const char __docstr__[] =
"Demonstration of arbitrary resampler object whereby an input signal"
" is resampled at an arbitrary rate.";

#include <stdio.h>
#include <stdlib.h>
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
    liquid_argparse_add(char*, filename, "resamp_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    r,   1.10, 'r', "resampling rate (output/input)", NULL);
    liquid_argparse_add(unsigned, m,     13, 'm', "resampling filter semi-length (filter delay)", NULL);
    liquid_argparse_add(float,    As,  60.0, 'a', "resampling filter stop-band attenuation [dB]", NULL);
    liquid_argparse_add(float,    bw,  0.45, 'w', "resampling filter bandwidth (normalized)", NULL);
    liquid_argparse_add(unsigned, npfb,  64, 'b', "number of filters in bank (timing resolution)", NULL);
    liquid_argparse_add(unsigned, n,    400, 'n', "number of input samples", NULL);
    liquid_argparse_add(float,    fc, 0.044, 'f', "complex sinusoid frequency", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (r <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"resampling rate must be greater than zero");
    if (m == 0)
        return liquid_error(LIQUID_EICONFIG,"filter semi-length must be greater than zero");
    if (bw == 0.0f || bw >= 0.5f)
        return liquid_error(LIQUID_EICONFIG,"filter bandwidth must be in (0,0.5)");
    if (As < 0.0f)
        return liquid_error(LIQUID_EICONFIG,"filter stop-band attenuation must be greater than zero");
    if (npfb == 0)
        return liquid_error(LIQUID_EICONFIG,"filter bank size must be greater than zero");
    if (n == 0)
        return liquid_error(LIQUID_EICONFIG,"number of input samples must be greater than zero");

    unsigned int i;

    // number of input samples (zero-padded)
    unsigned int nx = n + m;

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1 * nx * r) + 4;

    // arrays
    LIQUID_VLA(liquid_float_complex, x, nx);
    LIQUID_VLA(liquid_float_complex, y, y_len);

    // create resampler
    resamp_crcf q = resamp_crcf_create(r,m,bw,As,npfb);
    resamp_crcf_print(q);

    // generate input signal
    float wsum = 0.0f;
    for (i=0; i<nx; i++) {
        // compute window
        float w = i < n ? liquid_kaiser(i, n, 10.0f) : 0.0f;

        // apply window to complex sinusoid
        x[i] = cexpf(_Complex_I*2*M_PI*fc*i) * w;

        // accumulate window
        wsum += w;
    }

    // resample
    unsigned int ny=0;
    // execute on block of samples
    resamp_crcf_execute_block(q, x, nx, y, &ny);

    // clean up allocated objects
    resamp_crcf_destroy(q);

    // 
    // analyze resulting signal
    //

    // check that the actual resampling rate is close to the target
    float r_actual = (float)ny / (float)nx;
    float fy = fc / r;      // expected output frequency

    // run FFT and ensure that carrier has moved and that image
    // frequencies and distortion have been adequately suppressed
    unsigned int nfft = 1 << liquid_nextpow2(ny);
    LIQUID_VLA(liquid_float_complex, yfft, nfft);   // fft input
    LIQUID_VLA(liquid_float_complex, Yfft, nfft);   // fft output
    for (i=0; i<nfft; i++)
        yfft[i] = i < ny ? y[i] : 0.0f;
    fft_run(nfft, yfft, Yfft, LIQUID_FFT_FORWARD, 0);
    fft_shift(Yfft, nfft);  // run FFT shift

    // find peak frequency
    float Ypeak = 0.0f;
    float fpeak = 0.0f;
    float max_sidelobe = -1e9f;     // maximum side-lobe [dB]
    float main_lobe_width = 0.07f;  // TODO: figure this out from Kaiser's equations
    for (i=0; i<nfft; i++) {
        // normalized output frequency
        float f = (float)i/(float)nfft - 0.5f;

        // scale FFT output appropriately
        float Ymag = 20*log10f( cabsf(Yfft[i] / (r * wsum)) );

        // find frequency location of maximum magnitude
        if (Ymag > Ypeak || i==0) {
            Ypeak = Ymag;
            fpeak = f;
        }

        // find peak side-lobe value, ignoring frequencies
        // within a certain range of signal frequency
        if ( fabsf(f-fy) > main_lobe_width )
            max_sidelobe = Ymag > max_sidelobe ? Ymag : max_sidelobe;
    }

    // print results and check frequency location
    printf("  desired resampling rate   :   %12.8f\n", r);
    printf("  measured resampling rate  :   %12.8f    (%u/%u)\n", r_actual, ny, nx);
    printf("  peak spectrum             :   %12.8f dB (expected 0.0 dB)\n", Ypeak);
    printf("  peak frequency            :   %12.8f    (expected %-12.8f)\n", fpeak, fy);
    printf("  max sidelobe              :   %12.8f dB (expected at least %.2f dB)\n", max_sidelobe, -As);

    // export results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"m=%u;\n", m);
    fprintf(fid,"npfb=%u;\n",  npfb);
    fprintf(fid,"r=%12.8f;\n", r);
    fprintf(fid,"n=%u;\n", n);

    fprintf(fid,"nx = %u;\n", nx);
    fprintf(fid,"x = zeros(1,nx);\n");
    for (i=0; i<nx; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    fprintf(fid,"ny = %u;\n", ny);
    fprintf(fid,"y = zeros(1,ny);\n");
    for (i=0; i<ny; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"tx=[0:(length(x)-1)];\n");
    fprintf(fid,"ty=[0:(length(y)-1)]/r-m;\n");
    fprintf(fid,"figure('Color','white','position',[500 500 600 800]);\n");
    fprintf(fid,"subplot(4,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,real(y),'-s','Color',[0.5 0 0],    'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(4,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,imag(y),'-s','Color',[0 0.5 0],    'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"nfft=2^nextpow2(max(nx,ny));\n");
    fprintf(fid,"%% estimate PSD, normalize by array length\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x,nfft)/length(x))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y,nfft)/length(y))));\n");
    fprintf(fid,"G=max(X);\n");
    fprintf(fid,"X=X-G;\n");
    fprintf(fid,"Y=Y-G;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"if r>1, fx = f/r; fy = f;   %% interpolated\n");
    fprintf(fid,"else,   fx = f;   fy = f*r; %% decimated\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"subplot(4,1,3:4);\n");
    fprintf(fid,"  plot(fx,X,'Color',[0.5 0.5 0.5],fy,Y,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');");
    fprintf(fid,"  axis([-0.5 0.5 -120 20]);\n");

    fclose(fid);
    printf("results written to %s\n",filename);

    printf("done.\n");
    return 0;
}
