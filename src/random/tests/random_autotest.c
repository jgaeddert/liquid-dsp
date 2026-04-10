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

#include "liquid.autotest.h"
#include "liquid.internal.h"

#define LIQUID_RANDOM_AUTOTEST_NUM_TRIALS (250000)
#define LIQUID_RANDOM_AUTOTEST_ERROR_TOL  (0.15)

LIQUID_AUTOTEST(randf,"test generating uniform random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;

    // uniform
    for (i=0; i<N; i++) {
        x = randf();
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    LIQUID_CHECK_DELTA(m1, 0.5f, tol);
    LIQUID_CHECK_DELTA(m2, 1/12.0f, tol);
}

LIQUID_AUTOTEST(randnf,"test generating normal random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;

    // uniform
    for (i=0; i<N; i++) {
        x = randnf();
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    LIQUID_CHECK_DELTA(m1, 0.0f, tol);
    LIQUID_CHECK_DELTA(m2, 1.0f, tol);
}

LIQUID_AUTOTEST(crandnf,"test generating complex circular Gauss random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    liquid_float_complex x;
    float m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;

    // uniform
    for (i=0; i<N; i++) {
        crandnf(&x);
        m1 += crealf(x) + cimagf(x);
        m2 += crealf(x)*crealf(x) + cimagf(x)*cimagf(x);
    }
    N *= 2; // double N for real and imag components
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    LIQUID_CHECK_DELTA(m1, 0.0f, tol);
    LIQUID_CHECK_DELTA(m2, 1.0f, tol);
}

LIQUID_AUTOTEST(randweibf,"test generating Weibull random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;
    float alpha=1.0f, beta=2.0f, gamma=6.0f;

    // uniform
    for (i=0; i<N; i++) {
        x = randweibf(alpha, beta, gamma);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    // compute expected moments (closed-form solution)
    float t0     = liquid_gammaf(1. + 1./alpha);
    float t1     = liquid_gammaf(1. + 2./alpha);
    float m1_exp = beta * t0 + gamma;
    float m2_exp = beta*beta*( t1 - t0*t0 );
    //printf("m1: %12.8f (expected %12.8f)\n", m1, m1_exp);
    //printf("m2: %12.8f (expected %12.8f)\n", m2, m2_exp);

    LIQUID_CHECK_DELTA(m1, m1_exp, tol);
    LIQUID_CHECK_DELTA(m2, m2_exp, tol);
}

LIQUID_AUTOTEST(randricekf,"test generating Rice-K random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;
    float K=2.0f, omega=1.0f;

    // uniform
    for (i=0; i<N; i++) {
        x = randricekf(K, omega);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N);

    LIQUID_CHECK_DELTA(m1, 0.92749f, tol);
    LIQUID_CHECK_DELTA(m2, omega, tol);
}

LIQUID_AUTOTEST(randexpf,"test generating exponential random variables","",0.1)
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol = LIQUID_RANDOM_AUTOTEST_ERROR_TOL;
    float lambda = 2.3f;

    // uniform
    for (i=0; i<N; i++) {
        x = randexpf(lambda);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    // compute expected moments (closed-form solution)
    float m1_exp = 1. / lambda;
    float m2_exp = 1. / (lambda * lambda);
    //printf("m1: %12.8f (expected %12.8f)\n", m1, m1_exp);
    //printf("m2: %12.8f (expected %12.8f)\n", m2, m2_exp);

    LIQUID_CHECK_DELTA(m1, m1_exp, tol);
    LIQUID_CHECK_DELTA(m2, m2_exp, tol);
}

LIQUID_AUTOTEST(random_config,"test random configuration","",0.1)
{
    _liquid_error_downgrade_enable();
    // exponential: lambda out of range
    LIQUID_CHECK( randexpf    (       -1.0f) ==  0.0f );
    LIQUID_CHECK( randexpf_pdf( 0.0f, -1.0f) ==  0.0f );
    LIQUID_CHECK( randexpf_cdf( 0.0f, -1.0f) ==  0.0f );
    // exponential: pdf, cdf with valid input, but negative variable
    LIQUID_CHECK( randexpf_pdf(-2.0f,  2.3f) ==  0.0f );
    LIQUID_CHECK( randexpf_cdf(-2.0f,  2.3f) ==  0.0f );

    // gamma: parameters out of range (alpha)
    LIQUID_CHECK( randgammaf    (       -1.0f,  1.0f) ==  0.0f );
    LIQUID_CHECK( randgammaf_pdf( 0.0f, -1.0f,  1.0f) ==  0.0f );
    LIQUID_CHECK( randgammaf_cdf( 0.0f, -1.0f,  1.0f) ==  0.0f );
    // gamma: parameters out of range (beta)
    LIQUID_CHECK( randgammaf    (        1.0f, -1.0f) ==  0.0f );
    LIQUID_CHECK( randgammaf_pdf( 0.0f,  1.0f, -1.0f) ==  0.0f );
    LIQUID_CHECK( randgammaf_cdf( 0.0f,  1.0f, -1.0f) ==  0.0f );
    // gamma: delta function parameter out of range
    LIQUID_CHECK( randgammaf_delta(-1.0f) ==  0.0f );
    // gamma: pdf, cdf with valid input, but negative variable
    LIQUID_CHECK( randgammaf_pdf(-2.0f, 1.2f, 2.3f) ==  0.0f );
    LIQUID_CHECK( randgammaf_cdf(-2.0f, 1.2f, 2.3f) ==  0.0f );

    // nakagami-m: parameters out of range (m)
    LIQUID_CHECK( randnakmf    (       0.2f,  1.0f) ==  0.0f );
    LIQUID_CHECK( randnakmf_pdf( 0.0f, 0.2f,  1.0f) ==  0.0f );
    LIQUID_CHECK( randnakmf_cdf( 0.0f, 0.2f,  1.0f) ==  0.0f );
    // nakagami-m: parameters out of range (omega)
    LIQUID_CHECK( randnakmf    (       1.0f, -1.0f) ==  0.0f );
    LIQUID_CHECK( randnakmf_pdf( 0.0f, 1.0f, -1.0f) ==  0.0f );
    LIQUID_CHECK( randnakmf_cdf( 0.0f, 1.0f, -1.0f) ==  0.0f );
    // nakagami-m: pdf, cdf with valid input, but negative variable
    LIQUID_CHECK( randnakmf_pdf(-2.0f, 1.2f, 2.3f) ==  0.0f );
    LIQUID_CHECK( randnakmf_cdf(-2.0f, 1.2f, 2.3f) ==  0.0f );
    _liquid_error_downgrade_disable();
}

