// 
// bpresync_example.c
//
// Test binary pre-demodulator synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "liquid.h"

#define OUTPUT_FILENAME "bpresync_example.m"

// print usage/help message
void usage()
{
    printf("bpresync_example -- test binary pre-demodulation synchronization\n");
    printf("options (default values in <>):\n");
    printf("  h     : print usage/help\n");
    printf("  k     : samples/symbol, default: 2\n");
    printf("  m     : filter delay [symbols], default: 5\n");
    printf("  n     : number of data symbols, default: 64\n");
    printf("  b     : bandwidth-time product, 0 < b <= 1, default: 0.3\n");
    printf("  F     : carrier frequency offset, default: 0.02\n");
    printf("  S     : SNR [dB], default: 20\n");
}

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int k=2;                   // filter samples/symbol
    unsigned int m=5;                   // filter delay (symbols)
    float beta=0.3f;                    // bandwidth-time product
    float dt = 0.0f;                    // fractional sample timing offset
    unsigned int num_sync_symbols = 64; // number of synchronization symbols
    float SNRdB = 20.0f;                // signal-to-noise ratio [dB]
    float dphi = 0.02f;                 // carrier frequency offset
    float phi  = 0.0f;                  // carrier phase offset

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:n:b:F:S:")) != EOF) {
        switch (dopt) {
        case 'h': usage();                          return 0;
        case 'k': k = atoi(optarg);                 break;
        case 'm': m = atoi(optarg);                 break;
        case 'n': num_sync_symbols = atoi(optarg);  break;
        case 'b': beta = atof(optarg);              break;
        case 'F': dphi = atof(optarg);              break;
        case 'S': SNRdB = atof(optarg);             break;
        default:
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
    unsigned int num_symbols = num_sync_symbols + 2*m + 10;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    float complex seq[num_sync_symbols];    // synchronization pattern (symbols)
    float complex s0[k*num_sync_symbols];   // synchronization pattern (samples)
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    float complex rxy[num_samples];         // pre-demod correlation output
    float dphi_hat[num_samples];            // carrier offset estimate

    // create transmit/receive interpolator/decimator
    interp_crcf interp_tx = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,k,m,beta,dt);

    // generate synchronization pattern (BPSK) and interpolate
    for (i=0; i<num_sync_symbols + 2*m; i++) {
        float complex sym = 0.0f;
    
        if (i < num_sync_symbols) {
            sym = rand() % 2 ? -1.0f : 1.0f;
            seq[i] = sym;
        }

        if (i < 2*m) interp_crcf_execute(interp_tx, sym, s0);
        else         interp_crcf_execute(interp_tx, sym, &s0[k*(i-2*m)]);
    }

    // reset interpolator
    interp_crcf_clear(interp_tx);

    // interpolate input
    for (i=0; i<num_symbols; i++) {
        float complex sym = i < num_sync_symbols ? seq[i] : 0.0f;

        interp_crcf_execute(interp_tx, sym, &x[k*i]);
    }

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i]*cexpf(_Complex_I*(dphi*i + phi)) + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

    // create cross-correlator
    bpresync_cccf sync = bpresync_cccf_create(s0, k*num_sync_symbols, 0.05f, 11);
    bpresync_cccf_print(sync);

    // push signal through cross-correlator
    float rxy_max = 0.0f;
    for (i=0; i<num_samples; i++) {
        
        // correlate
        bpresync_cccf_correlate(sync, y[i], &rxy[i], &dphi_hat[i]);

        // detect...
        if (cabsf(rxy[i]) > 0.6f) {
            printf("****** preamble found, rxy = %12.8f (dphi-hat: %12.8f), i=%3u ******\n",
                    cabsf(rxy[i]), dphi_hat[i], i);
        }
        
        // retain maximum
        if (cabsf(rxy[i]) > rxy_max)
            rxy_max = cabsf(rxy[i]);
    }

    // destroy objects
    interp_crcf_destroy(interp_tx);
    bpresync_cccf_destroy(sync);
    
    // print results
    printf("rxy (max) : %12.8f\n", rxy_max);

    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"k           = %u;\n", k);

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
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(y), t,imag(y));\n");
    fprintf(fid,"  axis([0 num_symbols -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('received signal');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,abs(rxy));\n");
    fprintf(fid,"  axis([0 num_symbols 0 1.5]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('correlator output');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
