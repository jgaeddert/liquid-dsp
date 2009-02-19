//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.h"

#define FRAME64_RAMP_UP_LEN 64
#define FRAME64_PHASING_LEN 64
#define FRAME64_PN_LEN      64
#define FRAME64_RAMP_DN_LEN 64

// internal
//void framegen64_encode_header(unsigned char * _header_dec, unsigned char * _header_enc);
void framegen64_byte_to_syms(unsigned char _byte, unsigned char * _syms);
void framegen64_syms_to_byte(unsigned char * _syms, unsigned char * _byte);

struct framegen64_s {
    modem mod;
    fec enc;

    // buffers: preamble (BPSK)
    float ramp_up[FRAME64_RAMP_UP_LEN];
    float phasing[FRAME64_PHASING_LEN];
    float pn_sequence[FRAME64_PN_LEN];
    float ramp_dn[FRAME64_RAMP_DN_LEN];

    // header (QPSK)
    unsigned char header[32];
    unsigned char header_enc[64];
    unsigned char header_sym[256];

    // payload (QPSK)
    unsigned char payload[64];
    unsigned char payload_enc[128];
    unsigned char payload_sym[512];

    // pulse-shaping filter
    interp_crcf interp;
};

framegen64 framegen64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta)
{
    framegen64 fg = (framegen64) malloc(sizeof(struct framegen64_s));

    unsigned int i;

    // generate ramp_up
    for (i=0; i<FRAME64_RAMP_UP_LEN; i++) {
        fg->ramp_up[i] = (i%2) ? 1.0f : -1.0f;
        fg->ramp_up[i] *= kaiser(i,2*FRAME64_RAMP_UP_LEN,8.0f);
    }

    // generate ramp_dn
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        fg->ramp_dn[i] = (i%2) ? 1.0f : -1.0f;
        fg->ramp_dn[i] *= kaiser(i+FRAME64_RAMP_DN_LEN,2*FRAME64_RAMP_DN_LEN,8.0f);
    }

    // generate phasing pattern
    for (i=0; i<FRAME64_PHASING_LEN; i++)
        fg->phasing[i] = (i%2) ? 1.0f : -1.0f;

    // generate pn sequence
    msequence ms = msequence_create(6);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create pulse-shaping filter (k=2)
    unsigned int h_len = 2*2*_m + 1;
    float h[h_len];
    design_rrc_filter(2,_m,_beta,0,h);
    fg->interp = interp_crcf_create(2, h, h_len);

    // create FEC encoder
    fg->enc = fec_create(FEC_HAMMING74, NULL);

    // create modulator
    fg->mod = modem_create(MOD_QPSK, 2);

    return fg;
}

void framegen64_destroy(framegen64 _fg)
{
    interp_crcf_destroy(_fg->interp);
    fec_destroy(_fg->enc);
    free_modem(_fg->mod);
    free(_fg);
}

void framegen64_print(framegen64 _fg)
{
    printf("framegen:\n");
}

void framegen64_execute(framegen64 _fg, unsigned char * _payload, float complex * _y)
{
    // scramble payload
    memcpy(_fg->payload, _payload, 64);
    scramble_data(_fg->payload, 64);

    // compute crc32 on payload, append to header
    unsigned int payload_key = crc32_generate_key(_fg->payload, 64);
    _fg->header[0] = (payload_key >> 24) & 0xff;
    _fg->header[1] = (payload_key >> 16) & 0xff;
    _fg->header[2] = (payload_key >>  8) & 0xff;
    _fg->header[3] = (payload_key      ) & 0xff;

    // encode payload
    fec_encode(_fg->enc, 64, _fg->payload, _fg->payload_enc);

    // compute crc32 on header, append
    unsigned int i;
    for (i=4; i<28; i++)
        _fg->header[i] = rand() & 0xff;
    unsigned int header_key = crc32_generate_key(_fg->header, 28);
    _fg->header[28] = (header_key >> 28) & 0xff;
    _fg->header[29] = (header_key >> 16) & 0xff;
    _fg->header[30] = (header_key >>  8) & 0xff;
    _fg->header[31] = (header_key      ) & 0xff;

    // scramble header
    scramble_data(_fg->header, 32);

    // encode header
    fec_encode(_fg->enc, 32, _fg->header, _fg->header_enc);

    // generate header symbols
    for (i=0; i<64; i++)
        framegen64_byte_to_syms(_fg->header_enc[i], &(_fg->header_sym[4*i]));

    // generate payload symbols
    for (i=0; i<128; i++)
        framegen64_byte_to_syms(_fg->payload_enc[i], &(_fg->payload_sym[4*i]));

    unsigned int n=0;

    // ramp up
    for (i=0; i<FRAME64_RAMP_UP_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->ramp_up[i], &_y[n]);
        n+=2;
    }

    // phasing
    for (i=0; i<FRAME64_PHASING_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->phasing[i], &_y[n]);
        n+=2;
    }

    // p/n sequence
    for (i=0; i<FRAME64_PN_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->pn_sequence[i], &_y[n]);
        n+=2;
    }

    float complex x;
    // header
    for (i=0; i<256; i++) {
        modulate(_fg->mod, _fg->header_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // payload
    for (i=0; i<512; i++) {
        modulate(_fg->mod, _fg->payload_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // ramp down
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->ramp_dn[i], &_y[n]);
        n+=2;
    }

    assert(n==2048);
}

void framegen64_flush(framegen64 _fg, unsigned int _n, float complex * _y)
{
    if (_n % 2) {
        printf("error: framegen64_flush(), _n must be even\n");
        exit(-1);
    }

    unsigned int i;
    for (i=0; i<_n; i+=2)
        interp_crcf_execute(_fg->interp, 0, &_y[i]);
}

// 
// Internal
// 

void framegen64_byte_to_syms(unsigned char _byte, unsigned char * _syms)
{
    _syms[0] = (_byte >> 6) & 0x03;
    _syms[1] = (_byte >> 4) & 0x03;
    _syms[2] = (_byte >> 2) & 0x03;
    _syms[3] = (_byte     ) & 0x03;
}

void framegen64_syms_to_byte(unsigned char * _syms, unsigned char * _byte)
{
    unsigned char b=0;
    b |= (_syms[0] << 6) & 0xc0;
    b |= (_syms[1] << 4) & 0x30;
    b |= (_syms[2] << 2) & 0x0c;
    b |= (_syms[3]     ) & 0x03;
    *_byte = b;
}

