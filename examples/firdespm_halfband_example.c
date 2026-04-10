const char __docstr__[] = "Create halfband filter using firdespm";

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
    liquid_argparse_add(char*,    filename, "firdespm_halfband_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,   12, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    As,  60, 'a', "filter stop-band attenuation [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int h_len = 4*m + 1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdespm_halfband_as(m, As, h);

    // print coefficients
    unsigned int i;
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

    fprintf(fid,"nfft=120*(2^nextpow2(h_len));\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"  plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  title('Filter design (firdespm)');\n");
    fprintf(fid,"  axis([-0.5 0.5 -120 5]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

