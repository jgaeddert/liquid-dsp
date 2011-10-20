//
// estimate required SNR for given error rate
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "liquid.internal.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("generate_per_data options:\n");
    printf("  u/h   : print usage\n");
    printf("  v|q   : verbose|quiet, default: verbose\n");
    printf("  B|P   : simulate for BER|PER, default: BER\n");
    printf("  E     : target error rate, default: 1e-5\n");
    printf("  n     : frame length [bytes], default: 1024\n");
    printf("  x     : maximum number of trials, default: 10,000 (BER) or 100 (PER)\n");
    printf("  o     : output filename\n");
}

typedef struct {
    modulation_scheme ms;
    unsigned int bps;
    float SNRdB;
    float EbN0dB;
} modset;

int main(int argc, char*argv[])
{
    srand( time(NULL) );

    // options
    int verbose = 1;
    int which_ber_per  = ESTIMATE_SNR_BER;
    int which_snr_ebn0 = ESTIMATE_SNR;
    float error_rate = 1e-5f;
    unsigned int frame_len = 1024;
    unsigned long int max_trials = 0;

    char filename[256] = "estimate_snr_modem.out";

    int dopt;
    while ((dopt = getopt(argc,argv,"uhvqBPE:n:x:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'v': verbose = 1;                      break;
        case 'q': verbose = 0;                      break;
        case 'B': which_ber_per = ESTIMATE_SNR_BER; break;
        case 'P': which_ber_per = ESTIMATE_SNR_PER; break;
        case 'E': error_rate = atof(optarg);        break;
        case 'n': frame_len = atoi(optarg);         break;
        case 'x': max_trials = atoi(optarg);        break;
        case 'o':
            strncpy(filename, optarg, 255);
            filename[255] = '\0';
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
            max_trials = 4000000;
        else
            max_trials = 400;
    }

    unsigned int i;

    // initial options structure
    simulate_per_opts opts;

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

    // set FEC, framing length
    opts.fec0   = LIQUID_FEC_NONE;
    opts.fec1   = LIQUID_FEC_NONE;
    opts.dec_msg_len = frame_len;
    opts.soft_decoding = 0;

    // try to open output file
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"#\n");
    fprintf(fid,"# options:\n");
    fprintf(fid,"#  min. packet errors  :   %lu\n", opts.min_packet_errors);
    fprintf(fid,"#  min. bit errors     :   %lu\n", opts.min_bit_errors);
    fprintf(fid,"#  min. packet trials  :   %lu\n", opts.min_packet_trials);
    fprintf(fid,"#  min. bit trials     :   %lu\n", opts.min_bit_trials);
    fprintf(fid,"#  max. packet trials  :   %lu\n", opts.max_packet_trials);
    fprintf(fid,"#  max. bit trials     :   %lu\n", opts.max_bit_trials);
    fprintf(fid,"#  estimate on BER|PER :   %s\n", which_ber_per  == ESTIMATE_SNR_BER ? "BER" : "PER");
    fprintf(fid,"#  estimate SNR|Eb/N0  :   %s\n", which_snr_ebn0 == ESTIMATE_SNR ? "SNR" : "Eb/N0");
    fprintf(fid,"#  target error rate   :   %-12.4e\n", error_rate);
    fprintf(fid,"#  frame length        :   %u bytes\n", opts.dec_msg_len);
    fprintf(fid,"#  fec (inner)         :   %s\n", fec_scheme_str[opts.fec0][0]);
    fprintf(fid,"#  fec (outer)         :   %s\n", fec_scheme_str[opts.fec1][0]);
    fprintf(fid,"#\n");
    fprintf(fid,"# %-12s %4s %4s %12s %12s %12s\n", "modem", "id", "b/s", "rate [b/s]", "SNR [dB]", "Eb/N0 [dB]");

    for (i=1; i<LIQUID_MODEM_NUM_FULL_SCHEMES; i++) {
        // set up options from internal structure in modem_common.c
        opts.ms     = modulation_types[i].ms;
        opts.bps    = modulation_types[i].bps;

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

        // save to output file
        fprintf(fid," \"%s\"", modulation_types[i].name);
        unsigned int k;
        for (k=strlen(modulation_types[i].name)+1; k<12; k++)
            fprintf(fid," ");

        // save to output file
        fprintf(fid," %4u %4u %12.6f %12.6f %12.6f\n",
                opts.ms,
                opts.bps,
                rate,
                SNRdB_hat,
                EbN0dB_hat);
    }

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}

