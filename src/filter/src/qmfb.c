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
// quadrature mirror filterbank (QMFB)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  QMFB()          name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct QMFB(_s) {
    float * h;          // filter prototype
    unsigned int h_len; // primitive filter length
    unsigned int m;
    float beta;         // filter bandwidth

    int type;

    // 
    TC * h0;
    TC * h1;
    WINDOW() w0;
    WINDOW() w1;
    unsigned int h_sub_len;
};

QMFB() QMFB(_create)(unsigned int _m, float _beta, int _type)
{
    QMFB() f = (QMFB()) malloc(sizeof(struct QMFB(_s)));

    // compute filter length
    f->m = _m;
    f->beta = _beta;
    f->type = _type;

    float h_prim[20] = {
        0.1605476e+0,
        0.4156381e+0,
        0.4591917e+0,
        0.1487153e+0,
       -0.1642893e+0,
       -0.1245206e+0,
        0.8252419e-1,
        0.8875733e-1,
       -0.5080163e-1,
       -0.6084593e-1,
        0.3518087e-1,
        0.3989182e-1,
       -0.2561513e-1,
       -0.2440664e-1,
        0.1860065e-1,
        0.1354778e-1,
       -0.1308061e-1,
       -0.7449561e-2,
        0.1293440e-1,
       -0.4995356e-2};
    f->m = 5;

    //f->h_len = 4*(f->m);
    f->h_len = 40;
    f->h = (float*) malloc((f->h_len)*sizeof(float));

    //f->h_sub_len = 2*(f->m);
    f->h_sub_len = 20;
    f->h0 = (TC *) malloc((f->h_sub_len)*sizeof(TC));
    f->h1 = (TC *) malloc((f->h_sub_len)*sizeof(TC));

    // design filter prototype
    design_rrc_filter(2,f->m,f->beta,0,f->h);

    // resample, reverse direction
    unsigned int i;
    for (i=0; i<f->h_sub_len; i++) {
        if (f->type == LIQUID_QMFB_ANALYZER) {
            // analysis
            f->h0[f->h_sub_len-i-1] = h_prim[i];
            f->h1[f->h_sub_len-i-1] = h_prim[f->h_sub_len-i-1] * ((i%2)==0 ? 1.0f : -1.0f);
        } else if (f->type == LIQUID_QMFB_SYNTHESIZER) {
            // synthesis
            f->h0[f->h_sub_len-i-1] = h_prim[f->h_sub_len-i-1];
            f->h1[f->h_sub_len-i-1] = h_prim[i] * ((i%2)==1 ? 1.0f : -1.0f);
            //f->h0[i] = h_prim[f->h_sub_len-i-1];
            //f->h1[i] = h_prim[i] * ((i%2)==1 ? 1.0f : -1.0f);
        } else {
            printf("error: qmfb_xxxt_create(), unknown type %d\n", f->type);
            exit(0);
        }
    }

    f->w0 = WINDOW(_create)(f->h_sub_len);
    f->w1 = WINDOW(_create)(f->h_sub_len);
    WINDOW(_clear)(f->w0);
    WINDOW(_clear)(f->w1);
    return f;
}

QMFB() QMFB(_recreate)(QMFB() _f, unsigned int _h_len)
{
    // TODO implement this method
    printf("error: qmfb_xxxt_recreate(), method not supported yet\n");
    exit(1);
    return NULL;
}

void QMFB(_destroy)(QMFB() _f)
{
    WINDOW(_destroy)(_f->w0);
    WINDOW(_destroy)(_f->w1);
    free(_f->h);
    free(_f->h0);
    free(_f->h1);
    free(_f);
}

void QMFB(_print)(QMFB() _f)
{
    printf("quadrature mirror filterbank: [%u taps]\n", _f->h_sub_len);
    unsigned int i;
#if 0
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = ", i+1);
        //PRINTVAL(_f->h[i]);
        printf("%8.4f", _f->h[i]);
        printf(";\n");
    }
#endif
    printf("---\n");
    for (i=0; i<_f->h_sub_len; i++) {
        printf("  h0(%4u) = ", i+1);
        //PRINTVAL(_f->h1[i]);
        printf("%12.8f", _f->h0[i]);
        printf(";\n");
    }
    printf("---\n");
    for (i=0; i<_f->h_sub_len; i++) {
        printf("  h1(%4u) = ", i+1);
        //PRINTVAL(_f->h1[i]);
        printf("%12.8f", _f->h1[i]);
        printf(";\n");
    }
}

void QMFB(_clear)(QMFB() _f)
{
    WINDOW(_clear)(_f->w0);
    WINDOW(_clear)(_f->w1);
}


void QMFB(_execute)(QMFB() _q,
                    TI   _x0,
                    TI   _x1,
                    TO * _y0,
                    TO * _y1)
{
    if (_q->type == LIQUID_QMFB_ANALYZER)
        QMFB(_analysis_execute)(_q,_x0,_x1,_y0,_y1);
    else
        QMFB(_synthesis_execute)(_q,_x0,_x1,_y0,_y1);
}

void QMFB(_analysis_execute)(QMFB() _q,
                             TI   _x0,
                             TI   _x1,
                             TO * _y0,
                             TO * _y1)
{
    TI * r; // read pointer
    TO z0, z1;

    // compute upper branch
    WINDOW(_push)(_q->w0, _x0);
    WINDOW(_push)(_q->w0, _x1);
    WINDOW(_read)(_q->w0, &r);
    DOTPROD(_run)(_q->h0, r, _q->h_sub_len, &z0);

    // compute lower branch
    WINDOW(_push)(_q->w1, _x0);
    WINDOW(_push)(_q->w1, _x1);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_run)(_q->h1, r, _q->h_sub_len, &z1);

    // compute output partition
    *_y0 = z0;
    *_y1 = z1;
}

void QMFB(_synthesis_execute)(QMFB() _q,
                              TI   _x0,
                              TI   _x1,
                              TO * _y0,
                              TO * _y1)
{
    TI * r; // read pointer
    TO y0a, y0b, y1a, y1b;

    // NOTE: ifft([_x0 _x1]) = [(_x0+_x1)/2 (_x0-_x1)/2]

    // compute upper branch
    WINDOW(_push)(_q->w0, _x0);
    WINDOW(_read)(_q->w0, &r);
    DOTPROD(_run)(_q->h0, r, _q->h_sub_len, &y0a);
    WINDOW(_push)(_q->w0, 0);
    WINDOW(_read)(_q->w0, &r);
    DOTPROD(_run)(_q->h0, r, _q->h_sub_len, &y1a);
    
    // compute lower branch
    WINDOW(_push)(_q->w1, _x1);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_run)(_q->h1, r, _q->h_sub_len, &y0b);
    WINDOW(_push)(_q->w1, 0);
    WINDOW(_read)(_q->w1, &r);
    DOTPROD(_run)(_q->h1, r, _q->h_sub_len, &y1b);
    
    *_y0 = (y0a + y0b)*2.0f;
    *_y1 = (y1a + y1b)*2.0f;
}

