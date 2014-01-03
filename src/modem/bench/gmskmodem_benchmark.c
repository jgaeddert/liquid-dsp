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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

// 
void benchmark_gmskmodem_modulate(struct rusage *_start,
                                  struct rusage *_finish,
                                  unsigned long int *_num_iterations)
{
    // options
    unsigned int k=2;   // filter samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float BT=0.3f;      // bandwidth-time product

    // create modem object
    gmskmod mod   = gmskmod_create(k, m, BT);

    float complex x[k];
    unsigned int symbol_in = 0;
    
    unsigned long int i;
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
        gmskmod_modulate(mod, symbol_in, x);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // destroy modem objects
    gmskmod_destroy(mod);
}

// 
void benchmark_gmskmodem_demodulate(struct rusage *_start,
                                    struct rusage *_finish,
                                    unsigned long int *_num_iterations)
{
    // options
    unsigned int k=2;   // filter samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float BT=0.3f;      // bandwidth-time product

    // create modem object
    gmskdem demod = gmskdem_create(k, m, BT);

    float complex x[k];
    unsigned int symbol_out = 0;
    
    unsigned long int i;
    for (i=0; i<k; i++)
        x[i] = randnf()*cexpf(_Complex_I*2*M_PI*randf());

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
        gmskdem_demodulate(demod, x, &symbol_out);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // destroy modem objects
    gmskdem_destroy(demod);
}

