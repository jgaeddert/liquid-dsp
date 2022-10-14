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

// basic frame synchronizer with 8 bytes header and 64 bytes payload

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#include "liquid.internal.h"

#define dsssframe64sync_ENABLE_EQ       0

// push samples through detection stage
int dsssframe64sync_execute_seekpn(dsssframe64sync _q,
                                   float complex   _x);

// step receiver mixer, matched filter, decimator
//  _q      :   frame synchronizer
//  _x      :   input sample
//  _y      :   output symbol
int dsssframe64sync_step(dsssframe64sync _q,
                         float complex   _x,
                         float complex * _y);

// push samples through synchronizer, saving received p/n symbols
int dsssframe64sync_execute_rxpreamble(dsssframe64sync _q,
                                       float complex   _x);

// receive payload symbols
int dsssframe64sync_execute_rxpayload(dsssframe64sync _q,
                                      float complex   _x);

// export debugging based on return value
//  0   : do not write file
//  >0  : write specific number (hex)
//  -1  : number of packets detected
//  -2  : id using first 4 bytes of header
//  -3  : write with random extension
int dsssframe64sync_debug_export(dsssframe64sync _q, int _code);

// dsssframe64sync object structure
struct dsssframe64sync_s {
    // callback
    framesync_callback  callback;   // user-defined callback function
    void *              userdata;   // user-defined data structure
    framesyncstats_s    framesyncstats; // frame statistic object (synchronizer)
    framedatastats_s    framedatastats; // frame statistic object (packet statistics)

    // synchronizer objects
    unsigned int        m;          // filter delay (symbols)
    float               beta;       // filter excess bandwidth factor
    qdetector_cccf      detector;   // pre-demod detector
    float               tau_hat;    // fractional timing offset estimate
    float               dphi_hat;   // carrier frequency offset estimate
    float               phi_hat;    // carrier phase offset estimate
    float               gamma_hat;  // channel gain estimate
    nco_crcf            mixer;      // coarse carrier frequency recovery

    // timing recovery objects, states
    firpfb_crcf         mf;         // matched filter decimator
    unsigned int        npfb;       // number of filters in symsync
    int                 mf_counter; // matched filter output timer
    unsigned int        pfb_index;  // filterbank index

    // preamble
    float complex preamble_pn[64];  // known 64-symbol p/n sequence
    float complex preamble_rx[64];  // received p/n symbols
    
    // payload decoder
    float complex payload_rx [630]; // received payload symbols with pilots
    float complex payload_sym[600]; // received payload symbols
    unsigned char payload_dec[ 72]; // decoded payload bytes
    qpacketmodem  dec;              // packet demodulator/decoder
    qpilotsync    pilotsync;        // pilot extraction, carrier recovery
    int           payload_valid;    // did payload pass crc?
    
    // status variables
    enum {
        dsssframe64sync_STATE_DETECTFRAME=0,    // detect frame (seek p/n sequence)
        dsssframe64sync_STATE_RXPREAMBLE,       // receive p/n sequence
        dsssframe64sync_STATE_RXPAYLOAD,        // receive payload data
    }            state;
    unsigned int preamble_counter;  // counter: num of p/n syms received
    unsigned int payload_counter;   // counter: num of payload syms received
};

