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
// fskframesync.c
//

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

void fskframesync_build_preamble(fskframesync _q);

unsigned int fskframesync_execute_seekpn(fskframesync _q, liquid_float_complex * _x, unsigned int _len);

int fskframesync_step(fskframesync _q, liquid_float_complex _x, liquid_float_complex * _y);

unsigned int fskframesync_execute_rxpreamble(fskframesync _q, liquid_float_complex * _x, unsigned int _len);

void fskframesync_decode_header(fskframesync _q);

void fskframesync_decode_payload(fskframesync _q);

unsigned int fskframesync_execute_rxheader(fskframesync _q, liquid_float_complex * _x, unsigned int _len);

unsigned int fskframesync_execute_rxpayload(fskframesync _q, liquid_float_complex * _x, unsigned int _len);

void fskframesync_configure_payload(fskframesync _q);

static fskframegenprops_s fskframesyncprops_header_default = {
    FSKFRAME_H_CRC,
    FSKFRAME_H_FEC0,
    FSKFRAME_H_FEC1,
    FSKFRAME_H_BITS_PER_SYMBOL,
    FSKFRAME_H_SAMPLES_PER_SYMBOL,
};

enum state {
    FSKFRAMESYNC_STATE_DETECTFRAME = 0,
    FSKFRAMESYNC_STATE_RXPREAMBLE,
    FSKFRAMESYNC_STATE_RXHEADER,
    FSKFRAMESYNC_STATE_RXPAYLOAD,
};

struct fskframesync_s {
    framesync_callback callback;
    void *             userdata;
    framesyncstats_s   framesyncstats;
    framedatastats_s   framedatastats;
    float bandwidth;

    qdetector_cccf detector;
    float          tau_hat;
    float          dphi_hat;
    float          phi_hat;
    float          gamma_hat;
    nco_crcf       mixer;

    firpfb_crcf  mf;
    unsigned int mf_k;
    unsigned int mf_m;
    float        mf_beta;
    unsigned int mf_npfb;
    int          mf_counter;
    unsigned int mf_pfb_index;

    liquid_float_complex * preamble_rx;
    unsigned int           preamble_len;

    fskframegenprops_s     header_props;
    unsigned int           header_user_len;
    unsigned int           header_dec_len;
    unsigned char *        header_dec;
    unsigned int           header_len;
    symbolwriter           header_enc_writer;
    packetizer             header_packetizer;
    fskdem                 header_demod;
    liquid_float_complex * header_samples;
    int                    header_valid;

    fskframegenprops_s     payload_props;
    unsigned int           payload_dec_len;
    unsigned char *        payload_dec;
    unsigned int           payload_enc_len;
    unsigned int           payload_len;
    symbolwriter           payload_enc_writer;
    fskdem                 payload_demod;
    liquid_float_complex * payload_samples;
    packetizer             payload_packetizer;
    int                    payload_valid;

    unsigned int sample_counter;
    enum state state;
};

fskframesync fskframesync_create(framesync_callback _callback, void * _userdata)
{
    fskframesync q = (fskframesync)calloc(1, sizeof(struct fskframesync_s));
    q->callback = _callback;
    q->userdata = _userdata;

    /*
    q->mf_k = 2;
    q->mf_m = 7;
    q->mf_beta = 0.3f;
    */

    fskframesync_set_bandwidth(q, 0.25f);

    /*
    q->mf_npfb = 32;
    q->mf = firpfb_crcf_create_rnyquist(LIQUID_FIRFILT_ARKAISER, q->mf_npfb, q->mf_k, q->mf_m, q->mf_beta);
    */

    q->mixer = nco_crcf_create(LIQUID_NCO);

    q->header_enc_writer  = symbolwriter_create();
    q->header_dec     = NULL;
    q->header_samples = NULL;
    q->header_user_len = FSKFRAME_H_USER_DEFAULT;
    fskframesync_set_header_props(q, NULL);

    q->payload_enc_writer  = symbolwriter_create();
    q->payload_dec     = NULL;
    q->payload_samples = NULL;

    fskframesync_reset(q);

    /*
    printf("sync preamble_len: %d\n", q->preamble_len);
    printf("sync header_len: %d\n", q->header_len);
    */

    return q;
}

