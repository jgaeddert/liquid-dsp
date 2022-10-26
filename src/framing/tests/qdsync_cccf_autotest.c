/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include "autotest/autotest.h"
#include "liquid.h"

// common structure for relaying information to/from callback
typedef struct {
    unsigned int    seq_len;
    float complex * buf;
    unsigned int    buf_len;
    unsigned int    count;
} autotest_qdsync_s;

// synchronization callback, return 0:continue, 1:reset
int autotest_qdsync_callback(float complex * _buf,
                             unsigned int    _buf_len,
                             void *          _context)
{
    printf("qdsync callback got %u samples\n", _buf_len);
    // save samples to buffer as appropriate
    autotest_qdsync_s * q = (autotest_qdsync_s *) _context;
    unsigned int i;
    for (i=0; i<_buf_len; i++) {
        if (q->count < q->seq_len) {
            // preamble sequence
        } else if (q->count < q->seq_len + q->buf_len) {
            // payload
            q->buf[q->count - q->seq_len] = _buf[i];
        } if (q->count == q->seq_len + q->buf_len) {
            // buffer full; reset synchronizer
            return 1;
        }
        q->count++;
    }
    return 0;
}

void autotest_qdsync()
{
    // options
    unsigned int seq_len      =  240;   // number of sync symbols
    unsigned int payload_len  =  800;   // number of payload symbols
    unsigned int k            =    2;   // samples/symbol
    unsigned int m            =    7;   // filter delay [symbols]
    float        beta         = 0.3f;   // excess bandwidth factor
    int          ftype        = LIQUID_FIRFILT_ARKAISER;
    float        nstd         = 0.001f;

    // generate synchronization sequence (QPSK symbols)
    float complex seq[seq_len];
    unsigned int i;
    for (i=0; i<seq_len ; i++) {
        seq[i] = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                 (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
    }

    // payload symbols
    float complex payload_tx[payload_len];  // transmitted
    float complex payload_rx[payload_len];  // received with initial correction
    for (i=0; i<payload_len ; i++) {
        payload_tx[i] = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                        (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
    }

    // create sync object
    autotest_qdsync_s obj = {.seq_len=seq_len, .buf=payload_rx, .buf_len=payload_len, .count=0};
    qdsync_cccf q = qdsync_cccf_create_linear(seq, seq_len, ftype, k, m, beta,
            autotest_qdsync_callback, (void*)&obj);
    qdsync_cccf_set_range(q, 0.001f);

    // create interpolator
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,0);

    // create delay object
    fdelay_crcf delay = fdelay_crcf_create_default(100);
    fdelay_crcf_set_delay(delay, 10*k - 0.4);

    // run signal through sync object
    float complex buf[k];
    for (i=0; i<4*seq_len + payload_len + 2*m + 50; i++) {
        // produce symbol (preamble sequence, payload, or zero)
        float complex s = 0;
        if (i < seq_len) {
            s = seq[i];
        } else if (i < seq_len + payload_len) {
            s = payload_tx[i - seq_len];
        }

        // interpolate symbol
        firinterp_crcf_execute(interp, s, buf);

        // apply delay in place
        fdelay_crcf_execute_block(delay, buf, k, buf);

        // add noise
        unsigned int j;
        for (j=0; j<k; j++)
            buf[j] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // run through synchronizer
        qdsync_cccf_execute(q, buf, k);
    }
    float dphi_hat = qdsync_cccf_get_dphi(q);
    float phi_hat  = qdsync_cccf_get_phi(q);

    // compute error in terms of offset from unity; might be residual carrier phase/gain
    // TODO: perform residual carrier/phase error correction?
    float rmse = 0.0f;
    for (i=0; i<payload_len; i++) {
        float e = cabsf(payload_rx[i]) - 1.0f;
        rmse += e*e;
    }
    rmse = 10*log10f( rmse / (float)payload_len );
    if (liquid_autotest_verbose)
        printf("qdsync: dphi: %12.4e, phi: %12.8f, rmse: %12.3f\n", dphi_hat, phi_hat, rmse);
    CONTEND_LESS_THAN( rmse,           -30.0f )
    CONTEND_LESS_THAN( fabsf(dphi_hat), 1e-3f )
    CONTEND_LESS_THAN( fabsf( phi_hat), 0.4f  )

    // clean up objects
    qdsync_cccf_destroy(q);
    firinterp_crcf_destroy(interp);
    fdelay_crcf_destroy(delay);
#if 0
    FILE * fid = fopen("qdsync_cccf_autotest.m","w");
    fprintf(fid,"clear all; close all;\n");
    for (i=0; i<payload_len; i++) {
        fprintf(fid,"s(%4u)=%12.4e+%12.4ej; r(%4u)=%12.4e+%12.4ej;\n",
            i+1, crealf(payload_tx[i]), cimagf(payload_tx[i]),
            i+1, crealf(payload_rx[i]), cimagf(payload_rx[i]));
    }
    fprintf(fid,"plot(r,'.','MarkerSize',6); grid on; axis square;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5); xlabel('I'); ylabel('Q');\n");
    fclose(fid);
#endif
}

