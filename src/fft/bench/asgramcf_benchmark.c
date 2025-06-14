/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// benchmark asgram objects

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void asgramcf_runbench(struct rusage *     _start,
                       struct rusage *     _finish,
                       unsigned long int * _num_iterations,
                       unsigned int        _nfft)
{
    // scale number of iterations to keep execution time relatively linear
    *_num_iterations = (*_num_iterations) * liquid_nextpow2(1+_nfft) / _nfft;
    *_num_iterations >>= 3;

    // create object
    asgramcf q = asgramcf_create(_nfft);

    // initialize buffer with random values
    unsigned long int i;
    unsigned int buf_len = 2400;
    float complex * buf = (float complex*) malloc(buf_len*sizeof(float complex));
    for (i=0; i<buf_len; i++)
        buf[i] = randnf() + randnf()*_Complex_I;

    // buffer for holding ASCII PSD output
    char psd[_nfft];
    float peakval, peakfreq;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // process input
        asgramcf_write(q, buf, buf_len);

        // get spectrum and feed back to input
        asgramcf_execute(q, psd, &peakval, &peakfreq);
        buf[0] = psd[0];
    }
    getrusage(RUSAGE_SELF, _finish);

    // scale iterations by buffer size to provide inpute rate
    *_num_iterations *= buf_len;

    free(buf);
    asgramcf_destroy(q);
}

// run several configurations
void benchmark_asgramcf_64(struct rusage * _s, struct rusage * _x,
    unsigned long int * _n) { asgramcf_runbench(_s, _x, _n, 64); }

void benchmark_asgramcf_80(struct rusage * _s, struct rusage * _x,
    unsigned long int * _n) { asgramcf_runbench(_s, _x, _n, 80); }

void benchmark_asgramcf_96(struct rusage * _s, struct rusage * _x,
    unsigned long int * _n) { asgramcf_runbench(_s, _x, _n, 96); }

void benchmark_asgramcf_120(struct rusage * _s, struct rusage * _x,
    unsigned long int * _n) { asgramcf_runbench(_s, _x, _n, 120); }

