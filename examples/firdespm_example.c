const char __docstr__[] =
"This example demonstrates finite impulse response filter design"
" using the Parks-McClellan algorithm.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firdespm_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, h_len, 91, 'p', "filter length", NULL);
    liquid_argparse_parse(argc,argv);

    // filter design parameters
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int num_bands = 4;
    float bands[8]   = {0.00f, 0.10f,
                        0.12f, 0.18f,
                        0.20f, 0.30f,
                        0.31f, 0.50f};

    float des[4]     = {1.0f, 0.0f, 0.1f, 0.0f};
    float weights[4] = {1.0f, 4.0f, 8.0f, 4.0f};
    liquid_firdespm_wtype wtype[4] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};

    unsigned int i;
    LIQUID_VLA(float, h, h_len);
    firdespm_run(h_len,num_bands,bands,des,weights,wtype,btype,h);

    // print coefficients
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", h_len);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title('Filter design (firdespm)');\n");
    fprintf(fid,"axis([-0.5 0.5 -60 5]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

