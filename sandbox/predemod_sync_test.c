// 
// predemod_sync_test.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <assert.h>
#include "liquid.h"

#define OUTPUT_FILENAME "predemod_sync_test.m"

// print usage/help message
void usage()
{
    printf("predemod_sync_test -- test pre-demodulation synchronization\n");
    printf("options:\n");
    printf("  h     : print usage/help\n");
    printf("  k     : samples/symbol, default: 2\n");
    printf("  m     : filter delay [symbols], default: 4\n");
    printf("  n     : number of data symbols, default: 64\n");
    printf("  b     : bandwidth-time product, (0,1), default: 0.3\n");
    printf("  t     : fractional sample offset, (-0.5,0.5), default: 0\n");
    printf("  s     : SNR [dB], default: 30\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int k=2;                   // filter samples/symbol
    unsigned int m=4;                   // filter delay (symbols)
    float beta=0.3f;                    // bandwidth-time product
    float dt = 0.0f;                    // fractional sample timing offset
    unsigned int num_sync_symbols = 64; // number of data symbols
    unsigned int npfb=16;               // number of filters in bank
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]
    unsigned int g = 0x0067;            // m-sequence generator polynomial
    float dphi = 0.0f;                  // carrier frequency offset
    float phi  = 0.0f;                  // carrier phase offset
    
    unsigned int num_delay_symbols = 12;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:n:b:t:s:")) != EOF) {
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'k': k     = atoi(optarg); break;
        case 'm': m     = atoi(optarg); break;
        case 'n': num_sync_symbols = atoi(optarg); break;
        case 'b': beta  = atof(optarg); break;
        case 's': SNRdB = atof(optarg); break;
        case 't': dt    = atof(optarg); break;
        default:
            exit(1);
        }
    }

    unsigned int i;

    // validate input
    if (beta <= 0.0f || beta >= 1.0f) {
        fprintf(stderr,"error: %s, bandwidth-time product must be in (0,1)\n", argv[0]);
        exit(1);
    } else if (dt < -0.5 || dt > 0.5) {
        fprintf(stderr,"error: %s, fractional sample offset must be in (0,1)\n", argv[0]);
        exit(1);
    }

    // derived values
    unsigned int num_symbols = num_delay_symbols + num_sync_symbols + 2*m;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    float complex seq[num_sync_symbols];    // data sequence (symbols)
    float complex s0[k*num_sync_symbols];   // data sequence (interpolated samples)
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    float complex z[num_samples];           // matched filter output
    float complex rxy[num_samples];         // pre-demod output

    // generate sequence
    for (i=0; i<num_sync_symbols; i++) {
        float sym_i = rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2;
        float sym_q = rand() % 2 ? M_SQRT1_2 : -M_SQRT1_2;
        seq[i] = sym_i + _Complex_I*sym_q;
    }

    // create interpolated sequence, compensating for filter delay
    interp_crcf interp_seq = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,k,m,beta,0.0f);
    for (i=0; i<num_sync_symbols+m; i++) {
        if      (i < m)                interp_crcf_execute(interp_seq, seq[i], &s0[0]);
        else if (i < num_sync_symbols) interp_crcf_execute(interp_seq, seq[i], &s0[k*(i-m)]);
        else                           interp_crcf_execute(interp_seq,      0, &s0[k*(i-m)]);
    }
    interp_crcf_destroy(interp_seq);

    // create transmit interpolator and generate sequence
    interp_crcf interp_tx = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,k,m,beta,dt);
    unsigned int n=0;
    for (i=0; i<num_delay_symbols; i++) {
        interp_crcf_execute(interp_tx, 0, &x[k*n]);
        n++;
    }
    for (i=0; i<num_sync_symbols; i++) {
        interp_crcf_execute(interp_tx, seq[i], &x[k*n]);
        n++;
    }
    for (i=0; i<2*m; i++) {
        interp_crcf_execute(interp_tx, 0, &x[k*n]);
        n++;
    }
    assert(n==num_symbols);
    interp_crcf_destroy(interp_tx);

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i] = x[i] + nstd*( randnf() + _Complex_I*randnf() );
    }

    // create matched filter and detect signal
    //firfilt_cccf fsync = firfilt_crcf_create(s0, k*num_sync_symbols);

    // destroy objects
    //firfilt_cccf_destroy(fsync);
    
    // print results
    //printf("rxy (max) : %12.8f\n", rxy_max);

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
    fprintf(fid,"num_sync_symbols = %u;\n", num_sync_symbols);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    // save sequence symbols
    fprintf(fid,"seq = zeros(1,num_sync_symbols);\n");
    for (i=0; i<num_sync_symbols; i++)
        fprintf(fid,"seq(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(seq[i]), cimagf(seq[i]));

    // save interpolated sequence
    fprintf(fid,"s   = zeros(1,k*num_sync_symbols);\n");
    for (i=0; i<k*num_sync_symbols; i++)
        fprintf(fid,"s(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(s0[i]), cimagf(s0[i]));

    fprintf(fid,"x   = zeros(1,num_samples);\n");
    fprintf(fid,"y   = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]),   cimagf(x[i]));
        fprintf(fid,"y(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]),   cimagf(y[i]));
        //fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }
    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
#if 0
    fprintf(fid,"z   = zeros(1,num_samples);\n");
    fprintf(fid,"rxy = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"z(%4u)     = %12.8f + j*%12.8f;\n", i+1, crealf(z[i]),   cimagf(z[i]));
        fprintf(fid,"rxy(%4u)   = %12.8f + j*%12.8f;\n", i+1, crealf(rxy[i]), cimagf(rxy[i]));
    }
    // save m-sequence
    ms = msequence_create(M,g,A);
    fprintf(fid,"N = %u;\n", N);
    fprintf(fid,"s = zeros(1,k*N);\n");
    for (i=0; i<N; i++)
        fprintf(fid,"s(%4u:%4u) = %3d;\n", k*i+1, k*(i+1), 2*(int)(msequence_advance(ms))-1);
    msequence_destroy(ms);

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
#endif
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(s),real(s), 1:length(s),imag(s));\n");
    
    fprintf(fid,"z = abs( filter(fliplr(conj(s)),1,y) );\n");
    fprintf(fid,"[zmax i] = max(z);\n");
    fprintf(fid,"plot(1:length(z),z,'-x');\n");
    fprintf(fid,"axis([(i-4*k) (i+4*k) 0 zmax*1.2]);\n");
    fprintf(fid,"grid on\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
