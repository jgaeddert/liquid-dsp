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
// flexframegen.c
//
// flexible frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_FLEXFRAMEGEN          1
#define DEBUG_FLEXFRAMEGEN_PRINT    0

// default flexframegen properties
static flexframegenprops_s flexframegenprops_default = {
    16,                 // rampup_len
    16,                 // phasing_len
    0,                  // payload_len
    LIQUID_CRC_16,      // check
    LIQUID_FEC_NONE,    // fec0
    LIQUID_FEC_NONE,    // fec1
    LIQUID_MODEM_BPSK,  // mod_scheme
    16                  // rampdn_len
};

void flexframegenprops_init_default(flexframegenprops_s * _props)
{
    memmove(_props, &flexframegenprops_default, sizeof(flexframegenprops_s));
}

struct flexframegen_s {
    // buffers: preamble (BPSK)
    float complex * ramp_up;            // ramp up sequence
    float complex * phasing;            // phasing pattern sequence
    float complex   pn_sequence[64];    // p/n sequence
    float complex * ramp_dn;            // ramp down sequence

    // header (QPSK)
    modem mod_header;                   // header QPSK modulator
    packetizer p_header;                // header packetizer
    unsigned char header[19];           // header data (uncoded)
    unsigned char header_enc[32];       // header data (encoded)
    unsigned char header_sym[256];      // header symbols
    float complex header_samples[256];  // header samples

    // payload
    packetizer p_payload;               // payload packetizer
    modem mod_payload;                  // payload modulator
    unsigned char * payload;            // payload data (bytes)
    unsigned char * payload_sym;        // payload symbols (modem input)
    float complex * payload_samples;    // payload samples (modem output)
    unsigned int payload_enc_msg_len;   // length of encoded payload
    unsigned int payload_numalloc;
    unsigned int payload_sym_numalloc;
    unsigned int payload_samples_numalloc;

    // properties
    flexframegenprops_s props;

    unsigned int pnsequence_len;        // p/n sequence length
    unsigned int num_payload_symbols;   // number of payload symbols
    unsigned int frame_len;             // number of frame symbols
};

flexframegen flexframegen_create(flexframegenprops_s * _props)
{
    flexframegen fg = (flexframegen) malloc(sizeof(struct flexframegen_s));

    unsigned int i;

    // generate pn sequence
    fg->pnsequence_len = 64;
    msequence ms = msequence_create(6, 0x005b, 1);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create header objects
    fg->mod_header = modem_create(LIQUID_MODEM_BPSK);
    fg->p_header   = packetizer_create(19, LIQUID_CRC_16, LIQUID_FEC_HAMMING128, LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(fg->p_header)==32);

    // initial memory allocation for payload
    fg->p_payload = packetizer_create(0, LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE);
    fg->payload = (unsigned char*) malloc(1*sizeof(unsigned char));
    fg->payload_numalloc = 1;
    fg->payload_sym = (unsigned char*) malloc(1*sizeof(unsigned char));
    fg->payload_sym_numalloc = 1;
    fg->payload_samples = (float complex*) malloc(1*sizeof(float complex));
    fg->payload_samples_numalloc = 1;

    // create payload modem (initially bpsk, overridden by properties)
    fg->mod_payload = modem_create(LIQUID_MODEM_BPSK);

    // initialize properties
    if (_props != NULL)
        flexframegen_setprops(fg, _props);
    else
        flexframegen_setprops(fg, &flexframegenprops_default);

    flexframegen_configure_payload_buffers(fg);

    return fg;
}

void flexframegen_destroy(flexframegen _fg)
{
    // destroy header objects
    packetizer_destroy(_fg->p_header);
    modem_destroy(_fg->mod_header);

    // free internal payload buffers
    packetizer_destroy(_fg->p_payload);
    free(_fg->payload);
    free(_fg->payload_sym);
    free(_fg->payload_samples);

    // free payload objects
    free(_fg->mod_payload);

    // destroy frame generator
    free(_fg);
}

