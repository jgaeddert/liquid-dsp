/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// fftq16_runbench.c : benchmark execution program (fixed-point)
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void fftq16_runbench(struct rusage *     _start,
                     struct rusage *     _finish,
                     unsigned long int * _num_iterations,
                     unsigned int        _nfft,
                     int                 _direction)
{
    // initialize arrays, plan
    cq16_t * x = (cq16_t *) malloc(_nfft*sizeof(cq16_t));
    cq16_t * y = (cq16_t *) malloc(_nfft*sizeof(cq16_t));
    int _method = 0;
   
    // create fixed-point FFT plan
    fftq16plan q = fftq16_create_plan(_nfft, x, y, _direction, _method);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_nfft; i++) {
        x[i].real = q16_float_to_fixed(randnf());
        x[i].imag = q16_float_to_fixed(randnf());
    }

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _nfft;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fftq16_execute(q);
        fftq16_execute(q);
        fftq16_execute(q);
        fftq16_execute(q);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    fftq16_destroy_plan(q);
    free(x);
    free(y);
}