// create dsssframe64sync object
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
dsssframe64sync dsssframe64sync_create(framesync_callback _callback,
                               void *             _userdata)
{
    dsssframe64sync q = (dsssframe64sync) malloc(sizeof(struct dsssframe64sync_s));
    q->callback = _callback;
    q->userdata = _userdata;
    q->m        = 7;    // filter delay (symbols)
    q->beta     = 0.3f; // excess bandwidth factor

    unsigned int i;

    // generate p/n sequence
    msequence ms = msequence_create(7, 0x0089, 1);
    for (i=0; i<64; i++) {
        q->preamble_pn[i]  = (msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2);
        q->preamble_pn[i] += (msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2)*_Complex_I;
    }
    msequence_destroy(ms);

    // create frame detector
    unsigned int k    = 2;    // samples/symbol
    q->detector = qdetector_cccf_create_linear(q->preamble_pn, 64, LIQUID_FIRFILT_ARKAISER, k, q->m, q->beta);
    qdetector_cccf_set_threshold(q->detector, 0.5f);

    // create symbol timing recovery filters
    q->npfb = 32;   // number of filters in the bank
    q->mf   = firpfb_crcf_create_rnyquist(LIQUID_FIRFILT_ARKAISER, q->npfb,k,q->m,q->beta);

    // create down-coverters for carrier phase tracking
    q->mixer = nco_crcf_create(LIQUID_NCO);
    
    // create payload demodulator/decoder object
    int check      = LIQUID_CRC_24;
    int fec0       = LIQUID_FEC_NONE;
    int fec1       = LIQUID_FEC_GOLAY2412;
    int mod_scheme = LIQUID_MODEM_QPSK;
    q->dec         = qpacketmodem_create();
    qpacketmodem_configure(q->dec, 72, check, fec0, fec1, mod_scheme);
    //qpacketmodem_print(q->dec);
    assert( qpacketmodem_get_frame_len(q->dec)==600 );

    // create pilot synchronizer
    q->pilotsync   = qpilotsync_create(600, 21);
    assert( qpilotsync_get_frame_len(q->pilotsync)==630);
 
    // reset global data counters
    dsssframe64sync_reset_framedatastats(q);

    // reset state and return
    dsssframe64sync_reset(q);
    return q;
}

// copy object
dsssframe64sync dsssframe64sync_copy(dsssframe64sync q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dsssframe64sync_copy(), object cannot be NULL");

    // allocate memory for new object
    dsssframe64sync q_copy = (dsssframe64sync) malloc(sizeof(struct dsssframe64sync_s));

    // copy entire memory space over and overwrite values as needed
    memmove(q_copy, q_orig, sizeof(struct dsssframe64sync_s));

    // set callback and userdata fields
    q_copy->callback = q_orig->callback;
    q_copy->userdata = q_orig->userdata;

    // copy objects
    q_copy->detector = qdetector_cccf_copy(q_orig->detector);
    q_copy->mixer    = nco_crcf_copy      (q_orig->mixer);
    q_copy->mf       = firpfb_crcf_copy   (q_orig->mf);
    q_copy->dec      = qpacketmodem_copy  (q_orig->dec);
    q_copy->pilotsync= qpilotsync_copy    (q_orig->pilotsync);

    return q_copy;
}

// destroy frame synchronizer object, freeing all internal memory
int dsssframe64sync_destroy(dsssframe64sync _q)
{
    // destroy synchronization objects
    qdetector_cccf_destroy(_q->detector);   // frame detector
    firpfb_crcf_destroy   (_q->mf);         // matched filter
    nco_crcf_destroy      (_q->mixer);      // coarse NCO
    qpacketmodem_destroy  (_q->dec);        // payload demodulator
    qpilotsync_destroy    (_q->pilotsync);  // pilot synchronizer

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

// print frame synchronizer object internals
int dsssframe64sync_print(dsssframe64sync _q)
{
    printf("dsssframe64sync:\n");
    return framedatastats_print(&_q->framedatastats);
}

// reset frame synchronizer object
int dsssframe64sync_reset(dsssframe64sync _q)
{
    // reset binary pre-demod synchronizer
    qdetector_cccf_reset(_q->detector);

    // reset carrier recovery objects
    nco_crcf_reset(_q->mixer);

    // reset symbol timing recovery state
    firpfb_crcf_reset(_q->mf);
        
    // reset state
    _q->state           = dsssframe64sync_STATE_DETECTFRAME;
    _q->preamble_counter= 0;
    _q->payload_counter = 0;
    
    // reset frame statistics
    _q->framesyncstats.evm = 0.0f;

    return LIQUID_OK;
}

// set the callback function
int dsssframe64sync_set_callback(dsssframe64sync        _q,
                                 framesync_callback _callback)
{
    _q->callback = _callback;
    return LIQUID_OK;
}

// set the user-defined data field (context)
int dsssframe64sync_set_userdata(dsssframe64sync _q,
                                 void *      _userdata)
{
    _q->userdata = _userdata;
    return LIQUID_OK;
}

// execute frame synchronizer
//  _q     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
int dsssframe64sync_execute(dsssframe64sync     _q,
                        float complex * _x,
                        unsigned int    _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        switch (_q->state) {
        case dsssframe64sync_STATE_DETECTFRAME:
            // detect frame (look for p/n sequence)
            dsssframe64sync_execute_seekpn(_q, _x[i]);
            break;
        case dsssframe64sync_STATE_RXPREAMBLE:
            // receive p/n sequence symbols
            dsssframe64sync_execute_rxpreamble(_q, _x[i]);
            break;
        case dsssframe64sync_STATE_RXPAYLOAD:
            // receive payload symbols
            dsssframe64sync_execute_rxpayload(_q, _x[i]);
            break;
        default:
            return liquid_error(LIQUID_EINT,"dsssframe64sync_exeucte(), unknown/unsupported state");
        }
    }
    return LIQUID_OK;
}

