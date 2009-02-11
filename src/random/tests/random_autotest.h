#ifndef __RANDOM_AUTOTEST_H__
#define __RANDOM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// uniform
void autotest_randf()
{
    unsigned long int N=250000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.02f;

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
    unsigned long int N=250000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.02f;

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
    unsigned long int N=250000; // number of trials
    unsigned long int i;
    float complex x;
    float m1=0.0f, m2=0.0f;
    float tol=0.02f;

    // uniform
    for (i=0; i<N; i++) {
        x = crandnf();
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
void autotest_rand_weibullf()
{
    unsigned long int N=250000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.02f;
    float alpha=1.0f, beta=2.0f, gamma=6.0f;

    // uniform
    for (i=0; i<N; i++) {
        x = rand_weibullf(alpha, beta, gamma);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    CONTEND_DELTA(m1, 1.2533f+gamma, tol);
    CONTEND_DELTA(m2, 0.42920f, tol);
}

// Rice-K
void autotest_rand_ricekf()
{
    unsigned long int N=250000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.02f;
    float K=2.0f, omega=1.0f;

    // uniform
    for (i=0; i<N; i++) {
        x = rand_ricekf(K, omega);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N);

    CONTEND_DELTA(m1, 0.92749f, tol);
    CONTEND_DELTA(m2, omega, tol);
}

#endif 

