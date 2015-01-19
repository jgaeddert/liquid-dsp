// 
// fskmodem_example.c
//
// This example demostrates the continuous phase frequency-shift keying
// (CP-FSK) modem in liquid. A message signal is modulated and the
// resulting signal is recovered using a demodulator object.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fskmodem_example.m"

// print usage/help message
void usage()
{
    printf("fskmodem_example -- continuous-phase frequency-shift keying example\n");
    printf("options:\n");
    printf("  h     : print help\n");
    printf("  m     : bits/symbol,              default:  1\n");
    printf("  k     : samples/symbol,           default:  8\n");
    printf("  f     : frequency spacing         default:  0.1\n");
    printf("  n     : number of data symbols,   default: 80\n");
    printf("  s     : SNR [dB],                 default: 40\n");
    printf("  F     : carrier frequency offset, default:  0\n");
    printf("  P     : carrier phase offset,     default:  0\n");
    printf("  T     : fractional symbol offset, default:  0\n");
}

int main(int argc, char*argv[])
{
    // options
    unsigned int bps         =   3;     // number of bits/symbol
    unsigned int k           =  16;     // filter samples/symbol
    unsigned int num_symbols = 200;     // number of data symbols
    float        SNRdB       = 40.0f;   // signal-to-noise ratio [dB]
    float        cfo         = 0.0f;    // carrier frequency offset
    float        cpo         = 0.0f;    // carrier phase offset
    float        tau         = 0.0f;    // fractional symbol timing offset
    float        bandwidth   = 0.20;    // frequency spacing

    int dopt;
    while ((dopt = getopt(argc,argv,"hm:k:f:n:s:F:P:T:")) != EOF) {
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'm': bps         = atoi(optarg);   break;
        case 'k': k           = atoi(optarg);   break;
        case 'f': bandwidth   = atof(optarg);   break;
        case 'n': num_symbols = atoi(optarg);   break;
        case 's': SNRdB       = atof(optarg);   break;
        case 'F': cfo         = atof(optarg);   break;
        case 'P': cpo         = atof(optarg);   break;
        case 'T': tau         = atof(optarg);   break;
        default:
            exit(1);
        }
    }

    unsigned int i;
    unsigned int j;

    k = 2*(1<<bps);

    // derived values
    unsigned int num_samples = k*num_symbols;
    unsigned int M           = 1 << bps;              // constellation size
    float        nstd        = powf(10.0f, -SNRdB/20.0f);
    float        M2          = 0.5f*(float)(M-1);

    // compute appropriate demodulation FFT size
    unsigned int K = 2*k;

    // arrays
    unsigned int  sym_in[num_symbols];      // input symbols
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    unsigned int  sym_out[num_symbols];     // output symbols

    // print frequency bins
    for (i=0; i<M; i++)
        printf("  s=%3u, f = %12.8f\n", i, ((float)i - M2) * bandwidth / M2);


    // generate message symbols and modulate
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym_in[i] = (i < M) ? i : rand() % M;

        // compute frequency
        float dphi = 2*M_PI*((float)sym_in[i] - M2) * bandwidth / M2;

        // generate random phase
        float phi  = randf() * 2 * M_PI;
        
        // modulate symbol
        for (j=0; j<k; j++)
            x[i*k+j] = cexpf(_Complex_I*phi + _Complex_I*j*dphi);
    }

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

#if 0
    // demodulate signal: least-squares method
    float complex buf_time[K];
    float complex buf_freq[K];
    fftplan fft = fft_create_plan(K, buf_time, buf_freq, LIQUID_FFT_FORWARD, 0);

    for (i=0; i<K; i++)
        buf_time[i] = 0.0f;
    unsigned int n = 0;
    j = 0;
    for (i=0; i<num_samples; i++) {
        // start filling time buffer with samples (assume perfect symbol timing)
        buf_time[n++] = y[i];

        // demodulate symbol
        if (n==k) {
            // reset counter
            n = 0;

            // compute transform, storing result in 'buf_freq'
            fft_execute(fft);

            // print results
            if (j==0) {
                unsigned int s;
                for (s=0; s<K; s++)
                    printf("  Y(%3u) = |%12.8f| <%12.8f>\n", s, cabsf(buf_freq[s]), cargf(buf_freq[s]));
            }
            sym_out[j++] = 0;
        }
    }
    // 
    fft_destroy_plan(fft);
#else
    // demodulate signal: high SNR method
    float complex buf_time[k];
    unsigned int n = 0;
    j = 0;
    for (i=0; i<num_samples; i++) {
        // start filling time buffer with samples (assume perfect symbol timing)
        buf_time[n++] = y[i];

        // demodulate symbol
        if (n==k) {
            // reset counter
            n = 0;

            // estimate frequency
            float complex metric = 0;
            unsigned int s;
            for (s=1; s<k; s++)
                metric += buf_time[s] * conjf(buf_time[s-1]);
            float dphi_hat = cargf(metric) / (2*M_PI);
            unsigned int v=( (unsigned int) roundf(dphi_hat*M2/bandwidth + M2) ) % M;
            sym_out[j++] = v;
            printf("%3u : %12.8f : %u\n", j, dphi_hat, v);
        }
    }
#endif

    // count errors
    unsigned int num_symbol_errors = 0;
    for (i=0; i<num_symbols; i++)
        num_symbol_errors += (sym_in[i] == sym_out[i]) ? 0 : 1;

    printf("symbol errors: %u / %u\n", num_symbol_errors, num_symbols);

    // compute power spectral density of transmitted signal
    unsigned int nfft = 2048;
    float psd[nfft];
    spgramcf periodogram = spgramcf_create_kaiser(nfft, nfft/2, 8.0f);
    spgramcf_estimate_psd(periodogram, x, num_samples, psd);
    spgramcf_destroy(periodogram);

    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"nfft        = %u;\n", nfft);

    fprintf(fid,"x   = zeros(1,num_samples);\n");
    fprintf(fid,"y   = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }
    // save power spectral density
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%4u) = %12.8f;\n", i+1, psd[i]);

    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"i = 1:k:num_samples;\n");
    fprintf(fid,"figure;\n");

    // plot time signal
    fprintf(fid,"subplot(2,1,1),\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"  plot(t,real(y),'-', 'Color',[0 0.3 0.5]);\n");
    fprintf(fid,"  plot(t,imag(y),'-', 'Color',[0 0.5 0.3]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis([0 10 -1.2 1.2]);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('x(t)');\n");
    fprintf(fid,"grid on;\n");

    // plot PSD
    fprintf(fid,"subplot(2,1,2),\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"plot(f,psd,'LineWidth',1.5,'Color',[0.5 0 0]);\n");
    fprintf(fid,"axis([-0.5 0.5 -40 20]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
