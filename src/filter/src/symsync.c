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
// Symbol synchronizer
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// use theoretical 2nd-order integrating PLL filter?
#define SYMSYNC_USE_PLL         0

#define DEBUG_SYMSYNC           0
#define DEBUG_SYMSYNC_PRINT     0
#define DEBUG_SYMSYNC_FILENAME  "symsync_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1024)

#if DEBUG_SYMSYNC
void SYMSYNC(_output_debug_file)(SYMSYNC() _q);
#endif


// defined:
//  SYMSYNC()   name-mangling macro
//  FIRPFB()    firpfb macro
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct SYMSYNC(_s) {
    TC * h;                     // matched filter
    TC * dh;                    // derivative matched filter
    unsigned int h_len;         // matched filter length
    unsigned int k;             // samples/symbol
    unsigned int num_filters;   // number of filters in the bank

    FIRPFB() mf;                // matched filter bank
    FIRPFB() dmf;               // derivative matched filter bank

    // timing error loop filter
    float bt;                   // loop filter bandwidth
    float alpha;                // percent of old error sample to retain
    float beta;                 // percent of new error sample to retain
    float q;                    // instantaneous timing error estimate
    float q_hat;                // filtered timing error estimate
    float q_prime;              // buffered timing error estimate

    float b_soft;               // soft filterbank index
    int b;                      // hard filterbank index
    float k_inv;                // 1/k

    // flow control
    float del;                  // input stride size (roughly k)
    float tau;                  // output flow control (enabled when tau >= 1)

    // lock
    int is_locked;              // synchronizer locked flag

#if SYMSYNC_USE_PLL
    // phase-locked loop
    float B[3];
    float A[3];
    iirfiltsos_rrrf pll;
#endif

#if 0
    fir_prototype p;    // prototype object
#endif

#if DEBUG_SYMSYNC
    windowf debug_del;
    windowf debug_tau;
    windowf debug_bsoft;
    windowf debug_b;
    windowf debug_q_hat;
#endif
};

// create synchronizer object
//
//  _k              :   samples per symbol
//  _num_filters    :   number of filters in the bank
//  _h              :   matched filter coefficients
//  _h_len          :   length of matched filter
SYMSYNC() SYMSYNC(_create)(unsigned int _k,
                           unsigned int _num_filters,
                           TC * _h,
                           unsigned int _h_len)
{
    SYMSYNC() q = (SYMSYNC()) malloc(sizeof(struct SYMSYNC(_s)));
    q->k = _k;
    q->k_inv = 1.0f / (float)(q->k);
    q->num_filters = _num_filters;

    // TODO: validate length
    q->h_len = (_h_len-1)/q->num_filters;
    
    // compute derivative filter
    TC dh[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++) {
        if (i==0) {
            dh[i] = _h[i+1] - _h[_h_len-1];
        } else if (i==_h_len-1) {
            dh[i] = _h[0]   - _h[i-1];
        } else {
            dh[i] = _h[i+1] - _h[i-1];
        }

        // apply scaling factor (derivative approximation is
        // scaled by the number of filters in the bank)
        dh[i] *= (float)_num_filters / 16.0f;
    }
    q->mf  = FIRPFB(_create)(q->num_filters, _h, _h_len);
    q->dmf = FIRPFB(_create)(q->num_filters, dh, _h_len);

    // reset state and initialize loop filter
#if SYMSYNC_USE_PLL
    q->A[0] = 1.0f;     q->B[0] = 0.0f;
    q->A[1] = 0.0f;     q->B[1] = 0.0f;
    q->A[2] = 0.0f;     q->B[2] = 0.0f;
    q->pll = iirfiltsos_rrrf_create(q->B, q->A);
#endif
    SYMSYNC(_clear)(q);
    SYMSYNC(_set_lf_bw)(q, 0.01f);

    // unlock loop control
    q->is_locked = 0;

#if DEBUG_SYMSYNC
    q->debug_del   =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_tau   =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_bsoft =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_b     = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_q_hat =  windowf_create(DEBUG_BUFFER_LEN);
#endif

    return q;
}

// destroy synchronizer object
void SYMSYNC(_destroy)(SYMSYNC() _q)
{
#if DEBUG_SYMSYNC
    SYMSYNC(_output_debug_file)(_q);
#endif

#if SYMSYNC_USE_PLL
    iirfiltsos_rrrf_destroy(_q->pll);
#endif

    FIRPFB(_destroy)(_q->mf);
    FIRPFB(_destroy)(_q->dmf);
    free(_q);
}