// 
// internal methods
//

// execute synchronizer, seeking p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
int dsssframe64sync_execute_seekpn(dsssframe64sync   _q,
                                float complex _x)
{
    // push through pre-demod synchronizer
    float complex * v = qdetector_cccf_execute(_q->detector, _x);

    // check if frame has been detected
    if (v != NULL) {
        // get estimates
        _q->tau_hat   = qdetector_cccf_get_tau  (_q->detector);
        _q->gamma_hat = qdetector_cccf_get_gamma(_q->detector);
        _q->dphi_hat  = qdetector_cccf_get_dphi (_q->detector);
        _q->phi_hat   = qdetector_cccf_get_phi  (_q->detector);
        //printf("***** frame detected! tau-hat:%8.4f, dphi-hat:%8.4f, gamma:%8.2f dB\n",
        //        _q->tau_hat, _q->dphi_hat, 20*log10f(_q->gamma_hat));

        // set appropriate filterbank index
        if (_q->tau_hat > 0) {
            _q->pfb_index = (unsigned int)(      _q->tau_hat  * _q->npfb) % _q->npfb;
            _q->mf_counter = 0;
        } else {
            _q->pfb_index = (unsigned int)((1.0f+_q->tau_hat) * _q->npfb) % _q->npfb;
            _q->mf_counter = 1;
        }
        
        // output filter scale
        firpfb_crcf_set_scale(_q->mf, 0.5f / _q->gamma_hat);

        // set frequency/phase of mixer
        nco_crcf_set_frequency(_q->mixer, _q->dphi_hat);
        nco_crcf_set_phase    (_q->mixer, _q->phi_hat );

        // update state
        _q->state = dsssframe64sync_STATE_RXPREAMBLE;

        // run buffered samples through synchronizer
        unsigned int buf_len = qdetector_cccf_get_buf_len(_q->detector);
        dsssframe64sync_execute(_q, v, buf_len);
    }
    return LIQUID_OK;
}

// step receiver mixer, matched filter, decimator
//  _q      :   frame synchronizer
//  _x      :   input sample
//  _y      :   output symbol
int dsssframe64sync_step(dsssframe64sync     _q,
                     float complex   _x,
                     float complex * _y)
{
    // mix sample down
    float complex v;
    nco_crcf_mix_down(_q->mixer, _x, &v);
    nco_crcf_step    (_q->mixer);
    
    // push sample into filterbank
    firpfb_crcf_push   (_q->mf, v);
    firpfb_crcf_execute(_q->mf, _q->pfb_index, &v);

    // increment counter to determine if sample is available
    _q->mf_counter++;
    int sample_available = (_q->mf_counter >= 1) ? 1 : 0;
    
    // set output sample if available
    if (sample_available) {

        // set output
        *_y = v;

        // decrement counter by k=2 samples/symbol
        _q->mf_counter -= 2;
    }

    // return flag
    return sample_available;
}

