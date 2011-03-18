/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
 *                                      Institute & State University
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

//
// Useful mathematical formulae
//
// References:
//  [Kaiser:1980] James F. Kaiser and Ronald W. Schafer, "On
//      the Use of I0-Sinh Window for Spectrum Analysis,"
//      IEEE Transactions on Acoustics, Speech, and Signal
//      Processing, vol. ASSP-28, no. 1, pp. 105--107,
//      February, 1980.
//  [harris:1978] frederic j. harris, "On the Use of Windows for Harmonic
//      Analysis with the Discrete Fourier Transform," Proceedings of the
//      IEEE, vol. 66, no. 1, January, 1978.
//  [Helstrom:1960] Helstrom, C. W. Statistical Theory of Signal
//      Detection. New York: Pergamon, 1960
//  [Proakis:2001] Proakis, J. Digital Communications. New York:
//      McGraw-Hill, 2001

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define NUM_LNGAMMA_ITERATIONS (64)
#define EULER_GAMMA            (0.57721566490153286)
float liquid_lngammaf(float _z)
{
    float g;
    if (_z < 0) {
        fprintf(stderr,"error: liquid_lngammaf(), undefined for z <= 0\n");
        exit(1);
    } else if (_z < 0.60) {
        // low value approximation
        g = -logf(_z) - EULER_GAMMA*_z;
        unsigned int n;
        float z_by_n;   // value of z/n
        for (n=1; n<NUM_LNGAMMA_ITERATIONS; n++) {
            z_by_n = _z / (float)n;
            g += -logf(1.0f + z_by_n) + z_by_n;
        }
    } else {
        // high value approximation
        g = 0.5*( logf(2*M_PI)-log(_z) );
        g += _z*( logf(_z+(1/(12.0f*_z-0.1f/_z)))-1);
    }
    return g;
}

float liquid_gammaf(float _z)
{
    if (_z < 0) {
        // use identities
        //  (1) gamma(z)*gamma(-z) = -pi / (z*sin(pi*z))
        //  (2) z*gamma(z) = gamma(1+z)
        //
        // therefore:
        //  gamma(z) = pi / ( gamma(1-z) * sin(pi*z) )
        float t0 = liquid_gammaf(1.0 - _z);
        float t1 = sinf(M_PI*_z);
        if (t0==0 || t1==0)
            fprintf(stderr,"warning: liquid_gammaf(), divide by zero\n");
        return M_PI / (t0 * t1);
    } else {
        return expf( liquid_lngammaf(_z) );
    }
}

float liquid_factorialf(unsigned int _n) {
    return fabsf(liquid_gammaf((float)(_n+1)));
}

// Bessel function of the first kind
#define NUM_BESSELJ0_ITERATIONS 16
float besselj_0(float _z)
{
    // large signal approximation, see
    // Gross, F. B "New Approximations to J0 and J1 Bessel Functions,"
    //   IEEE Trans. on Antennas and Propagation, vol. 43, no. 8,
    //   August, 1995
    if (fabsf(_z) > 10.0f)
        return sqrtf(2/(M_PI*fabsf(_z)))*cosf(fabsf(_z)-M_PI/4);

    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELJ0_ITERATIONS; k++) {
        t = powf(_z/2, (float)k) / tgamma((float)k+1);
        y += (k%2) ? -t*t : t*t;
    }
    return y;
}

// Modified bessel function of the first kind
#define NUM_BESSELI0_ITERATIONS 16
float besseli_0(float _z)
{
    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELI0_ITERATIONS; k++) {
        t = powf(_z/2, (float)k) / tgamma((float)k+1);
        y += t*t;
    }
    return y;
}

// Modified bessel function of the first kind
// TODO : check this computation
#define NUM_BESSELI_ITERATIONS 16
float besseli(float _nu,
              float _z)
{
    unsigned int k;
    float t0 = powf(_z*0.5f, _nu);
    float t1 = 1.0f;
    float y=0.0f;
    for (k=0; k<NUM_BESSELI_ITERATIONS; k++) {
        // compute: k! * Gamma(nu + k +1)
        float t2 = liquid_factorialf(k) * liquid_gammaf(_nu + (float)k + 1.0f);

        // accumulate y
        y += t1 / t2;

        // update t1 = (0.25*z^2)^k
        t1 *= 0.25f*_z*_z;
    }
    return t0 * y;
}

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
// TODO : check this computation
// [Helstrom:1960], [Proakis:2001]
#define NUM_MARCUMQ_ITERATIONS 16
float liquid_MarcumQ(int _M,
                     float _alpha,
                     float _beta)
{
    // expand as:
    //                               infty
    // Q_M(a,b) = exp(-(a^2+b^2)/2) * sum { (a/b)^k I_k(a*b) }
    //                               k=1-M

    return 0.0f;
}

