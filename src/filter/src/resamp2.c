/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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
// Halfband resampler (interpolator/decimator)
//

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
    TC * h;                 // filter prototype
    unsigned int m;         // primitive filter length
    unsigned int h_len;     // actual filter length: h_len = 4*m+1
    float f0;               // center frequency [-1.0 <= f0 <= 1.0]
    float As;               // stop-band attenuation [dB]

    // filter component
    TC * h1;                // filter branch coefficients
    DOTPROD() dp;           // inner dot product object
    unsigned int h1_len;    // filter length (2*m)

    // input buffers
    WINDOW() w0;            // input buffer (even samples)
    WINDOW() w1;            // input buffer (odd samples)

    // halfband filter operation
    unsigned int toggle;
};

// initialize coefficients
void RESAMP2(_init_coefficients)(RESAMP2() _q);

// create a resamp2 object
//  _m      :   filter semi-length (effective length: 4*_m+1)
//  _f0     :   center frequency of half-band filter
//  _As     :   stop-band attenuation [dB], _As > 0
RESAMP2() RESAMP2(_create)(unsigned int _m,
                           float        _f0,
                           float        _As)
{
    // validate input
    if (_m < 2) {
        fprintf(stderr,"error: resamp2_%s_create(), filter semi-length must be at least 2\n", EXTENSION_FULL);
        exit(1);
    } else if (_f0 < -0.5f || _f0 > 0.5f) {
        fprintf(stderr,"error: resamp2_%s_create(), f0 (%12.4e) must be in [-0.5,0.5]\n", EXTENSION_FULL, _f0);
        exit(1);
    } else if (_As < 0.0f) {
        fprintf(stderr,"error: resamp2_%s_create(), As (%12.4e) must be greater than zero\n", EXTENSION_FULL, _As);
        exit(1);
    }

    RESAMP2() q = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    q->m  = _m;
    q->f0 = _f0;
    q->As = _As;

    // change filter length as necessary
    q->h_len = 4*(q->m) + 1;
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));

    q->h1_len = 2*(q->m);
    q->h1 = (TC *) malloc((q->h1_len)*sizeof(TC));

    // design filter prototype
    unsigned int i;
    float t, h1, h2;
    float beta = kaiser_beta_As(q->As);
    for (i=0; i<q->h_len; i++) {
        t = (float)i - (float)(q->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = liquid_kaiser(i,q->h_len,beta);
#if TC_COMPLEX == 1
        float complex h3 = cosf(2.0f*M_PI*t*q->f0) + _Complex_I*sinf(2.0f*M_PI*t*q->f0);
        float complex hp = h1*h2*h3;
#else
        float h3 = cosf(2.0f*M_PI*t*q->f0);
        float hp = h1*h2*h3;
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
    RESAMP2(_init_coefficients)(q);
    q->dp = DOTPROD(_create)(q->h1, 2*q->m);

    // create window buffers
    q->w0 = WINDOW(_create)(2*(q->m));
    q->w1 = WINDOW(_create)(2*(q->m));

    RESAMP2(_reset)(q);

    return q;
}

// re-create a resamp2 object with new properties
//  _q          :   original resamp2 object
//  _m          :   filter semi-length (effective length: 4*_m+1)
//  _f0         :   center frequency of half-band filter
//  _As         :   stop-band attenuation [dB], _As > 0
RESAMP2() RESAMP2(_recreate)(RESAMP2()    _q,
                             unsigned int _m,
                             float        _f0,
                             float        _As)
{
    // TODO: only re-design filter if necessary
    RESAMP2(_destroy)(_q);
    return RESAMP2(_create)(_m, _f0, _As);
}

// destroy a resamp2 object, clearing up all allocated memory
void RESAMP2(_destroy)(RESAMP2() _q)
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
}

// print a resamp2 object's internals
void RESAMP2(_print)(RESAMP2() _q)
{
    printf("fir half-band resampler: [%u taps, f0=%12.8f]\n",
            _q->h_len,
            _q->f0);
    unsigned int i;
#if 0
    for (i=0; i<_q->h_len; i++) {
        printf("  h(%4u) = ", i+1);
        PRINTVAL_TC(_q->h[i],%12.8f);
        printf(";\n");
    }
#endif
    printf("---\n");
    for (i=0; i<_q->h1_len; i++) {
        printf("  h1(%4u) = ", i+1);
        PRINTVAL_TC(_q->h1[i],%12.8f);
        printf(";\n");
    }
}

