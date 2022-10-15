/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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

// Frame detector and synchronizer; uses a novel correlation method to
// detect a synchronization pattern, estimate carrier frequency and
// phase offsets as well as timing phase, then correct for these
// impairments in a simple interface suitable for custom frame recovery.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

// main object definition
struct qdsync_cccf_s {
    qdsync_callback callback;   //
    void *          context;    //
    qdetector_cccf  detector;   // detector
    // status variables
    enum {
        QDSYNC_STATE_DETECT=0,  // detect frame
        QDSYNC_STATE_SYNC,      // synchronize samples
    }            state;
    // resampler
    // nco
};

// create detector with generic sequence
//  _s      :   sample sequence
//  _s_len  :   length of sample sequence
qdsync_cccf qdsync_cccf_create(float complex * _s,
                               unsigned int    _s_len,
                               qdsync_callback _callback,
                               void *          _context)
{
    // validate input
    if (_s_len == 0)
        return liquid_error_config("qdsync_cccf_create(), sequence length cannot be zero");
    
    // allocate memory for main object and set internal properties
    qdsync_cccf q = (qdsync_cccf) malloc(sizeof(struct qdsync_cccf_s));

    // create detector
    q->detector = qdetector_cccf_create(_s, _s_len);

    // set callback and context values
    qdsync_cccf_set_callback(q, _callback);
    qdsync_cccf_set_context (q, _context );

    // reset and return object
    qdsync_cccf_reset(q);
    return q;
}

// copy object
qdsync_cccf qdsync_cccf_copy(qdsync_cccf q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("qdetector_%s_copy(), object cannot be NULL", "cccf");

    // create new object and copy base parameters
    qdsync_cccf q_copy = (qdsync_cccf) malloc(sizeof(struct qdsync_cccf_s));
    memmove(q_copy, q_orig, sizeof(struct qdsync_cccf_s));

    // copy sub-objects
    q_copy->detector = qdetector_cccf_copy(q_orig->detector);

    // return new object
    return q_copy;
}

int qdsync_cccf_destroy(qdsync_cccf _q)
{
    // destroy internal objects
    qdetector_cccf_destroy(_q->detector);

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

int qdsync_cccf_print(qdsync_cccf _q)
{
    printf("<liquid.qdsync_cccf>\n");
    return LIQUID_OK;
}

int qdsync_cccf_reset(qdsync_cccf _q)
{
    _q->state = QDSYNC_STATE_DETECT;
    return LIQUID_OK;
}

int qdsync_cccf_execute(qdsync_cccf            _q,
                        liquid_float_complex * _buf,
                        unsigned int           _buf_len)
{
    // TODO: switch based on state
    unsigned int i;
    void * p = NULL;
    for (i=0; i<_buf_len; i++) {
        switch (_q->state) {
        case QDSYNC_STATE_DETECT:
            p = qdetector_cccf_execute(_q->detector, _buf[i]);
            if (p != NULL) {
                if (_q->callback != NULL)
                    _q->callback(NULL, 0, _q->context);
                _q->state = QDSYNC_STATE_SYNC;
            }
            break;
        case QDSYNC_STATE_SYNC:
            if (_q->callback != NULL) {
                _q->callback(_buf, _buf_len, _q->context);
                return LIQUID_OK;
            }
            break;
        default:;
        }
    }
    return LIQUID_OK;
}

// get detection threshold
float qdsync_cccf_get_threshold(qdsync_cccf _q)
{
    return qdetector_cccf_get_threshold(_q->detector);
}

// set detection threshold
int qdsync_cccf_set_threshold(qdsync_cccf _q,
                              float       _threshold)
{
    return qdetector_cccf_set_threshold(_q->detector, _threshold);
}

// set callback method
int qdsync_cccf_set_callback(qdsync_cccf _q, qdsync_callback _callback)
{
    _q->callback = _callback;
    return LIQUID_OK;
}

// set context value
int qdsync_cccf_set_context (qdsync_cccf _q, void * _context)
{
    _q->context = _context;
    return LIQUID_OK;
}