void fskframesync_destroy(fskframesync _q)
{
    if (!_q) {
        return;
    }

    qdetector_cccf_destroy(_q->detector);
    nco_crcf_destroy(_q->mixer);
    // firpfb_crcf_destroy(_q->mf);
    free(_q->preamble_rx);
    free(_q->header_dec);
    symbolwriter_destroy(_q->header_enc_writer);
    packetizer_destroy(_q->header_packetizer);
    fskdem_destroy(_q->header_demod);
    free(_q->header_samples);
    free(_q->payload_dec);
    symbolwriter_destroy(_q->payload_enc_writer);
    fskdem_destroy(_q->payload_demod);
    free(_q->payload_samples);
    packetizer_destroy(_q->payload_packetizer);

    free(_q);
}

void fskframesync_reset(fskframesync _q)
{
    qdetector_cccf_reset(_q->detector);

    nco_crcf_reset(_q->mixer);
    // firpfb_crcf_reset(_q->mf);

    _q->state = FSKFRAMESYNC_STATE_DETECTFRAME;
    _q->sample_counter = 0;
}

int fskframesync_is_frame_open(fskframesync _q)
{
    return (_q->state == FSKFRAMESYNC_STATE_DETECTFRAME) ? 0 : 1;
}

void fskframesync_set_bandwidth(fskframesync _q, float _bw)
{
    _q->bandwidth = _bw;

    fskframesync_build_preamble(_q);
}

void fskframesync_build_preamble(fskframesync _q)
{
    unsigned int i;
    liquid_float_complex pn;

    if (_q->detector) {
        qdetector_cccf_destroy(_q->detector);
        _q->detector = NULL;
    }

    _q->preamble_len = 63 * FSKFRAME_PRE_K;

    _q->preamble_rx = (liquid_float_complex *)realloc(_q->preamble_rx, _q->preamble_len * sizeof(liquid_float_complex));
    liquid_float_complex * preamble_samples = (liquid_float_complex *)calloc(_q->preamble_len, sizeof(liquid_float_complex));
    fskmod preamble_mod = fskmod_create(1, FSKFRAME_PRE_K, _q->bandwidth);
    msequence ms = msequence_create(6, 0x6d, 1);
    /*
    printf("sync preamble:");
    */
    for (i = 0; i < 63; i++) {
        fskmod_modulate(preamble_mod, msequence_advance(ms), preamble_samples + (i * FSKFRAME_PRE_K));
        /*
        printf(" %.4f %.4f %.4f %.4f", preamble_samples[i * FSKFRAME_PRE_K], preamble_samples[i * FSKFRAME_PRE_K + 1], preamble_samples[i * FSKFRAME_PRE_K + 2], preamble_samples[i * FSKFRAME_PRE_K + 3]);
        */
    }
    /*
    printf("\n");
    */
    msequence_destroy(ms);
    _q->detector = qdetector_cccf_create(preamble_samples, 64);
    qdetector_cccf_set_threshold(_q->detector, 0.5f);
    fskmod_destroy(preamble_mod);
    free(preamble_samples);
}

