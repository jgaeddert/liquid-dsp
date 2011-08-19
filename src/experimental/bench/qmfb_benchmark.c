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

#include <sys/resource.h>
#include "liquid.experimental.h"

// Helper function to keep code base small
void qmfb_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{

    float beta=0.3f;

    qmfb_crcf q = qmfb_crcf_create(_h_len,beta,LIQUID_QMFB_ANALYZER);

    float complex x0 =  1.0f;
    float complex x1 = -1.0f;
    float complex y0, y1;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        qmfb_crcf_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_execute(q,x0,x1,&y0,&y1);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    qmfb_crcf_destroy(q);
}

#define QMFB_CRCF_BENCHMARK_API(H_LEN)  \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ qmfb_crcf_bench(_start, _finish, _num_iterations, H_LEN); }

void benchmark_qmfb_crcf_h8     QMFB_CRCF_BENCHMARK_API(8)
void benchmark_qmfb_crcf_h16    QMFB_CRCF_BENCHMARK_API(16)
void benchmark_qmfb_crcf_h32    QMFB_CRCF_BENCHMARK_API(32)
void benchmark_qmfb_crcf_h64    QMFB_CRCF_BENCHMARK_API(64)

