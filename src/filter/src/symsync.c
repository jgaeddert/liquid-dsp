//
// Symbol synchronizer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.composite.h"

#ifndef SYMSYNC_DEBUG
#define SYMSYNC_DEBUG 0
#endif

#define SYMSYNC_DEBUG_FILENAME "debug_symsync_internal.m"

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
    float eta;      // additional correction factor
    float bt;
    float xi, ac, alpha, beta;
    float q, q_hat, q_prime, tmp2;

    unsigned int v;
    float b_soft;
    int b;

    enum {SHIFT,SKIP,SKIP_PRIME,STUFF} state;

#if SYMSYNC_DEBUG
    FILE* fid;
    unsigned int n_debug;
#endif
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
    q->mf  = FIRPFB(_create)(q->num_filters, _h, _h_len);
    q->dmf = FIRPFB(_create)(q->num_filters, dh, _h_len);

    // reset state and initialize loop filter
    q->ac = 1.0f;
    q->delay = 0.5f* (float) (_h_len-1) / (float)(q->num_filters);
    SYMSYNC(_clear)(q);
    SYMSYNC(_set_lf_bw)(q, 0.01f);

#if SYMSYNC_DEBUG
    q->n_debug = 0;
    q->fid = fopen(SYMSYNC_DEBUG_FILENAME, "w");
    fprintf(q->fid, "%% %s, auto-generated file\n\n", SYMSYNC_DEBUG_FILENAME);

    fprintf(q->fid,"num_filters = %u\n",q->num_filters);
    fprintf(q->fid,"k = %u\n",q->k);
    fprintf(q->fid,"\n\n");

    fprintf(q->fid,"bt = %12.5e\n",q->bt);
    fprintf(q->fid,"xi = %12.5e\n",q->xi);
    fprintf(q->fid,"zeta = %12.5e\n",q->zeta);
    fprintf(q->fid,"delay = %12.5e\n",q->delay);
    fprintf(q->fid,"eta = %12.5e\n",q->eta);
    fprintf(q->fid,"alpha = %12.5e\n",q->alpha);
    fprintf(q->fid,"beta = %12.5e\n",q->beta);
    fprintf(q->fid,"\n\n");
#endif

    return q;
}

void SYMSYNC(_destroy)(SYMSYNC() _q)
{
#if SYMSYNC_DEBUG
    fprintf(_q->fid, "\n\n");
    fprintf(_q->fid, "t=0:%u;\n",_q->n_debug-1);
    fprintf(_q->fid, "i_skip  = find(state==%d);\n", SKIP);
    fprintf(_q->fid, "i_stuff = find(state==%d);\n", STUFF);
    fprintf(_q->fid, "figure;\n");
    fprintf(_q->fid, "hold on;\n");
    fprintf(_q->fid, "plot(t,b,'Color',[0.5 0.5 0.5]);\n");
    fprintf(_q->fid, "plot(t,b_soft,'LineWidth',2,'Color',[0 0.25 0.5]);\n");
    fprintf(_q->fid, "if length(i_skip>0),  plot(t(i_skip), b_soft(i_skip),'rx'); end;\n");
    fprintf(_q->fid, "if length(i_stuff>0), plot(t(i_stuff),b_soft(i_stuff),'bx'); end;\n");
    fprintf(_q->fid, "hold off;\n");
    fprintf(_q->fid, "grid on;\n");
    fprintf(_q->fid, "axis([t(1) t(end) -1 num_filters]);\n");
    fprintf(_q->fid, "legend('b','b (soft)',0);\n");
    fprintf(_q->fid, "xlabel('Symbol Index')\n");
    fprintf(_q->fid, "ylabel('Polyphase Filter Index')\n");
    fprintf(_q->fid, "%% done.\n");
    fclose(_q->fid);
    printf("symsync: internal results written to %s.\n", SYMSYNC_DEBUG_FILENAME);
#endif

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
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            break;
        case SKIP:
            // 'skip' input sample (shift in two values)
            // shift in first value
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            _q->state = SKIP_PRIME;
            //break;
            continue;
        case SKIP_PRIME:
            // 'skip' input sample (shift in two values)
            FIRPFB(_push)(_q->mf, _x[i]);
            FIRPFB(_push)(_q->dmf,_x[i]);
            i++;
            _q->state = SHIFT;
            break;
        case STUFF:
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
    //_q->zeta = _q->delay + 1.0f;
    //_q->bt = (_q->bt) / (1 + log10f(_q->zeta));
    //_q->xi = 2 * (_q->xi) * (_q->zeta);

    // TODO: formulate empirical relationship based upon simulation
    // of symbol synchronizer
    if (_q->delay < 1)
        _q->eta = 1.0f;
    else if (_q->delay < 11)
        //_q->eta = expf(-0.044335f*_q->delay -0.022152f);
        _q->eta = expf(-0.027502f*_q->delay + 0.010818f);
    else
        _q->eta = expf(-0.054050f*_q->delay +0.288521f);

    _q->eta = sqrtf(_q->eta);

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
    _q->q_prime = (_q->q)*(_q->beta) + (_q->tmp2)*(_q->eta);
    _q->q_hat = (_q->alpha)*(_q->q) + _q->q_prime;
    _q->tmp2 = _q->q_prime;

    //  3.  increment filter bank index
    _q->b_soft -= (_q->q_hat)*(_q->num_filters);
    _q->b = (int) roundf(_q->b_soft);

    //  4.  quantize filter bank index
    if (_q->b < 0) {
        // underflow: skip next sample
        _q->state = SKIP;
        while (_q->b < 0) {
            _q->b       += _q->num_filters;
            _q->b_soft  += _q->num_filters;
        }
    } else if (_q->b >= _q->num_filters) {
        // overflow: stuff previous sample
        _q->state = STUFF;
        while (_q->b >= _q->num_filters) {
            _q->b       -= _q->num_filters;
            _q->b_soft  -= _q->num_filters;
        }
    } else {
        // normal operation
        _q->state = SHIFT;
    }

    if (_q->b >= _q->num_filters) {
        printf("warning: symsync_advance_internal_loop(), filterbank index error\n");
        _q->b = (_q->b) % (_q->num_filters);
    }
    // assert(_q->b >= 0);
    // assert(_q->b < _q->num_filters);

#if SYMSYNC_DEBUG
    fprintf(_q->fid,"b_soft(%4u) = %12.5e;\n",_q->n_debug+1,_q->b_soft);
    fprintf(_q->fid,"b(%4u) = %d;\n",_q->n_debug+1,_q->b);
    fprintf(_q->fid,"state(%4u) = %d;\n", _q->n_debug+1,(int)(_q->state));
    _q->n_debug++;
#endif
}