// Marcum Q-function (M=1)
// TODO : check this computation
// [Helstrom:1960], [Proakis:2001]
#define NUM_MARCUMQ1_ITERATIONS 16
float liquid_MarcumQ1(float _alpha,
                      float _beta)
{
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
        y += t1 * besseli((float)k, a_mul_b);

        // update t1
        t1 *= a_div_b;
    }

    return t0 * y;
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
        fprintf(stderr,"error: liquid_nextpow2(), input must be greater than zero\n");
        exit(1);
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
        fprintf(stderr,"error: liquid_nchoosek(), _k cannot exceed _n\n");
        exit(1);
    } else if (_k == 0 || _k == _n) {
        return 1;
    }

    // take advantage of symmetry and take larger value
    if (_k < _n/2)
        _k = _n - _k;

    // use lngamma() function when _n is large
    if (_n > 12) {
        float t0 = liquid_lngammaf(_n + 1.0f);
        float t1 = liquid_lngammaf(_n - _k + 1.0f);
        float t2 = liquid_lngammaf(_k + 1.0f);

        return roundf(expf( t0 - t1 - t2 ));
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

// 
// Windowing functions
//

// Kaiser-Bessel derived window
float liquid_kbd(unsigned int _n,
                 unsigned int _N,
                 float _beta)
{
    // TODO add reference

    // validate input
    if (_n >= _N) {
        fprintf(stderr,"error: liquid_kbd(), index exceeds maximum\n");
        exit(1);
    } else if (_N == 0) {
        fprintf(stderr,"error: liquid_kbd(), window length must be greater than zero\n");
        exit(1);
    } else if ( _N % 2 ) {
        fprintf(stderr,"error: liquid_kbd(), window length must be odd\n");
        exit(1);
    }

    unsigned int M = _N / 2;
    if (_n >= M)
        return liquid_kbd(_N-_n-1,_N,_beta);

    float w0 = 0.0f;
    float w1 = 0.0f;
    float w;
    unsigned int i;
    for (i=0; i<=M; i++) {
        // compute Kaiser window
        w = kaiser(i,M+1,_beta,0.0f);

        // accumulate window sums
        w1 += w;
        if (i <= _n) w0 += w;
    }
    //printf("%12.8f / %12.8f = %12.8f\n", w0, w1, w0/w1);

    return sqrtf(w0 / w1);
}


// Kaiser-Bessel derived window (full window function)
void liquid_kbd_window(unsigned int _n,
                       float _beta,
                       float * _w)
{
    unsigned int i;
    // TODO add reference

    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: liquid_kbd_window(), window length must be greater than zero\n");
        exit(1);
    } else if ( _n % 2 ) {
        fprintf(stderr,"error: liquid_kbd_window(), window length must be odd\n");
        exit(1);
    } else if ( _beta < 0.0f ) {
        fprintf(stderr,"error: liquid_kbd_window(), _beta must be positive\n");
        exit(1);
    }

    // compute half length
    unsigned int M = _n / 2;

    // generate regular Kaiser window, length M+1
    float w_kaiser[M+1];
    for (i=0; i<=M; i++)
        w_kaiser[i] = kaiser(i,M+1,_beta,0.0f);

    // compute sum(wk[])
    float w_sum = 0.0f;
    for (i=0; i<=M; i++)
        w_sum += w_kaiser[i];

    // accumulate window
    float w_acc = 0.0f;
    for (i=0; i<M; i++) {
        w_acc += w_kaiser[i];
        _w[i] = sqrtf(w_acc / w_sum);
    }

    // window has even symmetry; flip around index M
    for (i=0; i<M; i++)
        _w[_n-i-1] = _w[i];
}


// Kaiser window [Kaiser:1980]
//  _n      :   sample index
//  _N      :   window length (samples)
//  _beta   :   window taper parameter
//  _mu     :   fractional sample offset
float kaiser(unsigned int _n,
             unsigned int _N,
             float _beta,
             float _mu)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: kaiser(), sample index must not exceed window length\n");
        exit(1);
    } else if (_beta < 0) {
        fprintf(stderr,"error: kaiser(), beta must be greater than or equal to zero\n");
        exit(1);
    } else if (_mu < -1 || _mu > 1) {
        fprintf(stderr,"error: kaiser(), fractional sample offset must be in [-1,1]\n");
        exit(1);
    }

    float t = (float)_n - (float)(_N-1)/2 + _mu;
    float r = 2.0f*t/(float)(_N);
    float a = besseli_0(_beta*sqrtf(1-r*r));
    float b = besseli_0(_beta);
    return a / b;
}

// Hamming window
float hamming(unsigned int _n, unsigned int _N)
{
    // TODO add reference
    return 0.53836 - 0.46164*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Hann window
float hann(unsigned int _n, unsigned int _N)
{
    // TODO test this function
    // TODO add reference
    return 0.5f - 0.5f*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Blackman-harris window [harris:1978]
float blackmanharris(unsigned int _n, unsigned int _N)
{
    // TODO test this function
    // TODO add reference
    float a0 = 0.35875;
    float a1 = 0.48829;
    float a2 = 0.14128;
    float a3 = 0.01168;
    float t = 2*M_PI*(float)_n / ((float)(_N-1));

    return a0 - a1*cosf(t) + a2*cosf(2*t) - a3*cosf(3*t);
}


