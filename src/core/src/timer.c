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

#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "liquid.h"

// timer data structure
struct liquid_timer_s
{
    // timer type
    int type;

#ifdef _WIN32
    // QPC tick at creation (LIQUID_TIMER_CLOCK)
    LARGE_INTEGER tic_qpc;

    // process CPU time at creation, 100ns units (LIQUID_TIMER_RUSAGE)
    ULARGE_INTEGER tic_cpu;
#else
    // time object was created (LIQUID_TIMER_CLOCK)
    struct timeval tic_timeval;

    // time object was created (LIQUID_TIMER_RUSAGE)
    struct rusage tic_rusage;
#endif
};

// create liquid_timer object
liquid_timer liquid_timer_create(int _type)
{
    liquid_timer q = (liquid_timer) malloc(sizeof(struct liquid_timer_s));

    //
    q->type = _type;
    if (q->type == LIQUID_TIMER_CLOCK) {
    } else if (q->type == LIQUID_TIMER_RUSAGE) {
    } else {
        free(q);
        return liquid_error_config("liquid_create(), invalid timer type");
    }

    // reset timer
    if (liquid_timer_tic(q))
    {
        free(q);
        return liquid_error_config("liquid_create(), could not reset timer");
    }
    return q;
}

// create liquid_timer object
int liquid_timer_destroy(liquid_timer _q)
{
    free(_q);
    return LIQUID_OK;
}

// create liquid_timer object
int liquid_timer_tic(liquid_timer _q)
{
    if (_q->type == LIQUID_TIMER_CLOCK) {
#ifdef _WIN32
        if (!QueryPerformanceCounter(&_q->tic_qpc))
            return liquid_error(LIQUID_EINT,"liquid_timer_tic(), QueryPerformanceCounter() failed");
#else
        if (gettimeofday(&_q->tic_timeval, NULL))
            return liquid_error(LIQUID_EINT,"liquid_timer_tic(), gettimeofday() returned invalid flag");
#endif
    } else if (_q->type == LIQUID_TIMER_RUSAGE) {
#ifdef _WIN32
        FILETIME _ct, _et, _kt, _ut;
        ULARGE_INTEGER _k, _u;
        if (!GetProcessTimes(GetCurrentProcess(), &_ct, &_et, &_kt, &_ut))
            return liquid_error(LIQUID_EINT,"liquid_timer_tic(), GetProcessTimes() failed");
        _k.LowPart = _kt.dwLowDateTime; _k.HighPart = _kt.dwHighDateTime;
        _u.LowPart = _ut.dwLowDateTime; _u.HighPart = _ut.dwHighDateTime;
        _q->tic_cpu.QuadPart = _k.QuadPart + _u.QuadPart;
#else
        getrusage(RUSAGE_SELF, &_q->tic_rusage);
#endif
    } else {
        return liquid_error(LIQUID_EINT,"liquid_timer_tic(), invalid timer type");
    }
    return LIQUID_OK;
}

// get elapsed time since 'tic' in seconds
float liquid_timer_toc(liquid_timer _q)
{
    if (_q->type == LIQUID_TIMER_CLOCK) {
#ifdef _WIN32
        LARGE_INTEGER _toc, _freq;
        if (!QueryPerformanceCounter(&_toc) || !QueryPerformanceFrequency(&_freq) || _freq.QuadPart == 0)
        {
            liquid_error(LIQUID_EINT,"liquid_timer_toc(), QueryPerformanceCounter() failed");
            return -1;
        }
        return (float)(_toc.QuadPart - _q->tic_qpc.QuadPart) / (float)_freq.QuadPart;
#else
        struct timeval toc;
        if (gettimeofday(&toc, NULL))
        {
            liquid_error(LIQUID_EINT,"liquid_timer_toc(), gettimeofday() returned invalid flag");
            return -1;
        }

        // compute execution time (in seconds)
        float s  = (float)(toc.tv_sec  - _q->tic_timeval.tv_sec);
        float us = (float)(toc.tv_usec - _q->tic_timeval.tv_usec);
        return s + us*1e-6f;
#endif

    } else if (_q->type == LIQUID_TIMER_RUSAGE) {
#ifdef _WIN32
        FILETIME _ct, _et, _kt, _ut;
        ULARGE_INTEGER _k, _u;
        GetProcessTimes(GetCurrentProcess(), &_ct, &_et, &_kt, &_ut);
        _k.LowPart = _kt.dwLowDateTime; _k.HighPart = _kt.dwHighDateTime;
        _u.LowPart = _ut.dwLowDateTime; _u.HighPart = _ut.dwHighDateTime;
        return 1e-7f * (float)(_k.QuadPart + _u.QuadPart - _q->tic_cpu.QuadPart);
#else
        struct rusage toc;
        getrusage(RUSAGE_SELF, &toc);
        // compute run time
        float time_s  = toc.ru_utime.tv_sec - _q->tic_rusage.ru_utime.tv_sec
                      + toc.ru_stime.tv_sec - _q->tic_rusage.ru_stime.tv_sec;
        float time_us = toc.ru_utime.tv_usec - _q->tic_rusage.ru_utime.tv_usec
                      + toc.ru_stime.tv_usec - _q->tic_rusage.ru_stime.tv_usec;
        return time_s + 1e-6f*time_us;
#endif
    }
    return liquid_error(LIQUID_EINT,"liquid_timer_toc(), invalid timer type");
}

// compact: create and start timer
liquid_timer liquid_tic(void)
{
    return liquid_timer_create(LIQUID_TIMER_CLOCK);
}

// compact: destroy timer and retrieve runtime in seconds
float liquid_toc(liquid_timer _q)
{
    float toc = liquid_timer_toc(_q);
    liquid_timer_destroy(_q);
    return toc;
}