void fskframesync_reconfigure_header(fskframesync _q)
{
    fskdem_destroy(_q->header_demod);
    _q->header_demod = fskdem_create(_q->header_props.bits_per_symbol,
                                     _q->header_props.samples_per_symbol,
                                     _q->bandwidth);

    _q->header_dec_len = FSKFRAME_H_DEC + _q->header_user_len;
    _q->header_dec = (unsigned char *)realloc(_q->header_dec, _q->header_dec_len * sizeof(unsigned char));
    _q->header_packetizer = packetizer_recreate(_q->header_packetizer,
                                                _q->header_dec_len,
                                                _q->header_props.check,
                                                _q->header_props.fec0,
                                                _q->header_props.fec1);
    unsigned int header_enc_len = packetizer_get_enc_msg_len(_q->header_packetizer);
    // printf("sync header enc len: %d\n", header_enc_len);
    symbolwriter_reset(_q->header_enc_writer, 8*header_enc_len);
    // printf("sync writer header len: %d\n", symbolwriter_length(_q->header_enc_writer));

    _q->header_samples = (liquid_float_complex *)realloc(_q->header_samples,
                                                         _q->header_props.samples_per_symbol * sizeof(liquid_float_complex));
    unsigned int num_header_symbols = (8 * header_enc_len) / _q->header_props.bits_per_symbol;
    if (header_enc_len % _q->header_props.bits_per_symbol) {
        num_header_symbols++;
    }
    _q->header_len = _q->header_props.samples_per_symbol * num_header_symbols;
}

void fskframesync_set_header_len(fskframesync _q, unsigned int _len)
{
    _q->header_user_len = _len;
    fskframesync_reconfigure_header(_q);
}

int fskframesync_set_header_props(fskframesync _q, fskframegenprops_s * _props)
{
    if (_props == NULL) {
        _props = &fskframesyncprops_header_default;
    }

    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: fskframesync_set_header_props(), invalid/unsupported CRC scheme\n");
        exit(1);
    }

    if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: fskframesync_set_header_props(), invalid/unsupported FEC scheme\n");
        exit(1);
    }

    if (_props->bits_per_symbol < 1 || _props->bits_per_symbol > 8) {
        fprintf(stderr, "error: fskframesync_set_header_props(), invalid/unsupported bits per symbol\n");
        exit(1);
    }

    // XXX 2048 or 256?
    if (_props->samples_per_symbol < (1 << _props->bits_per_symbol) || _props->samples_per_symbol > 2048) {
        fprintf(stderr, "error: fskframesync_set_header_props(), invalid/unsupported samples per symbol\n");
        exit(1);
    }

    memmove(&_q->header_props, _props, sizeof(fskframegenprops_s));
    fskframesync_reconfigure_header(_q);

    return 0;
}

void fskframesync_execute(fskframesync _q, liquid_float_complex * _buffer, unsigned int _buffer_len)
{
    unsigned int i;
    for (i = 0; i < _buffer_len; ) {
        unsigned int read;
        switch (_q->state) {
        case FSKFRAMESYNC_STATE_DETECTFRAME:
            // detect frame (look for p/n sequence)
            read = fskframesync_execute_seekpn(_q, _buffer, _buffer_len - i);
            break;
        case FSKFRAMESYNC_STATE_RXPREAMBLE:
            // receive p/n sequence symbols
            read = fskframesync_execute_rxpreamble(_q, _buffer, _buffer_len - i);
            break;
        case FSKFRAMESYNC_STATE_RXHEADER:
            // receive header symbols
            read = fskframesync_execute_rxheader(_q, _buffer, _buffer_len - i);
            break;
        case FSKFRAMESYNC_STATE_RXPAYLOAD:
            // receive payload symbols
            read = fskframesync_execute_rxpayload(_q, _buffer, _buffer_len - i);
            break;
        default:
            fprintf(stderr, "error: fskframesync_exeucte(), unknown/unsupported state\n");
            exit(1);
        }
        i += read;
        _buffer += read;
    }
}

