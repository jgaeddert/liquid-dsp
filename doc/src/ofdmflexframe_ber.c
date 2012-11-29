//
// ofdmflexframesync_sim.c
//
// Simulates the ofdmflexframesync object and observes the
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
    printf("ofdmflexframesync_sim\n");
    printf("  Simulates the ofdmflexframesync object and observes the\n");
    printf("  probability of properly receiving a packet based on\n");
    printf("  the encoding scheme vs. SNR.\n");
    printf("options:\n");
    printf("  u/h   : print usage/help\n");
    printf("  o     : output filename\n");
    printf("  s     : SNR start [dB], -5\n");
    printf("  d     : SNR step [dB], 1.0\n");
    printf("  x     : SNR max [dB], 10\n");
    printf("  n     : number of trials, 1000\n");
    printf("  f     : frame bytes (packet len), 64\n");
    printf("  M     : number of subcarriers (must be even), default: 64\n");
    printf("  C     : cyclic prefix length, default: 16\n");
    printf("  m     : mod scheme, default: qpsk\n");
    liquid_print_modulation_schemes();
    printf("  c     : fec coding scheme (inner)\n");
    printf("  k     : fec coding scheme (outer)\n");
    liquid_print_fec_schemes();
}


int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB_start = -5.0f;
    float SNRdB_step  = 1.0f;
    float SNRdB_max   = 10.0f;
    unsigned int num_trials = 1000;
    float noise_floor = -30.0f;
    const char * filename = "ofdmflexframesync_sim_results.m";
    modulation_scheme mod_scheme = LIQUID_MODEM_QPSK;
    unsigned int M=64;
    unsigned int cp_len = 16;
    unsigned int payload_len = 64;
    crc_scheme crc  = LIQUID_CRC_32;
    fec_scheme fec0 = LIQUID_FEC_NONE;
    fec_scheme fec1 = LIQUID_FEC_NONE;

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uho:s:d:x:n:f:M:C:m:c:k:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'o': filename = optarg;            break;
        case 's': SNRdB_start = atof(optarg);   break;
        case 'd': SNRdB_step = atof(optarg);    break;
        case 'x': SNRdB_max = atof(optarg);     break;
        case 'n': num_trials = atol(optarg);    break;
        case 'f': payload_len = atol(optarg);   break;
        case 'M': M = atoi(optarg);             break;
        case 'C': cp_len = atoi(optarg);        break;
        case 'm':
            mod_scheme = liquid_getopt_str2mod(optarg);
            if (mod_scheme == LIQUID_MODEM_UNKNOWN) {
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

    // bookkeeping variables
    unsigned int n, i;
    float SNRdB = SNRdB_start;
    unsigned int snr_length =
        (unsigned int) fabs( (SNRdB_max-SNRdB_start)/SNRdB_step ) + 2;
    float SNR[snr_length];
    unsigned int NUM_PACKETS_FOUND[snr_length];
    unsigned int NUM_HEADERS_DECODED[snr_length];
    unsigned int NUM_PAYLOADS_DECODED[snr_length];

    // frame generater properties
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check           = crc;
    fgprops.fec0            = fec0;
    fgprops.fec1            = fec1;
    fgprops.mod_scheme      = mod_scheme;

    // initialize subcarrier allocation
    unsigned char p[M];
#if 0
    // initialize subcarriers to default
    ofdmframe_init_default_sctype(M,p);
#else
    unsigned int guard = M / 6;
    unsigned int pilot_spacing = 8;
    unsigned int i0 = (M/2) - guard;
    unsigned int i1 = (M/2) + guard;
    for (i=0; i<M; i++) {
        if ( i == 0 || (i > i0 && i < i1) )
            p[i] = OFDMFRAME_SCTYPE_NULL;
        else if ( (i%pilot_spacing)==0 )
            p[i] = OFDMFRAME_SCTYPE_PILOT;
        else
            p[i] = OFDMFRAME_SCTYPE_DATA;
    }
#endif

#if 0
    // print approximate spectral efficiency
    unsigned int framebits = 8*payload_len;
    unsigned int framesyms = 8*fgprops.payload_len / fgprops.mod_bps;
    unsigned int frame_len = fgprops.rampup_len + fgprops.phasing_len +
                             framesyms +
                             fgprops.rampdn_len;
    printf("frame spec. efficiency : %12.8f\n", (float)framebits / (float)frame_len);
#endif

    // TODO : set frame synchronizer properties

    // print information
    printf("    mod scheme  :   %s\n", modulation_types[mod_scheme].fullname);
    printf("    fec (inner) :   %s\n", fec_scheme_str[fec0][1]);
    printf("    fec (outer) :   %s\n", fec_scheme_str[fec1][1]);
    printf("    packet len  :   %u bytes\n", payload_len);
    printf("starting trials: \n");

    unsigned int num_packets_found;
    unsigned int num_headers_decoded;
    unsigned int num_payloads_decoded;
    // start running batch trials
        
    printf("  %12s %8s %16s %16s %16s\n", "SNR [dB]", "trials", "frame", "header", "payload");
    for (n=0; n<snr_length; n++) {

        // run simulation
        float dphi = 0.0f;  // carrier frequency offset
        ofdmflexframesync_per(&fgprops,M,cp_len,p,NULL,payload_len,
                              noise_floor,SNRdB,dphi,
                              num_trials,
                              &num_packets_found,
                              &num_headers_decoded,
                              &num_payloads_decoded);

        // SNR trial successful, increase SNR
        SNR[n] = SNRdB;
        NUM_PACKETS_FOUND[n]    = num_packets_found;
        NUM_HEADERS_DECODED[n]  = num_headers_decoded;
        NUM_PAYLOADS_DECODED[n] = num_payloads_decoded;
        
        printf("  %12.4f %8u %8u(%5.1f%%) %8u(%5.1f%%) %8u(%5.1f%%)\n",
            SNRdB,
            num_trials,
            num_packets_found,   (float)num_packets_found/(float)num_trials*100.0f,
            num_headers_decoded, (float)num_headers_decoded/(float)num_trials*100.0f,
            num_payloads_decoded,(float)num_payloads_decoded/(float)num_trials*100.0f);
        SNRdB += SNRdB_step;
    } // snr_length


    // write results to file
    FILE * fid;
    if (strcmp(filename,"")!=0) {
        fid = fopen(filename, "w");
        fprintf(fid,"%% %s : auto-generated file\n", filename);
        fprintf(fid,"\n\n");
        fprintf(fid,"clear all\n");
        fprintf(fid,"close all\n");
        fprintf(fid,"num_trials = %u;\n", num_trials);
        fprintf(fid,"n = %u;\n", snr_length);
        for (i=0; i<snr_length; i++) {
            fprintf(fid,"SNRdB(%4u) = %12.8f;\n",i+1,SNR[i]);
            fprintf(fid,"num_packets_found(%4u)    = %u;\n",i+1,NUM_PACKETS_FOUND[i]);
            fprintf(fid,"num_headers_decoded(%4u)  = %u;\n",i+1,NUM_HEADERS_DECODED[i]);
            fprintf(fid,"num_payloads_decoded(%4u) = %u;\n",i+1,NUM_PAYLOADS_DECODED[i]);
        }
        fprintf(fid,"\n\n");
        fprintf(fid,"figure;\n");
        fprintf(fid,"plot(SNRdB, num_packets_found   / num_trials,\n");
        fprintf(fid,"     SNRdB, num_headers_decoded / num_trials,\n");
        fprintf(fid,"     SNRdB, num_payloads_decoded / num_trials);\n");
        fprintf(fid,"legend('preamble','header','payload',0);\n");
        fprintf(fid,"xlabel('SNR [dB]');\n");
        fprintf(fid,"ylabel('Probability of Detection');\n");
        fprintf(fid,"grid on;\n");
        fprintf(fid,"title('ofdmflexframesync synchronization performance');\n");
        fclose(fid);
        printf("results written to %s\n", filename);
    }

#if 1
    fid = fopen("ofdmflexframesync_sim.dat","w");
    fprintf(fid,"# %s : auto-generated file\n", "ofdmflexframesync_sim.dat");
    fprintf(fid,"# options:\n");
    fprintf(fid,"#   number of trials   : %u\n", num_trials);
    fprintf(fid,"#   payload length     : %u (bytes)\n", payload_len);
    fprintf(fid,"#   modulation         : %s\n", modulation_types[mod_scheme].fullname);
    fprintf(fid,"#   fec (inner)        : %s\n", fec_scheme_str[fec0][0]);
    fprintf(fid,"#   fec (outer)        : %s\n", fec_scheme_str[fec1][0]);
    fprintf(fid,"#   num subcarriers    : %u\n", M);
    fprintf(fid,"#   cyclic prefix len  : %u\n", cp_len);
    fprintf(fid,"#\n");
    fprintf(fid,"# %12s %12s %12s %12s\n",
        "SNR [dB]",
        "packets",
        "headers",
        "payloads");
    for (i=0; i<snr_length; i++) {
        fprintf(fid,"  %12.8f %12.8f %12.8f %12.8f\n",
                SNR[i],
                (float)NUM_PACKETS_FOUND[i]     / (float)num_trials,
                (float)NUM_HEADERS_DECODED[i]   / (float)num_trials,
                (float)NUM_PAYLOADS_DECODED[i]  / (float)num_trials);
    }
    fclose(fid);
#endif

    // clean up objects

    return 0;
}

