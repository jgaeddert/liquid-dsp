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
#include "liquid.h"

// Helper function to keep code base small
void iirfilt_crcf_bench(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations,
                        unsigned int        _order,
                        unsigned int        _format)
{
    unsigned int i;

    // scale number of iterations (trials)
    if (_format == LIQUID_IIRDES_TF) {
        // cycles/trial ~ 128 + 15.3*_order;
        *_num_iterations *= 1000;
        *_num_iterations /= (unsigned int)(128 + 15.3*_order);
    } else {
        // cycles/trial ~ 93 + 53.3*_order
        *_num_iterations *= 800;
        *_num_iterations /= (unsigned int)(93 + 53.3*_order);
    }

    // create filter object from prototype
    float fc    =  0.2f;    // filter cut-off frequency
    float f0    =  0.0f;    // filter center frequency (band-pass, band-stop)
    float Ap    =  0.1f;    // filter pass-band ripple
    float As    = 60.0f;    // filter stop-band attenuation
    iirfilt_crcf q = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER,
                                                   LIQUID_IIRDES_LOWPASS,
                                                   _format,
                                                   _order,
                                                   fc, f0, Ap, As);

    // initialize input/output
    float complex x[4];
    float complex y[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        iirfilt_crcf_execute(q, x[0], &y[0]);
        iirfilt_crcf_execute(q, x[1], &y[1]);
        iirfilt_crcf_execute(q, x[2], &y[2]);
        iirfilt_crcf_execute(q, x[3], &y[3]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // destroy filter object
    iirfilt_crcf_destroy(q);
}

#define IIRFILT_CRCF_BENCHMARK_API(N,T)     \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ iirfilt_crcf_bench(_start, _finish, _num_iterations, N, T); }

// benchmark regular transfer function form
void benchmark_iirfilt_crcf_4        IIRFILT_CRCF_BENCHMARK_API(4,    LIQUID_IIRDES_TF)
void benchmark_iirfilt_crcf_8        IIRFILT_CRCF_BENCHMARK_API(8,    LIQUID_IIRDES_TF)
void benchmark_iirfilt_crcf_16       IIRFILT_CRCF_BENCHMARK_API(16,   LIQUID_IIRDES_TF)
void benchmark_iirfilt_crcf_32       IIRFILT_CRCF_BENCHMARK_API(32,   LIQUID_IIRDES_TF)
void benchmark_iirfilt_crcf_64       IIRFILT_CRCF_BENCHMARK_API(64,   LIQUID_IIRDES_TF)

// benchmark second-order sections form
void benchmark_iirfilt_crcf_sos_4    IIRFILT_CRCF_BENCHMARK_API(4,    LIQUID_IIRDES_SOS)
void benchmark_iirfilt_crcf_sos_8    IIRFILT_CRCF_BENCHMARK_API(8,    LIQUID_IIRDES_SOS)
void benchmark_iirfilt_crcf_sos_16   IIRFILT_CRCF_BENCHMARK_API(16,   LIQUID_IIRDES_SOS)
void benchmark_iirfilt_crcf_sos_32   IIRFILT_CRCF_BENCHMARK_API(32,   LIQUID_IIRDES_SOS)
void benchmark_iirfilt_crcf_sos_64   IIRFILT_CRCF_BENCHMARK_API(64,   LIQUID_IIRDES_SOS)