unsigned int fskframesync_execute_seekpn(fskframesync _q,
                                         liquid_float_complex * _buffer,
                                         unsigned int _buffer_len)
{
    unsigned int i;
    float complex * v;
    for (i = 0; i < _buffer_len; i++) {
        // push through pre-demod synchronizer
        v = qdetector_cccf_execute(_q->detector, _buffer[i]);

        // check if frame has been detected
        if (v != NULL)
            break;
    }

    if (v == NULL) {
        return i;
    }

    // get estimates
    _q->tau_hat   = qdetector_cccf_get_tau(_q->detector);
    _q->gamma_hat = qdetector_cccf_get_gamma(_q->detector);
    _q->dphi_hat  = qdetector_cccf_get_dphi(_q->detector);
    _q->phi_hat   = qdetector_cccf_get_phi(_q->detector);

    /*
    // set appropriate filterbank index
    if (_q->tau_hat > 0) {
        _q->pfb_index  = (unsigned int)(_q->tau_hat * _q->npfb) % _q->npfb;
        _q->mf_counter = 0;
    } else {
        _q->pfb_index  = (unsigned int)((1.0f + _q->tau_hat) * _q->npfb) % _q->npfb;
        _q->mf_counter = 1;
    }

    // output filter scale (gain estimate, scaled by 1/2 for k=2 samples/symbol)
    firpfb_crcf_set_scale(_q->mf, 0.5f / _q->gamma_hat);
    */

    // set frequency/phase of mixer
    nco_crcf_set_frequency(_q->mixer, _q->dphi_hat);
    nco_crcf_set_phase(_q->mixer, _q->phi_hat);

    /*
    printf("rxpreamble\n");
    printf("dphi_hat %.4f phi_hat %.4f\n", _q->dphi_hat, _q->phi_hat);
    */

    // update state
    _q->state = FSKFRAMESYNC_STATE_RXPREAMBLE;

    // run buffered samples through synchronizer
    unsigned int buf_len = qdetector_cccf_get_buf_len(_q->detector);
    fskframesync_execute(_q, v, buf_len);

    return i;
}

int fskframesync_step(fskframesync           _q,
                      liquid_float_complex   _x,
                      liquid_float_complex * _y)
{
    // mix sample down
    nco_crcf_mix_down(_q->mixer, _x, _y);
    nco_crcf_step    (_q->mixer);

    /*
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
    */
    return 1;
}

unsigned int fskframesync_execute_rxpreamble(fskframesync _q,
                                             liquid_float_complex * _buffer,
                                             unsigned int _buffer_len)
{
    unsigned int i;
    liquid_float_complex mf_out;
    int sample_available;

    /*
    // this delay comes from firpfb_create_rnyquist()
    unsigned int mf_delay = 2 * _q->m;
    */

    unsigned int num_samples = _q->preamble_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    for (i = 0; i < num_samples; i++) {
        sample_available = fskframesync_step(_q, _buffer[i], &mf_out);

        if (!sample_available) {
            continue;
        }

        _q->preamble_rx[_q->sample_counter] = mf_out;

        _q->sample_counter++;

        if (_q->sample_counter == _q->preamble_len) {
            // printf("rxheader\n");
            _q->sample_counter = 0;
            _q->state = FSKFRAMESYNC_STATE_RXHEADER;
            assert(i == num_samples - 1);
        }
    }

    return num_samples;
}

void fskframesync_handle_invalid_header(fskframesync _q)
{
    _q->framedatastats.num_frames_detected++;

    if (_q->callback != NULL) {
        // TODO revisit these stats
        _q->framesyncstats.evm           = 0.f;
        _q->framesyncstats.rssi          = 20 * log10f(_q->gamma_hat);
        _q->framesyncstats.cfo           = nco_crcf_get_frequency(_q->mixer);
        _q->framesyncstats.framesyms     = NULL;
        _q->framesyncstats.num_framesyms = 0;
        _q->framesyncstats.mod_scheme    = LIQUID_MODEM_UNKNOWN;
        _q->framesyncstats.mod_bps       = 0;
        _q->framesyncstats.check         = LIQUID_CRC_UNKNOWN;
        _q->framesyncstats.fec0          = LIQUID_FEC_UNKNOWN;
        _q->framesyncstats.fec1          = LIQUID_FEC_UNKNOWN;

        _q->callback(_q->header_dec, _q->header_valid, NULL, 0, 0, _q->framesyncstats, _q->userdata);
    }

    fskframesync_reset(_q);
}