// execute synchronizer, receiving p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
int dsssframe64sync_execute_rxpreamble(dsssframe64sync   _q,
                                    float complex _x)
{
    // step synchronizer
    float complex mf_out = 0.0f;
    int sample_available = dsssframe64sync_step(_q, _x, &mf_out);

    // compute output if timeout
    if (sample_available) {

        // save output in p/n symbols buffer
        unsigned int delay = 2*_q->m;     // delay from matched filter
        if (_q->preamble_counter >= delay) {
            unsigned int index = _q->preamble_counter-delay;

            _q->preamble_rx[index] = mf_out;
        }

        // update p/n counter
        _q->preamble_counter++;

        // update state
        if (_q->preamble_counter == 64 + delay)
            _q->state = dsssframe64sync_STATE_RXPAYLOAD;
    }
    return LIQUID_OK;
}

// execute synchronizer, receiving payload
//  _q      :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
int dsssframe64sync_execute_rxpayload(dsssframe64sync   _q,
                                  float complex _x)
{
    // step synchronizer
    float complex mf_out = 0.0f;
    int sample_available = dsssframe64sync_step(_q, _x, &mf_out);

    // compute output if timeout
    if (sample_available) {
        // save payload symbols (modem input/output)
        _q->payload_rx[_q->payload_counter] = mf_out;

        // increment counter
        _q->payload_counter++;

        if (_q->payload_counter == 630) {
            // recover data symbols from pilots
            qpilotsync_execute(_q->pilotsync, _q->payload_rx, _q->payload_sym);

            // decode payload
            _q->payload_valid = qpacketmodem_decode(_q->dec,
                                                    _q->payload_sym,
                                                    _q->payload_dec);

            // update statistics
            _q->framedatastats.num_frames_detected++;
            _q->framedatastats.num_headers_valid  += _q->payload_valid;
            _q->framedatastats.num_payloads_valid += _q->payload_valid;
            _q->framedatastats.num_bytes_received += _q->payload_valid ? 64 : 0;

            // invoke callback
            if (_q->callback != NULL) {
                // set framesyncstats internals
                _q->framesyncstats.evm           = qpilotsync_get_evm(_q->pilotsync);
                _q->framesyncstats.rssi          = 20*log10f(_q->gamma_hat);
                _q->framesyncstats.cfo           = nco_crcf_get_frequency(_q->mixer);
                _q->framesyncstats.framesyms     = _q->payload_sym;
                _q->framesyncstats.num_framesyms = 600;
                _q->framesyncstats.mod_scheme    = LIQUID_MODEM_QPSK;
                _q->framesyncstats.mod_bps       = 2;
                _q->framesyncstats.check         = LIQUID_CRC_24;
                _q->framesyncstats.fec0          = LIQUID_FEC_NONE;
                _q->framesyncstats.fec1          = LIQUID_FEC_GOLAY2412;

                // invoke callback method
                int rc =
                _q->callback(&_q->payload_dec[0],   // header is first 8 bytes
                             _q->payload_valid,
                             &_q->payload_dec[8],   // payload is last 64 bytes
                             64,
                             _q->payload_valid,
                             _q->framesyncstats,
                             _q->userdata);
            }

            // reset frame synchronizer
            return dsssframe64sync_reset(_q);
        }
    }
    return LIQUID_OK;
}

// DEPRECATED: enable debugging
int dsssframe64sync_debug_enable(dsssframe64sync _q)
{
    return LIQUID_OK;
}

// DEPRECATED: disable debugging
int dsssframe64sync_debug_disable(dsssframe64sync _q)
{
    return LIQUID_OK;
}

// DEPRECATED: print debugging information
int dsssframe64sync_debug_print(dsssframe64sync   _q,
                             const char * _filename)
{
    return LIQUID_OK;
}

// get detection threshold
float dsssframe64sync_get_threshold(dsssframe64sync _q)
{
    return qdetector_cccf_get_threshold(_q->detector);
}

// set detection threshold
int dsssframe64sync_set_threshold(dsssframe64sync _q,
                              float       _threshold)
{
    return qdetector_cccf_set_threshold(_q->detector, _threshold);
}

// reset frame data statistics
int dsssframe64sync_reset_framedatastats(dsssframe64sync _q)
{
    return framedatastats_reset(&_q->framedatastats);
}

// retrieve frame data statistics
framedatastats_s dsssframe64sync_get_framedatastats(dsssframe64sync _q)
{
    return _q->framedatastats;
}

