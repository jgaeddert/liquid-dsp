/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// qpilotgen.c
//
// pilot injection
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>
#include <assert.h>

#include "liquid.internal.h"

struct qpilotgen_s {
    // properties
    unsigned int    payload_len;    // number of samples in payload
    unsigned int    pilot_spacing;  // spacing between pilot symbols
    unsigned int    num_pilots;     // total number of pilot symbols
    unsigned int    frame_len;      // total number of frame symbols
    float complex * pilots;         // pilot sequence
};

// create packet encoder
qpilotgen qpilotgen_create(unsigned int _payload_len,
                           unsigned int _pilot_spacing)
{
    // validate input
    if (_payload_len == 0) {
        fprintf(stderr,"error: qpilotgen_create(), frame length must be at least 1 symbol\n");
        exit(1);
    } else if (_pilot_spacing < 2) {
        fprintf(stderr,"error: qpilotgen_create(), pilot spacing must be at least 2 symbols\n");
        exit(1);
    }
    unsigned int i;

    // allocate memory for main object
    qpilotgen q = (qpilotgen) malloc(sizeof(struct qpilotgen_s));

    // set internal properties
    q->payload_len   = _payload_len;
    q->pilot_spacing = _pilot_spacing;

    // derived values
    div_t d = div(q->payload_len,(q->pilot_spacing - 1));
    q->num_pilots = d.quot + (d.rem ? 1 : 0);
    q->frame_len  = q->payload_len + q->num_pilots;

    // allocate memory for pilots
    q->pilots = (float complex*) malloc(q->num_pilots*sizeof(float complex));

    // find appropriate sequence size
    unsigned int m = liquid_nextpow2(q->num_pilots);

    // generate pilot sequence
    msequence seq = msequence_create_default(m);
    for (i=0; i<q->num_pilots; i++) {
        // generate symbol
        unsigned int s = msequence_generate_symbol(seq, 2);

        // save modulated symbol
        float theta = (2 * M_PI * (float)s / 4.0f) + M_PI / 4.0f;
        q->pilots[i] = cexpf(_Complex_I*theta);
    }
    msequence_destroy(seq);

    // return pointer to main object
    return q;
}

// recreate packet encoder
qpilotgen qpilotgen_recreate(qpilotgen    _q,
                             unsigned int _payload_len,
                             unsigned int _pilot_spacing)
{
    // TODO: only re-generate objects as necessary

    // destroy object
    if (_q != NULL)
        qpilotgen_destroy(_q);

    // create new object
    return qpilotgen_create(_payload_len, _pilot_spacing);
}

void qpilotgen_destroy(qpilotgen _q)
{
    // free arrays
    free(_q->pilots);
    
    // free main object memory
    free(_q);
}

void qpilotgen_reset(qpilotgen _q)
{
}

void qpilotgen_print(qpilotgen _q)
{
    printf("qpilotgen:\n");
    printf("  payload len   :   %u\n", _q->payload_len);
    printf("  pilot spacing :   %u\n", _q->pilot_spacing);
    printf("  num pilots    :   %u\n", _q->num_pilots);
    printf("  frame len     :   %u\n", _q->frame_len);
}

// get length of frame in symbols
unsigned int qpilotgen_get_frame_len(qpilotgen _q)
{
    return _q->frame_len;
}

// encode packet into modulated frame samples
// TODO: include method with just symbol indices? would be useful for
//       non-linear modulation types
void qpilotgen_execute(qpilotgen       _q,
                       float complex * _payload,
                       float complex * _frame)
{
    unsigned int i;
    unsigned int n = 0;
    unsigned int p = 0;
    for (i=0; i<_q->frame_len; i++) {
        if ( (i % _q->pilot_spacing)==0 )
            _frame[i] = _q->pilots[p++];
        else
            _frame[i] = _payload[n++];
    }
    //printf("n = %u (expected %u)\n", n, _q->payload_len);
    //printf("p = %u (expected %u)\n", p, _q->num_pilots);
    assert(n == _q->payload_len);
    assert(p == _q->num_pilots);
}

