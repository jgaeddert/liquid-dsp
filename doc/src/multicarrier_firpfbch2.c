//
// multicarrier_firpfbch2.c
//
// Example of the finite impulse response (FIR) polyphase filterbank
// (PFB) channelizer with an output rate of 2 Fs / M as an (almost)
// perfect reconstructive system.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME_TIME "firpfbch2_time.dat"
#define OUTPUT_FILENAME_FREQ "firpfbch2_freq.dat"

// print usage/help message
void usage()
{
    printf("%s [options]\n", __FILE__);
    printf("  h     : print help\n");
    printf("  M     : number of channels, default: 16\n");
    printf("  m     : prototype filter semi-length, default: 4\n");
    printf("  s     : prototype filter stop-band attenuation, default: 80\n");
    printf("  n     : number of 'symbols' to analyze, default: 20\n");
}

int main(int argc, char*argv[])
{
    // options
    unsigned int num_channels=16;   // number of channels
    unsigned int m = 5;             // filter semi-length (symbols)
    unsigned int num_symbols=25;    // number of symbols
    float As = 80.0f;               // filter stop-band attenuation
    
    int dopt;
    while ((dopt = getopt(argc,argv,"hM:m:s:n:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                     return 0;
        case 'M':   num_channels = atoi(optarg); break;
        case 'm':   m            = atoi(optarg); break;
        case 's':   As           = atof(optarg); break;
        case 'n':   num_symbols  = atof(optarg); break;
        default:
            exit(1);
        }
    }

    unsigned int i;

    // validate input
    if (num_channels < 2 || num_channels % 2) {
        fprintf(stderr,"error: %s, number of channels must be greater than 2 and even\n", argv[0]);
        exit(1);
    } else if (m == 0) {
        fprintf(stderr,"error: %s, filter semi-length must be greater than zero\n", argv[0]);
        exit(1);
    } else if (num_symbols == 0) {
        fprintf(stderr,"error: %s, number of symbols must be greater than zero", argv[0]);
        exit(1);
    }

    // derived values
    unsigned int num_samples = num_channels * num_symbols;

    // allocate arrays
    float complex x[num_samples];
    float complex y[num_samples];

    // generate input signal
    unsigned int w_len = (unsigned int)(0.4*num_samples);
    for (i=0; i<num_samples; i++) {
        //x[i] = (i==0) ? 1.0f : 0.0f;
        //x[i] = cexpf( (-0.05f + 0.07f*_Complex_I)*i );  // decaying complex exponential
        x[i] = cexpf( _Complex_I * (1.3f*i - 0.007f*i*i) );
        x[i] *= i < w_len ? hamming(i,w_len) : 0.0f;
        //x[i] = (i==0) ? 1.0f : 0.0f;
    }

    // create filterbank objects from prototype
    firpfbch2_crcf qa = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER,    num_channels, m, As);
    firpfbch2_crcf qs = firpfbch2_crcf_create_kaiser(LIQUID_SYNTHESIZER, num_channels, m, As);
    firpfbch2_crcf_print(qa);
    firpfbch2_crcf_print(qs);

    // run channelizer
    float complex Y[num_channels];
    for (i=0; i<num_samples; i+=num_channels/2) {
        // run analysis filterbank
        firpfbch2_crcf_execute(qa, &x[i], Y);

        // run synthesis filterbank
        firpfbch2_crcf_execute(qs, Y, &y[i]);
    }

    // destroy fiterbank objects
    firpfbch2_crcf_destroy(qa); // analysis fitlerbank
    firpfbch2_crcf_destroy(qs); // synthesis filterbank

    // print output
    for (i=0; i<num_samples; i++)
        printf("%4u : %12.8f + %12.8fj\n", i, crealf(y[i]), cimagf(y[i]));

    // compute RMSE
    float rmse = 0.0f;
    unsigned int delay = 2*num_channels*m - num_channels/2 + 1;
    for (i=0; i<num_samples; i++) {
        float complex err = y[i] - (i < delay ? 0.0f : x[i-delay]);
        rmse += crealf( err*conjf(err) );
    }
    rmse = sqrtf( rmse/(float)num_samples );
    printf("rmse : %12.4e\n", rmse);

    //
    // EXPORT DATA TO FILES
    //
    FILE * fid = NULL;
    fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"#\n");
    fprintf(fid,"# %8s %12s %12s %12s %12s %12s %12s\n",
            "time", "real(x)", "imag(x)", "real(y)", "imag(y)", "real(e)", "imag(e)");

    // save input and output arrays
    for (i=0; i<num_samples; i++) {
        float complex e = (i < delay) ? 0.0f : y[i] - x[i-delay];
        fprintf(fid,"  %8.1f %12.4e %12.4e %12.4e %12.4e %12.4e %12.4e\n",
                (float)i,
                crealf(x[i]), cimagf(x[i]),
                crealf(y[i]), cimagf(y[i]),
                crealf(e),    cimagf(e));
    }
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME_TIME);

    // 
    // export frequency data
    //
    unsigned int nfft = 2048;
    float complex y_time[nfft];
    float complex y_freq[nfft];
    for (i=0; i<nfft; i++)
        y_time[i] = i < num_samples ? y[i] : 0.0f;
    fft_run(nfft, y_time, y_freq, LIQUID_FFT_FORWARD, 0);

    // filter spectrum
    unsigned int h_len = 2*num_channels*m+1;
    float h[h_len];
    float fc = 0.5f/(float)num_channels;
    liquid_firdes_kaiser(h_len, fc, As, 0.0f, h);
    float complex h_time[nfft];
    float complex h_freq[nfft];
    for (i=0; i<nfft; i++)
        h_time[i] = i < h_len ? 2*h[i]*fc : 0.0f;
    fft_run(nfft, h_time, h_freq, LIQUID_FFT_FORWARD, 0);

    // error spectrum
    float complex e_time[nfft];
    float complex e_freq[nfft];
    for (i=0; i<nfft; i++)
        e_time[i] = i < delay || i > num_samples ? 0.0f : y[i] - x[i-delay];
    fft_run(nfft, e_time, e_freq, LIQUID_FFT_FORWARD, 0);

    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s: auto-generated file\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"#\n");
    fprintf(fid,"# nfft = %u\n", nfft);
    fprintf(fid,"# %12s %12s %12s %12s\n", "freq", "PSD [dB]", "filter [dB]", "error [dB]");

    // save input and output arrays
    for (i=0; i<nfft; i++) {
        float f = (float)i/(float)nfft - 0.5f;
        unsigned int k = (i + nfft/2)%nfft;
        fprintf(fid,"  %12.8f %12.8f %12.8f %12.8f\n",
                f,
                20*log10f(cabsf(y_freq[k])),
                20*log10f(cabsf(h_freq[k])),
                20*log10f(cabsf(e_freq[k])));
    }
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME_FREQ);

    printf("done.\n");
    return 0;
}
