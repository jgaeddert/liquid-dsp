const char __docstr__[] = "Kaiser-Bessel derived window example.";

#include <stdio.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "kbd_window_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned,    n,    64, 'n', "window length (samples)", NULL);
    liquid_argparse_add(float,       beta, 20, 'b', "Kaiser beta factor)", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int i;
    LIQUID_VLA(float, w, n);
    liquid_kbd_window(n,beta,w);

    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n=%u;\n",n);

    for (i=0; i<n; i++) {
        fprintf(fid,"w(%4u) = %12.4e;\n", i+1, w[i]);
        printf("w[%4u] = %12.4e;\n", i, w[i]);
    }   

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"W=20*log10(abs(fftshift(fft(w/sum(w),nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,w,'Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('window');\n");
    fprintf(fid,"  axis([0 n-1 -0.1 1.1]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f,W,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  axis([-0.5 0.5 -140 20]);\n");
    fprintf(fid,"title(['Kaiser-Bessel derived window']);\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

