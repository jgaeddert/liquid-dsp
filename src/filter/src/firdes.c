/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// Finite impulse response filter design
//
// References:
//  [Herrmann:1973] O. Herrmann, L. R. Rabiner, and D. S. K. Chan,
//      "Practical design rules for optimum finite impulse response
//      lowpass digital filters," Bell Syst. Tech. Journal, vol. 52,
//      pp. 769--99, July-Aug. 1973
//  [Vaidyanathan:1993] Vaidyanathan, P. P., "Multirate Systems and
//      Filter Banks," 1993, Prentice Hall, Section 3.2.1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// method to estimate required filter length
#define ESTIMATE_REQ_FILTER_LEN_METHOD_KAISER   (0)
#define ESTIMATE_REQ_FILTER_LEN_METHOD_HERRMANN (1)

#define ESTIMATE_REQ_FILTER_LEN_METHOD ESTIMATE_REQ_FILTER_LEN_METHOD_KAISER

// esimate required filter length given transition bandwidth and
// sidelobe suppression level
//  _df     :   transition bandwidth (0 < _df < 0.5)
//  _As     :   stopband suppression level [dB] (_As > 0)
unsigned int estimate_req_filter_len(float _df,
                                     float _As)
{
    if (_df > 0.5f || _df <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len(), invalid bandwidth : %f\n", _df);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len(), invalid stopband level : %f\n", _As);
        exit(1);
    }

    // compute filter length estimate
#if ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_KAISER
    // use Kaiser's estimate
    unsigned int h_len = (unsigned int) estimate_req_filter_len_Kaiser(_df,_As);
#elif ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_HERRMANN
    // use Herrmann's estimate
    unsigned int h_len = (unsigned int) estimate_req_filter_len_Herrmann(_df,_As);
#else
#   error "invalid required filter length estimation method"
#endif
    
    return h_len;
}

// estimate filter stop-band attenuation given
//  _df     :   transition bandwidth (0 < _b < 0.5)
//  _N      :   filter length
float estimate_req_filter_As(float _df,
                             unsigned int _N)
{
    // run search for stop-band attenuation which gives these results
    float As0   = 0.01f;    // lower bound
    float As1   = 200.0f;   // upper bound

    float As_hat = 0.0f;    // stop-band attenuation estimate
    float N_hat = 0.0f;     // filter length estimate

    // perform simple bisection search
    unsigned int num_iterations = 20;
    unsigned int i;
    for (i=0; i<num_iterations; i++) {
        // bisect limits
        As_hat = 0.5f*(As1 + As0);
#if ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_KAISER
        N_hat = estimate_req_filter_len_Kaiser(_df, As_hat);
#elif ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_HERRMANN
        N_hat = estimate_req_filter_len_Herrmann(_df, As_hat);
#else
#       error "invalid required filter length estimation method"
#endif

        //printf("range[%8.2f, %8.2f] As-hat=%8.2fdB, N=%8.2f (target: %3u taps)\n",
        //        As0, As1, As_hat, N_hat, _N);

        // update limits
        if (N_hat < (float)_N) {
            As0 = As_hat;
        } else {
            As1 = As_hat;
        }
    }
    return As_hat;
}

// estimate filter transition bandwidth given
//  _As     :   sidelobe suppression level [dB] (As > 0)
//  _N      :   filter length
float estimate_req_filter_df(float _As,
                             unsigned int _N)
{
    // run search for stop-band attenuation which gives these results
    float df0   = 1e-3f;    // lower bound
    float df1   = 0.499f;   // upper bound

    float df_hat = 0.0f;    // stop-band attenuation estimate
    float N_hat = 0.0f;     // filter length estimate

    // perform simple bisection search
    unsigned int num_iterations = 20;
    unsigned int i;
    for (i=0; i<num_iterations; i++) {
        // bisect limits
        df_hat = 0.5f*(df1 + df0);
#if ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_KAISER
        N_hat = estimate_req_filter_len_Kaiser(df_hat, _As);
#elif ESTIMATE_REQ_FILTER_LEN_METHOD == ESTIMATE_REQ_FILTER_LEN_METHOD_HERRMANN
        N_hat = estimate_req_filter_len_Herrmann(df_hat, _As);
#else
#       error "invalid required filter length estimation method"
#endif

        printf("range[%8.5f, %8.5f] df-hat=%8.5fdB, N=%8.2f (target: %3u taps)\n",
                df0, df1, df_hat, N_hat, _N);

        // update limits
        if (N_hat < (float)_N) {
            df1 = df_hat;
        } else {
            df0 = df_hat;
        }
    }
    return df_hat;

}