void fskframesync_decode_header(fskframesync _q)
{
    unsigned int i;
    const unsigned char * encoded = symbolwriter_bytes(_q->header_enc_writer);
    /*
    printf("encoded:");
    for (i = 0; i < symbolwriter_length(_q->header_enc_writer) / 8; i++) {
        printf(" %02x", encoded[i]);
    }
    printf("\n");
    */
    _q->header_valid = packetizer_decode(_q->header_packetizer, encoded, _q->header_dec);

    if (!_q->header_valid) {
        return;
    }

    unsigned int n = _q->header_user_len;

    if (_q->header_dec[n+0] != FSKFRAME_PROTOCOL) {
        fprintf(stderr,"warning: fskframesync_decode_header(), invalid framing version\n");
        _q->header_valid = 0;
        return;
    }

    unsigned int payload_dec_len = (_q->header_dec[n+1] << 8) | (_q->header_dec[n+2]);

    unsigned int check = (_q->header_dec[n+3] >> 5) & 0x07;
    unsigned int fec0  = (_q->header_dec[n+3]     ) & 0x1f;
    unsigned int bps   = (_q->header_dec[n+4] >> 5) & 0x07;
    unsigned int fec1  = (_q->header_dec[n+4]     ) & 0x1f;

    unsigned int samples_per_symbol = _q->header_dec[n+5];

    if (check == LIQUID_CRC_UNKNOWN || check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "warning: fskframesync_decode_header(), decoded CRC exceeds available\n");
        _q->header_valid = 0;
        return;
    } else if (fec0 == LIQUID_FEC_UNKNOWN || fec0 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,
                "warning: fskframesync_decode_header(), decoded FEC (inner) exceeds available\n");
        _q->header_valid = 0;
        return;
    } else if (fec1 == LIQUID_FEC_UNKNOWN || fec1 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,
                "warning: fskframesync_decode_header(), decoded FEC (outer) exceeds available\n");
        _q->header_valid = 0;
        return;
    }

    fskdem_destroy(_q->payload_demod);
    _q->payload_demod = fskdem_create(bps,
                                      samples_per_symbol,
                                      _q->bandwidth);

    _q->payload_dec_len = payload_dec_len;
    _q->payload_dec = (unsigned char *)realloc(_q->payload_dec, (_q->payload_dec_len) * sizeof(unsigned char));
    _q->payload_packetizer = packetizer_recreate(_q->payload_packetizer,
                                                 _q->payload_dec_len,
                                                 (crc_scheme)check,
                                                 (fec_scheme)fec0,
                                                 (fec_scheme)fec1);

    _q->payload_enc_len = packetizer_get_enc_msg_len(_q->payload_packetizer);
    symbolwriter_reset(_q->payload_enc_writer, 8*_q->payload_enc_len);

    _q->payload_props.check = check;
    _q->payload_props.fec0 = fec0;
    _q->payload_props.fec1 = fec1;
    _q->payload_props.bits_per_symbol = bps;
    _q->payload_props.samples_per_symbol = samples_per_symbol;

    _q->payload_samples = (liquid_float_complex *)realloc(_q->payload_samples, _q->payload_props.samples_per_symbol * sizeof(liquid_float_complex));

    unsigned int num_payload_symbols = (8 * _q->payload_enc_len) / _q->payload_props.bits_per_symbol;
    if (_q->payload_enc_len % _q->payload_props.bits_per_symbol) {
        num_payload_symbols++;
    }
    _q->payload_len = _q->payload_props.samples_per_symbol * num_payload_symbols;
}