// get flexframegen properties
//  _fg     :   frame generator object
//  _props  :   frame generator properties structure pointer
void flexframegen_getprops(flexframegen _fg,
                           flexframegenprops_s * _props)
{
    // copy properties structure to output pointer
    memmove(_props, &_fg->props, sizeof(flexframegenprops_s));
}

// set flexframegen properties
//  _fg     :   frame generator object
//  _props  :   frame generator properties structure pointer
void flexframegen_setprops(flexframegen _fg,
                           flexframegenprops_s * _props)
{
    // validate input
    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported CRC scheme\n");
        exit(1);
    } else if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported FEC scheme\n");
        exit(1);
    } else if (_props->mod_scheme == LIQUID_MODEM_UNKNOWN ) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported modulation scheme\n");
        exit(1);
    }

    // copy properties to internal structure
    memmove(&_fg->props, _props, sizeof(flexframegenprops_s));

    // re-create payload packetizer
    _fg->p_payload = packetizer_recreate(_fg->p_payload,
                                         _fg->props.payload_len,
                                         _fg->props.check,
                                         _fg->props.fec0,
                                         _fg->props.fec1);
    _fg->payload_enc_msg_len = packetizer_get_enc_msg_len(_fg->p_payload);

    // re-create modem
    modem_destroy(_fg->mod_payload);
    _fg->mod_payload = modem_create(_fg->props.mod_scheme);

    // re-compute payload and frame lengths
    flexframegen_compute_payload_len(_fg);
    flexframegen_compute_frame_len(_fg);

    // reconfigure payload buffers (reallocate as necessary)
    flexframegen_configure_payload_buffers(_fg);
}

// print flexframegen object internals
void flexframegen_print(flexframegen _fg)
{
    printf("flexframegen [%u samples]:\n", _fg->frame_len);
    printf("    ramp up len         :   %u\n", _fg->props.rampup_len);
    printf("    phasing len         :   %u\n", _fg->props.phasing_len);
    printf("    p/n sequence len    :   %u\n", _fg->pnsequence_len);
    printf("    payload len, uncoded:   %u bytes\n", _fg->props.payload_len);
    printf("    payload crc         :   %s\n", crc_scheme_str[_fg->props.check][1]);
    printf("    fec (inner)         :   %s\n", fec_scheme_str[_fg->props.fec0][1]);
    printf("    fec (outer)         :   %s\n", fec_scheme_str[_fg->props.fec1][1]);
    printf("    payload len, coded  :   %u bytes\n", _fg->payload_enc_msg_len);
    printf("    modulation scheme   :   %s\n", modulation_types[_fg->props.mod_scheme].name);
    printf("    num payload symbols :   %u\n", _fg->num_payload_symbols);
    printf("    ramp dn len         :   %u\n", _fg->props.rampdn_len);
}

// get frame length (number of samples)
unsigned int flexframegen_getframelen(flexframegen _fg)
{
    return _fg->frame_len;
}

