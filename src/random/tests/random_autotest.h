#ifndef __RANDOM_AUTOTEST_H__
#define __RANDOM_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/random.h"

// uniform
void autotest_randf()
{
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.01f;

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
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float x, y, m1=0.0f, m2=0.0f;
    float tol=0.01f;

    // uniform
    for (i=0; i<N; i++) {
        randnf(&x, &y);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    CONTEND_DELTA(m1, 0.0f, tol);
    CONTEND_DELTA(m2, 1.0f, tol);
}

// Weibull
void autotest_rand_weibullf()
{
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float x, m1=0.0f, m2=0.0f;
    float tol=0.01f;
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

#endif 

