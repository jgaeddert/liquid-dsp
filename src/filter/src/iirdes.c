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
// iir (infinite impulse response) filter design
//
// References
//  [Constantinides:1967] A. G. Constantinides, "Frequency
//      Transformations for Digital Filters." IEEE Electronic
//      Letters, vol. 3, no. 11, pp 487-489, 1967.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define LIQUID_IIRDES_DEBUG_PRINT 0

// Sorts array _z of complex numbers into complex conjugate pairs to
// within a tolerance. Conjugate pairs are ordered by increasing real
// component with the negative imaginary element first. All pure-real
// elements are placed at the end of the array.
//
// Example:
//      v:              liquid_cplxpair(v):
//      10 + j*3        -3 - j*4
//       5 + j*0         3 + j*4
//      -3 + j*4        10 - j*3
//      10 - j*3        10 + j*3
//       3 + j*0         3 + j*0
//      -3 + j*4         5 + j*0
//
//  _z      :   complex array (size _n)
//  _n      :   number of elements in _z
//  _tol    :   tolerance for finding complex pairs
//  _p      :   resulting pairs, pure real values of _z at end
void liquid_cplxpair(float complex * _z,
                     unsigned int _n,
                     float _tol,
                     float complex * _p)
{
    // validate input
    if (_tol < 0) {
        fprintf(stderr,"error: liquid_cplxpair(), tolerance must be positive\n");
        exit(1);
    }

    // keep track of which elements have been paired
    unsigned char paired[_n];
    memset(paired,0,sizeof(paired));
    unsigned int num_pairs=0;

    unsigned int i,j,k=0;
    for (i=0; i<_n; i++) {
        // ignore value if already paired, or if imaginary
        // component is less than tolerance
        if (paired[i] || fabsf(cimagf(_z[i])) < _tol)
            continue;

        for (j=0; j<_n; j++) {
            // ignore value if already paired, or if imaginary
            // component is less than tolerance
            if (j==i || paired[j] || fabsf(cimagf(_z[j])) < _tol)
                continue;

            if ( fabsf(cimagf(_z[i])+cimagf(_z[j])) < _tol &&
                 fabsf(crealf(_z[i])-crealf(_z[j])) < _tol )
            {
                // found complex conjugate pair
                _p[k++] = _z[i];
                _p[k++] = _z[j];
                paired[i] = 1;
                paired[j] = 1;
                num_pairs++;
                break;
            }
        }
    }
    assert(k <= _n);

    // sort through remaining unpaired values and ensure
    // they are purely real
    for (i=0; i<_n; i++) {
        if (paired[i])
            continue;

        if (cimagf(_z[i]) > _tol) {
            fprintf(stderr,"warning, liquid_cplxpair(), complex numbers cannot be paired\n");
        } else {
            _p[k++] = _z[i];
            paired[i] = 1;
        }
    }

    // clean up result
    //  * force pairs to be perfect conjugates with
    //    negative imaginary component first
    //  * complex conjugate pairs are ordered by
    //    increasing real component
    //  * pure-real elements are ordered by increasing
    //    value
    liquid_cplxpair_cleanup(_p, _n, num_pairs);
}