// exectue frame generator (create the frame)
//  _fg         :   frame generator object
//  _header     :   14-byte header
//  _payload    :   variable payload buffer (configured by setprops method)
//  _y          :   output frame symbols [size: frame_len x 1]
void flexframegen_execute(flexframegen _fg,
                          unsigned char * _header,
                          unsigned char * _payload,
                          float complex * _y)
{
    // write frame
    // TODO: write frame in pieces so as not to require excessively large output buffer
    unsigned int i, n=0;

    // ramp up
    for (i=0; i<_fg->props.rampup_len; i++) {
        _y[n] = ((n%2) ? 1.0f : -1.0f) * 0.5f * (1.0f - cos(M_PI*(float)(i)/(float)(_fg->props.rampup_len)));
        //_y[n++] = ((i%2) ? 1.0f : -1.0f) * kaiser(i, 2*_fg->props.rampup_len, 10.0f, 0.0f);
        //_y[n++] = ((i%2) ? 1.0f : -1.0f) * ((float)(i) / (float)(_fg->props.rampup_len));
        n++;
    }

    // phasing pattern
    for (i=0; i<_fg->props.phasing_len; i++) {
        _y[n] = (n%2) ? 1.0f : -1.0f;
        n++;
    }

    // p/n sequence
    for (i=0; i<64; i++)
        _y[n++] = _fg->pn_sequence[i];

    // copy and encode header
    memmove(_fg->header, _header, 14*sizeof(unsigned char));
    flexframegen_encode_header(_fg);
    flexframegen_modulate_header(_fg);
    memmove(&_y[n], _fg->header_samples, 256*sizeof(float complex));
    n += 256;

    // encode payload
    packetizer_encode(_fg->p_payload, _payload, _fg->payload);
    flexframegen_modulate_payload(_fg);
    memmove(&_y[n], _fg->payload_samples, (_fg->num_payload_symbols)*sizeof(float complex));
    n += _fg->num_payload_symbols;

    // ramp down
    for (i=0; i<_fg->props.rampdn_len; i++)
        _y[n++] = ((i%2) ? 1.0f : -1.0f) * 0.5f * (1.0f + cos(M_PI*(float)(i)/(float)(_fg->props.rampup_len)));
        //_y[n++] = ((i%2) ? 1.0f : -1.0f) * (1.0f - kaiser(i, 2*_fg->props.rampdn_len, 10.0f, 0.0f));
        //_y[n++] = ((i%2) ? 1.0f : -1.0f) * ((float)(_fg->props.rampdn_len-i) / (float)(_fg->props.rampdn_len));

    assert(n == _fg->frame_len);
}

//
// internal
//

// compute length of payload (number of symbols)
void flexframegen_compute_payload_len(flexframegen _fg)
{
    // num_payload_symbols = ceil( payload_len / bps )

    // compute integer division, keeping track of remainder
    unsigned int bps = modulation_types[_fg->props.mod_scheme].bps;
    div_t d = div(8*_fg->payload_enc_msg_len, bps);

    // extend number of payload symbols if remainder is present
    _fg->num_payload_symbols = d.quot + (d.rem ? 1 : 0);
}

// compute length of frame (number of symbols)
void flexframegen_compute_frame_len(flexframegen _fg)
{
    // compute payload length
    flexframegen_compute_payload_len(_fg);

    _fg->frame_len = 0;

    _fg->frame_len += _fg->props.rampup_len;    // ramp up length
    _fg->frame_len += _fg->props.phasing_len;   // phasing length
    _fg->frame_len += _fg->pnsequence_len;      // p/n sequence length
    _fg->frame_len += 256;                      // header length
    _fg->frame_len += _fg->num_payload_symbols; // payload length
    _fg->frame_len += _fg->props.rampdn_len;    // ramp down length
}

// configures payload buffers, reallocating memory if necessary
void flexframegen_configure_payload_buffers(flexframegen _fg)
{
    // compute frame length, including payload length
    flexframegen_compute_frame_len(_fg);

    // payload data (bytes)
    if (_fg->payload_numalloc != _fg->payload_enc_msg_len) {
        _fg->payload = (unsigned char*) realloc(_fg->payload, _fg->payload_enc_msg_len);
        _fg->payload_numalloc = _fg->payload_enc_msg_len;
        //printf("reallocating payload (payload data) : %u\n", _fg->payload_numalloc);
    }

    // payload symbols (modem input)
    if (_fg->payload_sym_numalloc != _fg->num_payload_symbols) {
        _fg->payload_sym = (unsigned char*) realloc(_fg->payload_sym, _fg->num_payload_symbols);
        _fg->payload_sym_numalloc = _fg->num_payload_symbols;
        //printf("reallocating payload_sym (payload symbols) : %u\n", _fg->payload_sym_numalloc);
    }

    // payload symbols (modem output)
    if (_fg->payload_samples_numalloc != _fg->num_payload_symbols) {
        _fg->payload_samples = (float complex*) realloc(_fg->payload_samples, _fg->num_payload_symbols*sizeof(float complex));
        _fg->payload_samples_numalloc = _fg->num_payload_symbols;
        //printf("reallocating payload_samples (modulated payload symbols) : %u\n",
        //        _fg->payload_samples_numalloc);
    }

}

