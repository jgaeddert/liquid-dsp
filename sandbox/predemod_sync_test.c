// 
// predemod_sync_test.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "predemod_sync_test.m"

// print usage/help message
void usage()
{
    printf("predemod_sync_test -- test pre-demodulation synchronization\n");
    printf("options (default values in <>):\n");
    printf("  h     : print usage/help\n");
    printf("  k     : samples/symbol <4>\n");
    printf("  m     : filter delay [symbols], <3>\n");
    printf("  n     : number of data symbols <64>\n");
    printf("  b     : bandwidth-time product, 0 <= b <= 1, <0.3>\n");
    printf("  s     : SNR [dB] <30>\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int k=4;                   // filter samples/symbol
    unsigned int m=3;                   // filter delay (symbols)
    float beta=0.5f;                    // bandwidth-time product
    float dt = 0.2f;                    // fractional sample timing offset
    unsigned int num_data_symbols = 64; // number of data symbols
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]
    unsigned int g = 0x0067;            // m-sequence generator polynomial
    float dphi = 0.0f;                  // carrier frequency offset
    float phi  = 0.0f;                  // carrier phase offset

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:n:b:s:")) != EOF) {
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'k': k = atoi(optarg);     break;
        case 'm': m = atoi(optarg);     break;
        case 'n': num_data_symbols = atoi(optarg); break;
        case 'b': beta = atof(optarg);  break;
        case 's': SNRdB = atof(optarg); break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            exit(1);
        }
    }

    unsigned int i;

    // validate input
    if (beta <= 0.0f || beta >= 1.0f) {
        fprintf(stderr,"error: %s, bandwidth-time product must be in (0,1)\n", argv[0]);
        exit(1);
    }

    // derived values
    unsigned int num_symbols = num_data_symbols + 2*m;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    float complex rxy[num_samples];         // pre-demod output

    // create transmit/receive interpolator/decimator
    interp_crcf interp_tx = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,k,m,beta,dt);

    // create m-sequence generator
    unsigned int M = liquid_msb_index(g) - 1;   // m-sequence shift register length
    unsigned int N = (1 << m) - 1;              // m-sequence length
    unsigned int A = 1;                         // m-sequence initial state
    msequence ms = msequence_create(M,N,A);

    // create cross-correlator
    bsync_crcf sync = bsync_crcf_create_msequence(g,k);

    // generate symbols and interpolate
    for (i=0; i<num_symbols; i++) {
        unsigned int bit = i < N ? msequence_advance(ms) : rand() % 2;

        float complex symbol = i < num_data_symbols ? (bit ? 1.0f : -1.0f) : 0.0f;
        interp_crcf_execute(interp_tx, symbol, &x[k*i]);
    }

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i]*cexpf(_Complex_I*(dphi*i + phi)) + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    
    // push through synchronizer
    for (i=0; i<num_samples; i++) {
        bsync_crcf_correlate(sync, y[i], &rxy[i]);
    }

    // destroy objects
    interp_crcf_destroy(interp_tx);
    msequence_destroy(ms);
    bsync_crcf_destroy(sync);

    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %f;\n", beta);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"x   = zeros(1,num_samples);\n");
    fprintf(fid,"y   = zeros(1,num_samples);\n");
    fprintf(fid,"rxy = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]),   cimagf(x[i]));
        fprintf(fid,"y(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]),   cimagf(y[i]));
        fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }
    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,abs(rxy));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('correlator output');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
