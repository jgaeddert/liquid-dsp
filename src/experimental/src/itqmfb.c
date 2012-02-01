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
// iterative tree quadrature mirror filterbank (ITQMFB)
//
// References:
//   Vaidyanathan, P. P., "Multirate Systems and Filter Banks,"
//   1993, Prentice Hall, Section 5.8
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  ITQMFB()        name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  QMFB()          quadrature mirror filter bank (qmfb) macro
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

#define ITQMFB_DEBUG 0

struct ITQMFB(_s) {
    float * h;          // filter prototype
    unsigned int m;     // primitive filter length
    unsigned int h_len; // actual filter length: h_len = 4*m+1
    float beta;         // filter bandwidth/stop-band attenuation
    int type;           // analyzer/synthesizer

    unsigned int num_layers;
    unsigned int num_banks;
    unsigned int num_channels;

    // QMFB input/output buffer(s)
    TI * buffer0;
    TI * buffer1;

    QMFB() * bank;
};

ITQMFB() ITQMFB(_create)(unsigned int _n, unsigned int _m, float _As, int _type)
{
    ITQMFB() f = (ITQMFB()) malloc(sizeof(struct ITQMFB(_s)));

    f->num_layers = _n;
    f->num_channels = 1<<_n;
    f->num_banks  = f->num_channels - 1;
    f->type = _type;

    // allocate memory for banks
    f->bank = (QMFB()*) malloc((f->num_banks)*sizeof(QMFB()));

    // allocate memory for buffers
    f->buffer0 = (TI*) malloc((f->num_channels)*sizeof(TI));
    f->buffer1 = (TI*) malloc((f->num_channels)*sizeof(TI));

    // create banks
    unsigned int i;
    for (i=0; i<f->num_banks; i++)
        f->bank[i] = QMFB(_create)(_m, _As, _type);

    return f;
}

ITQMFB() ITQMFB(_recreate)(ITQMFB() _f, unsigned int _h_len)
{
    // TODO implement this method
    printf("error: itqmfb_%s_recreate(), method not supported yet\n", EXTENSION_FULL);
    exit(1);
    return NULL;
}

void ITQMFB(_destroy)(ITQMFB() _f)
{
    // destroy banks
    unsigned int i;
    for (i=0; i<_f->num_banks; i++)
        QMFB(_destroy)(_f->bank[i]);
    free(_f->bank);
    free(_f->buffer0);
    free(_f->buffer1);
    free(_f);
}

void ITQMFB(_print)(ITQMFB() _f)
{
    printf("iterative tree quadrature mirror filterbank:\n");
    printf("    num channels    :   %u\n", _f->num_channels);
    printf("    num banks       :   %u\n", _f->num_banks);
}

void ITQMFB(_clear)(ITQMFB() _f)
{
    unsigned int i;
    for (i=0; i<_f->num_banks; i++)
        QMFB(_clear)(_f->bank[i]);

    // clear buffers (not really necessary)
    for (i=0; i<_f->num_channels; i++) {
        _f->buffer0[i] = 0;
        _f->buffer1[i] = 0;
    }
}

void ITQMFB(_execute)(ITQMFB() _q,
                      TO * _x,
                      TO * _y)
{
    if (_q->type == LIQUID_ITQMFB_ANALYZER)
        ITQMFB(_analysis_execute)(_q, _x, _y);
    else
        ITQMFB(_synthesis_execute)(_q, _x, _y);
}

