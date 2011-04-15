//
// simulate_per_test
//
// Find SNR, Eb/N0 which achieves a specific BER|PER
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("simulate_per_test options:\n");
    printf("  u/h   : print usage\n");
    printf("  n     : number of decoded bytes, default: 1024\n");
    printf("  p     : modulation depth, default: 2 [bits/symbol]\n");
    printf("  m     : modulation scheme, default: psk\n");

    // print all available MOD schemes
    unsigned int i;
    for (i=0; i<LIQUID_NUM_MOD_SCHEMES; i++)
        printf("          %s\n", modulation_scheme_str[i][0]);

    printf("  c     : coding scheme (inner), default: h74\n");
    printf("  k     : coding scheme (outer), default: none\n");
    // print all available FEC schemes
    for (i=0; i<LIQUID_NUM_FEC_SCHEMES; i++)
        printf("          [%s] %s\n", fec_scheme_str[i][0], fec_scheme_str[i][1]);
}


int main(int argc, char*argv[]) {
    srand( time(NULL) );

    // options
    simulate_per_opts opts;
    opts.ms = LIQUID_MODEM_BPSK;
    opts.bps = 1;
    opts.fec0 = FEC_NONE;
    opts.fec1 = FEC_NONE;
    opts.dec_msg_len = 1024;

    opts.min_packet_errors  = 5;
    opts.min_bit_errors     = 10;
    opts.min_packet_trials  = 50;
    opts.min_bit_trials     = 10000;

    opts.max_packet_trials  = 1000;
    opts.max_bit_trials     = 1000000;

    // read command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uhn:p:m:c:k:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage();                          return 0;
        case 'n': opts.dec_msg_len = atoi(optarg);  break;
        case 'p': opts.bps = atoi(optarg);          break;
        case 'm':
            opts.ms = liquid_getopt_str2mod(optarg);
            if (opts.ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: modem_example, unknown/unsupported modulation scheme \"%s\"\n", optarg);
                exit(1);
            }
            break;
        case 'c':
            // inner FEC scheme
            opts.fec0 = liquid_getopt_str2fec(optarg);
            if (opts.fec0 == FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                exit(-1);
            }
            break;
        case 'k':
            // outer FEC scheme
            opts.fec1 = liquid_getopt_str2fec(optarg);
            if (opts.fec1 == FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                exit(-1);
            }
            break;
        default:
            fprintf(stderr,"error: unknown/invalid option\n");
            exit(-1);
        }
    }


    // SNR range, steps
    float SNRdB_min = 0.0f;
    float SNRdB_max = 20.0f;
    unsigned int num_steps = 21;

    // derived values
    float SNRdB_step = (SNRdB_max - SNRdB_min)/(float)(num_steps-1);

    //
    float SNRdB;

    // generate results structure
    simulate_per_results results;

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        SNRdB = SNRdB_min + i*SNRdB_step;

        simulate_per(opts, SNRdB, &results);

        //printf("  %12.8f : %12.4e\n", SNRdB, PER);
        printf(" %c SNR: %6.2f, bits: %8lu / %8lu (%12.4e), packets: %6lu / %6lu (%6.2f%%)\n",
                results.success ? '*' : ' ',
                SNRdB,
                results.num_bit_errors,     results.num_bit_trials,     results.BER,
                results.num_packet_errors,  results.num_packet_trials,  results.PER*100.0f);
    }

    printf("done.\n");
    return 0;
}
