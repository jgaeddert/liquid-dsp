//
// simulate detector performance
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

// default output filename
#define OUTPUT_FILENAME "detector_results.dat"

// print usage/help message
void usage()
{
    printf("simulate_ber options:\n");
    printf("  h     : print usage/help\n");
    printf("  o     : output filename, default: %s\n", OUTPUT_FILENAME);
    printf("  s     : SNR start [dB], -5\n");
    printf("  d     : SNR step [dB], 0.2\n");
    printf("  x     : SNR max [dB], 25\n");
    printf("  F     : dphi max [rad/sample], 0.02\n");
    printf("  t     : number of trials, default: 16000\n");
    printf("  n     : frame length [symbols], default: 255\n");
    printf("  k     : samples/symbol, default: 2\n");
    printf("  T     : detection threshold, default: 0.3\n");
}

// test detector
//  _num_symbols    :   length of sequence in symbols
//  _k              :   samples/symbol (over-sampling rate)
//  _threshold      :   detection threshold
//  _SNRdB          :   signal-to-noise ratio [dB]
//  _dphi_max       :   maximum carrier offset
//  _num_trials     :   number of trials to run
//  _num_detected   :   number of frames detected
//  _dphi_hat_rmse  :   RMSE of carrier offset estimate
//  _tau_hat_rmse   :   RMSE of delay estimate
//  _gamma_hat_rmse :   RMSE of ...
void detector_test(unsigned int   _num_symbols,
                   unsigned int   _k,
                   float          _threshold,
                   float          _SNRdB,
                   float          _dphi_max,
                   unsigned int   _num_trials,
                   unsigned int * _num_detected,
                   float *        _dphi_hat_rmse,
                   float *        _tau_hat_rmse,
                   float *        _gamma_hat_rmse);

int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB_min          = -10.0f;  // starting SNR
    float SNRdB_step         =   0.5f;  // SNR step size
    float SNRdB_max          =  10.0f;  // maximum SNR
    float dphi_max           =   0.02f; // maximum carrier offset
    unsigned int num_trials  = 16000;   // number of trials
    unsigned int num_symbols = 255;     // sequence length (symbols)
    unsigned int k           = 2;       // samples/symbol
    float threshold          = 0.3f;    // detection threshold
    const char * filename    = OUTPUT_FILENAME;

    // get command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"ho:s:d:x:F:t:n:k:T:")) != EOF){
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'o': filename      = optarg;       break;
        case 's': SNRdB_min     = atof(optarg); break;
        case 'd': SNRdB_step    = atof(optarg); break;
        case 'x': SNRdB_max     = atof(optarg); break;
        case 'F': dphi_max      = atof(optarg); break;
        case 't': num_trials    = atoi(optarg); break;
        case 'n': num_symbols   = atoi(optarg); break;
        case 'k': k             = atoi(optarg); break;
        case 'T': threshold     = atof(optarg); break;
        default:
            exit(1);
        }
    }

    // validate options
    if (SNRdB_step <= 0.0f) {
        printf("error: SNRdB_step must be greater than zero\n");
        exit(-1);
    } else if (SNRdB_max < SNRdB_min) {
        printf("error: SNRdB_max must be greater than SNRdB_min\n");
        exit(-1);
    }

    unsigned int i;

    // open output file
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: could not open '%s' for writing\n", filename);
        exit(1);
    }
    fprintf(fid,"# %s : auto-generated file\n", filename);
    fprintf(fid,"# invoked as: ");
    for (i=0; i<argc; i++) fprintf(fid,"%s ", argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"#\n");
    fprintf(fid,"#  num trials          :   %u\n", num_trials);
    fprintf(fid,"#  num symbols         :   %u\n", num_symbols);
    fprintf(fid,"#  k (samples/symbol)  :   %u\n", k);
    fprintf(fid,"#  max carrier offset  :   %12.8f radians/sample\n", dphi_max);
    fprintf(fid,"#\n");
    fprintf(fid,"# %12s %12s %12s %12s %12s %12s %12s\n",
            "SNR [dB]",
            "rmse{dphi}",
            "rmse{tau}",
            "rmse{gamma}",
            "detect. prob.",
            "num detected",
            "num trials");
    fclose(fid);

    // run simulation for increasing SNR levels
    float SNRdB = SNRdB_min;
    while (SNRdB < SNRdB_max) {
        // set up values
        float dphi_hat_rmse       = 0.0f;
        float tau_hat_rmse        = 0.0f;
        float gamma_hat_rmse      = 0.0f;
        unsigned int num_detected = 0;

        // run simulation
        detector_test(num_symbols, k, threshold, SNRdB, dphi_max, num_trials,
                      &num_detected,
                      &dphi_hat_rmse,
                      &tau_hat_rmse,
                      &gamma_hat_rmse);

        // compute detection probability
        float detection_probability = (float)num_detected / (float)num_trials;

        // append results to file
        fid = fopen(filename,"a");
        fprintf(fid,"  %12.8f %12.4e %12.4e %12.4e %12.10f %12u %12u\n",
                SNRdB,
                dphi_hat_rmse,
                tau_hat_rmse,
                gamma_hat_rmse,
                detection_probability,
                num_detected,
                num_trials);
        fclose(fid);

        // increase SNR and continue
        SNRdB += SNRdB_step;
    }

    printf("results written to '%s'\n", filename);

    return 0;
}