// clear internal buffer
void RESAMP2(_reset)(RESAMP2() _q)
{
    WINDOW(_reset)(_q->w0);
    WINDOW(_reset)(_q->w1);

    _q->toggle = 0;
}

// get filter delay (samples)
unsigned int RESAMP2(_get_delay)(RESAMP2() _q)
{
    return 2*_q->m - 1;
}

// execute resamp2 as half-band filter
//  _q      :   resamp2 object
//  _x      :   input sample
//  _y0     :   output sample pointer (low frequency)
//  _y1     :   output sample pointer (high frequency)
void RESAMP2(_filter_execute)(RESAMP2() _q,
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
#if defined LIQUID_FIXED && TO_COMPLEX==1
    //*_y0 = CQ(_add)(yi, yq);
    //*_y1 = CQ(_sub)(yi, yq);

    _y0->real = 0.5f*(yi.real + yq.real);  // lower band
    _y0->imag = 0.5f*(yi.imag + yq.imag);  // lower band

    _y1->real = 0.5f*(yi.real - yq.real);  // upper band
    _y1->imag = 0.5f*(yi.imag - yq.imag);  // upper band
#else
    *_y0 = 0.5f*(yi + yq);  // lower band
    *_y1 = 0.5f*(yi - yq);  // upper band
#endif
}

// execute analysis half-band filterbank
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output array [size: 2 x 1]
void RESAMP2(_analyzer_execute)(RESAMP2() _q,
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

    // set return value
#if defined LIQUID_FIXED && TO_COMPLEX==1
    _y[0].real = y1.real + y0.real;
    _y[0].imag = y1.imag + y0.imag;

    _y[1].real = y1.real - y0.real;
    _y[1].imag = y1.imag - y0.imag;
#else
    _y[0] = y1 + y0;
    _y[1] = y1 - y0;
#endif
}

// execute synthesis half-band filterbank
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output array [size: 2 x 1]
void RESAMP2(_synthesizer_execute)(RESAMP2() _q,
                                   TI *      _x,
                                   TO *      _y)
{
#if defined LIQUID_FIXED && TO_COMPLEX==1
    //TI x0 = {_x[0].real + _x[1].real, _x[0].imag + _x[1].imag};  // delay branch input
    //TI x1 = {_x[0].real - _x[1].real, _x[0].imag - _x[1].imag};  // filter branch input
    
    TI x0 = CQ(_add)(_x[0], _x[1]); // delay branch input
    TI x1 = CQ(_sub)(_x[0], _x[1]); // filter branch inpu
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
}


// execute half-band decimation
//  _q      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output sample pointer
void RESAMP2(_decim_execute)(RESAMP2() _q,
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

    // set return value
#if defined LIQUID_FIXED && TO_COMPLEX==1
    //_y->real = y0.real + y1.real;
    //_y->imag = y0.imag + y1.imag;
    *_y = CQ(_add)(y0, y1);
#else
    *_y = y0 + y1;
#endif
}

// execute half-band interpolation
//  _q      :   resamp2 object
//  _x      :   input sample
//  _y      :   output array [size: 2 x 1]
void RESAMP2(_interp_execute)(RESAMP2() _q,
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
}

// 
// internal methods
//

// initialize coefficients
void RESAMP2(_init_coefficients)(RESAMP2() _q)
{
    // design half-band filter prototype (real coefficients)
    float h[_q->h_len];
    liquid_firdes_kaiser(_q->h_len, 0.25f, _q->As, 0.0f, h);

    unsigned int i;
#if TC_COMPLEX == 1
    // modulate filter coefficients
    float complex hc[_q->h_len];
    float t;
    for (i=0; i<_q->h_len; i++) {
        t = (float)i - (float)(_q->h_len-1)/2.0f;
        hc[i] = h[i] * liquid_cexpjf(t);
    }
#else
    // set pointer to original coefficients
    float * hc = h;
#endif

    // resample, alternate sign, [reverse direction]
    unsigned int j=0;
    for (i=1; i<_q->h_len; i+=2) {
#if defined LIQUID_FIXED && TC_COMPLEX == 1
        _q->h1[j++] = CQ(_float_to_fixed)(hc[_q->h_len - i - 1]);
#elif defined LIQUID_FIXED && TC_COMPLEX == 0
        _q->h1[j++] = Q(_float_to_fixed)(hc[_q->h_len - i - 1]);
#else
        _q->h1[j++] = hc[_q->h_len - i - 1];
#endif
    }
}

