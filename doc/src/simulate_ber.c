//
// simulate BER, PER data
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

// default output filename
#define OUTPUT_FILENAME "ber_results.dat"

// print usage/help message
void usage()
{
    printf("simulate_ber options:\n");
    printf("  u/h   : print usage\n");
    printf("  v/q   : verbose/quiet\n");
    printf("  o     : output filename, default: %s\n", OUTPUT_FILENAME);
    printf("  s     : SNR start [dB], -5\n");
    printf("  d     : SNR step [dB], 0.2\n");
    printf("  x     : SNR max [dB], 25\n");
    printf("  b     : BER min, 1e-4\n");
    printf("  t     : max trials, 20000000\n");
    printf("  n     : min trials, 100000\n");
    printf("  e     : min errors, 1000\n");
    printf("  f     : frame bytes, 256\n");
    printf("  m     : mod scheme\n");
    liquid_print_modulation_schemes();
    printf("  c     : fec coding scheme (inner)\n");
    printf("  k     : fec coding scheme (outer)\n");
    liquid_print_fec_schemes();
    printf("  i     : interleaving? default: off\n");
    printf("  S/H   : soft/hard decoding, default: hard\n");
}

int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB_min     = -5.0f;    // starting SNR
    float SNRdB_step    =  0.2f;    // SNR step size
    float SNRdB_max     = 40.0f;    // maximum SNR
    float BER_min       =  1e-4f;   // minimum BER
    unsigned long int min_errors = 1000;
    unsigned long int min_trials = 100000;
    unsigned long int max_trials = 20000000;
    unsigned int frame_len = 256;
    modulation_scheme ms = LIQUID_MODEM_BPSK;
    fec_scheme fec0 = LIQUID_FEC_NONE;
    fec_scheme fec1 = LIQUID_FEC_NONE;
    int interleaving  = 0;
    int soft_decoding = 0;
    const char * filename = OUTPUT_FILENAME;
    int verbose = 1;

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uhvqo:s:d:x:b:t:n:e:f:m:c:k:iSH")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();      return 0;
        case 'v': verbose = 1;  break;
        case 'q': verbose = 0;  break;
        case 'o':
            filename = optarg;
            break;
        case 's': SNRdB_min = atof(optarg); break;
        case 'd': SNRdB_step = atof(optarg); break;
        case 'x': SNRdB_max = atof(optarg); break;
        case 'b': BER_min = atof(optarg); break;
        case 't': max_trials = atoi(optarg); break;
        case 'n': min_trials = atol(optarg); break;
        case 'e': min_errors = atol(optarg); break;
        case 'f': frame_len = atol(optarg); break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                printf("error: unknown mod. scheme: %s\n", optarg);
                exit(-1);
            }
            break;
        case 'c': fec0 = liquid_getopt_str2fec(optarg); break;
        case 'k': fec1 = liquid_getopt_str2fec(optarg); break;
        case 'i': interleaving  = 1;                    break;
        case 'S': soft_decoding = 1;                    break;
        case 'H': soft_decoding = 0;                    break;
        default:
            exit(1);
        }
    }

    // validate options
    if (SNRdB_step <= 0.0f) {
        printf("error: SNRdB_step must be greater than zero\n");
        exit(-1);
    } else if (SNRdB_max < SNRdB_min) {
        printf("error: SNRdB_max must be greater than SNRdB_min\n");
        exit(-1);
    } else if (max_trials < min_trials) {
        printf("error: max_trials must be greater than min_trials\n");
        exit(-1);
    } else if (max_trials < min_errors) {
        printf("error: max_trials must be greater than min_errors\n");
        exit(-1);
    }

    unsigned int i;
    simulate_per_opts opts;
    opts.ms = ms;
    opts.fec0 = fec0;
    opts.fec1 = fec1;
    opts.dec_msg_len = frame_len;
    opts.soft_decoding = soft_decoding;
    opts.interleaving  = interleaving;
    opts.verbose = verbose;

    // minimum number of errors to simulate
    opts.min_packet_errors  = 0;
    opts.min_bit_errors     = min_errors;

    // minimum number of trials to simulate
    opts.min_packet_trials  = 0;
    opts.min_bit_trials     = min_trials;

    // maximum number of trials to simulate (before bailing and
    // deeming simulation unsuccessful)
    opts.max_packet_trials  = -1; 
    opts.max_bit_trials     =  max_trials;

    // derived values
    unsigned int bps = modulation_types[ms].bps;    // get modulation depth
    float rate = bps * fec_get_rate(opts.fec0) * fec_get_rate(opts.fec1);

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
    fprintf(fid,"#  modulation depth    :   %u bits/symbol\n", bps);
    fprintf(fid,"#  fec (inner)         :   %s\n", fec_scheme_str[opts.fec0][1]);
    fprintf(fid,"#  fec (outer)         :   %s\n", fec_scheme_str[opts.fec1][1]);
    fprintf(fid,"#  frame length        :   %u bytes\n", opts.dec_msg_len);
    fprintf(fid,"#  asymptotic rate     :   %-12.8f bits/second/Hz\n", rate);
    fprintf(fid,"#  min packet errors   :   %lu\n", opts.min_packet_errors);
    fprintf(fid,"#  min bit errors      :   %lu\n", opts.min_bit_errors);
    fprintf(fid,"#  min packet trials   :   %lu\n", opts.min_packet_trials);
    fprintf(fid,"#  min bit trials      :   %lu\n", opts.min_bit_trials);
    fprintf(fid,"#  max packet trials   :   %lu\n", opts.max_packet_trials);
    fprintf(fid,"#  max bit trials      :   %lu\n", opts.max_bit_trials);
    fprintf(fid,"#\n");
    fprintf(fid,"# %12s %12s %12s %12s %12s %12s %12s %12s\n",
            "SNR [dB]",
            "Eb/N0 [dB]",
            "BER",
            "PER",
            "bit errs",
            "bits",
            "packet errs",
            "packets");

    // run simulation for increasing SNR levels
    simulate_per_results results;
    float SNRdB = SNRdB_min;
    float EbN0dB;
    while (SNRdB < SNRdB_max) {
        // 
        simulate_per(opts, SNRdB, &results);

        // compute Eb/N0
        EbN0dB = SNRdB - 10.0f*log10f(rate);

        // break if unsuccessful
        if (!results.success) break;

        // save data to file
        fprintf(fid,"  %12.8f %12.8f %12.4e %12.4e %12lu %12lu %12lu %12lu\n",
                SNRdB,
                EbN0dB,
                results.BER,
                results.PER,
                results.num_bit_errors,
                results.num_bit_trials,
                results.num_packet_errors,
                results.num_packet_trials);

        // break if BER exceeds minimum
        if (results.BER < BER_min) break;

        // increase SNR
        SNRdB += SNRdB_step;
    }

    // close output file
    fclose(fid);

    printf("results written to '%s'\n", filename);

    return 0;
}


