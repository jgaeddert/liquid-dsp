/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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
// iirhilb.c
//
// infinite impulse response (IIR) Hilbert transform
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  IIRHILB()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct IIRHILB(_s) {
    // filter objects
    IIRFILT()       filt_0; // upper filter branch
    IIRFILT()       filt_1; // lower filter branch

    // regular real-to-complex/complex-to-real operation
    unsigned int toggle;
};

// create iirhilb object
//  _ftype  : filter type (e.g. LIQUID_IIRDES_BUTTER)
//  _n      : filter order, _n > 0
//  _Ap     : pass-band ripple [dB], _Ap > 0
//  _As     : stop-band ripple [dB], _Ap > 0
IIRHILB() IIRHILB(_create)(liquid_iirdes_filtertype _ftype,
                           unsigned int             _n,
                           float                    _Ap,
                           float                    _As)
{
    // validate iirhilb inputs
    if (_n == 0) {
        fprintf(stderr,"error: iirhilb_create(), filter order must be greater than zero\n");
        exit(1);
    }

    // allocate memory for main object
    IIRHILB() q = (IIRHILB()) malloc(sizeof(struct IIRHILB(_s)));

    // design filters
    int     btype  = LIQUID_IIRDES_LOWPASS; // filter band type
    int     format = LIQUID_IIRDES_SOS;     // filter coefficients format
    float   fc     =   0.25f;               // cutoff frequency [normalized]
    float   f0     =   0.0f;                // center frequency [normalized]
    q->filt_0 = IIRFILT(_create_prototype)(_ftype,btype,format,_n,fc,f0,_Ap,_As);
    q->filt_1 = IIRFILT(_create_prototype)(_ftype,btype,format,_n,fc,f0,_Ap,_As);

    // reset internal state and return object
    IIRHILB(_reset)(q);
    return q;
}

// Create a default iirhilb object with a particular filter order.
//  _n      : filter order, _n > 0
IIRHILB() IIRHILB(_create_default)(unsigned int _n)
{
    // validate iirhilb inputs
    if (_n == 0) {
        fprintf(stderr,"error: iirhilb_create_default(), filter order must be greater than zero\n");
        exit(1);
    }

    // allocate memory for main object
    IIRHILB() q = (IIRHILB()) malloc(sizeof(struct IIRHILB(_s)));

    // design filters
    int     ftype  = LIQUID_IIRDES_BUTTER;  // filter design type
    int     btype  = LIQUID_IIRDES_LOWPASS; // filter band type
    int     format = LIQUID_IIRDES_SOS;     // filter coefficients format
    float   fc     =   0.25f;               // cutoff frequency [normalized]
    float   f0     =   0.0f;                // center frequency [normalized]
    float   Ap     =   0.1f;                // pass-band ripple [dB]
    float   As     =   60.0f;               // stop-band attenuation [dB]
    q->filt_0 = IIRFILT(_create_prototype)(ftype,btype,format,_n,fc,f0,Ap,As);
    q->filt_1 = IIRFILT(_create_prototype)(ftype,btype,format,_n,fc,f0,Ap,As);

    // reset internal state and return object
    IIRHILB(_reset)(q);
    return q;
}

// destroy iirhilb object
void IIRHILB(_destroy)(IIRHILB() _q)
{
    // destroy window buffers
    IIRFILT(_destroy)(_q->filt_0);
    IIRFILT(_destroy)(_q->filt_1);

    // free main object memory
    free(_q);
}

// print iirhilb object internals
void IIRHILB(_print)(IIRHILB() _q)
{
    printf("iir hilbert transform\n");
}

// reset iirhilb object internal state
void IIRHILB(_reset)(IIRHILB() _q)
{
    // clear window buffers
    IIRFILT(_reset)(_q->filt_0);
    IIRFILT(_reset)(_q->filt_1);

    // reset toggle flag
    _q->toggle = 0;
}

// execute Hilbert transform (real to complex)
//  _q      :   iirhilb object
//  _x      :   real-valued input sample
//  _y      :   complex-valued output sample
void IIRHILB(_r2c_execute)(IIRHILB()   _q,
                           T           _x,
                           T complex * _y)
{
#if 0
    T * r;  // buffer read pointer
    T yi;   // in-phase component
    T yq;   // quadrature component

    if ( _q->toggle == 0 ) {
        // push sample into upper branch
        WINDOW(_push)(_q->w0, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->w0, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->w1, &r);
        
        // execute dotprod
        DOTPROD(_execute)(_q->dpq, r, &yq);
    } else {
        // push sample into lower branch
        WINDOW(_push)(_q->w1, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->w1, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->w0, &r);

        // execute dotprod
        DOTPROD(_execute)(_q->dpq, r, &yq);
    }

    // toggle flag
    _q->toggle = 1 - _q->toggle;

    // set return value
    *_y = yi + _Complex_I * yq;
#endif
}

