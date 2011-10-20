//
// estimate required SNR for given error rate
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("generate_per_data options:\n");
    printf("  u/h   : print usage\n");
    printf("  v|q   : verbose|quiet, default: verbose\n");
    printf("  B|P   : simulate for BER|PER, default: PER\n");
    printf("  s|e   : estimate SNR|Eb/N0, default: SNR\n");
    printf("  S|H   : soft/hard decoding, default: hard\n");
    printf("  E     : target error rate, default: 0.05\n");
    printf("  n     : frame length [bytes], default: 1024\n");
    //printf("  m         : minimum number of errors\n");
    //printf("  t         : minimum number of trials\n");
    printf("  x     : maximum number of trials, default: 10,000 (BER) or 100 (PER)\n");
    printf("  c     : coding scheme: h74 default\n");
    liquid_print_fec_schemes();
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
}

typedef struct {
    modulation_scheme ms;
    unsigned int bps;
    float cycles_per_bit;
} modset;

typedef struct {
    fec_scheme fec;
    float rate;
    float cycles_per_bit;
} fecset;

int main(int argc, char*argv[])
{
    srand( time(NULL) );

    // options
    int verbose = 1;
    int which_ber_per  = ESTIMATE_SNR_BER;
    int which_snr_ebn0 = ESTIMATE_SNR;
    float error_rate = 1e-3f;
    unsigned int frame_len = 1024;
    unsigned long int max_trials = 0;

    unsigned int bps=2;
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    fec_scheme fec0 = LIQUID_FEC_NONE;  // inner code
    fec_scheme fec1 = LIQUID_FEC_NONE;  // outer code
    int soft_decoding = 0;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhvqBPseSHE:n:x:c:m:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'v': verbose = 1;                      break;
        case 'q': verbose = 0;                      break;
        case 'B': which_ber_per = ESTIMATE_SNR_BER; break;
        case 'P': which_ber_per = ESTIMATE_SNR_PER; break;
        case 's': which_snr_ebn0 = ESTIMATE_SNR;    break;
        case 'e': which_snr_ebn0 = ESTIMATE_EBN0;   break;
        case 'S': soft_decoding = 1;                break;
        case 'H': soft_decoding = 0;                break;
        case 'E': error_rate = atof(optarg);        break;
        case 'n': frame_len = atoi(optarg);         break;
        case 'x': max_trials = atoi(optarg);        break;
        case 'c':
            // FEC scheme
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported inner FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'm':
            liquid_getopt_str2modbps(optarg, &ms, &bps);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: modem_example, unknown/unsupported modulation scheme \"%s\"\n", optarg);
                return 1;
            }
            break;
        default:
            exit(1);
        }
    }

    // validate input
    if (error_rate <= 0.0f) {
        fprintf(stderr,"error: error rate must be greater than 0\n");
        exit(1);
    } else if (frame_len == 0 || frame_len > 10000) {
        fprintf(stderr,"error: frame length must be in [1, 10,000]\n");
        exit(1);
    } else if (which_ber_per == ESTIMATE_SNR_BER && error_rate >= 0.5) {
        fprintf(stderr,"error: error rate must be less than 0.5 when simulating BER\n");
        exit(1);
    } else if (error_rate >= 1.0f) {
        fprintf(stderr,"error: error rate must be less than 1\n");
        exit(1);
    }

    if (max_trials == 0) {
        // unspecified: use defaults
        if (which_ber_per == ESTIMATE_SNR_BER)
            max_trials = 800000;
        else
            max_trials = 200;
    }

    simulate_per_opts opts;
    opts.ms     = ms;
    opts.bps    = bps;
    opts.fec0   = fec0;
    opts.fec1   = fec1;
    opts.dec_msg_len = frame_len;
    opts.soft_decoding = soft_decoding;

    // minimum number of errors to simulate
    opts.min_packet_errors  = which_ber_per==ESTIMATE_SNR_PER ? 10      : 0;
    opts.min_bit_errors     = which_ber_per==ESTIMATE_SNR_BER ? 50      : 0;

    // minimum number of trials to simulate
    opts.min_packet_trials  = which_ber_per==ESTIMATE_SNR_PER ? 500     : 0;
    opts.min_bit_trials     = which_ber_per==ESTIMATE_SNR_BER ? 5000    : 0;

    // maximum number of trials to simulate (before bailing and
    // deeming simulation unsuccessful)
    opts.max_packet_trials  = which_ber_per==ESTIMATE_SNR_PER ? max_trials : -1; 
    opts.max_bit_trials     = which_ber_per==ESTIMATE_SNR_BER ? max_trials : -1; 

    // estimate SNR for a specific PER
    printf("%u-%s // %s // %s (%s: %e)\n", 1<<opts.bps,
                                           modulation_scheme_str[opts.ms][0],
                                           fec_scheme_str[opts.fec0][0],
                                           fec_scheme_str[opts.fec1][0],
                                           which_ber_per == ESTIMATE_SNR_BER ? "BER" : "PER",
                                           error_rate);

    // run estimation
    float x_hat = estimate_snr(opts, which_ber_per, which_snr_ebn0, error_rate);

    // compute rate [b/s/Hz]
    float rate = opts.bps * fec_get_rate(opts.fec0) * fec_get_rate(opts.fec1);

    // set estimated values
    float SNRdB_hat;
    float EbN0dB_hat;
    if (which_snr_ebn0==ESTIMATE_SNR) {
        SNRdB_hat = x_hat;
        EbN0dB_hat = SNRdB_hat - 10*log10f(rate);
    } else {
        SNRdB_hat = x_hat + 10*log10f(rate);
        EbN0dB_hat = x_hat;
    }
    if (verbose) {
        printf("++ SNR (est) : %8.4fdB (Eb/N0 = %8.4fdB) for %s: %12.4e\n",
                SNRdB_hat,
                EbN0dB_hat,
                which_ber_per == ESTIMATE_SNR_BER ? "BER" : "PER",
                error_rate);
    }


    printf("done.\n");
    return 0;
}

