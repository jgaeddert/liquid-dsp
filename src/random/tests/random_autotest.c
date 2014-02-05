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

#include "autotest/autotest.h"
#include "liquid.h"

#define LIQUID_RANDOM_AUTOTEST_NUM_TRIALS (100000)
#define LIQUID_RANDOM_AUTOTEST_ERROR_TOL  (0.1)

// uniform
void autotest_randf()
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

    CONTEND_DELTA(m1, 0.5f, tol);
    CONTEND_DELTA(m2, 1/12.0f, tol);
}

// Gauss
void autotest_randnf()
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

    CONTEND_DELTA(m1, 0.0f, tol);
    CONTEND_DELTA(m2, 1.0f, tol);
}

// complex Gauss
void autotest_crandnf()
{
    unsigned long int N = LIQUID_RANDOM_AUTOTEST_NUM_TRIALS;
    unsigned long int i;
    float complex x;
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

    CONTEND_DELTA(m1, 0.0f, tol);
    CONTEND_DELTA(m2, 1.0f, tol);
}

// Weibull
void autotest_randweibf()
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

    CONTEND_DELTA(m1, 1.2533f+gamma, tol);
    CONTEND_DELTA(m2, 0.42920f, tol);
}

// Rice-K
void autotest_randricekf()
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

    CONTEND_DELTA(m1, 0.92749f, tol);
    CONTEND_DELTA(m2, omega, tol);
}

