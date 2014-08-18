//
// symtrack_cccf_example.c
//
// This example demonstrates how to recover data symbols using the symtrack
// object.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME "symtrack_cccf_example.m"

// print usage/help message
void usage()
{
    printf("symtrack_cccf_example [options]\n");
    printf("  h     : print this help file\n");
    printf("  m     : filter delay (symbols),  default: 3\n");
    printf("  b     : filter excess bandwidth, default: 0.5\n");
    printf("  s     : signal-to-noise ratio,   default: 30dB\n");
    printf("  w     : timing pll bandwidth,    default: 0.02\n");
    printf("  n     : number of symbols,       default: 200\n");
    printf("  t     : timing phase offset [%% symbol], t in [-0.5,0.5], default: -0.2\n");
}


int main(int argc, char*argv[]) {
    srand(time(NULL));

    // options
    unsigned int m           = 3;       // filter delay (symbols)
    float        beta        = 0.5f;    // filter excess bandwidth factor
    unsigned int num_symbols = 800;     // number of data symbols
    float        SNRdB       = 30.0f;   // signal-to-noise ratio
    float        bandwidth   =  0.02f;  // loop filter bandwidth
    float        tau         = -0.2f;   // fractional symbol offset
    
    int dopt;
    while ((dopt = getopt(argc,argv,"hm:b:s:w:n:t:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                        return 0;
        case 'm':   m           = atoi(optarg);     break;
        case 'b':   beta        = atof(optarg);     break;
        case 's':   SNRdB       = atof(optarg);     break;
        case 'w':   bandwidth   = atof(optarg);     break;
        case 'n':   num_symbols = atoi(optarg);     break;
        case 't':   tau         = atof(optarg);     break;
        default:
            exit(1);
        }
    }

    // validate input
    if (m < 1) {
        fprintf(stderr,"error: m (filter delay) must be greater than 0\n");
        exit(1);
    } else if (beta <= 0.0f || beta > 1.0f) {
        fprintf(stderr,"error: beta (excess bandwidth factor) must be in (0,1]\n");
        exit(1);
    } else if (bandwidth <= 0.0f) {
        fprintf(stderr,"error: timing PLL bandwidth must be greater than 0\n");
        exit(1);
    } else if (num_symbols == 0) {
        fprintf(stderr,"error: number of symbols must be greater than 0\n");
        exit(1);
    } else if (tau < -1.0f || tau > 1.0f) {
        fprintf(stderr,"error: timing phase offset must be in [-1,1]\n");
        exit(1);
    }

    unsigned int i;

    // derived/fixed values
    unsigned int k=2;   // samples per symbol
    unsigned int num_samples = k*num_symbols;
    float        gamma       = 0.1f;        // channel gain

    float complex x[num_samples];           // interpolated samples
    float complex y[num_samples];           // 
    float complex sym_out[num_symbols + 64];// synchronized symbols

    // 
    // generate input sequence
    //

    // design interpolating filter
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(LIQUID_FIRFILT_RRC,k,m,beta,tau);
    for (i=0; i<num_symbols; i++) {
        // generate random QPSK symbol
        float complex s = cexpf(_Complex_I*0.5f*M_PI*((rand() % 4) + 0.5f));

        // interpolate
        firinterp_crcf_execute(interp, s, &x[i*k]);
    }
    firinterp_crcf_destroy(interp);

    // TODO: 

    // 
    // add channel gain, noise
    //
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples; i++) {
        y[i] =  x[i] + nstd*(randnf() + _Complex_I*randnf());
        y[i] *= gamma;
    }

    // 
    // create and run symbol synchronizer
    //

    symtrack_cccf symtrack = symtrack_cccf_create(LIQUID_FIRFILT_RRC,
                                            k, m, beta, LIQUID_MODEM_QPSK);

    unsigned int num_symbols_sync = 0;
    symtrack_cccf_execute_block(symtrack, y, num_samples, sym_out, &num_symbols_sync);
    symtrack_cccf_destroy(symtrack);

    // print results
    printf("symbols in  : %u\n", num_symbols);
    printf("symbols out : %u\n", num_symbols_sync);

    // clean it up
    printf("done.\n");
    return 0;
}
