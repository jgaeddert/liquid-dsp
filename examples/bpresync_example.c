// 
// bpresync_example.c
//
// Test binary pre-demodulator synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "bpresync_example.m"

// print usage/help message
void usage()
{
    printf("bpresync_example -- test binary pre-demodulation synchronization\n");
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
    unsigned int m=5;                   // filter delay (symbols)
    float beta=0.5f;                    // bandwidth-time product
    float dt = 0.0f;                    // fractional sample timing offset
    unsigned int num_sync_symbols = 64; // number of synchronization symbols
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]
    float dphi = 0.0f;                  // carrier frequency offset
    float phi  = 0.0f;                  // carrier phase offset

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:n:b:s:")) != EOF) {
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'k': k = atoi(optarg);             break;
        case 'm': m = atoi(optarg);             break;
        case 'n': num_sync_symbols = atoi(optarg);   break;
        case 'b': beta = atof(optarg);          break;
        case 's': SNRdB = atof(optarg);         break;
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
    unsigned int num_symbols = num_sync_symbols + 2*m;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    float complex seq[num_sync_symbols];    // synchronization pattern (symbols)
    float complex s0[k*num_sync_symbols];   // synchronization pattern (samples)
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    float complex z[num_samples];           // matched filter output
    float complex rxy[num_samples];         // pre-demod output
    float complex dphi_hat[num_samples];    // pre-demod output

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

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i]*cexpf(_Complex_I*(dphi*i + phi)) + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

    // create cross-correlator
    bpresync_cccf sync = bpresync_cccf_create(s0, k*num_sync_symbols, 0.0f, 1);

    // push signal through cross-correlator
    float rxy_max = 0.0f;
    for (i=0; i<num_samples; i++) {
        
        // correlate
        bpresync_cccf_correlate(sync, z[i], &rxy[i], &dphi_hat[i]);

        // detect...
        if (cabsf(rxy[i]) > 0.6f) {
            printf("****** preamble found, rxy = %12.8f, i=%3u ******\n", cabsf(rxy[i]), i);
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
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %f;\n", beta);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"x   = zeros(1,num_samples);\n");
    fprintf(fid,"y   = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]),   cimagf(x[i]));
        fprintf(fid,"y(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]),   cimagf(y[i]));
        fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }
    fprintf(fid,"z   = zeros(1,num_samples);\n");
    fprintf(fid,"rxy = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"z(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(z[i]),   cimagf(z[i]));
        fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }

    // save synchronization sequence
    fprintf(fid,"num_sync_symbols = %u;\n", num_sync_symbols);
    fprintf(fid,"num_sync_samples = %u;\n", num_sync_symbols*k);
    fprintf(fid,"s = zeros(1,k*N);\n");
    //for (i=0; i<N; i++)
    //    fprintf(fid,"s(%4u:%4u) = %3d;\n", k*i+1, k*(i+1), 2*(int)(msequence_advance(ms))-1);

    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,abs(rxy));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('correlator output');\n");
    fprintf(fid,"grid on;\n");

    // save...
    fprintf(fid,"[v i] = max(abs(rxy));\n");
    fprintf(fid,"i0=i-(k*N)+1;\n");
    fprintf(fid,"i1=i;\n");
    fprintf(fid,"z_hat = z(i0:i1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t_hat=0:(k*N-1);\n");
    fprintf(fid,"plot(t_hat,real(z_hat),...\n");
    fprintf(fid,"     t_hat,imag(z_hat),...\n");
    fprintf(fid,"     t_hat(1:k:end),real(z_hat(1:k:end)),'x','MarkerSize',2,...\n");
    fprintf(fid,"     t_hat,s,'-k');\n");

    // run fft for timing recovery
    fprintf(fid,"Z_hat = fft(z_hat);\n");
    fprintf(fid,"S     = fft(s);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(fftshift(arg(Z_hat./S)));\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
