// 
// fecsoft_ber_test.c
//
// Simulate error rate using soft vs. hard decoding
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fecsoft_ber_test.m"

void usage()
{
    printf("fec_ber\n");
    printf("  Simulates soft decoding\n");
    printf("options:\n");
    printf("  u/h   : print usage/help\n");
    printf("  s     : SNR start [dB], default: -5\n");
    printf("  x     : SNR max [dB], default: 5\n");
    printf("  n     : number of SNR steps, default: 21\n");
    printf("  t     : number of trials, default: 500\n");
    printf("  f     : frame size, default: 64\n");
    printf("  c     : coding scheme, (h74 default):\n");
    liquid_print_fec_schemes();
}

int main(int argc, char *argv[]) {
    // set initial seed to random
    srand(time(NULL));

    // options
    unsigned int n = 64;                    // frame size (bytes)
    float SNRdB_min = -5.0f;                // signal-to-noise ratio (minimum)
    float SNRdB_max =  5.0f;                // signal-to-noise ratio (maximum)
    unsigned int num_snr = 21;              // number of SNR steps
    unsigned int num_trials=500;            // number of trials
    fec_scheme fs = LIQUID_FEC_HAMMING74;   // error-correcting scheme

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uhs:x:n:t:f:c:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 's': SNRdB_min = atof(optarg);     break;
        case 'x': SNRdB_max = atof(optarg);     break;
        case 'n': num_snr = atoi(optarg);       break;
        case 't': num_trials = atoi(optarg);    break;
        case 'f': n = atoi(optarg);             break;
        case 'c':
            fs = liquid_getopt_str2fec(optarg);
            if (fs == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported fec scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        default:
            printf("error: %s, unknown option\n", argv[0]);
            exit(-1);
        }
    }

    unsigned int i;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    printf("dec msg len : %u\n", n);
    printf("enc msg len : %u\n", n_enc);
    unsigned char msg_org[n];            // original data message
    unsigned char msg_enc[n_enc];        // encoded data message
    float complex sym_rec[8*n_enc];      // received BPSK symbols
    unsigned char msg_cor_soft[8*n_enc]; // corrupted data message (soft bits)
    unsigned char msg_cor_hard[n_enc];   // corrupted data message (hard bits)
    unsigned char msg_dec_soft[n];       // decoded data message (soft bits)
    unsigned char msg_dec_hard[n];       // decoded data message (soft bits)

    // create object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    unsigned int bit_errors_soft[num_snr];
    unsigned int bit_errors_hard[num_snr];

    //
    // set up parameters
    //
    float SNRdB_step = (SNRdB_max - SNRdB_min) / (num_snr-1);

    // 
    // start trials
    //
    
    printf("  %8s [%8s] %8s %8s\n", "SNR [dB]", "trials", "soft", "hard");
    unsigned int s;
    for (s=0; s<num_snr; s++) {
        // compute SNR for this level
        float SNRdB = SNRdB_min + s*SNRdB_step;
        float nstd = powf(10.0f, -SNRdB/10.0f);

        // reset results
        bit_errors_soft[s] = 0;
        bit_errors_hard[s] = 0;

        unsigned int t;
        for (t=0; t<num_trials; t++) {
            // generate data
            for (i=0; i<n; i++)
                msg_org[i] = rand() & 0xff;

            // encode message
            fec_encode(q, n, msg_org, msg_enc);

            // modulate with BPSK
            for (i=0; i<n_enc; i++) {
                sym_rec[8*i+0] = (msg_enc[i] & 0x80) ? 1.0f : -1.0f;
                sym_rec[8*i+1] = (msg_enc[i] & 0x40) ? 1.0f : -1.0f;
                sym_rec[8*i+2] = (msg_enc[i] & 0x20) ? 1.0f : -1.0f;
                sym_rec[8*i+3] = (msg_enc[i] & 0x10) ? 1.0f : -1.0f;
                sym_rec[8*i+4] = (msg_enc[i] & 0x08) ? 1.0f : -1.0f;
                sym_rec[8*i+5] = (msg_enc[i] & 0x04) ? 1.0f : -1.0f;
                sym_rec[8*i+6] = (msg_enc[i] & 0x02) ? 1.0f : -1.0f;
                sym_rec[8*i+7] = (msg_enc[i] & 0x01) ? 1.0f : -1.0f;
            }

            // add noise
            for (i=0; i<8*n_enc; i++)
                sym_rec[i] += nstd*randnf()*cexpf(_Complex_I*2*M_PI*randf());

            // demodulate using LLR
            for (i=0; i<8*n_enc; i++) {
                float LLR = 2.0f * crealf(sym_rec[i]);
                int soft_bit = (int) (16*LLR + 127);
                if (soft_bit > 255) soft_bit = 255;
                if (soft_bit <   0) soft_bit = 0;
                msg_cor_soft[i] = (unsigned char)(soft_bit);
            }

            // convert to hard bits (hard decoding)
            for (i=0; i<n_enc; i++) {
                msg_cor_hard[i] = 0x00;

                msg_cor_hard[i] |=(msg_cor_soft[8*i+0] >> 0) & 0x80;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+1] >> 1) & 0x40;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+2] >> 2) & 0x20;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+3] >> 3) & 0x10;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+4] >> 4) & 0x08;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+5] >> 5) & 0x04;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+6] >> 6) & 0x02;
                msg_cor_hard[i] |=(msg_cor_soft[8*i+7] >> 7) & 0x01;
            }

            // decode
            fec_decode(q, n, msg_cor_hard, msg_dec_hard);
            fec_decode_soft(q, n, msg_cor_soft, msg_dec_soft);
            
            // tabulate results
            bit_errors_soft[s] += count_bit_errors_array(msg_org, msg_dec_soft, n);
            bit_errors_hard[s] += count_bit_errors_array(msg_org, msg_dec_hard, n);
        }

        // print results for this SNR step
        printf("  %8.3f [%8u] %8u %8u\n",
                SNRdB,
                8*n*num_trials,
                bit_errors_soft[s],
                bit_errors_hard[s]);
    }

    // clean up objects
    fec_destroy(q);

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n = %u;\n", 4);
    fprintf(fid,"k = %u;\n", 7);
    fprintf(fid,"num_snr = %u;\n", num_snr);
    fprintf(fid,"num_trials = %u;\n", num_trials);
    for (i=0; i<num_snr; i++) {
        fprintf(fid,"SNRdB(%4u) = %12.8f;\n",i+1, SNRdB_min + i*SNRdB_step);
        fprintf(fid,"bit_errors_soft(%6u) = %u;\n", i+1, bit_errors_soft[i]);
        fprintf(fid,"bit_errors_hard(%6u) = %u;\n", i+1, bit_errors_hard[i]);
    }
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(SNRdB, bit_errors_soft / (n*num_trials) + 1e-12,\n");
    fprintf(fid,"         SNRdB, bit_errors_hard / (n*num_trials) + 1e-12);\n");
    fprintf(fid,"axis([%f %f 1e-4 1]);\n", SNRdB_min, SNRdB_max);
    fprintf(fid,"legend('soft','hard',1);\n");
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Bit Error Rate');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

