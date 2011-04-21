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

//
// ofdmoqamframe64 common data
//  - physical layer convergence procedure (PLCP)
//

#include <math.h>

#include "liquid.internal.h"

int ofdmoqamframe64_getsctype(unsigned int _id)
{
    if (_id==0 || (_id>26 && _id<38))
        return OFDMOQAMFRAME64_SCTYPE_NULL;
    else if (_id==11 || _id==25 || _id==39 || _id==53)
        return OFDMOQAMFRAME64_SCTYPE_PILOT;
    else
        return OFDMOQAMFRAME64_SCTYPE_DATA;
}

void ofdmoqamframe64_init_S0(float complex * _S0)
{
    msequence ms = msequence_create_default(4);
    modem mod = modem_create(LIQUID_MODEM_QPSK,2);
    unsigned int s;
    float complex sym;
    unsigned int num_subcarriers = 64;
    float zeta = 1.0f;
    unsigned int j;
    unsigned int sctype;

    // short sequence
    for (j=0; j<num_subcarriers; j++) {
        sctype = ofdmoqamframe64_getsctype(j);
        if (sctype == OFDMOQAMFRAME64_SCTYPE_NULL) {
            // NULL subcarrier
            _S0[j] = 0.0f;
        } else {
            if ((j%4) == 2) {
                // even subcarrer, skipping ever other (14 total)
                s = msequence_generate_symbol(ms,2);
                modem_modulate(mod,s,&sym);
                // retain only quadrature component (time aligned
                // without half-symbol delay), and amplitude-
                // compensated.
                _S0[j] = cimagf(sym) * _Complex_I * zeta * 2.0f * sqrtf(2.0f);
            } else {
                // odd subcarrer
                _S0[j] = 0.0f;
            }
        }
    }
    msequence_destroy(ms);
    modem_destroy(mod);
}

void ofdmoqamframe64_init_S1(float complex * _S1)
{
    msequence ms = msequence_create_default(5);
    modem mod = modem_create(LIQUID_MODEM_QPSK,2);
    unsigned int s;
    float complex sym;
    unsigned int num_subcarriers = 64;
    float zeta = 1.0f;
    unsigned int j;
    unsigned int sctype;

    // long sequence
    for (j=0; j<num_subcarriers; j++) {
        sctype = ofdmoqamframe64_getsctype(j);
        if (sctype == OFDMOQAMFRAME64_SCTYPE_NULL) {
            // NULL subcarrier
            _S1[j] = 0.0f;
        } else {
            s = msequence_generate_symbol(ms,2);
            modem_modulate(mod,s,&sym);
            if ((j%2) == 0) {
                // even subcarrer
                _S1[j] = zeta * sqrtf(2.0f) * crealf(sym);
            } else {
                // odd subcarrer
                _S1[j] = zeta * sqrtf(2.0f) * cimagf(sym) * _Complex_I;
            }
        }
    }
    msequence_destroy(ms);
    modem_destroy(mod);
}

void ofdmoqamframe64_init_S2(float complex * _S2)
{
    msequence ms = msequence_create_default(4);
    modem mod = modem_create(LIQUID_MODEM_QPSK,2);
    unsigned int s;
    float complex sym;
    unsigned int num_subcarriers = 64;
    float zeta = 1.0f;
    unsigned int j;
    unsigned int sctype;

    // short sequence
    for (j=0; j<num_subcarriers; j++) {
        sctype = ofdmoqamframe64_getsctype(j);
        if (sctype == OFDMOQAMFRAME64_SCTYPE_NULL) {
            // NULL subcarrier
            _S2[j] = 0.0f;
        } else {
            if ((j%2) == 0) {
                // even subcarrer
                _S2[j] = 0.0f;
            } else {
                // odd subcarrer
                s = msequence_generate_symbol(ms,2);
                modem_modulate(mod,s,&sym);
                // retain only quadrature component (time aligned
                // without half-symbol delay), and amplitude-
                // compensated.
                _S2[j] = cimagf(sym) * _Complex_I * zeta * 2.0f;
            }
        }
    }
    msequence_destroy(ms);
    modem_destroy(mod);
}


