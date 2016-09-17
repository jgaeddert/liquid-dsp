/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
//  [Kaiser:1980] James F. Kaiser and Ronald W. Schafer, "On
//      the Use of I0-Sinh Window for Spectrum Analysis,"
//      IEEE Transactions on Acoustics, Speech, and Signal
//      Processing, vol. ASSP-28, no. 1, pp. 105--107,
//      February, 1980.
//  [harris:1978] frederic j. harris, "On the Use of Windows for Harmonic
//      Analysis with the Discrete Fourier Transform," Proceedings of the
//      IEEE, vol. 66, no. 1, January, 1978.
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
// TODO : check this computation
// [Helstrom:1960], [Proakis:2001], [Helstrom:1992]
#define NUM_MARCUMQ_ITERATIONS 16
float liquid_MarcumQf(int _M,
                      float _alpha,
                      float _beta)
{
#if 0
    // expand as:
    //                               infty
    // Q_M(a,b) = exp(-(a^2+b^2)/2) * sum { (a/b)^k I_k(a*b) }
    //                               k=1-M
    return 0.0f
#else

    // use approximation [Helstrom:1992] (Eq. 25)
    // Q_M(a,b) ~ erfc(x),
    //   x = (b-a-M)/sigma^2,
    //   sigma = M + 2a

    // compute sigma
    float sigma = (float)(_M) + 2.0f*_alpha;

    // compute x
    float x = (_beta - _alpha - (float)_M) / (sigma*sigma);

    // return erfc(x)
    return erfcf(x);
#endif
}

// Marcum Q-function (M=1)
// TODO : check this computation
// [Helstrom:1960], [Proakis:2001]
#define NUM_MARCUMQ1_ITERATIONS 64
float liquid_MarcumQ1f(float _alpha,
                       float _beta)
{
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
             float        _beta,
             float        _mu)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: kaiser(), sample index must not exceed window length\n");
        exit(1);
    } else if (_beta < 0) {
        fprintf(stderr,"error: kaiser(), beta must be greater than or equal to zero\n");
        exit(1);
    } else if (_mu < -0.5 || _mu > 0.5) {
        fprintf(stderr,"error: kaiser(), fractional sample offset must be in [-0.5,0.5]\n");
        exit(1);
    }

    float t = (float)_n - (float)(_N-1)/2 + _mu;
    float r = 2.0f*t/(float)(_N);
    float a = liquid_besseli0f(_beta*sqrtf(1-r*r));
    float b = liquid_besseli0f(_beta);
    return a / b;
}

// Hamming window
float hamming(unsigned int _n,
              unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: hamming(), sample index must not exceed window length\n");
        exit(1);
    }

    // TODO add reference
    return 0.53836 - 0.46164*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Hann window
float hann(unsigned int _n,
           unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: hann(), sample index must not exceed window length\n");
        exit(1);
    }

    // TODO test this function
    // TODO add reference
    return 0.5f - 0.5f*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Blackman-harris window [harris:1978]
float blackmanharris(unsigned int _n,
                     unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: blackmanharris(), sample index must not exceed window length\n");
        exit(1);
    }

    // TODO test this function
    // TODO add reference
    float a0 = 0.35875f;
    float a1 = 0.48829f;
    float a2 = 0.14128f;
    float a3 = 0.01168f;
    float t = 2*M_PI*(float)_n / ((float)(_N-1));

    return a0 - a1*cosf(t) + a2*cosf(2*t) - a3*cosf(3*t);
}

// 7th-order Blackman-harris window
float blackmanharris7(unsigned int _n, unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: blackmanharris7(), sample index must not exceed window length\n");
        exit(1);
    }

	float a0 = 0.27105f;
	float a1 = 0.43329f;
	float a2 = 0.21812f;
	float a3 = 0.06592f;
	float a4 = 0.01081f;
	float a5 = 0.00077f;
	float a6 = 0.00001f;
	float t = 2*M_PI*(float)_n / ((float)(_N-1));

	return a0 - a1*cosf(  t) + a2*cosf(2*t) - a3*cosf(3*t)
			  + a4*cosf(4*t) - a5*cosf(5*t) + a6*cosf(6*t);
}

// Flat-top window
float flattop(unsigned int _n, unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: flattop(), sample index must not exceed window length\n");
        exit(1);
    }

	float a0 = 1.000f;
	float a1 = 1.930f;
	float a2 = 1.290f;
	float a3 = 0.388f;
	float a4 = 0.028f;
	float t = 2*M_PI*(float)_n / ((float)(_N-1));

	return a0 - a1*cosf(t) + a2*cosf(2*t) - a3*cosf(3*t) + a4*cosf(4*t);
}

// Triangular window
float triangular(unsigned int _n,
                 unsigned int _N,
                 unsigned int _L)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: triangular(), sample index must not exceed window length\n");
        exit(1);
    } else if (_L != _N-1 && _L != _N && _L != _N+1) {
        fprintf(stderr,"error: triangular(), sub-length must be in _N+{-1,0,1}\n");
        exit(1);
    } else if (_L == 0) {
        fprintf(stderr,"error: triangular(), sub-length must be greater than zero\n");
        exit(1);
    }

	float _num   = (float)_n - (float)((_N-1)/2.0f);
	float _denom = ((float)_L)/2.0f;
	return 1.0 - fabsf(_num / _denom);
}

// raised-cosine tapering window
//  _n      :   window index
//  _t      :   taper length
//  _N      :   full window length
float liquid_rcostaper_windowf(unsigned int _n,
                               unsigned int _t,
                               unsigned int _N)
{
    // validate input
    if (_n > _N) {
        fprintf(stderr,"error: liquid_rcostaper_windowf(), sample index must not exceed window length\n");
        exit(1);
    } else if (_t > _N/2) {
        fprintf(stderr,"error: liquid_rcostaper_windowf(), taper length cannot exceed half window length\n");
        exit(1);
    }

    // reverse time for ramp-down section
    if (_n > _N - _t - 1)
        _n = _N - _n - 1;

    // return ramp or flat component
    return (_n < _t) ? 0.5f - 0.5f*cosf(M_PI*((float)_n + 0.5f) / (float)_t) : 1.0f;
}


