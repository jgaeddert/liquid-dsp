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

// frequency modulator benchmark
void benchmark_freqmod(struct rusage *     _start,
                       struct rusage *     _finish,
                       unsigned long int * _num_iterations)
{
    // create modulator
    float   kf  = 0.05f; // modulation index
    freqmod mod = freqmod_create(kf);

    float         m[20];    // message signal
    float complex r[20];    // modulated signal

    unsigned long int i;

    // generate message signal (sum of sines)
    for (i=0; i<20; i++) {
        m[i] = 0.3f*cosf(2*M_PI*1*i/20.0f + 0.0f) +
               0.2f*cosf(2*M_PI*2*i/20.0f + 0.4f) +
               0.4f*cosf(2*M_PI*3*i/20.0f + 1.7f);
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        freqmod_modulate(mod, m[ 0], &r[ 0]);
        freqmod_modulate(mod, m[ 1], &r[ 1]);
        freqmod_modulate(mod, m[ 2], &r[ 2]);
        freqmod_modulate(mod, m[ 3], &r[ 3]);
        freqmod_modulate(mod, m[ 4], &r[ 4]);
        freqmod_modulate(mod, m[ 5], &r[ 5]);
        freqmod_modulate(mod, m[ 6], &r[ 6]);
        freqmod_modulate(mod, m[ 7], &r[ 7]);
        freqmod_modulate(mod, m[ 8], &r[ 8]);
        freqmod_modulate(mod, m[ 9], &r[ 9]);
        freqmod_modulate(mod, m[10], &r[10]);
        freqmod_modulate(mod, m[11], &r[11]);
        freqmod_modulate(mod, m[12], &r[12]);
        freqmod_modulate(mod, m[13], &r[13]);
        freqmod_modulate(mod, m[14], &r[14]);
        freqmod_modulate(mod, m[15], &r[15]);
        freqmod_modulate(mod, m[16], &r[16]);
        freqmod_modulate(mod, m[17], &r[17]);
        freqmod_modulate(mod, m[18], &r[18]);
        freqmod_modulate(mod, m[19], &r[19]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 20;

    // destroy modulator
    freqmod_destroy(mod);
}