// print synchronizer object internals
void SYMSYNC(_print)(SYMSYNC() _q)
{
    printf("symbol synchronizer [k: %u, num_filters: %u]\n",
        _q->k, _q->num_filters);
    FIRPFB(_print)(_q->mf);
    FIRPFB(_print)(_q->dmf);
}

// execute synchronizer on input data array
//  _q      :   synchronizer object
//  _x      :   input data array
//  _nx     :   number of input samples
//  _y      :   output data array
//  _ny     :   number of samples written to output buffer
void SYMSYNC(_execute)(SYMSYNC() _q,
                       TI * _x,
                       unsigned int _nx,
                       TO * _y,
                       unsigned int *_ny)
{
    unsigned int i, ny=0, k=0;
    for (i=0; i<_nx; i++) {
        SYMSYNC(_step)(_q, _x[i], &_y[ny], &k);
        ny += k;
        //printf("%u\n",k);
    }
    *_ny = ny;
}

// set synchronizer loop filter bandwidth
//  _q      :   synchronizer object
//  _bt     :   bandwidth
void SYMSYNC(_set_lf_bw)(SYMSYNC() _q,
                         float _bt)
{
    // set loop filter bandwidth
    _q->bt = _bt;

#if SYMSYNC_USE_PLL
    float zeta = 1.1f;
    float K = 1000.0f;
    iirdes_pll_active_lag(0.5f*_q->bt, zeta, K, _q->B, _q->A);
    iirfiltsos_rrrf_set_coefficients(_q->pll, _q->B, _q->A);
#else
    _q->alpha = 1.00f - (_q->bt);   // percent of old sample to retain
    _q->beta  = 0.22f * (_q->bt);   // percent of new sample to retain
#endif
}

// lock synchronizer object
void SYMSYNC(_lock)(SYMSYNC() _q)
{
    _q->is_locked = 1;
    _q->del = _q->k;    // fix step size to number of samples/symbol
}

// unlock synchronizer object
void SYMSYNC(_unlock)(SYMSYNC() _q)
{
    _q->is_locked = 0;
}

// clear synchronizer object
void SYMSYNC(_clear)(SYMSYNC() _q)
{
    // reset internal filterbank states
    FIRPFB(_clear)(_q->mf);
    FIRPFB(_clear)(_q->dmf);

    // reset loop filter states
    _q->q_hat = 0.0f;
    _q->q_prime = 0.0f;
    _q->b_soft = 0.0f;
    _q->b = 0;

    _q->tau = 0.0f;
    _q->q_hat   = 0.0f;
    _q->q_prime = 0.0f;
    _q->del = (float)(_q->k);
#if SYMSYNC_USE_PLL
    iirfiltsos_rrrf_clear(_q->pll);
#endif

    _q->is_locked = 0;
}

void SYMSYNC(_estimate_timing)(SYMSYNC() _q, TI * _v, unsigned int _n)
{
}

float SYMSYNC(_get_tau)(SYMSYNC() _q)
{
    return _q->tau;
}

// 
// internal methods
//

// advance synchronizer's internal loop filter
//  _q      :   synchronizer object
//  _mf     :   matched-filter output
//  _dmf    :   derivative matched-filter output
void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q,
                                     TO _mf,
                                     TO _dmf)
{
    //  1.  compute timing error signal, clipping large levels
    _q->q = 0.5f*(crealf(_mf)*crealf(_dmf) + cimagf(_mf)*cimagf(_dmf));
    if (_q->q > 1.0f)       _q->q =  1.0f;
    else if (_q->q < -1.0f) _q->q = -1.0f;

    //  2.  filter error signal: retain large percent (alpha) of
    //      old estimate and small percent (beta) of new estimate
#if SYMSYNC_USE_PLL
    iirfiltsos_rrrf_execute(_q->pll, _q->q, &_q->q_hat);
    _q->del = (float)(_q->k) + _q->q_hat;
    //_q->del = (float)(_q->k) * (1 + _q->q_hat);
#else
    _q->q_hat = (_q->q)*(_q->beta) + (_q->q_prime)*(_q->alpha);
    _q->q_prime = _q->q_hat;
    _q->del = (float)(_q->k) + _q->q_hat;
#endif

#if DEBUG_SYMSYNC_PRINT
    printf("q : %12.8f, del : %12.8f, q_hat : %12.8f\n", _q->q, _q->del, _q->q_hat);
#endif
}

