/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
void autotest_dc_gain_control() {
    // set paramaters
    float energy_start = 10.0f;     // initial input energy
    float energy_target = 0.01f;    // target signal energy
    float bt = 0.01f;               // bandwidth-time product
    float tol = 0.001f;             // error tolerance

    float g_target = 10*log10(energy_target/energy_start);

    // create AGC object and initialize
    agc_crcf g = agc_crcf_create();
    agc_crcf_set_target(g,energy_target);
    agc_crcf_set_bandwidth(g, bt);

    unsigned int i;
    float complex x = energy_start, y;
    for (i=0; i<256; i++) {
        agc_crcf_execute(g, x, &y);
    }
    float gain = 10*log10( agc_crcf_get_gain(g) );

    // Check results
    CONTEND_DELTA( crealf(y), energy_target,    tol );
    CONTEND_DELTA( cimagf(y), 0.0f,             tol );
    CONTEND_DELTA( gain,      g_target,         tol );

    agc_crcf_destroy(g);
}

// 
// Test AC gain control
//
void autotest_ac_gain_control() {
    // set paramaters
    float energy_start = 10.0f;     // initial input energy
    float energy_target = 0.01f;    // target signal energy
    float bt = 0.01f;               // bandwidth-time product
    float tol = 0.001f;             // error tolerance
    float theta = 0.0f;             // NCO angle
    float dtheta=0.1f;              // NCO frequency

    float g_target = 10*log10(energy_target/energy_start);

    // create AGC object and initialize
    agc_crcf g = agc_crcf_create();
    agc_crcf_set_target(g,energy_target);
    agc_crcf_set_bandwidth(g, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<256; i++) {
        x = cexpf(_Complex_I*theta) * energy_start;
        theta += dtheta;
        agc_crcf_execute(g, x, &y);
    }
    float gain = 10*log10( agc_crcf_get_gain(g) );

    // Check results
    CONTEND_DELTA( gain,      g_target,         tol );

    agc_crcf_destroy(g);
}



// 
// Test RSSI on sinusoidal input
//
void autotest_agc_rssi_sinusoid() {
    // set paramaters
    float gamma = 0.3f;             // nominal signal level
    float bt = 1e-3f;               // agc bandwidth
    float tol = 0.001f;             // error tolerance

    // signal properties
    float dphi = 0.1f;              // signal frequency

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_target(q, 1.0f);
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<512; i++) {
        // generate sample (complex sinusoid)
        x = cexpf(_Complex_I*dphi*i) * gamma;

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
void autotest_agc_rssi_noise() {
    // set paramaters
    float gamma = -30.0f;           // nominal signal level [dB]
    float bt = 1e-3f;               // agc bandwidth
    float tol = 2.0f;               // error tolerance [dB]

    // signal properties
    float nstd = powf(10.0f, gamma/10);

    // create AGC object and initialize
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_target(q, 1.0f);
    agc_crcf_set_bandwidth(q, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<1024; i++) {
        // generate sample (circular complex noise)
        x = nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // execute agc
        agc_crcf_execute(q, x, &y);
    }

    // get received signal strength indication
    float rssi = 10*log10( agc_crcf_get_signal_level(q) );

    if (liquid_autotest_verbose)
        printf("gamma : %12.8f, rssi : %12.8f\n", gamma, rssi);

    // Check results
    CONTEND_DELTA( rssi, gamma, tol );

    // destroy agc object
    agc_crcf_destroy(q);
}