// encode header of flexframe
void flexframegen_encode_header(flexframegen _fg)
{
    // first 14 bytes of header are user-defined

    // add payload length
    _fg->header[14] = (_fg->props.payload_len >> 8) & 0xff;
    _fg->header[15] = (_fg->props.payload_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    _fg->header[16]  = (unsigned int)(_fg->props.mod_scheme);

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [17]
    //  fec0    : least-significant 5 bits of [17]
    //  fec1    : least-significant 5 bits of [18]
    _fg->header[17]  = (_fg->props.check & 0x07) << 5;
    _fg->header[17] |= (_fg->props.fec0) & 0x1f;
    _fg->header[18]  = (_fg->props.fec1) & 0x1f;

    // scramble header
    scramble_data(_fg->header, 19);

    // run packet encoder
    packetizer_encode(_fg->p_header, _fg->header, _fg->header_enc);

#if DEBUG_FLEXFRAMEGEN_PRINT
    // print results
    printf("flexframegen_encode_header():\n");
    printf("    mod scheme  : %s\n", modulation_types[_fg->props.mod_scheme].fullname);
    printf("    payload len : %u\n", _fg->props.payload_len);
    printf("    header key  : 0x%.8x\n", header_key);

    printf("    user data   :");
    for (i=0; i<14; i++)
        printf(" %.2x", _user_header[i]);
    printf("\n");
#endif
}

// modulate header into QPSK symbols
void flexframegen_modulate_header(flexframegen _fg)
{
    unsigned int i;

    // unpack header symbols
    for (i=0; i<32; i++) {
        _fg->header_sym[8*i+0] = (_fg->header_enc[i] >> 7) & 0x01;
        _fg->header_sym[8*i+1] = (_fg->header_enc[i] >> 6) & 0x01;
        _fg->header_sym[8*i+2] = (_fg->header_enc[i] >> 5) & 0x01;
        _fg->header_sym[8*i+3] = (_fg->header_enc[i] >> 4) & 0x01;
        _fg->header_sym[8*i+4] = (_fg->header_enc[i] >> 3) & 0x01;
        _fg->header_sym[8*i+5] = (_fg->header_enc[i] >> 2) & 0x01;
        _fg->header_sym[8*i+6] = (_fg->header_enc[i] >> 1) & 0x01;
        _fg->header_sym[8*i+7] = (_fg->header_enc[i]     ) & 0x01;
    }

    // modulate symbols
    for (i=0; i<256; i++)
        modem_modulate(_fg->mod_header, _fg->header_sym[i], &_fg->header_samples[i]);
}

// modulate payload into symbols using user-defined modem
void flexframegen_modulate_payload(flexframegen _fg)
{
    // clear payload
    memset(_fg->payload_sym, 0x00, _fg->payload_enc_msg_len);

    // repack 8-bit payload bytes into 'bps'-bit payload symbols
    unsigned int bps = modulation_types[_fg->props.mod_scheme].bps;
    unsigned int num_written;
    liquid_repack_bytes(_fg->payload,     8,   _fg->payload_enc_msg_len,
                        _fg->payload_sym, bps, _fg->num_payload_symbols,
                        &num_written);

    // modulate symbols
    unsigned int i;
    for (i=0; i<_fg->num_payload_symbols; i++)
        modem_modulate(_fg->mod_payload, _fg->payload_sym[i], &_fg->payload_samples[i]);
}

