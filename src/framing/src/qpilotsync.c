/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
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
// qpilotsync.c
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

#define DEBUG_QPILOTSYNC 0

struct qpilotsync_s {
    // properties
    unsigned int    payload_len;    // number of samples in payload
    unsigned int    pilot_spacing;  // spacing between pilot symbols
    unsigned int    num_pilots;     // total number of pilot symbols
    unsigned int    frame_len;      // total number of frame symbols
    float complex * pilots;         // pilot sequence

    unsigned int    nfft;           // FFT size
    float complex * buf_time;       // FFT time buffer
    float complex * buf_freq;       // FFT freq buffer
    fftplan         fft;            // transform object

    float           dphi_hat;       // carrier frequency offset estimate
    float           phi_hat;        // carrier phase offset estimate
    float           g_hat;          // gain correction estimate
};

// create packet encoder
qpilotsync qpilotsync_create(unsigned int _payload_len,
                             unsigned int _pilot_spacing)
{
    // validate input
    if (_payload_len == 0) {
        fprintf(stderr,"error: qpilotsync_create(), frame length must be at least 1 symbol\n");
        exit(1);
    } else if (_pilot_spacing < 2) {
        fprintf(stderr,"error: qpilotsync_create(), pilot spacing must be at least 2 symbols\n");
        exit(1);
    }
    unsigned int i;

    // allocate memory for main object
    qpilotsync q = (qpilotsync) malloc(sizeof(struct qpilotsync_s));

    // set internal properties
    q->payload_len   = _payload_len;
    q->pilot_spacing = _pilot_spacing;

    // derived values
    q->num_pilots = qpilot_num_pilots(q->payload_len, q->pilot_spacing);
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

    // compute fft size and create transform objects
    q->nfft = 1 << liquid_nextpow2(q->num_pilots + (q->num_pilots>>1));
    q->buf_time = (float complex*) malloc(q->nfft*sizeof(float complex));
    q->buf_freq = (float complex*) malloc(q->nfft*sizeof(float complex));
    q->fft      = fft_create_plan(q->nfft, q->buf_time, q->buf_freq, LIQUID_FFT_FORWARD, 0);

    // reset and return pointer to main object
    qpilotsync_reset(q);
    return q;
}

// recreate packet encoder
qpilotsync qpilotsync_recreate(qpilotsync   _q,
                               unsigned int _payload_len,
                               unsigned int _pilot_spacing)
{
    // TODO: only re-generate objects as necessary

    // destroy object
    if (_q != NULL)
        qpilotsync_destroy(_q);

    // create new object
    return qpilotsync_create(_payload_len, _pilot_spacing);
}

void qpilotsync_destroy(qpilotsync _q)
{
    // free arrays
    free(_q->pilots);
    free(_q->buf_time);
    free(_q->buf_freq);

    // destroy objects
    fft_destroy_plan(_q->fft);
    
    // free main object memory
    free(_q);
}

void qpilotsync_reset(qpilotsync _q)
{
    // clear FFT input buffer
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->buf_time[i] = 0.0f;
    
    // reset estimates
    _q->dphi_hat = 0.0f;
    _q->phi_hat  = 0.0f;
    _q->g_hat    = 1.0f;
}

void qpilotsync_print(qpilotsync _q)
{
    printf("qpilotsync:\n");
    printf("  payload len   :   %u\n", _q->payload_len);
    printf("  pilot spacing :   %u\n", _q->pilot_spacing);
    printf("  num pilots    :   %u\n", _q->num_pilots);
    printf("  frame len     :   %u\n", _q->frame_len);
    printf("  nfft          :   %u\n", _q->nfft);
}

// get length of frame in symbols
unsigned int qpilotsync_get_frame_len(qpilotsync _q)
{
    return _q->frame_len;
}

