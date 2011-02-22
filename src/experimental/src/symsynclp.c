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
// Symbol synchronizer using interpolating polynomials
//
// References:
//  [Mengali:1997] Umberto Mengali and Aldo N. D'Andrea,
//      "Synchronization Techniques for Digital Receivers,"
//      Plenum Press, New York & London, 1997.
//  [Erup:1993] Lars Erup, Floyd M. Gardner, and Robert
//      A. Harris, "Interpolation in Digital Modems--Part II:
//      Implementation and Performance," IEEE Trans. on
//      Comm., vol. 41, no. 6, June, 1993.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG_SYMSYNCLP             0
#define DEBUG_SYMSYNCLP_PRINT       0
#define DEBUG_SYMSYNCLP_FILENAME    "symsynclp_internal_debug.m"
#define DEBUG_BUFFER_LEN            (1024)

#if DEBUG_SYMSYNCLP
void SYMSYNCLP(_output_debug_file)(SYMSYNCLP() _q);
#endif

// 
// internal method prototypes
// TODO : move to internal header
//

// defined:
//  SYMSYNCLP()   name-mangling macro
//  FIRFILT()    firpfb macro
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct SYMSYNCLP(_s) {
    unsigned int k;             // samples/symbol
    unsigned int order;         // polynomial order

    WINDOW() wmf;               // output matched filter window
    WINDOW() wdmf;              // output matched filter window
    TC * c;                     // interpolator coefficients, TODO : switch to float?
    TC * cprime;                // interpolator coefficients, TODO : switch to float?

    // timing error loop filter
    float bt;                   // loop filter bandwidth
    float alpha;                // percent of old error sample to retain
    float beta;                 // percent of new error sample to retain
    float q;                    // instantaneous timing error estimate
    float q_hat;                // filtered timing error estimate
    float q_prime;              // buffered timing error estimate

    // flow control
    float del;                  // input stride size (roughly k)
    float tau;                  // output flow control (enabled when tau >= 1)

    // lock
    int is_locked;              // synchronizer locked flag

#if 0
    fir_prototype p;    // prototype object
#endif

#if DEBUG_SYMSYNCLP
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
//  _order          :   polynomial order
SYMSYNCLP() SYMSYNCLP(_create)(unsigned int _k,
                               unsigned int _order)
{
    SYMSYNCLP() q = (SYMSYNCLP()) malloc(sizeof(struct SYMSYNCLP(_s)));
    q->k = _k;
    q->order = _order;

    // TODO : do not restrict order
#if 0
    if (q->order != 1) {
        fprintf(stderr,"error: symsynclp_xxxt_create(), order must be 1 (for now)\n");
        exit(1);
    }
#endif

    // allocate memory for post-filter
    q->wmf  = WINDOW(_create)(4);
    q->wdmf = WINDOW(_create)(4);
    q->c    = (TC*) malloc(4*sizeof(TC));
    q->cprime = (TC*) malloc(4*sizeof(TC));
    q->c[0] = 0.0f;
    q->c[1] = 1.0f;
    q->c[2] = 0.0f;
    q->c[3] = 0.0f;
    float mu = 0.0f;
    SYMSYNCLP(_compute_coefficients)(q, q->order, mu, q->c, q->cprime);
    
    // reset state and initialize loop filter
    SYMSYNCLP(_clear)(q);
    SYMSYNCLP(_set_lf_bw)(q, 0.01f);

    // unlock loop control
    q->is_locked = 0;

#if DEBUG_SYMSYNCLP
    q->debug_del   =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_tau   =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_bsoft =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_b     =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_q_hat =  windowf_create(DEBUG_BUFFER_LEN);
#endif

    return q;
}

// destroy synchronizer object
void SYMSYNCLP(_destroy)(SYMSYNCLP() _q)
{
#if DEBUG_SYMSYNCLP
    SYMSYNCLP(_output_debug_file)(_q);
#endif

    // destroy post-filter
    WINDOW(_destroy)(_q->wmf);
    WINDOW(_destroy)(_q->wdmf);
    free(_q->c);
    free(_q->cprime);

    free(_q);
}

// print synchronizer object internals
void SYMSYNCLP(_print)(SYMSYNCLP() _q)
{
    printf("symbol synchronizer [k: %u, order: %u]\n",
        _q->k, _q->order);
}

// execute synchronizer on input data array
//  _q      :   synchronizer object
//  _x      :   input data array
//  _nx     :   number of input samples
//  _y      :   output data array
//  _ny     :   number of samples written to output buffer
void SYMSYNCLP(_execute)(SYMSYNCLP() _q,
                         TI * _x,
                         unsigned int _nx,
                         TO * _y,
                         unsigned int *_ny)
{
    unsigned int i, ny=0, k=0;
    for (i=0; i<_nx; i++) {
        SYMSYNCLP(_step)(_q, _x[i], &_y[ny], &k);
        ny += k;
        //printf("%u\n",k);
    }
    *_ny = ny;
}

// set synchronizer loop filter bandwidth
//  _q      :   synchronizer object
//  _bt     :   bandwidth
void SYMSYNCLP(_set_lf_bw)(SYMSYNCLP() _q,
                           float _bt)
{
    // set loop filter bandwidth
    _q->bt = _bt;

    _q->alpha = 1.00f - 1.0f*(_q->bt);  // percent of old sample to retain
    _q->beta  = 0.02f * (_q->bt);       // percent of new sample to retain
}

// lock synchronizer object
void SYMSYNCLP(_lock)(SYMSYNCLP() _q)
{
    _q->is_locked = 1;
    _q->del = _q->k;    // fix step size to number of samples/symbol
}

// unlock synchronizer object
void SYMSYNCLP(_unlock)(SYMSYNCLP() _q)
{
    _q->is_locked = 0;
}

// clear synchronizer object
void SYMSYNCLP(_clear)(SYMSYNCLP() _q)
{
    // reset loop filter states
    _q->q_hat = 0.0f;
    _q->q_prime = 0.0f;

    _q->tau = 0.0f;
    _q->q_hat   = 0.0f;
    _q->q_prime = 0.0f;
    _q->del = (float)(_q->k);

    // clear post-filter windows
    WINDOW(_clear)(_q->wmf);
    WINDOW(_clear)(_q->wdmf);

    _q->is_locked = 0;
}

float SYMSYNCLP(_get_tau)(SYMSYNCLP() _q)
{
    return _q->tau;
}

// 
// internal methods
//

// compute coefficients
void SYMSYNCLP(_compute_coefficients)(SYMSYNCLP() _q,
                                      unsigned int _order,
                                      float _mu,
                                      float * _c,
                                      float * _cprime)
{
    _mu = 1.0f -_mu;
    float alpha = 0.5f;
    float mu2 = 0.0f;   // mu^2
    float mu3 = 0.0f;   // mu^3
    // TODO : make this more efficient
    switch (_order) {
    case 1:
        // linear interpolator
        // TODO : adjust length...
        _c[3] = 0.0f;
        _c[2] = _mu;
        _c[1] = 1.0f - _mu;
        _c[0] = 0.0f;
        break;
    case 2:
        // parabolic interpolator
        mu2 = _mu*_mu;
        _c[3] =  alpha*mu2 - alpha*_mu;
        _c[2] = -alpha*mu2 + (1+alpha)*_mu;
        _c[1] = -alpha*mu2 - (1-alpha)*_mu + 1;
        _c[0] =  alpha*mu2 - alpha*_mu;
        break;
    case 3:
        mu2 = _mu * _mu;  // mu^2
        mu3 = _mu * mu2;  // mu^3
        _c[3] =  mu3/6           - _mu/6;
        _c[2] = -mu3/2   + mu2/2 + _mu;
        _c[1] =  mu3/2   - mu2   - _mu/2  + 1;
        _c[0] = -mu3/6   + mu2/2 - _mu/3;
        break;
    default:
        fprintf(stderr,"error: symsynclp_xxxt_compute_coefficients(), invalid order %u\n", _order);
        exit(1);
    break;
    }

    // compute derivative
    _cprime[0] = ( _c[1] - _c[3] ) * 0.5f;
    _cprime[1] = ( _c[2] - _c[0] ) * 0.5f;
    _cprime[2] = ( _c[3] - _c[1] ) * 0.5f;
    _cprime[3] = ( _c[0] - _c[2] ) * 0.5f;
}

// advance synchronizer's internal loop filter
//  _q      :   synchronizer object
//  _mf     :   matched-filter output
//  _dmf    :   derivative matched-filter output
void SYMSYNCLP(_advance_internal_loop)(SYMSYNCLP() _q,
                                     TO _mf,
                                     TO _dmf)
{
    //  1.  compute timing error signal, clipping large levels
    _q->q = crealf(_mf)*crealf(_dmf) + cimagf(_mf)*cimagf(_dmf);
    if (_q->q > 1.0f)       _q->q =  1.0f;
    else if (_q->q < -1.0f) _q->q = -1.0f;

    //  2.  filter error signal: retain large percent (alpha) of
    //      old estimate and small percent (beta) of new estimate
    _q->q_hat = (_q->q)*(_q->beta) + (_q->q_prime)*(_q->alpha);
    _q->q_prime = _q->q_hat;
    _q->del = (float)(_q->k) + _q->q_hat;
    //_q->del = (float)(_q->k);

#if DEBUG_SYMSYNCLP_PRINT
    printf("q : %12.8f, del : %12.8f, q_hat : %12.8f\n", _q->q, _q->del, _q->q_hat);
#endif
}

// step synchronizer (execute on single input)
//
//  _q      :   synchronizer object
//  _x      :   input sample
//  _y      :   output sample buffer
//  _ny     :   number of output samples written to buffer
void SYMSYNCLP(_step)(SYMSYNCLP() _q,
                      TI _x,
                      TO * _y,
                      unsigned int *_ny)
{
    TO mf;      // matched-filter output
    TO dmf;     // derivative matched-filter output

    WINDOW(_push)(_q->wmf,  _x);

    unsigned int n = 0;

    while (_q->tau < 1.0f) {
#if DEBUG_SYMSYNCLP
        // save debugging variables
        windowf_push(_q->debug_del,    _q->del);
        windowf_push(_q->debug_tau,    _q->tau);
        windowf_push(_q->debug_q_hat,  _q->q_hat);
        // printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->b_soft);
#endif
        // 
        // compute post-filter outputs
        //

        // TODO : compute new coefficients
        SYMSYNCLP(_compute_coefficients)(_q, _q->order, _q->tau, _q->c, _q->cprime);

        TO * r;

        // MF : read buffer an execute dot product
        WINDOW(_read)(_q->wmf, &r);
        DOTPROD(_run4)(_q->c, r, 4, &mf);
        
        // dMF : read buffer an execute dot product
        if (!_q->is_locked) {
            DOTPROD(_run4)(_q->cprime, r, 4, &dmf);
        }

        // store matched-filter output
        _y[n] = mf;
#if DEBUG_SYMSYNCLP_PRINT
        printf("mf : %12.8f + j*%12.8f\n", crealf(mf), cimagf(mf));
#endif

        // apply loop filter
        if (!_q->is_locked)
            SYMSYNCLP(_advance_internal_loop)(_q, mf, dmf);

        // update flow control variables
        _q->tau     += _q->del;

        // increment output counter
        n++;
    }

    // decrement flow control variables
    _q->tau     -= 1.0f;

    // set number of outputs that were written to buffer
    *_ny = n;
}


//
// internal debugging
//

#if DEBUG_SYMSYNCLP
// print results to output debugging file
void SYMSYNCLP(_output_debug_file)(SYMSYNCLP() _q)
{
    FILE * fid = fopen(DEBUG_SYMSYNCLP_FILENAME, "w");
    if (!fid) {
        fprintf(stderr,"error: symsync_xxxt_output_debug_file(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s, auto-generated file\n\n", DEBUG_SYMSYNCLP_FILENAME);

    fprintf(fid,"order = %u\n",_q->order);
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

    // print filtered error signal
    fprintf(fid,"q_hat = zeros(1,n);\n");
    windowf_read(_q->debug_q_hat, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"q_hat(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"t=1:n;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,tau);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([t(1) t(end) 0 1]);\n");
    fprintf(fid,"legend('tau',0);\n");
    fprintf(fid,"xlabel('Symbol Index')\n");
    fprintf(fid,"ylabel('Delay estimate')\n");
    fprintf(fid,"%% done.\n");
    fclose(fid);
    printf("symsync: internal results written to %s.\n", DEBUG_SYMSYNCLP_FILENAME);
}
#endif


