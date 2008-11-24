//
//
//

#include <math.h>
#include <stdio.h>
#include "random.h"

// Gauss
void randnf(float * i, float * q)
{
    // generate two uniform random numbers
    float u1, u2;

    // ensure u1 does not equal zero
    do {
        u1 = randf();
    } while (u1 == 0.0f);

    u2 = randf();

    float x = sqrtf(-2*logf(u1));
    *i = x * sinf(2*M_PI*u2);
    *q = x * cosf(2*M_PI*u2);
}

// Complex Gauss
float complex crandnf()
{
    // generate two uniform random numbers
    float u1, u2;

    // ensure u1 does not equal zero
    do {
        u1 = randf();
    } while (u1 == 0.0f);

    u2 = randf();

    return sqrtf(-2*logf(u1)) * cexpf(2*M_PI*u2);
}

// Weibull
float rand_weibullf(float _alpha, float _beta, float _gamma)
{
#ifdef LIQUID_VALIDATE_INPUT
    // validate input
    if (_alpha <= 0) {
        printf("error: rand_weibullf(), alpha must be greater than zero\n");
        return 0.0f;
    } else if (_beta <= 0) {
        printf("error: rand_weibullf(), beta must be greater than zero\n");
        return 0.0f;
    }
#endif

    return _gamma + powf(-_beta/_alpha*logf(randf()), 1/_beta);
}

// Rice-K
float rand_ricekf(float _K, float _omega)
{
    float x, y;
    float s = sqrtf((_omega*_K)/(_K+1));
    float sig = sqrtf(0.5f*_omega/(_K+1));
    randnf(&x, &y);
    x *= sig;
    y *= sig;
    y += s;
    return sqrtf(x*x + y*y);
}


