/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

// Halfband resampler (interpolator/decimator)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  RESAMP2()       name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct RESAMP2(_s) {
    TC *            h;      // filter prototype
    unsigned int    m;      // primitive filter length
    unsigned int    h_len;  // actual filter length: h_len = 4*m+1
    float           f0;     // center frequency [-1.0 <= f0 <= 1.0]
    float           as;     // stop-band attenuation [dB]

    // filter component
    TC *            h1;     // filter branch coefficients
    DOTPROD()       dp;     // inner dot product object
    unsigned int    h1_len; // filter length (2*m)

    // input buffers
    WINDOW()        w0;     // input buffer (even samples)
    WINDOW()        w1;     // input buffer (odd samples)
    TC              scale;  // output scaling factor

    // halfband filter operation
    unsigned int    toggle;
};

// create a resamp2 object
//  _m      :   filter semi-length (effective length: 4*_m+1)
//  _f0     :   center frequency of half-band filter
//  _as     :   stop-band attenuation [dB], _as > 0
RESAMP2() RESAMP2(_create)(unsigned int _m,
                           float        _f0,
                           float        _as)
{
    // validate input
    if (_m < 2)
        return liquid_error_config("resamp2_%s_create(), filter semi-length must be at least 2", EXTENSION_FULL);
    if (_f0 < -0.5f || _f0 > 0.5f)
        return liquid_error_config("resamp2_%s_create(), f0 (%12.4e) must be in [-0.5,0.5]", EXTENSION_FULL, _f0);
    if (_as < 0.0f)
        return liquid_error_config("resamp2_%s_create(), as (%12.4e) must be greater than zero", EXTENSION_FULL, _as);

    RESAMP2() q = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    q->m  = _m;
    q->f0 = _f0;
    q->as = _as;

    // change filter length as necessary
    q->h_len = 4*(q->m) + 1;
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));

    q->h1_len = 2*(q->m);
    q->h1 = (TC *) malloc((q->h1_len)*sizeof(TC));

    // design filter prototype
    unsigned int i;
    float hf[q->h_len];
    liquid_firdespm_halfband_as(q->m, q->as, hf);
    for (i=0; i<q->h_len; i++) {
        // calculate filter coefficients
        float t = (float)i - (float)(q->h_len-1)/2.0f;
#if TC_COMPLEX == 1
        float complex hp = 2 * hf[i] * ( cosf(2.0f*M_PI*t*q->f0) + _Complex_I*sinf(2.0f*M_PI*t*q->f0) );
#else
        float hp = 2 * hf[i] * cosf(2.0f*M_PI*t*q->f0);
#endif

        // copy composite filter values
#if defined LIQUID_FIXED && TC_COMPLEX == 1
        q->h[i] = CQ(_float_to_fixed)(hp);
#elif defined LIQUID_FIXED && TC_COMPLEX == 0
        q->h[i] = Q(_float_to_fixed)(hp);
#else
        q->h[i] = hp; // float (either real or complex)
#endif
    }

    // resample, alternate sign, [reverse direction]
    unsigned int j=0;
    for (i=1; i<q->h_len; i+=2)
        q->h1[j++] = q->h[q->h_len - i - 1];

    // initialize coefficients and create dotprod object
    q->dp = DOTPROD(_create)(q->h1, 2*q->m);

    // create window buffers
    q->w0 = WINDOW(_create)(2*(q->m));
    q->w1 = WINDOW(_create)(2*(q->m));

    RESAMP2(_reset)(q);
    RESAMP2(_set_scale)(q, TC_ONE); // 1.0

    return q;
}

// re-create a resamp2 object with new properties
//  _q          :   original resamp2 object
//  _m          :   filter semi-length (effective length: 4*_m+1)
//  _f0         :   center frequency of half-band filter
//  _as         :   stop-band attenuation [dB], _as > 0
RESAMP2() RESAMP2(_recreate)(RESAMP2()    _q,
                             unsigned int _m,
                             float        _f0,
                             float        _as)
{
    RESAMP2(_destroy)(_q);
    return RESAMP2(_create)(_m, _f0, _as);
}

// copy object
RESAMP2() RESAMP2(_copy)(RESAMP2() q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("resamp2_%s_copy(), object cannot be NULL", EXTENSION_FULL);

    // create object, copy internal memory, overwrite with specific values
    RESAMP2() q_copy = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    memmove(q_copy, q_orig, sizeof(struct RESAMP2(_s)));

    // allocate memory for filter coefficients
    q_copy->h  = (TC *) malloc((q_copy->h_len )*sizeof(TC));
    q_copy->h1 = (TC *) malloc((q_copy->h1_len)*sizeof(TC));

    // copy filter coefficients
    memmove(q_copy->h,  q_orig->h,  (q_copy->h_len )*sizeof(TC));
    memmove(q_copy->h1, q_orig->h1, (q_copy->h1_len)*sizeof(TC));

    // copy dot product and window objects
    q_copy->dp = DOTPROD(_copy)(q_orig->dp);
    q_copy->w0 = WINDOW (_copy)(q_orig->w0);
    q_copy->w1 = WINDOW (_copy)(q_orig->w1);

    // return object
    return q_copy;
}

