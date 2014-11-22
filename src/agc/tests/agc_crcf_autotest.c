/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autotest/autotest.h"
#include "liquid.h"

// 
// Test DC gain control
//
void autotest_agc_crcf_dc_gain_control()
{
    // set paramaters
    float gamma = 0.1f;     // nominal signal level
    float bt    = 0.1f;     // bandwidth-time product
    float tol   = 0.001f;   // error tolerance

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x = gamma;    // input sample
    float complex y;            // output sample
    for (i=0; i<256; i++)
        agc_crcf_execute(q, x, &y);
    
    // Check results
    CONTEND_DELTA( crealf(y), 1.0f, tol );
    CONTEND_DELTA( cimagf(y), 0.0f, tol );
    CONTEND_DELTA( agc_crcf_get_gain(q), 1.0f/gamma, tol );

    // destroy AGC object
    agc_crcf_destroy(q);
}

// 
// Test AC gain control
//
void autotest_agc_crcf_ac_gain_control()
{
    // set paramaters
    float gamma = 0.1f;             // nominal signal level
    float bt    = 0.1f;             // bandwidth-time product
    float tol   = 0.001f;           // error tolerance
    float dphi  = 0.1f;             // NCO frequency

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x;
    float complex y;
    for (i=0; i<256; i++) {
        x = gamma * cexpf(_Complex_I*i*dphi);
        agc_crcf_execute(q, x, &y);
    }

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, agc_crcf_get_signal_level(q));

    // Check results
    CONTEND_DELTA( agc_crcf_get_gain(q), 1.0f/gamma, tol);

    // destroy AGC object
    agc_crcf_destroy(q);
}



// 
// Test RSSI on sinusoidal input
//
void autotest_agc_crcf_rssi_sinusoid()
{
    // set paramaters
    float gamma = 0.3f;         // nominal signal level
    float bt    = 0.05f;        // agc bandwidth
    float tol   = 0.001f;       // error tolerance

    // signal properties
    float dphi = 0.1f;          // signal frequency

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<512; i++) {
        // generate sample (complex sinusoid)
        x = gamma * cexpf(_Complex_I*dphi*i);

        // execute agc
        agc_crcf_execute(q, x, &y);
    }

    // get received signal strength indication
    float rssi = agc_crcf_get_signal_level(q);

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, rssi);

    // Check results
    CONTEND_DELTA( rssi, gamma, tol );

    // destroy agc object
    agc_crcf_destroy(q);
}


// 
// Test RSSI on noise input
//
void autotest_agc_crcf_rssi_noise()
{
    // set paramaters
    float gamma = -30.0f;   // nominal signal level [dB]
    float bt    =  0.01f;   // agc bandwidth
    float tol   =  0.2f;    // error tolerance [dB]

    // signal properties
    float nstd = powf(10.0f, gamma/20);

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<2000; i++) {
        // generate sample (circular complex noise)
        x = nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // execute agc
        agc_crcf_execute(q, x, &y);
    }

    // get received signal strength indication
    float rssi = agc_crcf_get_rssi(q);

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, rssi);

    // Check results
    CONTEND_DELTA( rssi, gamma, tol );

    // destroy agc object
    agc_crcf_destroy(q);
}



