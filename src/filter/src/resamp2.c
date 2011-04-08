/*
 * Copyright (c) 2007, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2010 Virginia Polytechnic Institute &
 *                                State University
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
    float fc;               // center frequency [-1.0 <= fc <= 1.0]
    float As;               // stop-band attenuation [dB]

    // filter component
    TC * h1;                // filter branch coefficients
    unsigned int h1_len;    // filter length (2*m)

    // input buffers
    WINDOW() w0;            // input buffer (even samples)
    WINDOW() w1;            // input buffer (odd samples)
};

// create a resamp2 object
//  _m      :   filter semi-length (effective length: 4*_m+1)
//  _fc     :   center frequency of half-band filter
//  _As     :   stop-band attenuation [dB], _As > 0
RESAMP2() RESAMP2(_create)(unsigned int _m,
                           float _fc,
                           float _As)
{
    // validate input
    if (_m < 2) {
        fprintf(stderr,"error: resamp2_xxxt_create(), filter semi-length must be at least 2\n");
        exit(1);
    }

    RESAMP2() f = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    f->m  = _m;
    f->fc = _fc;
    f->As = _As;
    if ( f->fc < -0.5f || f->fc > 0.5f ) {
        fprintf(stderr,"error: resamp2_xxxt_create(), fc (%12.4e) must be in (-1,1)\n", f->fc);
        exit(1);
    }

    // change filter length as necessary
    f->h_len = 4*(f->m) + 1;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    f->h1_len = 2*(f->m);
    f->h1 = (TC *) malloc((f->h1_len)*sizeof(TC));

    // design filter prototype
    unsigned int i;
    float t, h1, h2;
    TC h3;
    float beta = kaiser_beta_As(f->As);
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta,0);
#if TC_COMPLEX == 1
        h3 = cosf(2.0f*M_PI*t*f->fc) + _Complex_I*sinf(2.0f*M_PI*t*f->fc);
#else
        h3 = cosf(2.0f*M_PI*t*f->fc);
#endif
        f->h[i] = h1*h2*h3;
    }

    // resample, alternate sign, [reverse direction]
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->h1[j++] = f->h[f->h_len - i - 1];

    f->w0 = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->w0);

    f->w1 = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->w1);

    return f;
}

// re-create a resamp2 object
//  _f          :   original resamp2 object
//  _h_len      :   desired filter length (will force 4*m+1)
//  _fc         :   center frequency of half-band filter
//  _As         :   stop-band attenuation [dB], _As > 0
RESAMP2() RESAMP2(_recreate)(RESAMP2() _f,
                             unsigned int _h_len,
                             float _fc,
                             float _As)
{
#if 0
    unsigned int i;
    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    unsigned int m0 = _f->m;        // old m value
    unsigned int m1 = (_h_len-1)/4;
    if (m1 < 2)
        m1 = 2;

    // TODO: redesign filter anyway
    if (m1 == m0 && _f->fc == _fc && _f->As == _As)
        return _f;

    // compute new lengths
    _f->m = m1;
    _f->h_len = 4*(_f->m) + 1;
    _f->h1_len = 2*(_f->m);

    // set center frequency, stop-band attenuation
    _f->fc = _fc;
    _f->As = _As;

    // re-allocate memory
    _f->h  = (TC*) realloc(_f->h,  (_f->h_len)*sizeof(TC));
    _f->h1 = (TC*) realloc(_f->h1, (_f->h1_len)*sizeof(TC));
    printf("old window:\n");
    WINDOW(_print)(_f->w1);
    _f->w1 = WINDOW(_recreate)(_f->w1, 2*(_f->m));
    printf("new window:\n");
    WINDOW(_print)(_f->w1);

    // inefficient but effective
    TI* w0_tmp = (TI*) malloc(m0*sizeof(TI));       // create temporary array
    //memmove(w0_tmp, _f->w0, m0*sizeof(TI));         // copy old values
    for (i=0; i<m0; i++)
        w0_tmp[i] = _f->w0[(i+_f->w0_index)%(m0)];  // copy old values (reorder)
    printf("  old values:\n");
    for (i=0; i<m0; i++) {
        printf("  %4u : ", i);
        PRINTVAL_TC(w0_tmp[i],%12.8f);
        printf("\n");
    }
    _f->w0 = realloc(_f->w0, (_f->m)*sizeof(TI));   // reallocate memory
    if (_f->w0 == NULL) {
        fprintf(stderr,"error: could not reallocate delay line memory array\n");
        exit(1);
    }
    if (m1 > m0) {
        printf("  resamp2_xxxf_recreate(): extending filter\n");
        unsigned int t = m1-m0;
        // pad beginning with zeros
        for (i=0; i<t; i++)
            _f->w0[i] = 0;
        // push all old values
        for (i=0; i<m0; i++)
            _f->w0[i+t] = w0_tmp[i];
        _f->w0_index = 0;
    } else {
        printf("  resamp2_xxxf_recreate(): reducing filter\n");
        unsigned int t = m0-m1;
        // push most recent old values
        for (i=0; i<m1; i++)
            _f->w0[i] = w0_tmp[i+t];
        _f->w0_index = 0;
    }

    printf("  new values:\n");
    for (i=0; i<m1; i++) {
        printf("  %4u : ", i);
        PRINTVAL_TC(_f->w0[(i+_f->w0_index)%(m1)],%12.8f);
        printf("\n");
    }
    free(w0_tmp);       // free temporary memory block

    // design filter prototype
    float t, h1, h2;
    TC h3;
    float beta = 6.0f;
    for (i=0; i<_f->h_len; i++) {
        t = (float)i - (float)(_f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,_f->h_len,beta,0);
#if TC_COMPLEX == 1
        //h3 = cosf(M_PI*t*_f->fc) + _Complex_I*sinf(M_PI*t*_f->fc);
        h3 = liquid_cexpjf(M_PI*t*_f->fc);
#else
        h3 = cosf(M_PI*t*_f->fc);
#endif
        _f->h[i] = h1*h2*h3;
    }

    // resample, alternate sign, reverse direction
    unsigned int j=0;
    for (i=1; i<_f->h_len; i+=2)
        _f->h1[j++] = _f->h[_f->h_len - i - 1];

    return _f;
#else
    RESAMP2(_destroy)(_f);
    return RESAMP2(_create)(_h_len, _fc, _As);
#endif
}

// destroy a resamp2 object, clearing up all allocated memory
void RESAMP2(_destroy)(RESAMP2() _f)
{
    WINDOW(_destroy)(_f->w0);
    WINDOW(_destroy)(_f->w1);

    free(_f->h);
    free(_f->h1);
    free(_f);
}

// print a resamp2 object's internals
void RESAMP2(_print)(RESAMP2() _f)
{
    printf("fir half-band resampler: [%u taps, fc=%12.8f]\n",
            _f->h_len,
            _f->fc);
    unsigned int i;
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = ", i+1);
        PRINTVAL_TC(_f->h[i],%12.8f);
        printf(";\n");
    }
    printf("---\n");
    for (i=0; i<_f->h1_len; i++) {
        printf("  h1(%4u) = ", i+1);
        PRINTVAL_TC(_f->h1[i],%12.8f);
        printf(";\n");
    }
}

// clear internal buffer
void RESAMP2(_clear)(RESAMP2() _f)
{
    WINDOW(_clear)(_f->w0);
    WINDOW(_clear)(_f->w1);
}

// execute half-band decimation
//  _f      :   resamp2 object
//  _x      :   input array [size: 2 x 1]
//  _y      :   output sample pointer
void RESAMP2(_decim_execute)(RESAMP2() _f,
                             TI * _x,
                             TO *_y)
{
    TI * r;     // buffer read pointer
    TO y0;      // delay branch
    TO y1;      // filter branch

    // compute filter branch
    WINDOW(_push)(_f->w1, _x[0]);
    WINDOW(_read)(_f->w1, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &y1);

    // compute delay branch
    WINDOW(_push)(_f->w0, _x[1]);
    WINDOW(_index)(_f->w0, _f->m-1, &y0);

    // set return value
    *_y = y0 + y1;
}

// execute half-band interpolation
//  _f      :   resamp2 object
//  _x      :   input sample
//  _y      :   output array [size: 2 x 1]
void RESAMP2(_interp_execute)(RESAMP2() _f, TI _x, TO *_y)
{
    TI * r;  // buffer read pointer

    // compute delay branch
    WINDOW(_push)(_f->w0, _x);
    WINDOW(_index)(_f->w0, _f->m-1, &_y[0]);

    // compute second branch (filter)
    WINDOW(_push)(_f->w1, _x);
    WINDOW(_read)(_f->w1, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &_y[1]);
}

