// This example runs a bit error rate simulation for a specified modulation
// scheme and saves the resulting data points to a file for plotting.
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

// print usage/help message
void usage()
{
    printf("modemcf_example [options]\n");
    printf(" -h          : print help\n");
    printf(" -m <scheme> : modulation scheme\n");
    liquid_print_modulation_schemes();
}

int main(int argc, char*argv[])
{
    // simulation parameters
    modulation_scheme ms    = LIQUID_MODEM_QPSK; // modulation scheme
    float SNRdB_min         = -5.0f;             // starting SNR value
    float SNRdB_max         = 40.0f;             // maximum SNR value
    float SNRdB_step        =  1.0f;             // step size
    unsigned int num_trials = 1e6;               // number of symbols
    char filename[]         = "modemcf_ber_example.m";

    int dopt;
    while ((dopt = getopt(argc,argv,"hm:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();        return 0;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme '%s'\n",
                    argv[0], optarg);
                return 1;
            }
            break;
        default:
            exit(1);
        }
    }

    // create modem objects
    modemcf mod = modemcf_create(ms);   // modulator
    modemcf dem = modemcf_create(ms);   // demodulator

    // compute derived values and initialize counters
    unsigned int  m = modemcf_get_bps(mod);       // modulation bits/symbol
    unsigned int  M = 1 << m;                   // constellation size: 2^m
    unsigned int  i, sym_in, sym_out;
    float complex sample;

    // iterate through SNR values
    printf("# modulation scheme : %s\n", modulation_types[ms].name);
    printf("# %8s %8s %8s %12s\n", "SNR [dB]", "errors", "trials", "BER");
    float SNRdB = SNRdB_min;    // set SNR value
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all; SNRdB=[]; BER=[]; ms='%s';\n", modulation_types[ms].name);
    while (1) {
        // compute noise standard deviation
        float nstd = powf(10.0f, -SNRdB/20.0f);

        // reset modem objects (only necessary for differential schemes)
        modemcf_reset(mod);
        modemcf_reset(dem);

        // run trials
        unsigned int num_bit_errors = 0;
        for (i=0; i<num_trials; i++) {
            // generate random input symbol and modulate
            sym_in = rand() % M;
            modemcf_modulate(mod, sym_in, &sample);

            // add complex noise and demodulate
            sample += nstd*(randnf() + _Complex_I*randnf())/sqrtf(2.0f);
            modemcf_demodulate(dem, sample, &sym_out);

            // count errors
            num_bit_errors += count_bit_errors(sym_in, sym_out);
        }

        // compute results and print formatted results to screen
        unsigned int num_bit_trials = m * num_trials;
        float ber = (float)num_bit_errors / (float)(num_bit_trials);
        printf("  %8.2f %8u %8u %12.4e\n", SNRdB, num_bit_errors, num_bit_trials, ber);

        if (num_bit_errors > 0)
            fprintf(fid,"SNRdB(end+1)=%12.3f; BER(end+1)=%12.4e;\n", SNRdB, ber);

        // stop iterating if SNR exceed maximum or no errors were detected
        SNRdB += SNRdB_step;
        if (SNRdB > SNRdB_max || num_bit_errors == 0)
            break;

    }
    fprintf(fid,"figure; semilogy(SNRdB,BER,'-x'); grid on; axis([SNRdB(1) SNRdB(end) 1e-6 1]);\n");
    fprintf(fid,"xlabel('SNR [dB]'); ylabel('BER'); title(ms)\n");
    fclose(fid);
    printf("results written to %s\n", filename);

    // destroy modem objects and return
    modemcf_destroy(mod);
    modemcf_destroy(dem);
    return 0;
}
