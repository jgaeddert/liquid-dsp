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
//  _error_rate :   target error rate
float estimate_snr(simulate_per_opts _opts,
                   int _ber_per,
                   float _error_rate)
{
    // number of bisections
    // NOTE : this number shouldn't be too high, lest limited
    //        accuracy corrupt the linear curve fit
    unsigned int num_iterations = (_ber_per == ESTIMATE_SNR_BER) ? 6 : 8;

    // reduce number of iterations for v615 codec
    if (_opts.fec0 == LIQUID_FEC_CONV_V615 || _opts.fec1 == LIQUID_FEC_CONV_V615) {
        _opts.min_packet_errors >>= 2;
        _opts.min_bit_errors    >>= 2;
        _opts.min_packet_trials >>= 3;
        _opts.min_bit_trials    >>= 3;
        _opts.max_packet_trials >>= 4;
        _opts.max_bit_trials    >>= 4;
    }

#if 0
    // print options
    printf("            %12s %12s %12s\n", "min-errors", "min-trials", "max-trials");
    printf("  bits  :   %12lu %12lu %12lu\n", _opts.min_bit_errors, _opts.min_bit_trials, _opts.max_bit_trials);
    printf("  packet:   %12lu %12lu %12lu\n", _opts.min_packet_errors, _opts.min_packet_trials, _opts.max_packet_trials);
#endif

    // start at a really high SNR range
    float SNRdB_0 = -10.0f; // lower SNR bound
    float SNRdB_1 =  50.0f; // upper SNR bound
    float SNRdB;            // current SNR evaluation

    float BER_0, BER_1; // bit error rate bound
    float PER_0, PER_1; // packet error rate bound
    float BER, PER;     // current error rate evaluation
    float e;            // BER|PER selection

    simulate_per_results results;

    // simulate lower bound
    simulate_per(_opts, SNRdB_0, &results);
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
            printf("  SNR : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
                    SNRdB_0, SNRdB_1, BER_0, BER_1);
        } else {
            printf("  SNR : [%12.4f %12.4f], error rate : [%12.8f %12.8f]\n",
                    SNRdB_0, SNRdB_1, PER_0, PER_1);
        }
#endif

        // bisect SNR limits
        SNRdB = 0.5f*(SNRdB_0 + SNRdB_1);

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
            SNRdB_0 = SNRdB;
            BER_0   = BER;
            PER_0   = PER;
        } else {
            // use lower range
            SNRdB_1 = SNRdB;
            BER_1   = BER;
            PER_1   = PER;
        }

    }

    // apply linear fit to log of error rate
    //  (y-y0) = m*(x-x0)
    //  m = (y0-y1)/(x0-x1)
    //  x = (y-y0)/m + x0
    //
    //  x : SNRdB
    //  y : BER|PER
    float SNRdB_hat;
    float y0=0, y1=0;
    float eps = 1e-12f; // small, insignificant number to ensure no log(0)

    if (_ber_per == ESTIMATE_SNR_BER) {
        y0 = logf(BER_0 + eps);
        y1 = logf(BER_1 + eps);
    } else {
        y0 = logf(PER_0 + eps);
        y1 = logf(PER_1 + eps);
    }
    float m = (y0 - y1) / (SNRdB_0 - SNRdB_1);
    SNRdB_hat = (logf(_error_rate) - y0)/m + SNRdB_0;
#if 0
    printf("  SNR : [%12.4f %12.4f], error rate : [%12.4e %12.4e]\n",
            SNRdB_0, SNRdB_1, y0, y1);
#endif

    return SNRdB_hat;
}

