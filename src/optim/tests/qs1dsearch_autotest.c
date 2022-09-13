/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

float qs1dsearch_utility_min(float _v, void * _context)
{
    float v_opt = *(float*)(_context);
    float v = _v - v_opt;
    return tanhf(v)*tanhf(v);
}

float qs1dsearch_utility_max(float _v, void * _context)
{
    return -qs1dsearch_utility_min(_v, _context);
}

// test initialization on single value
void testbench_qs1dsearch(liquid_utility_1d _utility,
                          float             _v_opt,
                          float             _v_init,
                          int               _direction)
{
    // create qs1dsearch object and initialize
    qs1dsearch q = qs1dsearch_create(_utility, &_v_opt, _direction);
    qs1dsearch_init(q, _v_init);

    // run search
    unsigned int i;
    for (i=0; i<32; i++) {
        qs1dsearch_step(q);
        if (liquid_autotest_verbose)
            qs1dsearch_print(q);
    }

    // check result
    CONTEND_DELTA( qs1dsearch_get_opt_v(q), _v_opt,                    1e-3f );
    CONTEND_DELTA( qs1dsearch_get_opt_u(q), _utility(_v_opt, &_v_opt), 1e-3f );

    // print results
    if (liquid_autotest_verbose) {
        printf("%3u : u(%12.8f) = %12.4e, v_opt=%12.4e (error=%12.4e)\n",
            i, qs1dsearch_get_opt_v(q), qs1dsearch_get_opt_u(q),
            _v_opt, _v_opt - qs1dsearch_get_opt_v(q));
    }

    // clean it upt
    qs1dsearch_destroy(q);
}

void autotest_qs1dsearch_01() { testbench_qs1dsearch(qs1dsearch_utility_min, 0, -20, LIQUID_OPTIM_MINIMIZE); }
void autotest_qs1dsearch_02() { testbench_qs1dsearch(qs1dsearch_utility_max, 0, -20, LIQUID_OPTIM_MAXIMIZE); }

// test initialization on bounds
void testbench_qs1dsearch_bounds(liquid_utility_1d _utility,
                                 float             _v_opt,
                                 float             _v_lo,
                                 float             _v_hi,
                                 int               _direction)
{
    // create qs1dsearch object and initialize
    qs1dsearch q = qs1dsearch_create(_utility, &_v_opt, _direction);
    qs1dsearch_init_bounds(q, _v_lo, _v_hi);

    // run search
    unsigned int i;
    for (i=0; i<32; i++) {
        qs1dsearch_step(q);
        if (liquid_autotest_verbose)
            qs1dsearch_print(q);
    }

    // check result
    CONTEND_DELTA( qs1dsearch_get_opt_v(q), _v_opt,                    1e-3f );
    CONTEND_DELTA( qs1dsearch_get_opt_u(q), _utility(_v_opt, &_v_opt), 1e-3f );

    // print results
    if (liquid_autotest_verbose) {
        printf("%3u : u(%12.8f) = %12.4e, v_opt=%12.4e (error=%12.4e)\n",
            i, qs1dsearch_get_opt_v(q), qs1dsearch_get_opt_u(q),
            _v_opt, _v_opt - qs1dsearch_get_opt_v(q));
    }

    // clean it upt
    qs1dsearch_destroy(q);
}

void autotest_qs1dsearch_03() { testbench_qs1dsearch_bounds(qs1dsearch_utility_min, 0, -20, 10, LIQUID_OPTIM_MINIMIZE); }
void autotest_qs1dsearch_04() { testbench_qs1dsearch_bounds(qs1dsearch_utility_max, 0, -20, 10, LIQUID_OPTIM_MAXIMIZE); }

