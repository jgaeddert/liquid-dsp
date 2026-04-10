const char __docstr__[] =
"This example demonstrates the performance of the qpacket modem"
" object to combine forward error-correction and modulation in one"
" simple interface.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"
    
int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qpacketmodem_performance_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    mod,      "qpsk", 'm', "FEC scheme", liquid_argparse_modem);
    liquid_argparse_add(char*,    crc,     "crc32", 'v', "CRC scheme", liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0,     "g2412", 'c', "FEC scheme (inner)", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,      "none", 'k', "FEC scheme (outer)", liquid_argparse_fec);
    liquid_argparse_add(unsigned, payload_len, 72,  'n', "data length (bytes)", NULL);
    liquid_argparse_add(float,    SNRdB_min,  -5.0, 's', "starting SNR value", NULL);
    liquid_argparse_add(float,    SNRdB_max,  20.0, 'x', "maximum SNR value", NULL);
    liquid_argparse_add(float,    SNRdB_step,  0.5, 'd', "step size", NULL);
    liquid_argparse_add(unsigned, min_errors,   50, 'e', "minimum number of errors to simulation", NULL);
    liquid_argparse_add(unsigned, min_trials, 2000, 't', "minimum number of packet trials to simulate", NULL);
    liquid_argparse_add(unsigned, max_trials,40000, 'T', "maximum number of packet trials to simulate", NULL);
    liquid_argparse_parse(argc,argv);

    modulation_scheme ms    = (modulation_scheme)liquid_getopt_str2mod(mod);
    crc_scheme        check = (crc_scheme)liquid_getopt_str2crc(crc);
    fec_scheme        fec0  = (fec_scheme)liquid_getopt_str2fec(fs0);
    fec_scheme        fec1  = (fec_scheme)liquid_getopt_str2fec(fs1);

    // create and configure packet encoder/decoder object
    qpacketmodem q = qpacketmodem_create();
    qpacketmodem_configure(q, payload_len, check, fec0, fec1, ms);
    qpacketmodem_print(q);

    // get frame length
    unsigned int frame_len = qpacketmodem_get_frame_len(q);

    // initialize payload
    LIQUID_VLA(unsigned char, payload_tx, payload_len); // payload (transmitted)
    LIQUID_VLA(unsigned char, payload_rx, payload_len); // payload (received)
    LIQUID_VLA(liquid_float_complex, frame_tx, frame_len);   // frame samples (transmitted)
    LIQUID_VLA(liquid_float_complex, frame_rx, frame_len);   // frame samples (received)

    // output file
    FILE* fid = fopen(filename, "w");
    fprintf(fid,"%% %s: auto-generated file\n", filename);
    fprintf(fid,"clear all; close all; SNR=[]; ber=[]; per=[];\n");

    printf("  %8s %8s %8s %12s %8s %8s %6s\n",
            "SNR [dB]", "errors", "bits", "BER", "errors", "packets", "PER");
    float SNRdB = SNRdB_min;
    while (SNRdB < SNRdB_max) {
        // reset counters
        float nstd = powf(10.0f, -SNRdB/20.0f); // noise standard deviation
        unsigned int num_trials = 0, num_errors = 0;
        unsigned int num_bit_trials = 0, num_bit_errors = 0;
        while (1) {
            // initialize payload
            unsigned int i;
            for (i=0; i<payload_len; i++) {
                payload_tx[i] = rand() & 0xff;
                payload_rx[i] = 0x00;
            }

            // encode frame
            qpacketmodem_encode(q, payload_tx, frame_tx);

            // add noise
            for (i=0; i<frame_len; i++)
                frame_rx[i] = frame_tx[i] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

            // decode frame
            int crc_pass = qpacketmodem_decode(q, frame_rx, payload_rx);

            // accumulate errors
            num_bit_errors    += count_bit_errors_array(payload_tx, payload_rx, payload_len);
            num_errors += crc_pass ? 0 : 1;
            num_trials += 1;
            num_bit_trials    += 8*payload_len;

            if (num_trials < min_trials)
                continue;
            if (num_errors >= min_errors)
                break;
            if (num_trials >= max_trials)
                break;
        }
        float BER = (float)num_bit_errors    / (float)num_bit_trials;
        float PER = (float)num_errors / (float)num_trials;
        printf("  %8.2f %8u %8u %12.4e %8u %8u %6.2f%%\n",
                SNRdB,
                num_bit_errors, num_bit_trials, BER,
                num_errors, num_trials, PER*100.0f);
        fprintf(fid,"SNR(end+1)=%g; ber(end+1)=%g; per(end+1)=%g;\n", SNRdB, BER, PER);
        if (num_errors < min_errors)
            break;
        SNRdB += SNRdB_step;
    }

    // destroy allocated objects
    qpacketmodem_destroy(q);

    fprintf(fid,"figure('position',[100 100 500 720]);\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  gamma = 10.^(SNR/10);\n");
    fprintf(fid,"  ber_qpsk = 0.5*(1 - erf(sqrt(gamma)/sqrt(2)));\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"    semilogy(SNR, ber,     '-o', 'LineWidth',2, 'MarkerSize',2);\n");
    fprintf(fid,"    semilogy(SNR, ber_qpsk,'-o', 'LineWidth',2, 'MarkerSize',2);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  xlabel('SNR [dB]');\n");
    fprintf(fid,"  ylabel('Bit Error Rate');\n");
    fprintf(fid,"  axis([%f SNR(end)+5 1e-6 1]);\n", SNRdB_min);
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  legend('Measured','Uncoded QPSK','location','southwest');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  semilogy(SNR, per,'-o', 'LineWidth',2, 'MarkerSize',2);\n");
    fprintf(fid,"  xlabel('SNR [dB]');\n");
    fprintf(fid,"  ylabel('Packet Error Rate');\n");
    fprintf(fid,"  axis([%f SNR(end)+5 1e-3 1]);\n", SNRdB_min);
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}

