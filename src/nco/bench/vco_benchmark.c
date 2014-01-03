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

#include <sys/resource.h>
#include <string.h>

#include "liquid.h"

void benchmark_vco_sincos(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations)
{
    float s, c;
    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);

    unsigned int i;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        nco_crcf_sincos(p, &s, &c);
        nco_crcf_step(p);
    }
    getrusage(RUSAGE_SELF, _finish);

    nco_crcf_destroy(p);
}

void benchmark_vco_mix_up(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations)
{
    float complex x[16],  y[16];
    memset(x, 0, 16*sizeof(float complex));

    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);

    unsigned int i, j;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        for (j=0; j<16; j++) {
            nco_crcf_mix_up(p, x[j], &y[j]);
            nco_crcf_step(p);
        }

    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 16;
    nco_crcf_destroy(p);
}

void benchmark_vco_mix_block_up(struct rusage *_start,
                                struct rusage *_finish,
                                unsigned long int *_num_iterations)
{
    float complex x[16], y[16];
    memset(x, 0, 16*sizeof(float complex));

    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);

    unsigned int i;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        nco_crcf_mix_block_up(p, x, y, 16);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 16;
    nco_crcf_destroy(p);
}

