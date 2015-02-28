//
// qpacketmodem_performance_example.c
//
// This example demonstrates the performance of the qpacket modem
// object to combine forward error-correction and modulation in one
// simple interface.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>

#include "liquid.h"
    
#define OUTPUT_FILENAME "qpacketmodem_performance_example.m"

void usage()
{
    printf("qpacketmodem_performance_example [options]\n");
    printf("  h     : print usage\n");
    printf("  p     : payload length [bytes], default: 400\n");
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
    printf("  v     : data integrity check: crc32 default\n");
    liquid_print_crc_schemes();
    printf("  c     : coding scheme (inner): g2412 default\n");
    printf("  k     : coding scheme (outer): none default\n");
    liquid_print_fec_schemes();
    printf("  s     : SNR start [dB], default: -2\n");
    printf("  x     : SNR max [dB], default: 13\n");
    printf("  n     : number of SNR steps, default: 16\n");
    printf("  t     : number of trials, default: 800\n");
}

int main(int argc, char *argv[])
{
    //srand( time(NULL) );

    // options
    modulation_scheme ms                = LIQUID_MODEM_QPSK;    // mod. scheme
    crc_scheme        check             = LIQUID_CRC_32;        // data validity check
    fec_scheme        fec0              = LIQUID_FEC_GOLAY2412; // fec (inner)
    fec_scheme        fec1              = LIQUID_FEC_NONE;      // fec (outer)
    unsigned int      payload_len       = 400;                  // payload length
    float             SNRdB_min         = -5.0f;                // signal-to-noise ratio (minimum)
    float             SNRdB_max         = 10.0f;                // signal-to-noise ratio (maximum)
    unsigned int      num_snr           = 31;                   // number of SNR steps
    unsigned int      num_packet_trials = 800;                  // number of trials

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hp:m:v:c:k:s:x:n:t:")) != EOF){
        switch (dopt) {
        case 'h': usage();                                           return 0;
        case 'p': payload_len       = atol(optarg);                  break;
        case 'm': ms                = liquid_getopt_str2mod(optarg); break;
        case 'v': check             = liquid_getopt_str2crc(optarg); break;
        case 'c': fec0              = liquid_getopt_str2fec(optarg); break;
        case 'k': fec1              = liquid_getopt_str2fec(optarg); break;
        case 's': SNRdB_min         = atof(optarg);                  break;
        case 'x': SNRdB_max         = atof(optarg);                  break;
        case 'n': num_snr           = atoi(optarg);                  break;
        case 't': num_packet_trials = atoi(optarg);                  break;
        default:
            exit(-1);
        }
    }

    unsigned int i;

    // derived values
    float SNRdB_step = (SNRdB_max - SNRdB_min) / (num_snr-1);

    // create and configure packet encoder/decoder object
    qpacketmodem q = qpacketmodem_create();
    qpacketmodem_configure(q, payload_len, check, fec0, fec1, ms);
    qpacketmodem_print(q);

    // get frame length
    unsigned int frame_len = qpacketmodem_get_frame_len(q);
    unsigned int num_bit_trials = 8*num_packet_trials*payload_len;

    // initialize payload
    unsigned char payload_tx       [payload_len]; // payload (transmitted)
    unsigned char payload_rx       [payload_len]; // payload (received)
    float complex frame_tx         [frame_len];   // frame samples (transmitted)
    float complex frame_rx         [frame_len];   // frame samples (received)
    unsigned int  num_bit_errors   [num_snr];     // bit errors for each SNR point
    unsigned int  num_packet_errors[num_snr];     // packet errors for each SNR point
    float         BER              [num_snr];     // bit error rate
    float         PER              [num_snr];     // packet error rate

    printf("  %8s %8s %8s %12s %8s %8s %6s\n",
            "SNR [dB]", "errors", "bits", "BER", "errors", "packets", "PER");
    unsigned int s;
    for (s=0; s<num_snr; s++) {
        // compute SNR for this level
        float SNRdB = SNRdB_min + s*SNRdB_step; // SNR in dB for this round
        float nstd = powf(10.0f, -SNRdB/20.0f); // noise standard deviation

        // reset counters
        num_bit_errors[s]    = 0;
        num_packet_errors[s] = 0;

        unsigned int t;
        for (t=0; t<num_packet_trials; t++) {
            // initialize payload
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
            num_bit_errors[s]    += count_bit_errors_array(payload_tx, payload_rx, payload_len);
            num_packet_errors[s] += crc_pass ? 0 : 1;

        }
        BER[s] = (float)num_bit_errors[s]    / (float)num_bit_trials;
        PER[s] = (float)num_packet_errors[s] / (float)num_packet_trials;
        printf("  %8.2f %8u %8u %12.4e %8u %8u %6.2f%%\n",
                SNRdB,
                num_bit_errors[s], num_bit_trials, BER[s],
                num_packet_errors[s], num_packet_trials, PER[s]*100.0f);
    }

    // destroy allocated objects
    qpacketmodem_destroy(q);

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"payload_len       = %u;  %% payload length [bytes]\n", payload_len);
    fprintf(fid,"frame_len         = %u;  %% frame length [symbols]\n", frame_len);
    fprintf(fid,"num_snr           = %u;\n", num_snr);
    fprintf(fid,"num_packet_trials = %u;\n", num_packet_trials);
    fprintf(fid,"num_bit_trials    = %u;\n", num_bit_trials);
    fprintf(fid,"SNRdB_min         = %8.2f;\n", SNRdB_min);
    fprintf(fid,"SNRdB_max         = %8.2f;\n", SNRdB_max);
    fprintf(fid,"scheme            = '%s/%s/%s';\n", 
            fec_scheme_str[fec0][1],
            fec_scheme_str[fec1][1],
            modulation_types[ms].name);
    fprintf(fid,"rate              = 8*payload_len / frame_len; %% true rate [bits/symbol]\n");
    for (i=0; i<num_snr; i++) {
        fprintf(fid,"SNRdB(%4u) = %12.8f; ",  i+1, SNRdB_min + i*SNRdB_step);
        fprintf(fid,"BER(%6u)   = %12.4e; ",  i+1, BER[i]);
        fprintf(fid,"PER(%6u)   = %12.4e;\n", i+1, PER[i]);
    }
    
    // plot BER vs. SNR
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"SNRdB_bpsk = -15:0.5:40;\n");
    fprintf(fid,"semilogy(SNRdB_bpsk, 0.5*erfc(sqrt(10.^[SNRdB_bpsk/10]))+1e-12,'-x',\n");
    fprintf(fid,"         SNRdB,      BER, '-x');\n");
    fprintf(fid,"axis([SNRdB_min SNRdB_max 1e-6 1]);\n");
    fprintf(fid,"legend('uncoded BPSK',scheme,'location','northeast');\n");
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Bit Error Rate');\n");
    fprintf(fid,"title(['BER vs. SNR for ' scheme]);\n");
    fprintf(fid,"grid on;\n");

    // plot PER vs. SNR
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(SNRdB, PER, '-x');\n");
    fprintf(fid,"axis([SNRdB_min SNRdB_max 1e-3 1]);\n");
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Packet Error Rate');\n");
    fprintf(fid,"title(['PER vs. SNR for ' scheme]);\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);


    printf("done.\n");
    return 0;
}