// esimate required filter length given transition bandwidth and
// sidelobe suppression level (algorithm from [Vaidyanathan:1993])
//  _df     :   transition bandwidth (0 < _df < 0.5)
//  _As     :   stop-band attenuation [dB] (As > 0)
float estimate_req_filter_len_Kaiser(float _df,
                                     float _As)
{
    if (_df > 0.5f || _df <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len_Kaiser(), invalid bandwidth : %f\n", _df);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len(), invalid stopband level : %f\n", _As);
        exit(1);
    }

    // compute filter length estimate
    return (_As - 7.95f)/(14.36f*_df) + 1.0f;
}


// esimate required filter length given transition bandwidth and
// sidelobe suppression level (algorithm from [Herrmann:1973])
//  _df     :   transition bandwidth (0 < _df < 0.5)
//  _As     :   stop-band attenuation [dB] (As > 0)
float estimate_req_filter_len_Herrmann(float _df,
                                       float _As)
{
    if (_df > 0.5f || _df <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len_Herrmann(), invalid bandwidth : %f\n", _df);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: estimate_req_filter_len(), invalid stopband level : %f\n", _As);
        exit(1);
    }

    // compute delta_1, delta_2
    float d1, d2;
    d1 = d2 = powf(10.0, -_As/10.0);

    // compute log of delta_1, delta_2
    float t1 = log10f(d1);
    float t2 = log10f(d2);

    // compute D_infinity(delta_1, delta_2)
    float Dinf = (0.005309f*t1*t1 + 0.07114f*t1 - 0.4761f)*t2 -
                 (0.002660f*t1*t1 + 0.59410f*t1 + 0.4278f);

    // compute f(delta_1, delta_2)
    float f = 11.012f + 0.51244f*(t1-t2);

    // compute filter length estimate
    float h_len = (Dinf - f*_df*_df) / _df + 1.0f;
    
    return h_len;
}

// returns the Kaiser window beta factor : sidelobe suppression level
float kaiser_beta_slsl(float _slsl)
{
    // from [Vaidyanathan:1993]
    _slsl = fabsf(_slsl);
    float beta;
    if (_slsl > 50.0f)
        beta = 0.1102f*(_slsl - 8.7f);
    else if (_slsl > 21.0f)
        beta = 0.5842*powf(_slsl - 21, 0.4f) + 0.07886f*(_slsl - 21);
    else
        beta = 0.0f;

    return beta;
}

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _mu     : fractional sample offset [-0.5,0.5]
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n,
                       float _fc,
                       float _slsl,
                       float _mu,
                       float *_h)
{
    // validate inputs
    if (_mu < -0.5f || _mu > 0.5f) {
        fprintf(stderr,"error: fir_kaiser_window(), _mu (%12.4e) out of range [-0.5,0.5]\n", _mu);
        exit(1);
    } else if (_fc < 0.0f || _fc > 1.0f) {
        fprintf(stderr,"error: fir_kaiser_window(), cutoff frequency (%12.4e) out of range [0.0,1.0]\n", _fc);
        exit(1);
    } else if (_n == 0) {
        fprintf(stderr,"error: fir_kaiser_window(), filter length must be greater than zero\n");
        exit(1);
    }

    // chooise kaiser beta parameter (approximate)
    float beta = kaiser_beta_slsl(_slsl);

    float t, h1, h2; 
    unsigned int i;
    for (i=0; i<_n; i++) {
        t = (float)i - (float)(_n-1)/2 + _mu;
     
        // sinc prototype
        h1 = sincf(_fc*t);

        // kaiser window
        h2 = kaiser(i,_n,beta,_mu);

        //printf("t = %f, h1 = %f, h2 = %f\n", t, h1, h2);

        // composite
        _h[i] = h1*h2;
    }   
}


// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _theta  : LoS component angle of arrival
//  _h      : output coefficient buffer
void fir_design_doppler(unsigned int _n,
                        float _fd,
                        float _K,
                        float _theta,
                        float *_h)
{
    float t, J, r, w;
    float beta = 4; // kaiser window parameter
    unsigned int i;
    for (i=0; i<_n; i++) {
        // time sample
        t = (float)i - (float)(_n-1)/2;

        // Bessel
        J = 1.5*besselj_0(fabsf(2*M_PI*_fd*t));

        // Rice-K component
        r = 1.5*_K/(_K+1)*cosf(2*M_PI*_fd*t*cosf(_theta));

        // Window
        w = kaiser(i, _n, beta, 0);

        // composite
        _h[i] = (J+r)*w;

        //printf("t=%f, J=%f, r=%f, w=%f\n", t, J, r, w);
    }
}

// Design optimum FIR root-nyquist filter
//  _n      : filter length
//  _k      : samples/symbol
//  _beta   : excess bandwidth factor
void fir_design_optim_root_nyquist(unsigned int _n,
                                   unsigned int _k,
                                   float _slsl,
                                   float *_h)
{
    // validate inputs:
    //    _k >= 2
    //    _slsl < 0

    // begin with prototype
    //float fc = 1/((float)_k);
    //fir_design_windowed_sinc(_n, fc, _slsl, _h);

    // begin optimization:
}

// filter analysis


// liquid_filter_autocorr()
//
// Compute auto-correlation of filter at a specific lag.
//
//  _h      :   filter coefficients [size: _h_len]
//  _h_len  :   filter length
//  _lag    :   auto-correlation lag (samples)
float liquid_filter_autocorr(float * _h,
                             unsigned int _h_len,
                             int _lag)
{
    // auto-correlation is even symmetric
    _lag = abs(_lag);

    // lag outside of filter length is zero
    if (_lag >= _h_len) return 0.0f;

    // compute auto-correlation
    float rxx=0.0f; // initialize auto-correlation to zero
    unsigned int i;
    for (i=_lag; i<_h_len; i++)
        rxx += _h[i] * _h[i-_lag];

    return rxx;
}

// liquid_filter_isi()
//
// Compute inter-symbol interference (ISI)--both RMS and
// maximum--for the filter _h.
//
//  _h      :   filter coefficients [size: 2*_k*_m+1]
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _rms    :   output root mean-squared ISI
//  _max    :   maximum ISI
void liquid_filter_isi(float * _h,
                       unsigned int _k,
                       unsigned int _m,
                       float * _rms,
                       float * _max)
{
    unsigned int h_len = 2*_k*_m+1;

    // compute zero-lag auto-correlation
    float rxx0 = liquid_filter_autocorr(_h,h_len,0);
    //printf("rxx0 = %12.8f\n", rxx0);
    //exit(1);

    unsigned int i;
    float isi_rms = 0.0f;
    float isi_max = 0.0f;
    float e;
    for (i=1; i<=2*_m; i++) {
        e = liquid_filter_autocorr(_h,h_len,i*_k) / rxx0;
        e = fabsf(e);

        isi_rms += e*e;
        
        if (i==1 || e > isi_max)
            isi_max = e;
    }

    *_rms = sqrtf( isi_rms / (float)(2*_m) );
    *_max = isi_max;
}



