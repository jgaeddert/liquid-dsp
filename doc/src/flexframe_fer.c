//
// flexframesync_sim.c
//
// Simulates the flexframesync object and observes the
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
    printf("flexframesync_sim\n");
    printf("  Simulates the flexframesync object and observes the\n");
    printf("  probability of properly receiving a packet based on\n");
    printf("  the encoding scheme vs. SNR.\n");
    printf("options:\n");
    printf("  u/h   : print usage/help\n");
    printf("  o     : output filename\n");
    printf("  s     : SNR start [dB], -10\n");
    printf("  d     : SNR step [dB], 1.0\n");
    printf("  x     : SNR max [dB], 10\n");
    printf("  n     : number of trials, 800\n");
    printf("  f     : frame bytes (packet len), 256\n");
    printf("  m     : mod scheme, default: qpsk\n");
    liquid_print_modulation_schemes();
    printf("  c     : fec coding scheme (inner), default: h128\n");
    printf("  k     : fec coding scheme (outer), default: none\n");
    liquid_print_fec_schemes();
}


int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB_start       = -5.0f;
    float SNRdB_step        = 1.0f;
    float SNRdB_max         = 10.0f;
    unsigned int num_frames = 1000;
    //float noise_floor       = -30.0f;
    const char * filename   = "flexframe_fer_results.dat";
    modulation_scheme ms    = LIQUID_MODEM_QPSK;
    unsigned int payload_len= 256;
    crc_scheme check        = LIQUID_CRC_32;
    fec_scheme fec0         = LIQUID_FEC_HAMMING128;
    fec_scheme fec1         = LIQUID_FEC_NONE;
    int verbose             = 1;

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uho:s:d:x:n:f:m:c:k:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'o': filename = optarg;            break;
        case 's': SNRdB_start = atof(optarg);   break;
        case 'd': SNRdB_step = atof(optarg);    break;
        case 'x': SNRdB_max = atof(optarg);     break;
        case 'n': num_frames = atol(optarg);    break;
        case 'f': payload_len = atol(optarg);   break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                printf("error: unknown/unsupported mod. scheme: %s\n", optarg);
                exit(1);
            }
            break;
        case 'c':
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == LIQUID_FEC_UNKNOWN) {
                printf("error: unknown/unsupported fec scheme \"%s\"\n", optarg);
                exit(1);
            }
            break;
        case 'k':
            fec1 = liquid_getopt_str2fec(optarg);
            if (fec1 == LIQUID_FEC_UNKNOWN) {
                printf("error: unknown/unsupported fec scheme \"%s\"\n", optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            exit(1);
        }
    }

    // validate options
    if (SNRdB_step <= 0.0f) {
        printf("error: SNRdB_step must be greater than zero\n");
        exit(-1);
    } else if (SNRdB_max < SNRdB_start) {
        printf("error: SNRdB_max must be greater than SNRdB_start\n");
        exit(-1);
    }

    // set up framing simulation options
    flexframe_fer_opts opts;
    opts.ms         = ms;
    opts.check      = check;
    opts.fec0       = fec0;
    opts.fec1       = fec1;
    opts.payload_len= payload_len;
    opts.num_frames = num_frames;
    opts.verbose    = verbose;

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
    fprintf(fid,"#  modulation scheme   :   %s\n", modulation_types[opts.ms].fullname);
    fprintf(fid,"#  modulation depth    :   %u bits/symbol\n", modulation_types[opts.ms].bps);
    fprintf(fid,"#  check               :   %s\n", crc_scheme_str[opts.check][1]);
    fprintf(fid,"#  fec (inner)         :   %s\n", fec_scheme_str[opts.fec0][1]);
    fprintf(fid,"#  fec (outer)         :   %s\n", fec_scheme_str[opts.fec1][1]);
    fprintf(fid,"#  payload length      :   %u bytes\n", opts.payload_len);
    fprintf(fid,"#  frame trials        :   %u\n", opts.num_frames);
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
        flexframe_fer(opts, SNRdB, &results);

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

