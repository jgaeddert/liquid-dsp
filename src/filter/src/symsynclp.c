/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#define DEBUG_SYMSYNCLP             1
#define DEBUG_SYMSYNCLP_PRINT       0
#define DEBUG_SYMSYNCLP_FILENAME    "symsynclp_internal_debug.m"
#define DEBUG_BUFFER_LEN            (1024)

#if DEBUG_SYMSYNCLP
void SYMSYNCLP(_output_debug_file)(SYMSYNCLP() _q);
#endif


// defined:
//  SYMSYNCLP()     name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  WINDOW()        window macro
//  PRINTVAL()      print macro

struct SYMSYNCLP(_s) {
    unsigned int k;     // samples/symbol
    unsigned int p;     // polynomial order
    float * x;          // ...
    float * yi, * yq;
    float * pi, * pq;
    WINDOW() wmf;       // matched filter window

    // timing error loop filter
    float bt;       // filter bandwidth
    float alpha;    // percent of old error sample to retain
    float beta;     // percent of new error sample to retain
    float q;        // instantaneous timing error estimate
    float q_hat;    // filtered timing error estimate
    float q_prime;  // buffered timing error estimate

    unsigned int v;
    float b_soft;
    int b;
    float k_inv;

    // control
    float r;        // rate
    float tau;
    float del;

#if DEBUG_SYMSYNCLP
    FILE * fid;
    windowf debug_tau;
    windowf debug_delta;
    windowf debug_q_hat;
#endif
};

SYMSYNCLP() SYMSYNCLP(_create)(unsigned int _k,
                               unsigned int _p)
{
    SYMSYNCLP() q = (SYMSYNCLP()) malloc(sizeof(struct SYMSYNCLP(_s)));
    q->k = _k;  // samples/symbol
    q->p = _p;  // polynomial order

    // validate inputs
    if (q->p < 2) {
        printf("error: symsynclp_xxxt_create(), polynomial order must be at least 2\n");
        exit(0);
    }
    
    // create matched filter output window
    q->wmf  = WINDOW(_create)(q->p);

    // create buffers
    q->x  = (float*) malloc((q->p)*sizeof(float));
    q->yi = (float*) malloc((q->p)*sizeof(float));
    q->yq = (float*) malloc((q->p)*sizeof(float));
    q->pi = (float*) malloc((q->p)*sizeof(float));
    q->pq = (float*) malloc((q->p)*sizeof(float));

    // reset state and initialize loop filter
    SYMSYNCLP(_clear)(q);
    SYMSYNCLP(_set_lf_bw)(q, 0.01f);

#if DEBUG_SYMSYNCLP
    q->debug_tau   =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_delta =  windowf_create(DEBUG_BUFFER_LEN);
    q->debug_q_hat =  windowf_create(DEBUG_BUFFER_LEN);
#endif

    return q;
}

void SYMSYNCLP(_destroy)(SYMSYNCLP() _q)
{
#if DEBUG_SYMSYNCLP
    SYMSYNCLP(_output_debug_file)(_q);
    windowf_destroy(_q->debug_tau);
    windowf_destroy(_q->debug_delta);
    windowf_destroy(_q->debug_q_hat);
#endif
    WINDOW(_destroy)(_q->wmf);
    free(_q->x);
    free(_q->yi);
    free(_q->yq);
    free(_q->pi);
    free(_q->pq);
    free(_q);
}

void SYMSYNCLP(_print)(SYMSYNCLP() _q)
{
    printf("symbol synchronizer (Lagrange polynomials) [k: %u, order : %u]\n",
        _q->k, _q->p);
}

void SYMSYNCLP(_clear)(SYMSYNCLP() _q)
{
    // reset internal filterbank states

    WINDOW(_clear)(_q->wmf);

    // reset loop filter states
    _q->q_hat = 0.0f;
    _q->q_prime = 0.0f;
    _q->b_soft = 0.0f;
    _q->b = 0;
    _q->v = 0;

    _q->tau = 0.0f;
    _q->q_hat   = 0.0f;
    _q->q_prime = 0.0f;
    _q->del = (float)(_q->k);
}

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

void SYMSYNCLP(_set_lf_bw)(SYMSYNCLP() _q, float _bt)
{
    // set loop filter bandwidth
    _q->bt = _bt;

    _q->alpha = 1.00f - (_q->bt);   // percent of old sample to retain
    _q->beta  = 0.22f * (_q->bt);   // percent of new sample to retain
}

void SYMSYNCLP(_estimate_timing)(SYMSYNCLP() _q, TI * _v, unsigned int _n)
{
}

