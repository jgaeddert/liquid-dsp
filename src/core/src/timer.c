/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "liquid.h"

// timer data structure
struct liquid_timer_s
{
    // time object was created
    struct timeval tic;
};

// create liquid_timer object
liquid_timer liquid_timer_tic()
{
    liquid_timer q = (liquid_timer) malloc(sizeof(struct liquid_timer_s));
    if (gettimeofday(&q->tic, NULL))
    {
        free(q);
        return liquid_error_config("liquid_timer_tic(), gettimeofday() returned invalid flag");
    }
    return q;
}

// get elapsed time since 'tic' in seconds
float liquid_timer_toc(liquid_timer _q)
{
    struct timeval toc;
    if (gettimeofday(&toc, NULL))
    {
        free(_q);
        liquid_error(LIQUID_EINT,"liquid_timer_toc(), gettimeofday() returned invalid flag");
        return -1;
    }

    // compute execution time (in seconds)
    float s  = (float)(toc.tv_sec  - _q->tic.tv_sec);
    float us = (float)(toc.tv_usec - _q->tic.tv_usec);
    free(_q);
    return s + us*1e-6f;
}

