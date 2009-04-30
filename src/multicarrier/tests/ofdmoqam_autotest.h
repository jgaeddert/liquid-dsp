#ifndef __LIQUID_OFDMOQAM_AUTOTEST_H__
#define __LIQUID_OFDMOQAM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: test sub-band energy
//
void autotest_ofdmoqam_synthesis()
{
    unsigned int num_channels=4;
    unsigned int num_symbols=4;
    unsigned int m=2;
    float tol=0.05f;

    // derived values
    unsigned int frame_length = num_symbols+m;
    float complex symbol_matrix[num_channels][num_symbols];

    unsigned int i, j;
    ofdmoqam c = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);

    // create bank of filters (interpolators)
    unsigned int h_len = 2*num_channels*m + 1;
    float h[h_len];
    design_rrc_filter(num_channels,m,0.99f,0,h);
    interp_crcf interp[num_channels];
    for (i=0; i<num_channels; i++)
        interp[i] = interp_crcf_create(num_channels, h, h_len);

    // create bank of oscilators
    nco nco_synth[num_channels];
    float fc;   // carrier frequency
    for (i=0; i<num_channels; i++) {
        fc = (float)(i) / (float)(num_channels);
        nco_synth[i] = nco_create();
        nco_set_frequency(nco_synth[i],fc);
        nco_set_phase(nco_synth[i],0.0f);
    }

    // initialize symbol matrix
    for (i=0; i<num_channels; i++) {
        for (j=0; j<num_symbols; j++) {
            // generate random QPSK symbol
            symbol_matrix[i][j] = (rand()%2 ? 1.0f : -1.0f)
                                + (rand()%2 ? 1.0f : -1.0f) * _Complex_I;
        }
    }

    //
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<num_channels; j++) {
        }
    }

    // destroy objects
    ofdmoqam_destroy(c);
    for (i=0; i<num_channels; i++) {
        interp_crcf_destroy(interp[i]);
        nco_destroy(nco_synth[i]);
    }
}

#endif // __LIQUID_OFDMOQAM_AUTOTEST_H__

