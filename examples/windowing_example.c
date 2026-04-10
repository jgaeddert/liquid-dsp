const char __docstr__[] = "Demonstrate windowing functions.";

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename,  "windowing_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    wtype_str, "kaiser", 'w', "window type", liquid_argparse_window);
    liquid_argparse_add(unsigned, wlen,       51,      'n', "window length", NULL);
    liquid_argparse_add(float,    arg,       10,       'N', "window argument", NULL);
    liquid_argparse_parse(argc,argv);

    // compute window coefficients
    liquid_window_type  wtype = (liquid_window_type)liquid_getopt_str2window(wtype_str);
    LIQUID_VLA(float, w, wlen);
    unsigned int i;
    for (i=0; i<wlen; i++) {
        w[i] = liquid_windowf(wtype, i, wlen, arg);
        printf("%12.8f\n", w[i]);
    }

    // export output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"wlen=%u;\n",wlen);
    fprintf(fid,"nfft=%u;\n",1200);
    for (i=0; i<wlen; i++)
        fprintf(fid,"w(%4u) = %12.4e;\n", i+1, w[i]);
    fprintf(fid,"t=0:(wlen-1);\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure('position',[50 50 600 800]);\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,w,'Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Sample Index');\n");
    fprintf(fid,"  ylabel('Window Value');\n");
    fprintf(fid,"  axis([-1 wlen -0.1 1.1]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f,20*log10(abs(fftshift(fft(w,nfft)/sum(w)))),'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  axis([-0.5 0.5 -140 20]);\n");
    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

