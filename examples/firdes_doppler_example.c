const char __docstr__[] =
"This example demonstrates finite impulse response Doppler filter design";

#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firdes_doppler_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    fd,    0.2, 'f', "Normalized Doppler frequency", NULL);
    liquid_argparse_add(float,    K,      10, 'K', "Rice fading factor", NULL);
    liquid_argparse_add(float,    theta,   0, 't', "LoS component angle of arrival", NULL);
    liquid_argparse_add(unsigned, h_len, 161, 'p', "filter length", NULL);
    liquid_argparse_parse(argc,argv);

    // generate the filter
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_doppler(h_len,fd,K,theta,h);

    // output to file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n",h_len);
    unsigned int i;
    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Doppler filter design, fd:%.3f, K:%.1f, theta:%.3f, n:%u']);\n",
            fd, K, theta, h_len);
    fprintf(fid,"xlim([-0.5 0.5]);\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}

