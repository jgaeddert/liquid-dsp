const char __docstr__[] =
"This example demonstrates the halfband resampler cenetered at the"
" quarter sample rate to split the signal into positive and negative"
" frequency bands. Two distinct narrow-band signals are generated; one"
" at a positive frequency and one at a negative frequency. The resamp2"
" object is run as a filter to separate the two about the zero-frequency"
" center point.";

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
    liquid_argparse_add(char*, filename, "resamp2_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,            12, 'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(float,    As,           60, 'a', "resampling filter stop-band attenuation [dB]", NULL);
    liquid_argparse_add(unsigned, num_samples, 400, 'n', "number of low-rate samples", NULL);
    liquid_argparse_parse(argc,argv);

    // allocate memory for data arrays
    LIQUID_VLA(liquid_float_complex, x, num_samples);  // input signal
    LIQUID_VLA(liquid_float_complex, y0, num_samples);  //
    LIQUID_VLA(liquid_float_complex, y1, num_samples);  //

    // generate the two signals
    iirfilt_crcf lowpass = iirfilt_crcf_create_lowpass(6,0.02);
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // signal at negative frequency: tone
        liquid_float_complex x_neg = cexpf(-_Complex_I*2*M_PI*0.059f*i);

        // signal at positive frequency: filtered noise
        liquid_float_complex v;
        iirfilt_crcf_execute(lowpass, 4*randnf(), &v);
        liquid_float_complex x_pos = v * cexpf(_Complex_I*2*M_PI*0.073f*i);

        // composite
        x[i] = (x_neg + x_pos) * liquid_hamming(i,num_samples);
    }
    iirfilt_crcf_destroy(lowpass);

    // create/print the half-band resampler, with a specified
    // stopband attenuation level
    resamp2_cccf q = resamp2_cccf_create(m,-0.25f,As);
    resamp2_cccf_print(q);

    // run filter
    for (i=0; i<num_samples; i++)
        resamp2_cccf_filter_execute(q,x[i],&y0[i],&y1[i]);

    // 
    // print results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"num_samples=%u;\n", num_samples);

    // output results
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x( %3u) = %12.4e + j*%12.4e;\n", i+1, crealf( x[i]), cimagf( x[i]));
        fprintf(fid,"y0(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y0[i]), cimagf(y0[i]));
        fprintf(fid,"y1(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y1[i]), cimagf(y1[i]));
    }

    // plot temporal results
    fprintf(fid,"\n\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,real(x),'Color',[1 1 1]*0.7,'LineWidth',1);\n");
    fprintf(fid,"  plot(t,imag(x),'Color',[1 1 1]*0.5,'LineWidth',2);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  ylabel('original');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,real(y0),'Color',[1 1 1]*0.7,'LineWidth',1);\n");
    fprintf(fid,"  plot(t,imag(y0),'Color',[0 0.5 0.2],'LineWidth',2);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  ylabel('negative band');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,real(y1),'Color',[1 1 1]*0.7,'LineWidth',1);\n");
    fprintf(fid,"  plot(t,imag(y1),'Color',[0 0.2 0.5],'LineWidth',2);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  axis([0 num_samples -2 2]);\n");
    fprintf(fid,"  ylabel('positive band');\n");
    fprintf(fid,"  xlabel('sample index');\n");

    // plot spectrum results
    fprintf(fid,"\n\n");
    fprintf(fid,"nfft=2400;\n");
    fprintf(fid,"f  = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"w  = hamming(num_samples)' / num_samples;\n");
    fprintf(fid,"X  = 20*log10(abs(fftshift(fft( x.*w, nfft))));\n");
    fprintf(fid,"Y0 = 20*log10(abs(fftshift(fft(y0.*w, nfft))));\n");
    fprintf(fid,"Y1 = 20*log10(abs(fftshift(fft(y1.*w, nfft))));\n");
    fprintf(fid,"figure('Color','white');\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(f,X, 'Color',[1 1 1]*0.7,'LineWidth',2);\n");
    fprintf(fid,"  plot(f,Y0,'Color',[0 0.2 0.5]);\n");
    fprintf(fid,"  plot(f,Y1,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"legend('original','negative','positive','location','northeast');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");

    fclose(fid);
    printf("results written to %s\n",filename);

    printf("done.\n");
    return 0;
}
