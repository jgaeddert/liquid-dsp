//
// Random number generators
//

#ifndef __LIQUID_RANDOM_H__
#define __LIQUID_RANDOM_H__

#include <stdlib.h>
#include <complex.h>

// Uniform random number generator, (0,1]
#define randf() ((float) rand() / (float) RAND_MAX)

// Gaussian random number generator, N(0,1)
float randnf();
float complex crandnf();
float randn_pdf(float _x, float _eta, float _sig);
float randn_cdf(float _x, float _eta, float _sig);

// Weibull
//   f(x) = a*(x-g)^(b-1)*exp{-(a/b)*(x-g)^b}  x >= g
//          0                                  else
//   where
//     a = alpha : scaling parameter
//     b = beta  : shape parameter
//     g = gamma : location (threshold) parameter
//
float rand_weibullf(float _alpha, float _beta, float _gamma);
float rand_pdf_weibullf(float _x, float _a, float _b, float _g);
float rand_cdf_weibullf(float _x, float _a, float _b, float _g);

// Gamma
void rand_gammaf();

// Nakagami-m
void rand_nakagamimf(float _m, float _omega);
float rand_pdf_nakagamimf(float _x, float _m, float _omega);
float rand_cdf_nakagamimf(float _x, float _m, float _omega);

// Rice-K
float rand_ricekf(float _K, float _omega);
float rand_pdf_ricekf(float _x, float _K, float _omega);
float rand_cdf_ricekf(float _x, float _K, float _omega);

#endif /* __LIQUID_RANDOM_H__ */

