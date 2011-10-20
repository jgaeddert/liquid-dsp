//
// firdespm_example.c
//
// This example demonstrates finite impulse response filter design
// using the Parks-McClellan algorithm.
// SEE ALSO: firdes_kaiser_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firdespm_example.m"

// print usage/help message
void usage()
{
    printf("firdespm_example:\n");
    printf("  u/h   : print usage/help\n");
    printf("  f     : filter cutoff frequency,       0 < f < 0.5, default: 0.2\n");
    printf("  t     : filter transition bandwidth,   0 < t < 0.5, default: 0.1\n");
    printf("  s     : stop-band attenuation [dB],    0 < s,       default: 60\n");
}

int main(int argc, char*argv[]) {
    // options
    float fc=0.2f;          // filter cutoff frequency
    float ft=0.1f;          // filter transition
    float As=60.0f;         // stop-band attenuation [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"uhf:t:s:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'f': fc = atof(optarg);            break;
        case 't': ft = atof(optarg);            break;
        case 's': As = atof(optarg);            break;
        default:
            exit(1);
        }
    }
    printf("filter design parameters\n");
    printf("    cutoff frequency            :   %12.8f\n", fc);
    printf("    transition bandwidth        :   %12.8f\n", ft);
    printf("    stop-band attenuation [dB]  :   %12.8f\n", As);

    // derived values
    unsigned int h_len = estimate_req_filter_len(ft,As);
    printf("h_len : %u\n", h_len);
    float fp = fc - 0.5*ft;     // pass-band cutoff frequency
    float fs = fc + 0.5*ft;     // stop-band cutoff frequency
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;

    // derived values
    unsigned int num_bands = 2;
    float bands[4]   = {0.0f, fp, fs, 0.5f};
    float des[2]     = {1.0f, 0.0f};
    float weights[2] = {1.0f, 1.0f};
    liquid_firdespm_wtype wtype[2] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};

    unsigned int i;
    float h[h_len];
#if 0
    firdespm q = firdespm_create(n,num_bands,bands,des,weights,wtype,btype);
    firdespm_print(q);
    firdespm_execute(q,h);
    firdespm_destroy(q);
#else
    firdespm_run(h_len,num_bands,bands,des,weights,wtype,btype,h);
#endif

    // print coefficients
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", h_len);
    fprintf(fid,"fc=%12.4e;\n",fc);
    fprintf(fid,"As=%12.4e;\n",As);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Filter design/Kaiser window f_c: %f, S_L: %f, h: %u']);\n",
            fc, -As, h_len);
    fprintf(fid,"axis([-0.5 0.5 -As-40 10]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