// destroy a resamp2 object, clearing up all allocated memory
int RESAMP2(_destroy)(RESAMP2() _q)
{
    // destroy dotprod object
    DOTPROD(_destroy)(_q->dp);

    // destroy window buffers
    WINDOW(_destroy)(_q->w0);
    WINDOW(_destroy)(_q->w1);

    // free arrays
    free(_q->h);
    free(_q->h1);

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

// print a resamp2 object's internals
int RESAMP2(_print)(RESAMP2() _q)
{
    printf("<liquid.resamp2_%s, len=%u, f0=%12.8f>\n",
        EXTENSION_FULL, _q->h_len,_q->f0);
    return LIQUID_OK;
}

// clear internal buffer
int RESAMP2(_reset)(RESAMP2() _q)
{
    WINDOW(_reset)(_q->w0);
    WINDOW(_reset)(_q->w1);

    _q->toggle = 0;
    return LIQUID_OK;
}

// get filter delay (samples)
unsigned int RESAMP2(_get_delay)(RESAMP2() _q)
{
    return 2*_q->m - 1;
}

// set output scaling for filter
int RESAMP2(_set_scale)(RESAMP2() _q,
                        TC        _scale)
{
    _q->scale = _scale;
    return LIQUID_OK;
}

// get output scaling for filter
int RESAMP2(_get_scale)(RESAMP2() _q,
                         TC *      _scale)
{
    *_scale = _q->scale;
    return LIQUID_OK;
}

// execute resamp2 as half-band filter
//  _q      :   resamp2 object
//  _x      :   input sample
//  _y0     :   output sample pointer (low frequency)
//  _y1     :   output sample pointer (high frequency)
int RESAMP2(_filter_execute)(RESAMP2() _q,
                             TI        _x,
                             TO *      _y0,
                             TO *      _y1)
{
    TI * r;     // buffer read pointer
    TO yi;      // delay branch
    TO yq;      // filter branch

    if ( _q->toggle == 0 ) {
        // push sample into upper branch
        WINDOW(_push)(_q->w0, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->w0, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->w1, &r);
        DOTPROD(_execute)(_q->dp, r, &yq);
    } else {
        // push sample into lower branch
        WINDOW(_push)(_q->w1, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->w1, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->w0, &r);
        DOTPROD(_execute)(_q->dp, r, &yq);
    }

    // toggle flag
    _q->toggle = 1 - _q->toggle;

    // set return values, normalizing gain
#if defined LIQUID_FIXED && TO_COMPLEX==0
    *_y0 = (yi + yq)/2;
    *_y1 = (yi - yq)/2;

    *_y0 = MUL_TI_TC(*_y0, _q->scale);
    *_y1 = MUL_TI_TC(*_y1, _q->scale);
#elif defined LIQUID_FIXED && TO_COMPLEX==1
    //*_y0 = CQ(_add)(yi, yq);
    //*_y1 = CQ(_sub)(yi, yq);

    _y0->real = (yi.real + yq.real) / 2;  // lower band
    _y0->imag = (yi.imag + yq.imag) / 2;  // lower band

    _y1->real = (yi.real - yq.real) / 2;  // upper band
    _y1->imag = (yi.imag - yq.imag) / 2;  // upper band

    *_y0 = MUL_TI_TC(*_y0, _q->scale);
    *_y1 = MUL_TI_TC(*_y1, _q->scale);
#else
    // set return values, normalizing gain, applying scaling factor
    *_y0 = 0.5f*(yi + yq)*_q->scale;    // lower band
    *_y1 = 0.5f*(yi - yq)*_q->scale;    // upper band
#endif
    return LIQUID_OK;
}

// execute analysis half-band filterbank
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output array [size: 2 x 1]
int RESAMP2(_analyzer_execute)(RESAMP2() _q,
                               TI *      _x,
                               TO *      _y)
{
    TI * r;     // buffer read pointer
    TO y0;      // delay branch
    TO y1;      // filter branch
    // cut input in half
#if defined LIQUID_FIXED && TO_COMPLEX==1
    TI _x0_2 = CQ(_mul_scalar)(_x[0], Q(_one)>>1);
    TI _x1_2 = CQ(_mul_scalar)(_x[1], Q(_one)>>1);
#else
    TI _x0_2 = _x[0] / 2;
    TI _x1_2 = _x[1] / 2;
#endif

    // compute filter branch
    WINDOW(_push)(_q->w1, _x0_2);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_execute)(_q->dp, r, &y1);

    // compute delay branch
    WINDOW(_push)(_q->w0, _x1_2);
    WINDOW(_index)(_q->w0, _q->m-1, &y0);

    // set return value
#if defined LIQUID_FIXED && TO_COMPLEX==0
    _y[0] = y1 + y0;
    _y[1] = y1 - y0;

    _y[0] = MUL_TI_TC(_y[0], _q->scale);
    _y[1] = MUL_TI_TC(_y[1], _q->scale);
#elif defined LIQUID_FIXED && TO_COMPLEX==1
    _y[0].real = y1.real + y0.real;
    _y[0].imag = y1.imag + y0.imag;

    _y[1].real = y1.real - y0.real;
    _y[1].imag = y1.imag - y0.imag;

    _y[0] = MUL_TI_TC(_y[0], _q->scale);
    _y[1] = MUL_TI_TC(_y[1], _q->scale);
#else
    // set return value, applying scaling factor
    _y[0] = (y1 + y0) * _q->scale;
    _y[1] = (y1 - y0) * _q->scale;
#endif
    return LIQUID_OK;
}

