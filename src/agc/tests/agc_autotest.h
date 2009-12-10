/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __AGC_AUTOTEST_H__
#define __AGC_AUTOTEST_H__

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
    agc g = agc_create();
    agc_set_target(g,energy_target);
    agc_set_bandwidth(g, bt);

    unsigned int i;
    float complex x = energy_start, y;
    for (i=0; i<256; i++) {
        agc_execute(g, x, &y);
    }
    float gain = 10*log10( agc_get_gain(g) );

    // Check results
    CONTEND_DELTA( crealf(y), energy_target,    tol );
    CONTEND_DELTA( cimagf(y), 0.0f,             tol );
    CONTEND_DELTA( gain,      g_target,         tol );

    agc_destroy(g);
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
    agc g = agc_create();
    agc_set_target(g,energy_target);
    agc_set_bandwidth(g, bt);

    unsigned int i;
    float complex x, y;
    for (i=0; i<256; i++) {
        x = cexpf(_Complex_I*theta) * energy_start;
        theta += dtheta;
        agc_execute(g, x, &y);
    }
    float gain = 10*log10( agc_get_gain(g) );

    // Check results
    CONTEND_DELTA( gain,      g_target,         tol );

    agc_destroy(g);
}



#endif // __AGC_AUTOTEST_H__

