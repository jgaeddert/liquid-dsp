const char __docstr__[] =
"This example demonstrates a low-pass finite impulse response filter"
" design using the Parks-McClellan algorithm.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firdespm_lowpass_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, n,   57, 'n', "filter length", NULL);
    liquid_argparse_add(float,    fc, 0.2, 'f', "filter cutoff frequency", NULL);
    liquid_argparse_add(float,    As,  60, 'a', "filter stop-band attenuation [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int i;
    printf("filter design parameters\n");
    printf("  length                : %12u\n",      n);
    printf("  cutoff frequency      : %12.8f Fs\n", fc);
    printf("  stop-band attenuation : %12.3f dB\n", As);

    // design the filter
    LIQUID_VLA(float, h, n);
    firdespm_lowpass(n,fc,As,0,h);

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", n);
    fprintf(fid,"fc=%12.4e;\n",fc);
    fprintf(fid,"As=%12.4e;\n",As);

    for (i=0; i<n; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Filter design (firdespm) f_c: %.3f, S_L: %.3f, h: %u']);\n",
            fc, -As, n);
    fprintf(fid,"axis([-0.5 0.5 -As-20 10]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

