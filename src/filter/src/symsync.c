//
// Symbol synchronizer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../../dotprod/src/dotprod_internal.h"
#include "../../buffer/src/buffer.h"

// defined:
//  SYMSYNC()   name-mangling macro
//  FIRPFB()    firpfb macro
//  T           data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct SYMSYNC(_s) {
    T * h;
    T * dh;
    unsigned int k; // samples/symbol
    unsigned int h_len;
    unsigned int num_filters;

    FIRPFB() mf;    // matched filter
    FIRPFB() dmf;   // derivative matched filter

    fir_prototype p;

    // loop filter
    float delay;    // filter delay
    float zeta;     // loop filter correction factor
    float bt;
    float xi, ac, alpha, beta;
    float q, q_hat, q_prime;

    float b_soft;
    unsigned int b;

    enum {SHIFT,SKIP,STUFF} state;
};

SYMSYNC() SYMSYNC(_create)(unsigned int _k, unsigned int _num_filters, T * _h, unsigned int _h_len)
{
    SYMSYNC() q = (SYMSYNC()) malloc(sizeof(struct SYMSYNC(_s)));
    q->k = _k;
    q->num_filters = _num_filters;
    
    // compute derivative filter
    T dh[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++) {
        if (i==0) {
            dh[i] = _h[i+1] - _h[_h_len-1];
        } else if (i==_h_len-1) {
            dh[i] = _h[0]   - _h[i-1];
        } else {
            dh[i] = _h[i+1] - _h[i-1];
        }
    }

    q->mf  = FIRPFB(_create)(q->num_filters, _h, q->h_len);
    q->dmf = FIRPFB(_create)(q->num_filters, dh, q->h_len);

    // reset state and initialize loop filter
    q->ac = 1.0f;
    q->delay = 0.5f * (float) _h_len;
    SYMSYNC(_clear)(q);
    SYMSYNC(_set_lf_bw)(q, 0.05f);

    return q;
}

void SYMSYNC(_destroy)(SYMSYNC() _q)
{
    FIRPFB(_destroy)(_q->mf);
    FIRPFB(_destroy)(_q->dmf);
    free(_q);
}

void SYMSYNC(_print)(SYMSYNC() _q)
{
    printf("symbol synchronizer [k: %u, num_filters: %u]\n",
        _q->k, _q->num_filters);
    FIRPFB(_print)(_q->mf);
    FIRPFB(_print)(_q->dmf);
}

void SYMSYNC(_execute)(SYMSYNC() _q)
{
    //
}

void SYMSYNC(_set_lf_bw)(SYMSYNC() _q, float _bt)
{
    // set loop filter bandwidth
    _q->bt = _bt;
    _q->xi = 1.0f/sqrtf(2.0f);

    // compensate for filter delay (empirical relationship)
    _q->zeta = _q->delay + 1.0f;
    _q->bt = (_q->bt) / sqrtf(_q->zeta);
    _q->xi = 2 * (_q->xi) * (_q->beta);

    // compute filter coefficients
    _q->beta = 2*(_q->bt)/(_q->xi + 1.0f/(4*(_q->xi)));
    _q->alpha = 2*(_q->xi)*(_q->beta);
}

void SYMSYNC(_clear)(SYMSYNC() _q)
{
    // reset internal filterbank states
    FIRPFB(_clear)(_q->mf);
    FIRPFB(_clear)(_q->dmf);

    // reset loop filter states
    _q->q = 0.0f;
    _q->q_hat = 0.0f;
    _q->q_prime = 0.0f;
    _q->b_soft = 0.0f;
    _q->b = 0;
    _q->state = SHIFT;
}

void SYMSYNC(_estimate_timing)(SYMSYNC() _q, T * _v, unsigned int _n)
{
}