// test detector
//  _num_symbols    :   length of sequence in symbols
//  _k              :   samples/symbol (over-sampling rate)
//  _threshold      :   detection threshold
//  _SNRdB          :   signal-to-noise ratio [dB]
//  _dphi_max       :   maximum carrier offset
//  _num_trials     :   number of trials to run
//  _num_detected   :   number of frames detected
//  _dphi_hat_rmse  :   RMSE of carrier offset estimate
//  _tau_hat_rmse   :   RMSE of delay estimate
//  _gamma_hat_rmse :   RMSE of ...
void detector_test(unsigned int   _num_symbols,
                   unsigned int   _k,
                   float          _threshold,
                   float          _SNRdB,
                   float          _dphi_max,
                   unsigned int   _num_trials,
                   unsigned int * _num_detected,
                   float *        _dphi_hat_rmse,
                   float *        _tau_hat_rmse,
                   float *        _gamma_hat_rmse)
{
    // TODO: validate input

    // strip input values
    unsigned int num_symbols = _num_symbols;
    unsigned int k           = _k;
    float threshold          = _threshold;
    float SNRdB              = _SNRdB;
    float dphi_max           = _dphi_max;
    unsigned int num_trials  = _num_trials;

    unsigned int i;

    // fixed values
    float noise_floor = -80.0f;     // noise floor [dB]
    unsigned int m    =  11;        // resampling filter semi-length

    // derived values
    unsigned int num_samples = k*num_symbols + 2*m + 1;
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // arrays
    float complex s[k*num_symbols]; // synchronization pattern (samples)

    // generate synchronization pattern
    unsigned int mm = liquid_nextpow2(num_symbols);  // mm = ceil( log2(num_symbols) )
    msequence ms = msequence_create_default(mm);
    float complex v = 0.0f;
    for (i=0; i<k*num_symbols; i++) {
        if ( (i%k)==0 )
            v = msequence_advance(ms) ? 1.0f : -1.0f;
        s[i] = v;
    }
    msequence_destroy(ms);
        
    // create detector
    detector_cccf sync = detector_cccf_create(s, k*num_symbols, threshold, 2.0f*dphi_max);

    // create fractional sample interpolator
    unsigned int h_len = 2*m+1;
    unsigned int order = 4;
    float        fc    = 0.45f;
    float        As    = 60.0f;
    firfarrow_crcf fdelay = firfarrow_crcf_create(h_len, order, fc, As);

    // initialize...
    unsigned int num_detected = 0;
    float dphi_hat_rmse       = 0.0f;
    float tau_hat_rmse        = 0.0f;
    float gamma_hat_rmse      = 0.0f;

    // run trials
    unsigned int t;
    for (t=0; t<num_trials; t++) {
        // reset objects
        firfarrow_crcf_clear(fdelay);
        detector_cccf_reset(sync);

        // set properties
        float dt    = randf() - 0.5f;                   // fractional sample
        float delay = (float)(k*num_symbols + m) + dt;  // expected delay
        float dphi  = (2.0f*randf() - 1.0f)*dphi_max;   // carrier frequency offset
        float phi   = 2*M_PI*randf();                   // carrier phase offset

        // set fractional delay
        firfarrow_crcf_set_delay(fdelay, dt);

        // generate sequence and push through detector
        float tau_hat       = 0.5f;     // fractional sample offset estimate
        float dphi_hat      = 0.0f;     // carrier offset estimate
        float gamma_hat     = 1.0f;     // signal level estimate (linear)
        float delay_hat     = 0.0f;     // total delay offset estimate
        int signal_detected = 0;        // signal detected flag
        float complex x     = 0.0f;     // sample

        for (i=0; i<num_samples; i++) {
            // add fractional sample timing offset
            if (i < k*num_symbols) firfarrow_crcf_push(fdelay, s[i]);
            else                   firfarrow_crcf_push(fdelay, 0.0f);

            // compute output
            firfarrow_crcf_execute(fdelay, &x);

            // add channel gain
            x *= gamma;

            // add carrier offset
            x *= cexpf( _Complex_I*(dphi*i + phi) );

            // add noise
            x += nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

            // correlate
            int detected = detector_cccf_correlate(sync, x, &tau_hat, &dphi_hat, &gamma_hat);

            if (detected) {
                signal_detected = 1;
                delay_hat = (float)i + (float)tau_hat;
#if 0
                printf("****** preamble found, tau_hat=%8.5f, dphi_hat=%8.5f, gamma_hat=%8.6f\n",
                        tau_hat, dphi_hat, gamma_hat);
#endif
            }
        }

#if 0
        // convert to dB
        gamma     = 20*log10f(gamma);
        gamma_hat = 20*log10f(gamma_hat);
#endif

        // update...
        num_detected   += signal_detected ? 1 : 0;
        if (signal_detected) {
            dphi_hat_rmse  += powf( (dphi_hat  - dphi ), 2.0f );
            tau_hat_rmse   += powf( (delay_hat - delay), 2.0f );
            gamma_hat_rmse += 0.0f;
        }

#if 0
        if (!signal_detected) {
            printf("exiting prematurely\n");
            detector_cccf_destroy(sync);
            exit(1);
        }
#endif

        // peridically print results
        if ( ((t+1) % 10)==0 || (t==num_trials-1) ) {
            printf(" SNR: %6.2f[%6u/%6u] detected:%6u(%5.1f%%) dphi:%8.6f delay:%8.4f gamma:%8.4f\r",
                    SNRdB,
                    t+1, num_trials,
                    num_detected, 100.0f*(float)num_detected / (float)t,
                    num_detected == 0 ? 0 : sqrtf( dphi_hat_rmse  / (float)num_detected ),
                    num_detected == 0 ? 0 : sqrtf( tau_hat_rmse   / (float)num_detected ),
                    num_detected == 0 ? 0 : sqrtf( gamma_hat_rmse / (float)num_detected ));
            fflush(stdout);
        }
    }
    // print new line (refesh buffer)
    printf("\n");
        
    // destroy objects
    firfarrow_crcf_destroy(fdelay);
    detector_cccf_destroy(sync);

    // set return values
    *_num_detected   = num_detected;
    *_dphi_hat_rmse  = dphi_hat_rmse;
    *_tau_hat_rmse   = tau_hat_rmse;
    *_gamma_hat_rmse = gamma_hat_rmse;
}


