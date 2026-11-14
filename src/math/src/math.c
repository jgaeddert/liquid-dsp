/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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

//
// Useful mathematical formulae
//
// References:
//  [Helstrom:1960] Helstrom, C. W., Statistical Theory of Signal
//      Detection. New York: Pergamon Press, 1960
//  [Helstrom:1992] Helstrom, C. W. "Computing the Generalized Marcum Q-
//      Function," IEEE Transactions on Information Theory, vol. 38, no. 4,
//      July, 1992.
//  [Proakis:2001] Proakis, J. Digital Communications. New York:
//      McGraw-Hill, 2001

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

//                    infty
// Q(z) = 1/sqrt(2 pi) int { exp(-u^2/2) du }
//                      z
//
// Q(z) = (1/2)*(1 - erf(z/sqrt(2)))
float liquid_Qf(float _z)
{
    return 0.5f * (1.0f - erff(_z*M_SQRT1_2));
}

// Marcum Q-function
// [Helstrom:1960], [Proakis:2001]
//
// Q_M(a,b) is the probability that a non-central chi-square random
// variable with 2M degrees of freedom and non-centrality a^2 exceeds
// b^2. Expanding as a Poisson-weighted mixture of central chi-square
// distributions gives
//
//                infty
//   Q_M(a,b) =    sum   w_j * G_{M+j}
//                 j=0
//
// where w_j = exp(-lambda) lambda^j / j! with lambda = a^2/2, and
//       G_n = exp(-x) sum_{i<n} x^i / i! with x = b^2/2
//
// Every term is non-negative so there is no cancellation, and terms are
// evaluated in the log domain so large arguments neither overflow nor
// underflow. The number of terms grows with lambda, roughly
// lambda + 40 sqrt(lambda), so alpha is limited to keep run time bounded.
#define MARCUMQ_ALPHA_MAX (1000.0f)
float liquid_MarcumQf(int _M,
                      float _alpha,
                      float _beta)
{
    // validate input
    if (_M < 1) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQf(), M must be at least 1");
        return 0.0f;
    } else if (isnan(_alpha) || isnan(_beta)) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQf(), alpha and beta cannot be NaN");
        return 0.0f;
    } else if (_alpha < 0) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQf(), alpha must be >= 0");
        return 0.0f;
    } else if (_alpha > MARCUMQ_ALPHA_MAX) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQf(), alpha cannot exceed %g", MARCUMQ_ALPHA_MAX);
        return 0.0f;
    } else if (_beta < 0) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQf(), beta must be >= 0");
        return 0.0f;
    }

    // special cases: Q_M(a,0) = 1, Q_M(a,infty) = 0
    if (_beta == 0)
        return 1.0f;
    if (isinf(_beta))
        return 0.0f;

    double lambda = 0.5*(double)_alpha*(double)_alpha;
    double x      = 0.5*(double)_beta *(double)_beta;
    double ln_x   = log(x);

    // initialize G = G_M
    double G = 0.0;
    unsigned int i;
    for (i=0; i<(unsigned int)_M; i++)
        G += exp(-x + i*ln_x - lgamma((double)i + 1.0));

    // special case: Q_M(0,b) = G_M (only the j=0 term has weight)
    if (lambda == 0)
        return (float)(G > 1.0 ? 1.0 : G);

    // accumulate Poisson-weighted terms; stop once the remaining terms are
    // provably negligible relative to the sum (see bound below)
    double ln_lambda = log(lambda);
    double num_terms = lambda + 40.0*sqrt(lambda) + 100.0;
    double Q = 0.0;
    unsigned int j;
    for (j=0; (double)j < num_terms; j++) {
        double w = exp(-lambda + j*ln_lambda - lgamma((double)j + 1.0));
        Q += w * G;

        // Since G <= 1, the remaining sum is at most the Poisson tail
        // P(J > j) = sum_{k>j} w_k. Past the mean the ratios w_{k+1}/w_k =
        // lambda/(k+1) are at most lambda/(j+2) < 1, so the tail is at most
        // w_{j+1} / (1 - lambda/(j+2)) = w_{j+1} (j+2)/(j+2-lambda).
        if ((double)j + 2.0 > lambda) {
            double w_next = w * lambda / ((double)j + 1.0);
            if (w_next * ((double)j + 2.0) / ((double)j + 2.0 - lambda) < 1e-16*Q)
                break;
        }

        // update G_{M+j} -> G_{M+j+1}
        unsigned int n = (unsigned int)_M + j;
        G += exp(-x + n*ln_x - lgamma((double)n + 1.0));
    }

    // clip to valid range to absorb round-off
    if (Q < 0.0) return 0.0f;
    if (Q > 1.0) return 1.0f;
    return (float)Q;
}

