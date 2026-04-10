const char __docstr__[] =
"Halfband decimator.  This example demonstrates the interface to the"
" decimating halfband resampler.  A low-frequency input sinusoid is"
" generated and fed into the decimator two samples at a time,"
" producing one output at each iteration.  The results are written to"
" an output file.";

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
    liquid_argparse_add(char*, filename, "resamp2_crcf_decim_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,   12, 'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(float,    As,  60, 'a', "resampling filter stop-band attenuation [dB]", NULL);
    liquid_argparse_add(float,    bw, 0.1, 'w', "pulse bandwidth (relative)", NULL);
    liquid_argparse_parse(argc,argv);

    // allocate arrays
    unsigned int w_len = 43; // pulse length
    unsigned int num_samples = w_len + 2*m;
    LIQUID_VLA(liquid_float_complex, x, 2*num_samples); // input array
    LIQUID_VLA(liquid_float_complex, y, num_samples); // output array

    // generate input
    LIQUID_VLA(float, w, w_len);
    liquid_firdes_kaiser(w_len,bw,60.0f,0.0f,w);
    unsigned int i;
    for (i=0; i<2*num_samples; i++)
        x[i] = i < w_len ? w[i] : 0.0f;

    // create/print the half-band resampler
    resamp2_crcf q = resamp2_crcf_create(m,0,As);
    resamp2_crcf_print(q);
    unsigned int delay = resamp2_crcf_get_delay(q);

    // run the resampler
    for (i=0; i<num_samples; i++) {
        // execute the decimator
        resamp2_crcf_decim_execute(q, &x[2*i], &y[i]);

        // print results to screen
        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // destroy half-band resampler
    resamp2_crcf_destroy(q);

    // export results
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"delay      =%u;\n", delay);
    fprintf(fid,"bw         =%12.8f;\n", bw);
        
    for (i=0; i<num_samples; i++) {
        // save results to output file
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", 2*i+1,      crealf(x[2*i+0]),      cimagf(x[2*i+0]));
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n", 2*i+2,      crealf(x[2*i+1]),      cimagf(x[2*i+1]));
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1,   0.5f*crealf(y[i    ]), 0.5f*cimagf(y[i    ]));
    }

    // plot time series
    fprintf(fid,"tx =  0:(2*num_samples-1);\n");
    fprintf(fid,"ty = [0:(  num_samples-1)]*2 - delay;\n");
    fprintf(fid,"figure('position',[100 100 720 720]);\n");
    fprintf(fid,"subplot(4,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,real(y),'-s','Color',[0.5 0.0 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"  axis([-delay 2*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(4,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-s','Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"       ty,imag(y),'-s','Color',[0.0 0.5 0.0],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','decimated','location','northeast');");
    fprintf(fid,"  axis([-delay 2*num_samples -1.2 1.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('imag');\n");
    // plot spectrum
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"g = 2*bw;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(  x*g,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(2*y*g,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"subplot(4,1,3:4);\n");
    fprintf(fid,"plot(f,  X,'LineWidth',1,  'Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"     f/2,Y,'LineWidth',1.5,'Color',[0.1 0.3 0.5]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated','location','northeast');");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