// execute Hilbert transform (complex to real)
//  _q      :   iirhilb object
//  _y      :   complex-valued input sample
//  _x      :   real-valued output sample
void IIRHILB(_c2r_execute)(IIRHILB() _q,
                           T complex _x,
                           T *       _y)
{
    *_y = crealf(_x);
}

// execute Hilbert transform decimator (real to complex)
//  _q      :   iirhilb object
//  _x      :   real-valued input array [size: 2 x 1]
//  _y      :   complex-valued output sample
void IIRHILB(_decim_execute)(IIRHILB()   _q,
                             T *         _x,
                             T complex * _y)
{
    // mix down by Fs/4
    T xi = _q->toggle ? -_x[0] :  _x[0];
    T xq = _q->toggle ?  _x[1] : -_x[1];

    // upper branch
    T yi0, yi1;
    IIRFILT(_execute)(_q->filt_0, xi, &yi0);
    IIRFILT(_execute)(_q->filt_0,  0, &yi1);

    // lower branch
    T yq0, yq1;
    IIRFILT(_execute)(_q->filt_1,  0, &yq0);
    IIRFILT(_execute)(_q->filt_1, xq, &yq1);

    // set return value
    *_y = 2*(yi0 + _Complex_I*yq0);

    // toggle flag
    _q->toggle = 1 - _q->toggle;
}

// execute Hilbert transform decimator (real to complex) on
// a block of samples
//  _q      :   Hilbert transform object
//  _x      :   real-valued input array [size: 2*_n x 1]
//  _n      :   number of *output* samples
//  _y      :   complex-valued output array [size: _n x 1]
void IIRHILB(_decim_execute_block)(IIRHILB()    _q,
                                   T *          _x,
                                   unsigned int _n,
                                   T complex *  _y)
{
    unsigned int i;

    for (i=0; i<_n; i++)
        IIRHILB(_decim_execute)(_q, &_x[2*i], &_y[i]);
}

// execute Hilbert transform interpolator (complex to real)
//  _q      :   iirhilb object
//  _y      :   complex-valued input sample
//  _x      :   real-valued output array [size: 2 x 1]
void IIRHILB(_interp_execute)(IIRHILB() _q,
                              T complex _x,
                              T *       _y)
{
    // upper branch
    T yi0, yi1;
    IIRFILT(_execute)(_q->filt_0, crealf(_x), &yi0);
    IIRFILT(_execute)(_q->filt_0,          0, &yi1);

    // lower branch
    T yq0, yq1;
    IIRFILT(_execute)(_q->filt_1, cimagf(_x), &yq0);
    IIRFILT(_execute)(_q->filt_1,          0, &yq1);

    // mix up by Fs/4 and retain real component
    //     {yi0 + j yq0, yi1 + j yq1}
    // 0: Re{+1 (yi0 + j yq0), +j (yi1 + j yq1)} = Re{ yi0 + j yq0, -yq1 + j yi1} = { yi0, -yq1}
    // 1: Re{-1 (yi0 + j yq0), -j (yi1 + j yq1)} = Re{-yi0 - j yq0,  yq1 - j yi1} = {-yi0,  yq1}
    _y[0] = 2*(_q->toggle ? -yi0 :  yi0);
    _y[1] = 2*(_q->toggle ?  yq1 : -yq1);

    // toggle flag
    _q->toggle = 1 - _q->toggle;
}

// execute Hilbert transform interpolator (complex to real)
// on a block of samples
//  _q      :   Hilbert transform object
//  _x      :   complex-valued input array [size: _n x 1]
//  _n      :   number of *input* samples
//  _y      :   real-valued output array [size: 2*_n x 1]
void IIRHILB(_interp_execute_block)(IIRHILB()    _q,
                                    T complex *  _x,
                                    unsigned int _n,
                                    T *          _y)
{
    unsigned int i;

    for (i=0; i<_n; i++)
        IIRHILB(_interp_execute)(_q, _x[i], &_y[2*i]);
}