// Marcum Q-function (M=1)
// TODO : check this computation
// [Helstrom:1960], [Proakis:2001]
#define NUM_MARCUMQ1_ITERATIONS 64
float liquid_MarcumQ1f(float _alpha,
                       float _beta)
{
    if (_alpha <= 0) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQ1f(), alpha must be >= 0");
        return 0.0f;
    } else if (_beta < 0) {
        liquid_error(LIQUID_EIVAL,"liquid_MarcumQ1f(), beta must be >= 0");
        return 0.0f;
    }
#if 1
    // expand as:                    infty
    // Q_1(a,b) = exp(-(a^2+b^2)/2) * sum { (a/b)^k I_k(a*b) }
    //                                k=0

    float t0 = expf( -0.5f*(_alpha*_alpha + _beta*_beta) );
    float t1 = 1.0f;

    float a_div_b = _alpha / _beta;
    float a_mul_b = _alpha * _beta;

    float y = 0.0f;
    unsigned int k;
    for (k=0; k<NUM_MARCUMQ1_ITERATIONS; k++) {
        // accumulate y
        y += t1 * liquid_besselif((float)k, a_mul_b);

        // update t1
        t1 *= a_div_b;
    }

    return t0 * y;
#else
    
    // call generalized Marcum-Q function with M=1
    return liquid_MarcumQf(1, _alpha, _beta);
#endif
}

// compute sinc(x) = sin(pi*x) / (pi*x)
float sincf(float _x) {
    // _x ~ 0 approximation
    //if (fabsf(_x) < 0.01f)
    //    return expf(-lngammaf(1+_x) - lngammaf(1-_x));

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (fabsf(_x) < 0.01f)
        return cosf(M_PI*_x/2.0f)*cosf(M_PI*_x/4.0f)*cosf(M_PI*_x/8.0f);

    return sinf(M_PI*_x)/(M_PI*_x);
}

// next power of 2 : y = ceil(log2(_x))
unsigned int liquid_nextpow2(unsigned int _x)
{
    if (_x == 0) {
        liquid_error(LIQUID_EICONFIG,"liquid_nextpow2(), input must be greater than zero");
        return 0;
    }

    _x--;
    unsigned int n=0;
    while (_x > 0) {
        _x >>= 1;
        n++;
    }
    return n;
}

// (n choose k) = n! / ( k! (n-k)! )
float liquid_nchoosek(unsigned int _n, unsigned int _k)
{
    // 
    if (_k > _n) {
        liquid_error(LIQUID_EICONFIG,"liquid_nchoosek(), _k cannot exceed _n");
        return 0.0f;
    } else if (_k == 0 || _k == _n) {
        return 1.0f;
    }

    // take advantage of symmetry and take larger value
    if (_k < _n/2)
        _k = _n - _k;

    // use lngamma() function when _n is large
    if (_n > 12) {
        double t0 = lgamma((double)_n + 1.0f);
        double t1 = lgamma((double)_n - (double)_k + 1.0f);
        double t2 = lgamma((double)_k + 1.0f);

        return round(exp( t0 - t1 - t2 ));
    }

    // old method
    float rnum=1, rden=1;
    unsigned int i;
    for (i=_n; i>_k; i--)
        rnum *= i;
    for (i=1; i<=_n-_k; i++)
        rden *= i;
    return rnum / rden;
}

