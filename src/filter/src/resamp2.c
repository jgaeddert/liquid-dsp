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
    TC * h;             // filter prototype
    unsigned int m;     // primitive filter length
    unsigned int h_len; // actual filter length: h_len = 4*m+1

    // lower branch (filter)
    TC * h1;
    WINDOW() w1;
    unsigned int h1_len;

    // upper branch (delay line)
    TI * w0;
    unsigned int w0_index;
};

RESAMP2() RESAMP2(_create)(unsigned int _h_len)
{
    RESAMP2() f = (RESAMP2()) malloc(sizeof(struct RESAMP2(_s)));
    f->h_len = _h_len;

    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    f->m = (_h_len-1)/4;
    if (f->m < 2)
        f->m = 2;

    f->h_len = 4*(f->m) + 1;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    f->h1_len = 2*(f->m);
    f->h1 = (TC *) malloc((f->h1_len)*sizeof(TC));

    // design filter prototype
    unsigned int i;
    float t, h1, h2;
    float beta = 6.0f;
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta,0);
        f->h[i] = h1*h2;
    }

    // resample, alternate sign, reverse direction
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->h1[j++] = f->h[f->h_len - i - 1];

    f->w1 = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->w1);

    f->w0 = (TI*)malloc((f->m)*sizeof(TI));
    for (i=0; i<f->m; i++)
        f->w0[i] = 0;
    f->w0_index = 0;

    return f;
}

RESAMP2() RESAMP2(_recreate)(RESAMP2() _f, unsigned int _h_len)
{
    unsigned int i;
    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    unsigned int m0 = _f->m;        // old m value
    unsigned int m1 = (_h_len-1)/4;
    if (m1 < 2)
        m1 = 2;

    // TODO: redesign filter anyway
    if (m1 == m0)
        return _f;

    // compute new lengths
    _f->m = m1;
    _f->h_len = 4*(_f->m) + 1;
    _f->h1_len = 2*(_f->m);

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
        PRINTVAL(w0_tmp[i]);
        printf("\n");
    }
    _f->w0 = realloc(_f->w0, (_f->m)*sizeof(TI));   // reallocate memory
    if (_f->w0 == NULL) {
        printf("error: could not reallocate delay line memory array\n");
        exit(0);
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
        PRINTVAL(_f->w0[(i+_f->w0_index)%(m1)]);
        printf("\n");
    }
    free(w0_tmp);       // free temporary memory block

    // design filter prototype
    float t, h1, h2;
    float beta = 6.0f;
    for (i=0; i<_f->h_len; i++) {
        t = (float)i - (float)(_f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,_f->h_len,beta,0);
        _f->h[i] = h1*h2;
    }

    // resample, alternate sign, reverse direction
    unsigned int j=0;
    for (i=1; i<_f->h_len; i+=2)
        _f->h1[j++] = _f->h[_f->h_len - i - 1];

    return _f;
}

void RESAMP2(_destroy)(RESAMP2() _f)
{
    WINDOW(_destroy)(_f->w1);
    free(_f->w0);
    free(_f->h);
    free(_f->h1);
    free(_f);
}

void RESAMP2(_print)(RESAMP2() _f)
{
    printf("fir half-band resampler: [%u]\n", _f->h_len);
    unsigned int i;
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = ", i+1);
        PRINTVAL(_f->h[i]);
        printf(";\n");
    }
    printf("---\n");
    for (i=0; i<_f->h1_len; i++) {
        printf("  h1(%4u) = ", i+1);
        PRINTVAL(_f->h1[i]);
        printf(";\n");
    }
}

void RESAMP2(_clear)(RESAMP2() _f)
{
    WINDOW(_clear)(_f->w1);
    unsigned int i;
    for (i=0; i<_f->m; i++)
        _f->w0[i] = 0;
    _f->w0_index = 0;
}

void RESAMP2(_decim_execute)(RESAMP2() _f, TI * _x, TO *_y)
{
    TI * r;
    TO y0, y1;

    // compute filter branch
    WINDOW(_push)(_f->w1, _x[0]);
    WINDOW(_read)(_f->w1, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &y1);

    // compute delay branch
    y0 = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x[1];
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // set return value
    *_y = y0 + y1;
}

void RESAMP2(_interp_execute)(RESAMP2() _f, TI _x, TO *_y)
{
    TI * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
    // compute first branch (delay)
    _y[0] = _f->w0[_f->w0_index];
    _f->w0[_f->w0_index] = _x;
    _f->w0_index = (_f->w0_index+1) % (_f->m);

    // compute second branch (filter)
    WINDOW(_push)(_f->w1, _x);
    WINDOW(_read)(_f->w1, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run4)(_f->h1, r, _f->h1_len, &_y[1]);

}