// post-process cleanup used with liquid_cplxpair
//
// once pairs have been identified and ordered, this method
// will clean up the result by ensuring the following:
//  * pairs are perfect conjugates
//  * pairs have negative imaginary component first
//  * pairs are ordered by increasing real component
//  * pure-real elements are ordered by increasing value
//
//  _p          :   pre-processed complex array [size: _n x 1]
//  _n          :   array length
//  _num_pairs  :   number of complex conjugate pairs
void liquid_cplxpair_cleanup(float complex * _p,
                             unsigned int _n,
                             unsigned int _num_pairs)
{
    unsigned int i;
    unsigned int j;
    float complex tmp;

    // ensure perfect conjugates, with negative imaginary
    // element coming first
    for (i=0; i<_num_pairs; i++) {
        // ensure perfect conjugates
        _p[2*i+0] = cimagf(_p[2*i]) < 0 ? _p[2*i] : conjf(_p[2*i]);
        _p[2*i+1] = conjf(_p[2*i+0]);
    }

    // sort conjugate pairs
    for (i=0; i<_num_pairs; i++) {
        for (j=_num_pairs-1; j>i; j--) {
            if ( crealf(_p[2*(j-1)]) > crealf(_p[2*j]) ) {
                // swap pairs
                tmp = _p[2*(j-1)+0];
                _p[2*(j-1)+0] = _p[2*j+0];
                _p[2*j    +0] = tmp;

                tmp = _p[2*(j-1)+1];
                _p[2*(j-1)+1] = _p[2*j+1];
                _p[2*j    +1] = tmp;
            }
        }
    }

    // sort pure-real values
    for (i=2*_num_pairs; i<_n; i++) {
        for (j=_n-1; j>i; j--) {
            if ( crealf(_p[j-1]) > crealf(_p[j]) ) {
                // swap elements
                tmp = _p[j-1];
                _p[j-1] = _p[j];
                _p[j  ] = tmp;
            }
        }
    }

}


// 
// new IIR design
//

// Compute frequency pre-warping factor.  See [Constantinides:1967]
//  _btype  :   band type (e.g. LIQUID_IIRDES_HIGHPASS)
//  _fc     :   low-pass cutoff frequency
//  _f0     :   center frequency (band-pass|stop cases only)
float iirdes_freqprewarp(liquid_iirdes_bandtype _btype,
                         float _fc,
                         float _f0)
{
    float m = 0.0f;
    if (_btype == LIQUID_IIRDES_LOWPASS) {
        // low pass
        m = tanf(M_PI * _fc);
    } else if (_btype == LIQUID_IIRDES_HIGHPASS) {
        // high pass
        m = -cosf(M_PI * _fc) / sinf(M_PI * _fc);
    } else if (_btype == LIQUID_IIRDES_BANDPASS) {
        // band pass
        m = (cosf(2*M_PI*_fc) - cosf(2*M_PI*_f0) )/ sinf(2*M_PI*_fc);
    } else if (_btype == LIQUID_IIRDES_BANDSTOP) {
        // band stop
        m = sinf(2*M_PI*_fc)/(cosf(2*M_PI*_fc) - cosf(2*M_PI*_f0));
    }
    m = fabsf(m);

    return m;
}

// convert analog zeros, poles, gain to digital zeros, poles gain
//  _za     :   analog zeros (length: _nza)
//  _nza    :   number of analog zeros
//  _pa     :   analog poles (length: _npa)
//  _npa    :   number of analog poles
//  _ka     :   nominal gain (NOTE: this does not necessarily carry over from analog gain)
//  _m      :   frequency pre-warping factor
//  _zd     :   digital zeros (length: _npa)
//  _pd     :   digital poles (length: _npa)
//  _kd     :   digital gain
//
// The filter order is characterized by the number of analog
// poles.  The analog filter may have up to _npa zeros.
// The number of digital zeros and poles is equal to _npa.
void bilinear_zpkf(float complex * _za,
                   unsigned int _nza,
                   float complex * _pa,
                   unsigned int _npa,
                   float complex _ka,
                   float _m,
                   float complex * _zd,
                   float complex * _pd,
                   float complex * _kd)
{
    unsigned int i;

    // filter order is equal to number of analog poles
    unsigned int n = _npa;
    float complex G = _ka;  // nominal gain
    for (i=0; i<n; i++) {
        // compute digital zeros (pad with -1s)
        if (i < _nza) {
            float complex zm = _za[i] * _m;
            _zd[i] = (1.0 + zm)/(1.0 - zm);
        } else {
            _zd[i] = -1.0;
        }

        // compute digital poles
        float complex pm = _pa[i] * _m;
        _pd[i] = (1.0 + pm)/(1.0 - pm);

        // compute digital gain
        G *= (1.0 - _pd[i])/(1.0 - _zd[i]);
    }
    *_kd = G;

#if LIQUID_IIRDES_DEBUG_PRINT
    // print poles and zeros
    printf("zpk_a2df() poles (discrete):\n");
    for (i=0; i<n; i++)
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_pd[i]), cimagf(_pd[i]));
    printf("zpk_a2df() zeros (discrete):\n");
    for (i=0; i<n; i++)
        printf("  zd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_zd[i]), cimagf(_zd[i]));
    printf("zpk_a2df() gain (discrete):\n");
    printf("  kd      = %12.8f + j*%12.8f\n", crealf(G), cimagf(G));
#endif
}

