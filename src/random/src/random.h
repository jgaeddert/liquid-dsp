//
// Random number generators
//

#ifndef __LIQUID_RANDOM_H__
#define __LIQUID_RANDOM_H__

#include <stdlib.h>

// Uniform random number generator, (0,1]
#define randf() ((float) rand() / (float) RAND_MAX)

// Gaussian random number generator, N(0,1)
void randnf(float * i, float * q);

// Weibull
//   f(x) = a*(x-g)^(b-1)*exp{-(a/b)*(x-g)^b}  x >= g
//          0                                  else
//   where
//     a = alpha : scaling parameter
//     b = beta  : shape parameter
//     g = gamma : location (threshold) parameter
//
float rand_weibullf(float _alpha, float _beta, float _gamma);

// Gamma
void rand_gammaf();

// Nakagami-m
void rand_nakagamimf(float _m, float _omega);

// Rice-K
float rand_ricekf(float _K, float _omega);

#endif /* __LIQUID_RANDOM_H__ */