void ITQMFB(_analysis_execute)(ITQMFB() _q,
                               TO * _x,
                               TO * _y)
{
    unsigned int i,j,k,t,n=0;
    unsigned int num_inputs=_q->num_channels;
    unsigned int num_outputs;
    unsigned int i0a, i0b, i1a, i1b;
    TO * b0 = NULL;    // input buffer
    TO * b1 = NULL;    // output buffer
    memmove(_q->buffer0,_x,(_q->num_channels)*sizeof(TO));
    for (i=0; i<_q->num_layers; i++) {
        k = 1<<i;
#if ITQMFB_DEBUG
        printf("----------\n");
        printf("layer  : %3u (%3u banks in this layer)\n", i, k);
#endif
        num_outputs = num_inputs/2;

        // set input/output buffers
        b0 = (i%2)==0 ? _q->buffer0 : _q->buffer1;
        b1 = (i%2)==0 ? _q->buffer1 : _q->buffer0;

#if ITQMFB_DEBUG
        for (j=0; j<_q->num_channels; j++)
            printf("  b0[%3u] = %12.8f + j*%12.8f\n", j+1, crealf(b0[j]), cimagf(b0[j]));
#endif

        for (j=0; j<k; j++) {
#if ITQMFB_DEBUG
            printf("  bank : %3u (%3u inputs > %3u outputs)\n", n,num_inputs,num_outputs);
#endif

            for (t=0; t<num_outputs; t++) {
                i0a = j*num_inputs + 2*t+0;
                i0b = j*num_inputs + 2*t+1;
                i1a = j*num_inputs + t;
                i1b = j*num_inputs + t + num_outputs;
#if ITQMFB_DEBUG
                printf("    executing bank %3u (%3u,%3u) > (%3u,%3u)\n", n,
                            i0a, i0b, i1a, i1b);
#endif
                QMFB(_analysis_execute)(_q->bank[n], b0[i0a], b0[i0b], b1+i1a, b1+i1b);
            }
            
            n++;
        }
        num_inputs >>= 1;
#if ITQMFB_DEBUG
        for (j=0; j<_q->num_channels; j++)
            printf("  b1[%3u] = %12.8f + j*%12.8f\n", j+1, crealf(b1[j]), cimagf(b1[j]));
#endif

    }

    memmove(_y,b1,(_q->num_channels)*sizeof(TO));
}

void ITQMFB(_synthesis_execute)(ITQMFB() _q,
                                TO * _y,
                                TO * _x)
{
#if ITQMFB_DEBUG
    printf("\n\n\nsynthesis:\n");
#endif
    unsigned int i,j,k,t,n=0;
    unsigned int num_inputs=1;
    unsigned int num_outputs;
    unsigned int i0a, i0b, i1a, i1b;

    TO * b0 = NULL;    // input buffer
    TO * b1 = NULL;    // output buffer
    memmove(_q->buffer0,_y,(_q->num_channels)*sizeof(TO));
    for (i=0; i<_q->num_layers; i++) {
        k = 1<<(_q->num_layers - i - 1);
#if ITQMFB_DEBUG
        printf("----------\n");
        printf("layer  : %3u (%3u banks in this layer)\n", i, k);
#endif
        num_outputs = num_inputs*2;

        // set input/output buffers
        b0 = (i%2)==0 ? _q->buffer0 : _q->buffer1;
        b1 = (i%2)==0 ? _q->buffer1 : _q->buffer0;

#if ITQMFB_DEBUG
        for (j=0; j<_q->num_channels; j++)
            printf("  b0[%3u] = %12.8f + j*%12.8f\n", j+1, crealf(b0[j]), cimagf(b0[j]));
#endif

        for (j=0; j<k; j++) {
#if ITQMFB_DEBUG
            printf("  bank : %3u (%3u inputs > %3u outputs)\n", n,num_inputs,num_outputs);
#endif

            for (t=0; t<num_inputs; t++) {
                i0a = j*num_outputs + t;
                i0b = j*num_outputs + t + num_inputs;
                i1a = j*num_outputs + 2*t+0;
                i1b = j*num_outputs + 2*t+1;
#if ITQMFB_DEBUG
                printf("    executing bank %3u (%3u,%3u) > (%3u,%3u)\n", n,
                            i0a, i0b, i1a, i1b);
#endif
                QMFB(_synthesis_execute)(_q->bank[n], b0[i0a], b0[i0b], b1+i1a, b1+i1b);
            }
            
            n++;
        }
        num_inputs <<= 1;
#if ITQMFB_DEBUG
        for (j=0; j<_q->num_channels; j++)
            printf("  b1[%3u] = %12.8f + j*%12.8f\n", j+1, crealf(b1[j]), cimagf(b1[j]));
#endif

    }

    memmove(_x,b1,(_q->num_channels)*sizeof(TO));
}

