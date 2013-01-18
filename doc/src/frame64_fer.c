//
// frame64_fer.c
//
// Simulates the framesync64 object and observes the
// probability of properly receiving a packet based on
// the encoding scheme vs. SNR.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <complex.h>
#include <string.h>

#include "liquid.doc.h"

void usage()
{
    printf("frame64_fer\n");
    printf("  Simulates the frame64 object and observes the\n");
    printf("  probability of properly receiving a packet based on\n");
    printf("  the encoding scheme vs. SNR.\n");
    printf("options:\n");
    printf("  u/h   : print usage/help\n");
    printf("  o     : output filename\n");
    printf("  s     : SNR start [dB], -9\n");
    printf("  d     : SNR step [dB], 1.0\n");
    printf("  x     : SNR max [dB], 6\n");
    printf("  n     : number of trials, 2000\n");
}


int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB_start       = -9.0f;
    float SNRdB_step        =  1.0f;
    float SNRdB_max         =  6.0f;
    unsigned int num_frames = 2000;
    //float noise_floor       = -40.0f;
    const char * filename   = "frame64_fer_results.dat";
    int verbose             = 1;

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uho:s:d:x:n:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'o': filename = optarg;            break;
        case 's': SNRdB_start = atof(optarg);   break;
        case 'd': SNRdB_step = atof(optarg);    break;
        case 'x': SNRdB_max = atof(optarg);     break;
        case 'n': num_frames = atol(optarg);    break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            exit(1);
        }
    }

    // validate options
    if (SNRdB_step <= 0.0f) {
        fprintf(stderr,"error: %s, SNRdB_step must be greater than zero\n", argv[0]);
        exit(-1);
    } else if (SNRdB_max < SNRdB_start) {
        fprintf(stderr,"error: %s, SNRdB_max must be greater than SNRdB_start\n", argv[0]);
        exit(-1);
    }

    // create results objects
    fer_results results;

    // bookkeeping variables
    unsigned int i;
    float SNRdB = SNRdB_start;

    // open output file
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: could not open '%s' for writing\n", filename);
        exit(1);
    }
    fprintf(fid,"# %s : auto-generated file\n", filename);
    fprintf(fid,"# invoked as: ");
    for (i=0; i<argc; i++) fprintf(fid,"%s ", argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"#\n");
    fprintf(fid,"#  frame trials        :   %u\n", num_frames);
    fprintf(fid,"#\n");
    fprintf(fid,"# %8s %12s %12s %12s %12s %12s %12s %12s\n",
            "SNR [dB]",
            "FER (frame)",
            "HER (header)",
            "PER (packet)",
            "frames",
            "headers",
            "packets",
            "num trials");
    fclose(fid);

    // start running batch trials
    while (SNRdB <= SNRdB_max) {

        // run trials
        frame64_fer(num_frames, SNRdB, verbose, &results);

        // append results to file
        fid = fopen(filename,"a");
        fprintf(fid,"  %8.2f %12.10f %12.10f %12.10f %12u %12u %12u %12u\n",
                SNRdB,
                results.FER,
                results.HER,
                results.PER,
                results.num_missed_frames,
                results.num_header_errors,
                results.num_packet_errors,
                results.num_frames);
        fclose(fid);

        SNRdB += SNRdB_step;
    }

    printf("results written to '%s'\n", filename);

    return 0;
}

