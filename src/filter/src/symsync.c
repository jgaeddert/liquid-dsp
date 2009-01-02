//
// Symbol synchronizer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../../dotprod/src/dotprod.h"
#include "../../buffer/src/buffer.h"

#define SYMSYNC_DEBUG

// defined:
//  SYMSYNC()   name-mangling macro
//  FIRPFB()    firpfb macro
//  T           data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

// internal prototypes
void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q, T mf, T dmf);

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
    float q, q_hat, q_prime, tmp2;

    unsigned int v;
    float b_soft;
    unsigned int b;

    enum {SHIFT,SKIP,SKIP_PRIME,STUFF} state;
};

SYMSYNC() SYMSYNC(_create)(unsigned int _k, unsigned int _num_filters, T * _h, unsigned int _h_len)
{
    SYMSYNC() q = (SYMSYNC()) malloc(sizeof(struct SYMSYNC(_s)));
    q->k = _k;
    q->num_filters = _num_filters;

    // TODO: validate length
    q->h_len = (_h_len-1)/q->num_filters;
    
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

#ifdef SYMSYNC_DEBUG
    for (i=0; i<_h_len; i++) {
        printf("h(%3u) = ", i+1);
        PRINTVAL(_h[i]);
        printf(";\n");
    }
    for (i=0; i<_h_len; i++) {
        printf("dh(%3u) = ", i+1);
        PRINTVAL(dh[i]);
        printf(";\n");
    }
#endif

    q->mf  = FIRPFB(_create)(q->num_filters, _h, _h_len);
    q->dmf = FIRPFB(_create)(q->num_filters, dh, _h_len);

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

void SYMSYNC(_execute)(SYMSYNC() _q, T * _x, unsigned int _nx, T * _y, unsigned int *_ny)
{
    //
    T mf;   // matched filter output
    T dmf;  // derivative matched filter output

    unsigned int ny=0;

    unsigned int i=0;
    //for (i=0; i<_nx; i++) {
    while (i < _nx) {
        switch (_q->state) {
        case SHIFT:
            // 'shift' sample into state registers (normal operation)
            printf("symsync: shift\n");
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            break;
        case SKIP:
            printf("symsync: skip\n");
            // 'skip' input sample (shift in two values)
            // shift in first value
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            _q->state = SKIP_PRIME;
            //break;
            continue;
        case SKIP_PRIME:
            printf("symsync: skip (prime)\n");
            // 'skip' input sample (shift in two values)
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            _q->state = SHIFT;
            break;
        case STUFF:
            printf("symsync: stuff\n");
            // 'stuff' input sample (effectively repeat sample)
            _q->state = SHIFT;
            break;
        default:
            break;
        }

        // increment output counter
        _q->v++;

        // enable output
        if (_q->v >= _q->k) {
            _q->v -= _q->k;

            // compute MF/dMF outputs
            FIRPFB(_execute)(_q->mf,  _q->b, &mf);
            FIRPFB(_execute)(_q->dmf, _q->b, &dmf);
            _y[ny++] = mf;

            // run loop
            //  1.  compute timing error signal
            //  2.  filter error signal
            //  3.  increment filter bank index
            //  4.  quantize filter bank index
            SYMSYNC(_advance_internal_loop)(_q, mf, dmf);
        }
    }

    // return
    *_ny = ny;
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
    _q->tmp2 = 0.0f;
    _q->b_soft = 0.0f;
    _q->b = 0;
    _q->state = SHIFT;
    _q->v = 0;
}

void SYMSYNC(_estimate_timing)(SYMSYNC() _q, T * _v, unsigned int _n)
{
}

void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q, T mf, T dmf)
{
    //  1.  compute timing error signal
    _q->q = -( crealf(mf)*crealf(dmf) + cimagf(mf)*cimagf(dmf) )/2;

    //  2.  filter error signal
    _q->q_prime = (_q->q)*(_q->beta) + _q->tmp2;
    _q->q_hat = (_q->alpha)*(_q->q) + _q->q_prime;
    _q->tmp2 = _q->q_prime;

    //  3.  increment filter bank index
    _q->b_soft -= (_q->q_hat)*(_q->num_filters);

    //  4.  quantize filter bank index
    if (_q->b_soft < 0) {
        // underflow: skip next sample
        _q->state = SKIP;
        while (_q->b_soft < 0)
            _q->b_soft += _q->num_filters;
    } else if (_q->b_soft >= _q->num_filters) {
        // overflow: stuff previous sample
        _q->state = STUFF;
        while (_q->b_soft >= _q->num_filters)
            _q->b_soft -= _q->num_filters;
    } else {
        // normal operation
        _q->state = SHIFT;
    }
    _q->b = (unsigned int) (_q->b_soft);

    _q->b = 0;
    // assert(_q->b >= 0);
    // assert(_q->b < _q->num_filters);
}

