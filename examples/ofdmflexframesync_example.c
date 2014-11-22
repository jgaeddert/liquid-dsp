//
// ofdmflexframesync_example.c
//
// Example demonstrating the OFDM flexible frame synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

void usage()
{
    printf("ofdmflexframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  s     : signal-to-noise ratio [dB], default: 20\n");
    printf("  F     : carrier frequency offset, default: 0.002\n");
    printf("  M     : number of subcarriers (must be even), default: 64\n");
    printf("  C     : cyclic prefix length, default: 16\n");
    printf("  n     : payload length [bytes], default: 120\n");
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
    printf("  v     : data integrity check: crc32 default\n");
    liquid_print_crc_schemes();
    printf("  c     : coding scheme (inner): h74 default\n");
    printf("  k     : coding scheme (outer): none default\n");
    liquid_print_fec_schemes();
    printf("  d     : enable debugging\n");
}

// callback function
int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata);

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int M = 64;                // number of subcarriers
    unsigned int cp_len = 16;           // cyclic prefix length
    unsigned int taper_len = 4;         // taper length
    unsigned int payload_len = 120;     // length of payload (bytes)
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    fec_scheme fec0  = LIQUID_FEC_NONE;
    fec_scheme fec1  = LIQUID_FEC_HAMMING128;
    crc_scheme check = LIQUID_CRC_32;
    float noise_floor = -30.0f;         // noise floor [dB]
    float SNRdB = 20.0f;                // signal-to-noise ratio [dB]
    float dphi = 0.02f;                 // carrier frequency offset
    int debug_enabled =  0;             // enable debugging?

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"uhds:F:M:C:n:m:v:c:k:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'd': debug_enabled = 1;            break;
        case 's': SNRdB         = atof(optarg); break;
        case 'F': dphi          = atof(optarg); break;
        case 'M': M             = atoi(optarg); break;
        case 'C': cp_len        = atoi(optarg); break;
        case 'n': payload_len   = atol(optarg); break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported mod. scheme: %s\n", argv[0], optarg);
                exit(-1);
            }
            break;
        case 'v':
            // data integrity check
            check = liquid_getopt_str2crc(optarg);
            if (check == LIQUID_CRC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported CRC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'c':
            // inner FEC scheme
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported inner FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'k':
            // outer FEC scheme
            fec1 = liquid_getopt_str2fec(optarg);
            if (fec1 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported outer FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        default:
            exit(-1);
        }
    }

    unsigned int i;

    // TODO : validate options

    // derived values
    unsigned int frame_len = M + cp_len;
    float complex buffer[frame_len]; // time-domain buffer
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // allocate memory for header, payload
    unsigned char header[8];
    unsigned char payload[payload_len];

    // initialize subcarrier allocation
    unsigned char p[M];
    ofdmframe_init_default_sctype(M, p);

    // create frame generator
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check           = check;
    fgprops.fec0            = fec0;
    fgprops.fec1            = fec1;
    fgprops.mod_scheme      = ms;
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, taper_len, p, &fgprops);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, taper_len, p, callback, (void*)payload);
    if (debug_enabled)
        ofdmflexframesync_debug_enable(fs);

    // initialize header/payload and assemble frame
    for (i=0; i<8; i++)
        header[i] = i & 0xff;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload, payload_len);
    ofdmflexframegen_print(fg);
    ofdmflexframesync_print(fs);

    // initialize frame synchronizer with noise
    for (i=0; i<1000; i++) {
        float complex noise = nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
        ofdmflexframesync_execute(fs, &noise, 1);
    }

    // generate frame, push through channel
    int last_symbol=0;
    nco_crcf nco = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_frequency(nco, dphi);
    while (!last_symbol) {
        // generate symbol
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer);

        // apply channel
        for (i=0; i<frame_len; i++) {
            float complex noise = nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
            buffer[i] *= gamma;
            buffer[i] += noise;
            
            nco_crcf_mix_up(nco, buffer[i], &buffer[i]);
            nco_crcf_step(nco);
        }

        // receive symbol
        ofdmflexframesync_execute(fs, buffer, frame_len);
    }
    nco_crcf_destroy(nco);

    // export debugging file
    if (debug_enabled)
        ofdmflexframesync_debug_print(fs, "ofdmflexframesync_debug.m");

    // destroy objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

// callback function
int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata)
{
    printf("**** callback invoked : rssi = %8.3f dB, evm = %8.3f dB, cfo = %8.5f\n", _stats.rssi, _stats.evm, _stats.cfo);

    unsigned int i;

    // print header data to standard output
    printf("  header rx  :");
    for (i=0; i<8; i++)
        printf(" %.2X", _header[i]);
    printf("\n");

    // print payload data to standard output
    printf("  payload rx :");
    for (i=0; i<_payload_len; i++) {
        printf(" %.2X", _payload[i]);
        if ( ((i+1)%26)==0 && i !=_payload_len-1 )
            printf("\n              ");
    }
    printf("\n");

    // count errors in received payload and print to standard output
    unsigned char * payload_tx = (unsigned char*) _userdata;
    unsigned int num_errors = count_bit_errors_array(_payload, payload_tx, _payload_len);
    printf("  bit errors : %u / %u\n", num_errors, 8*_payload_len);

    return 0;
}