// step synchronizer (execute on single input)
//
//  _q      :   synchronizer object
//  _x      :   input sample
//  _y      :   output sample buffer
//  _ny     :   number of output samples written to buffer
void SYMSYNC(_step)(SYMSYNC() _q,
                    TI _x,
                    TO * _y,
                    unsigned int *_ny)
{
    // push sample into MF and dMF filterbanks
    FIRPFB(_push)(_q->mf,  _x);
    FIRPFB(_push)(_q->dmf, _x);

    TO mf;      // matched-filter output
    TO dmf;     // derivative matched-filter output

    unsigned int n = 0;

    while (_q->b < _q->num_filters) {
#if DEBUG_SYMSYNC
        // save debugging variables
        windowf_push(_q->debug_del,    _q->del);
        windowf_push(_q->debug_tau,    _q->tau);
        windowf_push(_q->debug_bsoft,  _q->b_soft);
        windowf_push(_q->debug_b,      _q->b);
        windowf_push(_q->debug_q_hat,  _q->q_hat);
        // printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->b_soft);
#endif
        // compute filterbank outputs
        FIRPFB(_execute)(_q->mf,  _q->b, &mf);
        if (!_q->is_locked)
            FIRPFB(_execute)(_q->dmf, _q->b, &dmf);
        mf *= _q->k_inv;

        // store matched-filter output
        _y[n] = mf;
#if DEBUG_SYMSYNC_PRINT
        printf("mf : %12.8f + j*%12.8f\n", crealf(mf), cimagf(mf));
#endif

        // apply loop filter
        if (!_q->is_locked)
            SYMSYNC(_advance_internal_loop)(_q, mf, dmf);

        // update flow control variables
#if SYMSYNC_USE_PLL
        _q->tau     = _q->del;
#else
        _q->tau     += _q->del;
#endif
        _q->b_soft  =  _q->tau * (float)(_q->num_filters);
        _q->b       =  (int)roundf(_q->b_soft);

        // increment output counter
        n++;
    }

    // decrement flow control variables
    _q->tau     -= 1.0f;
    _q->b_soft  -= (float)(_q->num_filters);
    _q->b       -= _q->num_filters;

    // set number of outputs that were written to buffer
    *_ny = n;
}


//
// internal debugging
//

#if DEBUG_SYMSYNC
// print results to output debugging file
void SYMSYNC(_output_debug_file)(SYMSYNC() _q)
{
    FILE * fid = fopen(DEBUG_SYMSYNC_FILENAME, "w");
    if (!fid) {
        fprintf(stderr,"error: symsync_xxxt_output_debug_file(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s, auto-generated file\n\n", DEBUG_SYMSYNC_FILENAME);

    fprintf(fid,"num_filters = %u\n",_q->num_filters);
    fprintf(fid,"k = %u\n",_q->k);
    fprintf(fid,"\n\n");

    fprintf(fid,"alpha = %12.5e\n",_q->alpha);
    fprintf(fid,"beta = %12.5e\n",_q->beta);
    fprintf(fid,"\n\n");

    fprintf(fid,"n = %u;\n", DEBUG_BUFFER_LEN);
    float * r;
    unsigned int i;

    // print del buffer
    fprintf(fid,"del = zeros(1,n);\n");
    windowf_read(_q->debug_del, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"del(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print tau buffer
    fprintf(fid,"tau = zeros(1,n);\n");
    windowf_read(_q->debug_tau, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"tau(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print bsoft buffer
    fprintf(fid,"b_soft = zeros(1,n);\n");
    windowf_read(_q->debug_bsoft, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"b_soft(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print b (filterbank index) buffer
    fprintf(fid,"b = zeros(1,n);\n");
    windowf_read(_q->debug_b, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"b(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print filtered error signal
    fprintf(fid,"q_hat = zeros(1,n);\n");
    windowf_read(_q->debug_q_hat, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"q_hat(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"t=1:n;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(t,b,'Color',[0.5 0.5 0.5]);\n");
    fprintf(fid,"plot(t,b_soft,'LineWidth',2,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([t(1) t(end) -1 num_filters]);\n");
    fprintf(fid,"legend('b','b (soft)',0);\n");
    fprintf(fid,"xlabel('Symbol Index')\n");
    fprintf(fid,"ylabel('Polyphase Filter Index')\n");
    fprintf(fid,"%% done.\n");
    fclose(fid);
    printf("symsync: internal results written to %s.\n", DEBUG_SYMSYNC_FILENAME);
}
#endif