void SYMSYNCLP(_advance_internal_loop)(SYMSYNCLP() _q, TO mf, TO dmf)
{
    //  1.  compute timing error signal, clipping large levels
    _q->q = tanhf( 0.5f*(crealf(mf)*crealf(dmf) + cimagf(mf)*cimagf(dmf)) );

    //  2.  filter error signal: retain large percent (alpha) of
    //      old estimate and small percent (beta) of new estimate
    _q->q_hat = (_q->q)*(_q->beta) + (_q->q_prime)*(_q->alpha);
    _q->q_prime = _q->q_hat;

    _q->del -= _q->q_hat;

#if DEBUG_SYMSYNCLP_PRINT
    printf("del : %12.8f, q_hat : %12.8f\n", _q->del, _q->q_hat);
#endif
}

//
// internal debugging
//

#if DEBUG_SYMSYNCLP
void SYMSYNCLP(_output_debug_file)(SYMSYNCLP() _q)
{
    FILE * fid = fopen(DEBUG_SYMSYNCLP_FILENAME, "w");
    fprintf(fid, "%% %s, auto-generated file\n\n", DEBUG_SYMSYNCLP_FILENAME);

    fprintf(fid,"k = %u\n",_q->k);
    fprintf(fid,"\n\n");

    fprintf(fid,"alpha = %12.5e\n",_q->alpha);
    fprintf(fid,"beta = %12.5e\n",_q->beta);
    fprintf(fid,"\n\n");

    fprintf(fid,"n = %u;\n", DEBUG_BUFFER_LEN);
    float * r;
    unsigned int i;

    // print
    fprintf(fid,"tau = zeros(1,n);\n");
    windowf_read(_q->debug_tau, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"tau(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print delta buffer (timing frequency)
    fprintf(fid,"delta = zeros(1,n);\n");
    windowf_read(_q->debug_delta, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"delta(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print filtered error signal
    fprintf(fid,"q_hat = zeros(1,n);\n");
    windowf_read(_q->debug_q_hat, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"q_hat(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    fclose(fid);
    printf("symsync: internal results written to %s.\n", DEBUG_SYMSYNCLP_FILENAME);
}
#endif

void SYMSYNCLP(_step)(SYMSYNCLP() _q, TI _x, TO * _y, unsigned int *_ny)
{
    WINDOW(_push)(_q->wmf,_x);
    TO mf=0;
    TO dmf=0;

    unsigned int n = 0;

    while (_q->tau < 1.0f) {
#if DEBUG_SYMSYNC
        windowf_push(_q->debug_tau,  _q->tau);
        //uiwindow_push(_q->debug_b,     _q->b);
        windowf_push(_q->debug_delta,  _q->del);
        windowf_push(_q->debug_q_hat,  _q->q_hat);
        // printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->b_soft);
#endif
        // compute interpolants
        //FIRPFB(_execute)(_q->mf,  _q->b, &mf);
        //FIRPFB(_execute)(_q->dmf, _q->b, &dmf);

        unsigned int i;
        TO * r;
        WINDOW(_read)(_q->wmf, &r);
        for (i=0; i<_q->p; i++) {
            _q->x[i]  = ((float)(i) - (float)(_q->p) + 1.0f) / ((float)(_q->k));
            _q->yi[i] = crealf(r[i]);
            _q->yq[i] = cimagf(r[i]);
        }

        POLY(_fit)(_q->x, _q->yi, _q->p, _q->pi, _q->p);
        POLY(_fit)(_q->x, _q->yq, _q->p, _q->pq, _q->p);

        mf = POLY(_val)(_q->pi, _q->p, _q->tau) +
             POLY(_val)(_q->pq, _q->p, _q->tau) * _Complex_I;

        // differentiate polynomial
        for (i=1; i<_q->p; i++) {
            _q->pi[i] *= (float)(i);
            _q->pq[i] *= (float)(i);
        }
        dmf = POLY(_val)(_q->pi+1, _q->p-1, _q->tau) +
              POLY(_val)(_q->pq+1, _q->p-1, _q->tau) * _Complex_I;

        dmf *= 0.1f;

        // store output
        _y[n] = mf;
#if 1//DEBUG_SYMSYNC_PRINT
        printf("mf : %12.8f + j*%12.8f\n", crealf(mf), cimagf(mf));
        printf("     %12.8f + j*%12.8f\n", crealf(dmf), cimagf(dmf));
#endif

        // apply loop filter
        SYMSYNCLP(_advance_internal_loop)(_q, mf, dmf);

        _q->tau     += _q->del;

        n++;
    }


    _q->tau     -= 1.0f;
    //_q->b_soft  -= (float)(_q->num_filters);
    //_q->b       -= _q->num_filters;
    *_ny = n;
}

