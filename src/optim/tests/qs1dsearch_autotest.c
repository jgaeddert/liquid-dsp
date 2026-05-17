/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.autotest.h"
#include "liquid.internal.h"

float qs1dsearch_umin(float _v, void * _context)
{
    float v_opt = *(float*)(_context);
    float v = _v - v_opt;
    return tanhf(v)*tanhf(v);
}

float qs1dsearch_umax(float _v, void * _context)
    { return -qs1dsearch_umin(_v, _context); }

// test initialization on single value
void testbench_qs1dsearch(liquid_autotest   __q__,
                          liquid_utility_1d _utility,
                          float             _v_opt,
                          float             _v_lo,
                          float             _v_hi,
                          int               _bounds,
                          int               _direction)
{
    // create qs1dsearch object and initialize
    qs1dsearch q = qs1dsearch_create(_utility, &_v_opt, _direction);
    if (_bounds) qs1dsearch_init_bounds(q, _v_lo, _v_hi);
    else         qs1dsearch_init       (q, _v_lo);

    // run search
    unsigned int i;
    for (i=0; i<32; i++)
        qs1dsearch_step(q);

    // check result
    LIQUID_CHECK_DELTA( qs1dsearch_get_opt_v(q), _v_opt,                    1e-3f );
    LIQUID_CHECK_DELTA( qs1dsearch_get_opt_u(q), _utility(_v_opt, &_v_opt), 1e-3f );

    // print results
    liquid_log_debug("%3u : u(%12.8f) = %12.4e, v_opt=%12.4e (error=%12.4e)",
        i, qs1dsearch_get_opt_v(q), qs1dsearch_get_opt_u(q),
        _v_opt, _v_opt - qs1dsearch_get_opt_v(q));

    // clean it upt
    qs1dsearch_destroy(q);
}

// unbounded:                                                      (  obj, utility,        opt, lo, hi, bound, dir)
LIQUID_AUTOTEST(qs1dsearch_min_01,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -40,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_02,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -20,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_03,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0,  -4,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_05,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0,   0,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_06,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0,   4,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_07,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0,  20,  0, 0, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_08,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0,  40,  0, 0, LIQUID_OPTIM_MINIMIZE); }
// bounded:                                                        (  obj, utility,        opt, lo, hi, bound, dir)
LIQUID_AUTOTEST(qs1dsearch_min_10,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -30, 15, 1, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_11,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -20, 15, 1, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_12,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -10, 15, 1, LIQUID_OPTIM_MINIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_min_13,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umin, 0, -.1, 15, 1, LIQUID_OPTIM_MINIMIZE); }

// repeat to maximize

// unbounded:                                                      (  obj, utility,        opt, lo, hi, bound, dir)
LIQUID_AUTOTEST(qs1dsearch_max_01,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -40,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_02,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -20,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_03,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0,  -4,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_05,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0,   0,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_06,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0,   4,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_07,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0,  20,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_08,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0,  40,  0, 0, LIQUID_OPTIM_MAXIMIZE); }
// bounded:                                                        (  obj, utility,        opt, lo, hi, bound, dir)
LIQUID_AUTOTEST(qs1dsearch_max_10,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -30, 15, 1, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_11,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -20, 15, 1, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_12,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -10, 15, 1, LIQUID_OPTIM_MAXIMIZE); }
LIQUID_AUTOTEST(qs1dsearch_max_13,"","",0.1) { testbench_qs1dsearch(__q__,qs1dsearch_umax, 0, -.1, 15, 1, LIQUID_OPTIM_MAXIMIZE); }

LIQUID_AUTOTEST(qs1dsearch_config,"qs1dsearch configuration","",0.1)
{
    _liquid_error_downgrade_enable();
    // check invalid function calls
    LIQUID_CHECK(NULL ==qs1dsearch_create(NULL, NULL, LIQUID_OPTIM_MAXIMIZE)) // utility is NULL
    LIQUID_CHECK(NULL ==qs1dsearch_copy  (NULL))

    // create proper object and test configurations
    float v_opt = 0;
    qs1dsearch q = qs1dsearch_create(qs1dsearch_umax, &v_opt, LIQUID_OPTIM_MAXIMIZE);
    LIQUID_CHECK(LIQUID_OK == qs1dsearch_print(q))

    // test configurations
    LIQUID_CHECK(LIQUID_OK != qs1dsearch_step(q)) // object not yet initialized
    qs1dsearch_init(q, 20);

    LIQUID_CHECK(LIQUID_OK == qs1dsearch_execute(q))

    // run a few steps
    LIQUID_CHECK( 0 ==  qs1dsearch_get_num_steps(q))
    qs1dsearch_step(q);
    qs1dsearch_step(q);
    qs1dsearch_step(q);
    LIQUID_CHECK( 3 ==  qs1dsearch_get_num_steps(q))

    // destroy objects
    qs1dsearch_destroy(q);
    _liquid_error_downgrade_disable();
}

