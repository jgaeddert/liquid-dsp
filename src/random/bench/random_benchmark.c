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

#include "liquid.benchmark.h"

// BENCHMARK: uniform
LIQUID_BENCHMARK(random_uniform, "randf (uniform)", "random,uniform")
{
    float x = 0.0f;
    unsigned long int i;
    // start trials (4 randf per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        x += randf();
        x += randf();
        x += randf();
        x += randf();
    }
    float extime = liquid_toc(timer);
    return extime;
}

// BENCHMARK: normal
LIQUID_BENCHMARK(random_normal, "randnf (normal)", "random,normal")
{
    float x = 0.0f;
    unsigned long int i;
    // start trials (4 randnf per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        x += randnf();
        x += randnf();
        x += randnf();
        x += randnf();
    }
    float extime = liquid_toc(timer);
    return extime;
}

// BENCHMARK: complex normal
LIQUID_BENCHMARK(random_complex_normal, "crandnf (complex normal)", "random,complex,normal")
{
    float complex x = 0.0f;
    unsigned long int i;
    // start trials (4 crandnf per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        crandnf(&x);
        crandnf(&x);
        crandnf(&x);
        crandnf(&x);
    }
    float extime = liquid_toc(timer);
    return extime;
}

// BENCHMARK: Weibull
LIQUID_BENCHMARK(random_weibull, "randweibf (Weibull)", "random,weibull")
{
    float x=0.0f;
    float alpha=1.0f;
    float beta=2.0f;
    float gamma=6.0f;
    unsigned long int i;
    // start trials (4 randweibf per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        x += randweibf(alpha,beta,gamma);
        x += randweibf(alpha,beta,gamma);
        x += randweibf(alpha,beta,gamma);
        x += randweibf(alpha,beta,gamma);
    }
    float extime = liquid_toc(timer);
    return extime;
}

// BENCHMARK: Rice-K
LIQUID_BENCHMARK(random_ricek, "randricekf (Rice-K)", "random,rice")
{
    float x = 0.0f;
    float K=2.0f;
    float omega=1.0f;
    unsigned long int i;
    // start trials (4 randricekf per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        x += randricekf(K,omega);
        x += randricekf(K,omega);
        x += randricekf(K,omega);
        x += randricekf(K,omega);
    }
    float extime = liquid_toc(timer);
    return extime;
}