// encode packet into modulated frame samples
// TODO: include method with just symbol indices? would be useful for
//       non-linear modulation types
void qpilotsync_execute(qpilotsync      _q,
                        float complex * _frame,
                        float complex * _payload)
{
    unsigned int i;
    unsigned int n = 0;
    unsigned int p = 0;

    // extract pilots and de-rotate with known sequence
    for (i=0; i<_q->num_pilots; i++) {
        _q->buf_time[i] = _frame[i*_q->pilot_spacing] * conjf(_q->pilots[i]);

#if DEBUG_QPILOTSYNC
        printf("(%8.4f,%8.4f) = (%8.4f,%8.4f) * conj(%8.4f,%8.4f)\n",
            crealf(_q->buf_time[i]),
            cimagf(_q->buf_time[i]),
            crealf(_frame[i*_q->pilot_spacing]),
            cimagf(_frame[i*_q->pilot_spacing]),
            crealf(_q->pilots[i]),
            cimagf(_q->pilots[i]));
#endif
    }

    // compute frequency offset by computing transform and finding peak
    fft_execute(_q->fft);
    unsigned int i0 = 0;
    float        y0 = 0;
    for (i=0; i<_q->nfft; i++) {
#if DEBUG_QPILOTSYNC
        printf("X(%3u) = %12.8f + 1i*%12.8f; %% %12.8f\n",
                i+1, crealf(_q->buf_freq[i]), cimagf(_q->buf_freq[i]), cabsf(_q->buf_freq[i]));
#endif
        if (i==0 || cabsf(_q->buf_freq[i]) > y0) {
            i0 = i;
            y0 = cabsf(_q->buf_freq[i]);
        }
    }

    // interpolate and recover frequency
    unsigned int ineg = (i0 + _q->nfft - 1) % _q->nfft;
    unsigned int ipos = (i0 +            1) % _q->nfft;
    float        ypos = cabsf(_q->buf_freq[ipos]);
    float        yneg = cabsf(_q->buf_freq[ineg]);
    float        a    =  0.5f*(ypos + yneg) - y0;
    float        b    =  0.5f*(ypos - yneg);
    //float        c    =  y0;
    float        idx  = -b / (2.0f*a); //-0.5f*(ypos - yneg) / (ypos + yneg - 2*y0);
    float index = (float)i0 + idx;
    _q->dphi_hat = (i0 > _q->nfft/2 ? index-(float)_q->nfft : index) * 2*M_PI / (float)(_q->nfft * _q->pilot_spacing);
#if DEBUG_QPILOTSYNC
    printf("X[%3u] = %12.8f <%12.8f>\n", ineg, yneg, cargf(_q->buf_freq[ineg]));
    printf("X[%3u] = %12.8f <%12.8f>\n", i0,   y0,   cargf(_q->buf_freq[i0]));
    printf("X[%3u] = %12.8f <%12.8f>\n", ipos, ypos, cargf(_q->buf_freq[ipos]));
    printf("yneg  = %12.8f;\n", yneg);
    printf("ypos  = %12.8f;\n", ypos);
    printf("y0    = %12.8f;\n", y0);
    printf("interpolated peak at %12.8f (%u + %12.8f)\n", index, i0, idx);
#endif

    // estimate carrier phase offset
#if 0
    // METHOD 1: linear interpolation of phase in FFT output buffer
    float v0 = cargf(_q->buf_freq[ idx < 0 ? ineg : i0   ]);
    float v1 = cargf(_q->buf_freq[ idx < 0 ? i0   : ipos ]);
    float xp = idx < 0 ? 1+idx : idx;
    float phi_hat  = (v1-v0)*xp + v0;
    //printf("v0 = %12.8f, v1 = %12.8f, xp = %12.8f\n", v0, v1, xp);

    // channel gain: use quadratic interpolation of FFT amplitude to find peak
    //               correlation output in the frequency domain
    float g_hat = (a*idx*idx + b*idx + c) / (float)(_q->num_pilots);
#else
    // METHOD 2: compute metric by de-rotating pilots and measuring resulting phase
    // NOTE: this is possibly more accurate than the above method but might also
    //       be more computationally complex
    float complex metric = 0;
    for (i=0; i<_q->num_pilots; i++)
        metric += _q->buf_time[i] * cexpf(-_Complex_I*_q->dphi_hat*i*(float)(_q->pilot_spacing));
    //printf("metric : %12.8f <%12.8f>\n", cabsf(metric), cargf(metric));
    _q->phi_hat = cargf(metric);
    _q->g_hat   = cabsf(metric) / (float)(_q->num_pilots);
#endif

#if DEBUG_QPILOTSYNC
    // print estimates of carrier frequency, phase, gain
    printf("dphi-hat    :   %12.8f\n", _q->dphi_hat);
    printf(" phi-hat    :   %12.8f\n",  _q->phi_hat);
    printf("   g-hat    :   %12.8f\n",    _q->g_hat);
#endif

    // frequency correction
    float g = 1.0f / _q->g_hat;

    // recover frame symbols
    for (i=0; i<_q->frame_len; i++) {
        if ( (i % _q->pilot_spacing)==0 )
            p++;
        else
            _payload[n++] = g * _frame[i] * cexpf(-_Complex_I*(_q->dphi_hat*i + _q->phi_hat));
    }
#if DEBUG_QPILOTSYNC
    printf("n = %u (expected %u)\n", n, _q->payload_len);
    printf("p = %u (expected %u)\n", p, _q->num_pilots);
    assert(n == _q->payload_len);
    assert(p == _q->num_pilots);
#endif
}

// get estimates
float qpilotsync_get_dphi(qpilotsync _q)
{
    return _q->dphi_hat;
}

float qpilotsync_get_phi(qpilotsync _q)
{
    return _q->phi_hat;
}

float qpilotsync_get_gain(qpilotsync _q)
{
    return _q->g_hat;
}


