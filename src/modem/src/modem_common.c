/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
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

//
// modem_common.c : common utilities specific to precision
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

// modem structure used for both modulation and demodulation 
//
// The modem structure implements a variety of common modulation schemes,
// including (differential) phase-shift keying, and (quadrature) amplitude
// modulation.
//
// While the same modem structure may be used for both modulation and
// demodulation for most schemes, it is important to use separate objects
// for differential-mode modems (e.g. LIQUID_MODEM_DPSK) as the internal state
// will change after each symbol.  It is usually good practice to keep
// separate instances of modulators and demodulators.
struct modem_s {
    modulation_scheme scheme;       // modulation scheme

    unsigned int m;                 // bits per symbol (modulation depth)
    unsigned int M;                 // constellation size, M=2^m

    float alpha;                    // scaling factor to ensure unity energy

    // Reference vector for demodulating linear arrays
    //
    // By storing these values in an array they do not need to be
    // calculated during run-time.  This speeds up the demodulation by
    // approximately 8%.
    float ref[MAX_MOD_BITS_PER_SYMBOL];

    // modulation
    float complex * symbol_map;     // complete symbol map
    int modulate_using_map;         // modulate using map (look-up table) flag

    // demodulation
    float complex r;                // received state vector
    float complex x_hat;            // estimated symbol (demodulator)

    // QAM modem
    unsigned int m_i;               // bits per symbol, in-phase
    unsigned int M_i;               // in-phase dimension, M_i=2^{m_i}
    unsigned int m_q;               // bits per symbol, quadrature
    unsigned int M_q;               // quadrature dimension, M_q=2^{m_q}

    // PSK/DPSK modem
    float d_phi;                    // half of phase between symbols
    float dpsk_phi;                 // angle state for differential PSK

    // APSK modem
    unsigned int apsk_num_levels;   // number of levels
    unsigned int * apsk_p;          // number of symbols per level
    float * apsk_r;                 // radii of levels
    float * apsk_r_slicer;          // slicer radii of levels
    float * apsk_phi;               // phase offset of levels
    unsigned int * apsk_symbol_map; // symbol mapping

    // modulate function pointer
    void (*modulate_func)(modem _mod,
                          unsigned int _symbol_in,
                          float complex * _y);

    // demodulate function pointer
    void (*demodulate_func)(modem _demod,
                            float complex _x,
                            unsigned int * _symbol_out);

    // soft demodulation
    //int demodulate_soft;    // soft demodulation flag
    // neighbors array
    unsigned char * demod_soft_neighbors;   // array of nearest neighbors
    unsigned int demod_soft_p;              // number of neighbors in array
};

// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod)
{
    return rand() % (_mod->M);
}

// Get modem depth (bits/symbol)
unsigned int modem_get_bps(modem _mod)
{
    return _mod->m;
}