// execute synthesis half-band filterbank
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output array [size: 2 x 1]
int RESAMP2(_synthesizer_execute)(RESAMP2() _q,
                                  TI *      _x,
                                  TO *      _y)
{
#if defined LIQUID_FIXED && TO_COMPLEX==1
    //TI x0 = {_x[0].real + _x[1].real, _x[0].imag + _x[1].imag};  // delay branch input
    //TI x1 = {_x[0].real - _x[1].real, _x[0].imag - _x[1].imag};  // filter branch input
    
    TI x0 = CQ(_add)(_x[0], _x[1]); // delay branch input
    TI x1 = CQ(_sub)(_x[0], _x[1]); // filter branch input
#else
    TI x0 = _x[0] + _x[1];  // delay branch input
    TI x1 = _x[0] - _x[1];  // filter branch input
#endif

    // compute delay branch
    WINDOW(_push)(_q->w0, x0);
    WINDOW(_index)(_q->w0, _q->m-1, &_y[0]);

    // compute second branch (filter)
    TI * r;                 // buffer read pointer
    WINDOW(_push)(_q->w1, x1);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_execute)(_q->dp, r, &_y[1]);

    // apply scaling factor
#if defined LIQUID_FIXED
    _y[0] = MUL_TI_TC(_y[0], _q->scale);
    _y[1] = MUL_TI_TC(_y[1], _q->scale);
#else
    _y[0] *= _q->scale;
    _y[1] *= _q->scale;
#endif
    return LIQUID_OK;
}


// execute half-band decimation
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output sample pointer
int RESAMP2(_decim_execute)(RESAMP2() _q,
                            TI *      _x,
                            TO *      _y)
{
    TI * r;     // buffer read pointer
    TO y0;      // delay branch
    TO y1;      // filter branch

    // compute filter branch
    WINDOW(_push)(_q->w1, _x[0]);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_execute)(_q->dp, r, &y1);

    // compute delay branch
    WINDOW(_push)(_q->w0, _x[1]);
    WINDOW(_index)(_q->w0, _q->m-1, &y0);

    // set return value, applying scaling factor
#if defined LIQUID_FIXED && TO_COMPLEX==0
    *_y = MUL_TI_TC(y0 + y1, _q->scale);
#elif defined LIQUID_FIXED && TO_COMPLEX==1
    *_y = CQ(_add)(y0, y1);
    *_y = MUL_TI_TC(*_y, _q->scale);
#else
    *_y = (y0 + y1) * _q->scale;
#endif
    return LIQUID_OK;
}

// execute half-band interpolation
//  _q      :   resamp2 object
//  _x      :   input sample
//  _y      :   output array [size: 2 x 1]
int RESAMP2(_interp_execute)(RESAMP2() _q,
                             TI        _x,
                             TO *      _y)
{
    TI * r;  // buffer read pointer

    // compute delay branch
    WINDOW(_push)(_q->w0, _x);
    WINDOW(_index)(_q->w0, _q->m-1, &_y[0]);

    // compute second branch (filter)
    WINDOW(_push)(_q->w1, _x);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_execute)(_q->dp, r, &_y[1]);

    // apply scaling factor
    _y[0] = MUL_TI_TC(_y[0], _q->scale);
    _y[1] = MUL_TI_TC(_y[1], _q->scale);
    return LIQUID_OK;
}