// convert discrete z/p/k form to transfer function form
//  _zd     :   digital zeros (length: _n)
//  _pd     :   digital poles (length: _n)
//  _n      :   filter order
//  _k      :   digital gain
//  _b      :   output numerator (length: _n+1)
//  _a      :   output denominator (length: _n+1)
void iirdes_dzpk2tff(float complex * _zd,
                     float complex * _pd,
                     unsigned int _n,
                     float complex _k,
                     float * _b,
                     float * _a)
{
    unsigned int i;
    float complex q[_n+1];

    // expand poles
    polycf_expandroots(_pd,_n,q);
    for (i=0; i<=_n; i++)
        _a[i] = crealf(q[_n-i]);

    // expand zeros
    polycf_expandroots(_zd, _n, q);
    for (i=0; i<=_n; i++)
        _b[i] = crealf(q[_n-i]*_k);
}

// converts discrete-time zero/pole/gain (zpk) recursive (iir)
// filter representation to second-order sections (sos) form
//
//  _zd     :   discrete zeros array (size _n)
//  _pd     :   discrete poles array (size _n)
//  _n      :   number of poles, zeros
//  _kd     :   gain
//
//  _B      :   output numerator matrix (size (L+r) x 3)
//  _A      :   output denominator matrix (size (L+r) x 3)
//
//  L is the number of sections in the cascade:
//      r = _n % 2
//      L = (_n - r) / 2;
void iirdes_dzpk2sosf(float complex * _zd,
                      float complex * _pd,
                      unsigned int _n,
                      float complex _kd,
                      float * _B,
                      float * _A)
{
    int i;
    float tol=1e-6f; // tolerance for conjuate pair computation

    // find/group complex conjugate pairs (poles)
    float complex zp[_n];
    liquid_cplxpair(_zd,_n,tol,zp);

    // find/group complex conjugate pairs (zeros)
    float complex pp[_n];
    liquid_cplxpair(_pd,_n,tol,pp);

    // TODO : group pole pairs with zero pairs

    // _n = 2*L + r
    unsigned int r = _n % 2;        // odd/even order
    unsigned int L = (_n - r)/2;    // filter semi-length

#if LIQUID_IIRDES_DEBUG_PRINT
    printf("  n=%u, r=%u, L=%u\n", _n, r, L);
    printf("poles :\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(_pd[i]), cimagf(_pd[i]));
    printf("zeros :\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(_zd[i]), cimagf(_zd[i]));

    printf("poles (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(pp[i]), cimagf(pp[i]));
    printf("zeros (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(zp[i]), cimagf(zp[i]));
#endif

    float complex z0, z1;
    float complex p0, p1;
    for (i=0; i<L; i++) {
        p0 = -pp[2*i+0];
        p1 = -pp[2*i+1];

        z0 = -zp[2*i+0];
        z1 = -zp[2*i+1];

        // expand complex pole pairs
        _A[3*i+0] = 1.0;
        _A[3*i+1] = crealf(p0+p1);
        _A[3*i+2] = crealf(p0*p1);

        // expand complex zero pairs
        _B[3*i+0] = 1.0;
        _B[3*i+1] = crealf(z0+z1);
        _B[3*i+2] = crealf(z0*z1);
    }

    // add remaining zero/pole pair if order is odd
    if (r) {
        _A[3*i+0] =  1.0;
        _A[3*i+1] = -pp[_n-1];
        _A[3*i+2] =  0.0;

        _B[3*i+0] =  1.0;
        _B[3*i+1] = -zp[_n-1];
        _B[3*i+2] =  0.0;
    }

    // distribute gain equally amongst all feed-forward
    // coefficients
    float k = powf( crealf(_kd), 1.0f/(float)(L+r) );

    // adjust gain of first element
    for (i=0; i<L+r; i++) {
        _B[3*i+0] *= k;
        _B[3*i+1] *= k;
        _B[3*i+2] *= k;
    }
}

// digital z/p/k low-pass to high-pass transformation
//  _zd     :   digital zeros (low-pass prototype)
//  _pd     :   digital poles (low-pass prototype)
//  _n      :   low-pass filter order
//  _zdt    :   digital zeros transformed [length: _n]
//  _pdt    :   digital poles transformed [length: _n]
void iirdes_dzpk_lp2hp(liquid_float_complex * _zd,
                       liquid_float_complex * _pd,
                       unsigned int _n,
                       liquid_float_complex * _zdt,
                       liquid_float_complex * _pdt)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        _zdt[i] = -_zd[i];
        _pdt[i] = -_pd[i];
    }
}


// digital z/p/k low-pass to band-pass transformation
//  _zd     :   digital zeros (low-pass prototype)
//  _pd     :   digital poles (low-pass prototype)
//  _n      :   low-pass filter order
//  _f0     :   center frequency
//  _zdt    :   digital zeros transformed [length: 2*_n]
//  _pdt    :   digital poles transformed [length: 2*_n]
void iirdes_dzpk_lp2bp(liquid_float_complex * _zd,
                       liquid_float_complex * _pd,
                       unsigned int _n,
                       float _f0,
                       liquid_float_complex * _zdt,
                       liquid_float_complex * _pdt)
{
    // 
    float c0 = cosf(2*M_PI*_f0);

    // transform zeros, poles using quadratic formula
    unsigned int i;
    float complex t0;
    for (i=0; i<_n; i++) {
        t0 = 1 + _zd[i];
        _zdt[2*i+0] = 0.5f*(c0*t0 + csqrtf(c0*c0*t0*t0 - 4*_zd[i]));
        _zdt[2*i+1] = 0.5f*(c0*t0 - csqrtf(c0*c0*t0*t0 - 4*_zd[i]));

        t0 = 1 + _pd[i];
        _pdt[2*i+0] = 0.5f*(c0*t0 + csqrtf(c0*c0*t0*t0 - 4*_pd[i]));
        _pdt[2*i+1] = 0.5f*(c0*t0 - csqrtf(c0*c0*t0*t0 - 4*_pd[i]));
    }
}

// IIR filter design template
//  _ftype      :   filter type (e.g. LIQUID_IIRDES_BUTTER)
//  _btype      :   band type (e.g. LIQUID_IIRDES_BANDPASS)
//  _format     :   coefficients format (e.g. LIQUID_IIRDES_SOS)
//  _n          :   filter order
//  _fc         :   low-pass prototype cut-off frequency
//  _f0         :   center frequency (band-pass, band-stop)
//  _Ap         :   pass-band ripple in dB
//  _As         :   stop-band ripple in dB
//  _B          :   numerator
//  _A          :   denominator
void liquid_iirdes(liquid_iirdes_filtertype _ftype,
                   liquid_iirdes_bandtype   _btype,
                   liquid_iirdes_format     _format,
                   unsigned int _n,
                   float _fc,
                   float _f0,
                   float _Ap,
                   float _As,
                   float * _B,
                   float * _A)
{
    // validate input
    if (_fc <= 0 || _fc >= 0.5) {
        fprintf(stderr,"error: liquid_iirdes(), cutoff frequency out of range\n");
        exit(1);
    } else if (_f0 < 0 || _f0 > 0.5) {
        fprintf(stderr,"error: liquid_iirdes(), center frequency out of range\n");
        exit(1);
    } else if (_Ap <= 0) {
        fprintf(stderr,"error: liquid_iirdes(), pass-band ripple out of range\n");
        exit(1);
    } else if (_As <= 0) {
        fprintf(stderr,"error: liquid_iirdes(), stop-band ripple out of range\n");
        exit(1);
    } else if (_n == 0) {
        fprintf(stderr,"error: liquid_iirdes(), filter order must be > 0\n");
        exit(1);
    }

    // number of analaog poles/zeros
    unsigned int npa = _n;
    unsigned int nza;

    // analog poles/zeros/gain
    float complex pa[_n];
    float complex za[_n];
    float complex ka;
    float complex k0 = 1.0f; // nominal digital gain

    // derived values
    unsigned int r = _n%2;      // odd/even filter order
    unsigned int L = (_n-r)/2;  // filter semi-length

    // specific filter variables
    float epsilon, Gp, Gs, ep, es;

    // compute zeros and poles of analog prototype
    switch (_ftype) {
    case LIQUID_IIRDES_BUTTER:
        // Butterworth filter design : no zeros, _n poles
        nza = 0;
        k0 = 1.0f;
        butter_azpkf(_n,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY1:
        // Cheby-I filter design : no zeros, _n poles, pass-band ripple
        nza = 0;
        epsilon = sqrtf( powf(10.0f, _Ap / 10.0f) - 1.0f );
        k0 = r ? 1.0f : 1.0f / sqrt(1.0f + epsilon*epsilon);
        cheby1_azpkf(_n,epsilon,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY2:
        // Cheby-II filter design : _n-r zeros, _n poles, stop-band ripple
        nza = 2*L;
        epsilon = powf(10.0f, -_As/20.0f);
        k0 = 1.0f;
        cheby2_azpkf(_n,epsilon,za,pa,&ka);
        break;
    case LIQUID_IIRDES_ELLIP:
        // elliptic filter design : _n-r zeros, _n poles, pass/stop-band ripple
        nza = 2*L;
        Gp = powf(10.0f, -_Ap / 20.0f);     // pass-band gain
        Gs = powf(10.0f, -_As / 20.0f);     // stop-band gain
        ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);    // pass-band epsilon
        es = sqrtf(1.0f/(Gs*Gs) - 1.0f);    // stop-band epsilon
        k0 = r ? 1.0f : 1.0f / sqrt(1.0f + ep*ep);
        ellip_azpkf(_n,ep,es,za,pa,&ka);
        break;
    case LIQUID_IIRDES_BESSEL:
        // Bessel filter design : no zeros, _n poles
        nza = 0;
        k0 = 1.0f;
        bessel_azpkf(_n,za,pa,&ka);
        break;
    default:
        fprintf(stderr,"error: liquid_iirdes(), unknown filter type\n");
        exit(1);
    }

#if LIQUID_IIRDES_DEBUG_PRINT
    unsigned int i;

    printf("poles (analog):\n");
    for (i=0; i<npa; i++)
        printf("  pa[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
    printf("zeros (analog):\n");
    for (i=0; i<nza; i++)
        printf("  za[%3u] = %12.8f + j*%12.8f\n", i, crealf(za[i]), cimagf(za[i]));
    printf("gain (analog):\n");
    printf("  ka : %12.8f + j*%12.8f\n", crealf(ka), cimagf(ka));
#endif

    // complex digital poles/zeros/gain
    // NOTE: allocated double the filter order to cover band-pass, band-stop cases
    float complex zd[2*_n];
    float complex pd[2*_n];
    float complex kd;
    float m = iirdes_freqprewarp(_btype,_fc,_f0);
    //printf("m : %12.8f\n", m);
    bilinear_zpkf(za,    nza,
                  pa,    npa,
                  k0,    m,
                  zd, pd, &kd);

#if LIQUID_IIRDES_DEBUG_PRINT
    printf("zeros (digital, low-pass prototype):\n");
    for (i=0; i<_n; i++)
        printf("  zd[%3u] = %12.4e + j*%12.4e;\n", i, crealf(zd[i]), cimagf(zd[i]));
    printf("poles (digital, low-pass prototype):\n");
    for (i=0; i<_n; i++)
        printf("  pd[%3u] = %12.4e + j*%12.4e;\n", i, crealf(pd[i]), cimagf(pd[i]));
    printf("gain (digital):\n");
    printf("  kd : %12.8f + j*%12.8f\n", crealf(kd), cimagf(kd));
#endif

    // negate zeros, poles for high-pass and band-stop cases
    if (_btype == LIQUID_IIRDES_HIGHPASS ||
        _btype == LIQUID_IIRDES_BANDSTOP)
    {
        // run transform, place resulting zeros, poles
        // back in same original arrays
        iirdes_dzpk_lp2hp(zd, pd, _n, zd, pd);
    }

    // transform zeros, poles in band-pass, band-stop cases
    // NOTE: this also doubles the filter order
    if (_btype == LIQUID_IIRDES_BANDPASS ||
        _btype == LIQUID_IIRDES_BANDSTOP)
    {
        // allocate memory for transformed zeros, poles
        float complex zd1[2*_n];
        float complex pd1[2*_n];

        // run zeros, poles low-pass -> band-pass trasform
        iirdes_dzpk_lp2bp(zd, pd,   // low-pass prototype zeros, poles
                          _n,       // filter order
                          _f0,      // center frequency
                          zd1,pd1); // transformed zeros, poles (length: 2*n)

        // copy transformed zeros, poles
        memmove(zd, zd1, 2*_n*sizeof(float complex));
        memmove(pd, pd1, 2*_n*sizeof(float complex));

        // update paramters : n -> 2*n
#if LIQUID_IIRDES_DEBUG_PRINT
        r = 0;
        L = _n;
#endif
        _n = 2*_n;
    }

    if (_format == LIQUID_IIRDES_TF) {
        // convert complex digital poles/zeros/gain into transfer
        // function : H(z) = B(z) / A(z)
        // where length(B,A) = low/high-pass ? _n + 1 : 2*_n + 1
        iirdes_dzpk2tff(zd,pd,_n,kd,_B,_A);

#if LIQUID_IIRDES_DEBUG_PRINT
        // print coefficients
        for (i=0; i<=_n; i++) printf("b[%3u] = %12.8f;\n", i, _B[i]);
        for (i=0; i<=_n; i++) printf("a[%3u] = %12.8f;\n", i, _A[i]);
#endif
    } else {
        // convert complex digital poles/zeros/gain into second-
        // order sections form :
        // H(z) = prod { (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2) }
        // where size(B,A) = low|high-pass  : [3]x[L+r]
        //                   band-pass|stop : [3]x[2*L]
        iirdes_dzpk2sosf(zd,pd,_n,kd,_B,_A);

#if LIQUID_IIRDES_DEBUG_PRINT
        // print coefficients
        printf("B [%u x 3] :\n", L+r);
        for (i=0; i<L+r; i++)
            printf("  %12.8f %12.8f %12.8f\n", _B[3*i+0], _B[3*i+1], _B[3*i+2]);
        printf("A [%u x 3] :\n", L+r);
        for (i=0; i<L+r; i++)
            printf("  %12.8f %12.8f %12.8f\n", _A[3*i+0], _A[3*i+1], _A[3*i+2]);
#endif

    }
}

// checks stability of iir filter
//  _b      :   feed-forward coefficients [size: _n x 1]
//  _a      :   feed-back coefficients [size: _n x 1]
//  _n      :   number of coefficients
int iirdes_isstable(float * _b,
                    float * _a,
                    unsigned int _n)
{
    // validate input
    if (_n < 2) {
        fprintf(stderr,"error: iirdes_isstable(), filter order too low\n");
        exit(1);
    }
    unsigned int i;

    // flip denominator, left to right
    float a_hat[_n];
    for (i=0; i<_n; i++)
        a_hat[i] = _a[_n-i-1];

    // compute poles (roots of denominator)
    float complex roots[_n-1];
    polyf_findroots_bairstow(a_hat, _n, roots);

#if 0
    // print roots
    printf("\nroots:\n");
    for (i=0; i<_n-1; i++)
        printf("  r[%3u] = %12.8f + j *%12.8f\n", i, crealf(roots[i]), cimagf(roots[i]));
#endif

    // compute magnitude of poles
    for (i=0; i<_n-1; i++) {
        if (cabsf(roots[i]) > 1.0)
            return 0;
    }

    return 1;
}


