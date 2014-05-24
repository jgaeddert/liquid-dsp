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

// frequency demodulator benchmark
void benchmark_freqdem(struct rusage *     _start,
                       struct rusage *     _finish,
                       unsigned long int * _num_iterations)
{
    // create demodulator
    float   kf  = 0.05f; // modulation index
    freqdem dem = freqdem_create(kf);

    float complex r[20];    // modulated signal
    float         m[20];    // message signal

    unsigned long int i;

    // generate modulated signal
    for (i=0; i<20; i++)
        r[i] = 0.3f*cexpf(_Complex_I*2*M_PI*i/20.0f);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        freqdem_demodulate(dem, r[ 0], &m[ 0]);
        freqdem_demodulate(dem, r[ 1], &m[ 1]);
        freqdem_demodulate(dem, r[ 2], &m[ 2]);
        freqdem_demodulate(dem, r[ 3], &m[ 3]);
        freqdem_demodulate(dem, r[ 4], &m[ 4]);
        freqdem_demodulate(dem, r[ 5], &m[ 5]);
        freqdem_demodulate(dem, r[ 6], &m[ 6]);
        freqdem_demodulate(dem, r[ 7], &m[ 7]);
        freqdem_demodulate(dem, r[ 8], &m[ 8]);
        freqdem_demodulate(dem, r[ 9], &m[ 9]);
        freqdem_demodulate(dem, r[10], &m[10]);
        freqdem_demodulate(dem, r[11], &m[11]);
        freqdem_demodulate(dem, r[12], &m[12]);
        freqdem_demodulate(dem, r[13], &m[13]);
        freqdem_demodulate(dem, r[14], &m[14]);
        freqdem_demodulate(dem, r[15], &m[15]);
        freqdem_demodulate(dem, r[16], &m[16]);
        freqdem_demodulate(dem, r[17], &m[17]);
        freqdem_demodulate(dem, r[18], &m[18]);
        freqdem_demodulate(dem, r[19], &m[19]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 20;

    // destroy demodulator
    freqdem_destroy(dem);
}


