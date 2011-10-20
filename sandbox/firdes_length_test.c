//
// firdes_length_test.c
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "firdes_length_test.m"

// print usage/help message
void usage()
{
    printf("firdes_length_test:\n");
    printf("  u/h   : print usage/help\n");
    printf("  t     : filter transition bandwidth,  0 < t < 0.5, default: 0.1\n");
    printf("  a     : filter attenuation (minimum) [dB], default: 20\n");
    printf("  A     : filter attenuation (maximum) [dB], default: 100\n");
    printf("  n     : number of steps, default: 41\n");
}

int main(int argc, char*argv[]) {
    // options
    float ft=0.1f;          // filter transition
    float As_min = 20.0f;
    float As_max = 100.0f;
    unsigned int num_As = 41;   // number of steps

    int dopt;
    while ((dopt = getopt(argc,argv,"uht:a:A:n:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                  return 0;
        case 't': ft = atof(optarg);        break;
        case 'a': As_min = atof(optarg);    break;
        case 'A': As_max = atof(optarg);    break;
        case 'n': num_As = atoi(optarg);    break;
        default:
            exit(1);
        }
    }

    // validate input
    if (As_min <= 0.0f || As_max <= 0.0f) {
        fprintf(stderr,"error: %s, attenuation must be greater than zero\n", argv[0]);
        exit(1);
    } else if (As_max <= As_min) {
        fprintf(stderr,"error: %s, minimum attenuation cannot exceed maximum\n", argv[0]);
        exit(1);
    } else if (num_As < 2) {
        fprintf(stderr,"error: %s, must have at least 2 steps", argv[0]);
        exit(1);
    }

    // derived values
    float As_step = (As_max - As_min) / (float)(num_As - 1);

    // output to file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"num_steps = %u;\n", num_As);
    fprintf(fid,"As=zeros(1,num_steps);\n");
    fprintf(fid,"n_Kaiser=zeros(1,num_steps);\n");
    fprintf(fid,"n_Herrmann=zeros(1,num_steps);\n");

    unsigned int i;
    for (i=0; i<num_As; i++) {
        float As = As_min + i*As_step;
        float n_Kaiser = estimate_req_filter_len_Kaiser(ft,As);
        float n_Herrmann = estimate_req_filter_len_Herrmann(ft,As);//+8);
        printf("As = %8.2f, n_Kaiser=%8.2f, n_Herrmann=%8.2f\n",
                As, n_Kaiser, n_Herrmann);

        fprintf(fid,"As(%4u) = %12.8f; n_Kaiser(%4u)=%12.8f; n_Herrmann(%4u)=%12.8f;\n",
                i+1, As,
                i+1, n_Kaiser,
                i+1, n_Herrmann);
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(As,n_Kaiser, As,n_Herrmann);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Stop-band Attenuation [dB]');\n");
    fprintf(fid,"ylabel('Filter Length');\n");
    fprintf(fid,"legend('Kaiser','Herrmann',0);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

