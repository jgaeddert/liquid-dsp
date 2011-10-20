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
    
#define OUTPUT_FILENAME "simulate_per_test.m"

// print usage/help message
void usage()
{
    printf("simulate_per_test options:\n");
    printf("  u/h   : print usage\n");
    printf("  n     : number of decoded bytes, default: 1024\n");
    printf("  p     : modulation depth, default: 2 [bits/symbol]\n");
    printf("  m     : modulation scheme, default: psk\n");
    liquid_print_modulation_schemes();
    printf("  c     : coding scheme (inner), default: h74\n");
    printf("  k     : coding scheme (outer), default: none\n");
    liquid_print_fec_schemes();
    printf("  S/H   : soft/hard decoding: default: hard\n");
}


int main(int argc, char*argv[]) {
    srand( time(NULL) );

    // options
    simulate_per_opts opts;
    opts.ms = LIQUID_MODEM_BPSK;
    opts.bps = 1;
    opts.fec0 = LIQUID_FEC_NONE;
    opts.fec1 = LIQUID_FEC_NONE;
    opts.dec_msg_len = 1024;
    opts.soft_decoding = 1;

    opts.min_packet_errors  = 5;
    opts.min_bit_errors     = 10;
    opts.min_packet_trials  = 50;
    opts.min_bit_trials     = 10000;

    opts.max_packet_trials  = 1000;
    opts.max_bit_trials     = 1000000;

    // read command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uhn:p:m:c:k:SH")) != EOF){
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
            if (opts.fec0 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                exit(-1);
            }
            break;
        case 'k':
            // outer FEC scheme
            opts.fec1 = liquid_getopt_str2fec(optarg);
            if (opts.fec1 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                exit(-1);
            }
            break;
        case 'S': opts.soft_decoding = 1;   break;
        case 'H': opts.soft_decoding = 0;   break;
        default:
            exit(1);
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

    // array for BER
    float BER[num_steps];

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

        // save BER in array
        BER[i] = results.BER;
    }

    // 
    // export data
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    for (i=0; i<num_steps; i++) {
        fprintf(fid,"SNRdB(%3u) = %12.8f;\n", i+1, SNRdB_min + i*SNRdB_step);
        fprintf(fid,"BER(%3u)   = %12.4e;\n", i+1, BER[i]);
    }
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(SNRdB, 0.5*erfc(sqrt(10.^[SNRdB/10]))+1e-12,'-x',\n");
    fprintf(fid,"         SNRdB, BER + 1e-12,  '-x');\n");
    fprintf(fid,"axis([%f %f 1e-6 1]);\n", SNRdB_min, SNRdB_max);
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Bit Error Rate');\n");
    fprintf(fid,"legend('uncoded BPSK','modem: %s (M=%u) // fec: %s',1);\n",
            modulation_scheme_str[opts.ms][0], 1<<opts.bps, fec_scheme_str[opts.fec0][0]);
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