unsigned int fskframesync_execute_rxheader(fskframesync _q,
                                           liquid_float_complex * _buffer,
                                           unsigned int _buffer_len)
{
    unsigned int i;
    liquid_float_complex mf_out;
    int sample_available;

    unsigned int num_samples = _q->header_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    /*
    printf("received preamble:");
    for (i = 0; i < _q->preamble_len; i++) {
        printf(" %.4f", _q->preamble_rx[i]);
    }
    printf("\n");
    */

    for (i = 0; i < num_samples; i++) {
        sample_available = fskframesync_step(_q, _buffer[i], &mf_out);

        if (!sample_available) {
            continue;
        }

        _q->header_samples[_q->sample_counter % _q->header_props.samples_per_symbol] = mf_out;
        _q->sample_counter++;

        if (_q->sample_counter % _q->header_props.samples_per_symbol == 0) {
            unsigned int sym = fskdem_demodulate(_q->header_demod, _q->header_samples);
            symbolwriter_write(_q->header_enc_writer, _q->header_props.bits_per_symbol, sym);
        }
    }

    // printf("sample counter %d\n", _q->sample_counter);

    if (_q->sample_counter == _q->header_len) {
        fskframesync_decode_header(_q);

        if (_q->header_valid) {
            _q->sample_counter = 0;
            // printf("rxpayload\n");
            _q->state = FSKFRAMESYNC_STATE_RXPAYLOAD;
        } else {
            // printf("invalid header\n");
            fskframesync_handle_invalid_header(_q);
        }
    }

    return num_samples;
}

void fskframesync_decode_payload(fskframesync _q)
{
    const unsigned char * encoded = symbolwriter_bytes(_q->payload_enc_writer);
    _q->payload_valid = packetizer_decode(_q->payload_packetizer, encoded, _q->payload_dec);

    if (_q->callback != NULL) {
        _q->framesyncstats.evm           = 0.f;
        _q->framesyncstats.rssi          = 20 * log10f(_q->gamma_hat);
        _q->framesyncstats.cfo           = nco_crcf_get_frequency(_q->mixer);
        _q->framesyncstats.framesyms     = NULL;
        _q->framesyncstats.num_framesyms = 0;
        _q->framesyncstats.mod_scheme    = LIQUID_MODEM_UNKNOWN;
        _q->framesyncstats.mod_bps       = _q->payload_props.bits_per_symbol;
        _q->framesyncstats.check         = _q->payload_props.check;
        _q->framesyncstats.fec0          = _q->payload_props.fec0;
        _q->framesyncstats.fec1          = _q->payload_props.fec1;

        _q->callback(_q->header_dec,
                     _q->header_valid,
                     _q->payload_dec,
                     _q->payload_dec_len,
                     _q->payload_valid,
                     _q->framesyncstats,
                     _q->userdata);
    }

}

unsigned int fskframesync_execute_rxpayload(fskframesync _q,
                                           liquid_float_complex * _buffer,
                                           unsigned int _buffer_len)
{
    unsigned int i;
    liquid_float_complex mf_out;
    int sample_available;

    unsigned int num_samples = _q->payload_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    for (i = 0; i < num_samples; i++) {
        sample_available = fskframesync_step(_q, _buffer[i], &mf_out);

        if (!sample_available) {
            continue;
        }

        _q->payload_samples[_q->sample_counter % _q->payload_props.samples_per_symbol] = mf_out;
        _q->sample_counter++;

        if (_q->sample_counter % _q->payload_props.samples_per_symbol == 0) {
            unsigned int sym = fskdem_demodulate(_q->payload_demod, _q->payload_samples);
            symbolwriter_write(_q->payload_enc_writer, _q->payload_props.bits_per_symbol, sym);
        }
    }

    if (_q->sample_counter == _q->payload_len) {
        fskframesync_decode_payload(_q);

        fskframesync_reset(_q);
    }

    return num_samples;
}
