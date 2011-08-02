//
// estimate_snr.c
//

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <string.h>
#include <math.h>

#include "liquid.doc.h"


// find approximate SNR to achieve a particular PER
//  _opts       :   simulation options
//  _ber_per    :   estimate using BER or PER?
//  _snr_ebn0   :   search for SNR or Eb/N0?
//  _error_rate :   target error rate
float estimate_snr(simulate_per_opts _opts,
                   int _ber_per,
                   int _snr_ebn0,
                   float _error_rate)
{
    // number of bisections
    // NOTE : this number shouldn't be too high, lest limited
    //        accuracy corrupt the linear curve fit
    unsigned int num_iterations = 8;

    // reduce number of iterations for v615 codec
    if (_opts.fec0 == LIQUID_FEC_CONV_V615 || _opts.fec1 == LIQUID_FEC_CONV_V615) {
        _opts.min_packet_errors >>= 2;
        _opts.min_bit_errors    >>= 2;
        _opts.min_packet_trials >>= 3;
        _opts.min_bit_trials    >>= 3;
        _opts.max_packet_trials >>= 4;
        _opts.max_bit_trials    >>= 4;
    }

    // compute asymptotic rate
    modem q = modem_create(_opts.ms, _opts.bps);
    float rate = modem_get_bps(q) * fec_get_rate(_opts.fec0) * fec_get_rate(_opts.fec1);
    modem_destroy(q);

#if 0
    // print options
    printf("            %12s %12s %12s\n", "min-errors", "min-trials", "max-trials");
    printf("  bits  :   %12lu %12lu %12lu\n", _opts.min_bit_errors, _opts.min_bit_trials, _opts.max_bit_trials);
    printf("  packet:   %12lu %12lu %12lu\n", _opts.min_packet_errors, _opts.min_packet_trials, _opts.max_packet_trials);
#endif

    // start at a really high SNR|Eb/N0 range
    float x0 = -10.0f;  // lower SNR|Eb/N0 bound
    float x1 =  50.0f;  // upper SNR|Eb/N0 bound
    float x;            // current SNR|Eb/N0 evaluation

    float BER_0, BER_1; // bit error rate bound
    float PER_0, PER_1; // packet error rate bound
    float BER, PER;     // current error rate evaluation
    float e;            // BER|PER selection

    simulate_per_results results;

    // simulate lower bound
    float SNRdB = (_snr_ebn0==ESTIMATE_SNR) ? x0 : x0 + 10.0f*log10f(rate);
    simulate_per(_opts, SNRdB, &results);
    BER_0 = results.BER;
    PER_0 = results.PER;

#if 0
    // simulate upper bound
    simulate_per(_opts, SNRdB_1, &results);
    BER_1 = results.BER;
    PER_1 = results.PER;
#else
    BER_1 = 0.0;
    PER_1 = 0.0;
#endif

    // TODO : check results
    // ensure PER_0 > _per > PER_1

    unsigned int i;
    for (i=0; i<num_iterations; i++) {
#if 1
        if (_ber_per == ESTIMATE_SNR_BER) {
            printf("  %s : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
                    _snr_ebn0 == ESTIMATE_SNR ? "SNR" : "Eb/N0",
                    x0, x1, BER_0, BER_1);
        } else {
            printf("  %s : [%12.4f %12.4f], error rate : [%12.8f %12.8f]\n",
                    _snr_ebn0 == ESTIMATE_SNR ? "SNR" : "Eb/N0",
                    x0, x1, PER_0, PER_1);
        }
#endif

        // bisect SNR limits
        x = 0.5f*(x0 + x1);
        SNRdB = (_snr_ebn0==ESTIMATE_SNR) ? x : x + 10.0f*log10f(rate);

        // simulate PER, BER
        simulate_per(_opts, SNRdB, &results);
        BER = results.BER;
        PER = results.PER;

        if (_ber_per == ESTIMATE_SNR_BER)
            e = BER;
        else
            e = PER;

        if (e > _error_rate) {
            // use upper range
            x0      = x;
            BER_0   = BER;
            PER_0   = PER;
        } else {
            // use lower range
            x1      = x;
            BER_1   = BER;
            PER_1   = PER;
        }

    }

    // apply linear fit to log of error rate
    //  (y-y0) = m*(x-x0)
    //  m = (y0-y1)/(x0-x1)
    //  x = (y-y0)/m + x0
    //
    //  x : SNR|Eb/N0
    //  y : BER|PER
    float x_hat;
    float y0=0, y1=0;
    float eps = 1e-12f; // small, insignificant number to ensure no log(0)

    if (_ber_per == ESTIMATE_SNR_BER) {
        y0 = logf(BER_0 + eps);
        y1 = logf(BER_1 + eps);
    } else {
        y0 = logf(PER_0 + eps);
        y1 = logf(PER_1 + eps);
    }
    float m = (y0 - y1) / (x0 - x1);
    x_hat = (logf(_error_rate) - y0)/m + x0;
#if 0
    printf("  SNR : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
            SNRdB_0, SNRdB_1, y0, y1);
#endif

    return x_hat;
}

// solve for SNR (Eb/N0) for BPSK for a given BER
float estimate_snr_bpsk(float _error_rate)
{
    // validate input
    if (_error_rate <= 0.0f) {
        fprintf(stderr,"error: estimate_snr_bpsk(), error rate must be greater than 0\n");
        exit(1);
    } else if (_error_rate >= 0.5f) {
        fprintf(stderr,"error: estimate_snr_bpsk(), error rate must be less than 0.5\n");
        exit(1);
    }

    // lower bound
    float EbN0dB_0 = -20;
    float BER_0 = 0.5f*erfcf(powf(10.0f,EbN0dB_0/20.0f));

    // upper bound
    float EbN0dB_1 =  20;
    float BER_1 = 0.5f*erfcf(powf(10.0f,EbN0dB_1/20.0f));

    // estimate
    float EbN0dB;
    float BER;

    unsigned int i;
    for (i=0; i<10; i++) {
#if 0
        printf("  Eb/N0 : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
                EbN0dB_0,
                EbN0dB_1,
                BER_0,
                BER_1);
#endif
        // split the difference and compute error rate
        EbN0dB = 0.5f*(EbN0dB_0 + EbN0dB_1);
        BER = 0.5f*erfcf(powf(10.0f,EbN0dB/20.0f));

        if (BER > _error_rate) {
            EbN0dB_0 = EbN0dB;
            BER_0    = BER;
        } else {
            EbN0dB_1 = EbN0dB;
            BER_1    = BER;
        }
    }

    // apply linear fit to log of error rate
    //  (y-y0) = m*(x-x0)
    //  m = (y0-y1)/(x0-x1)
    //  x = (y-y0)/m + x0
    //
    //  x : SNR
    //  y : BER
    float EbN0dB_hat;
    float x0 = EbN0dB_0;
    float x1 = EbN0dB_1;
    float y0=0, y1=0;
    float eps = 1e-16f; // small, insignificant number to ensure no log(0)

    y0 = logf(BER_0 + eps);
    y1 = logf(BER_1 + eps);
    float m = (y0 - y1) / (x0 - x1);
    EbN0dB_hat = (logf(_error_rate) - y0)/m + x0;
#if 0
    printf("  Eb/N0 : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
            EbN0dB_0, EbN0dB_1, expf(y0), expf(y1));
#endif

    return EbN0dB_hat;
}
