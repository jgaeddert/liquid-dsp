//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.h"

#define FRAME64_RAMP_UP_LEN 64
#define FRAME64_PHASING_LEN 64
#define FRAME64_PN_LEN      64
#define FRAME64_RAMP_DN_LEN 64

#define FRAMEGEN64_PHASING_0    ( 0.70711f + 0.70711f*_Complex_I)
#define FRAMEGEN64_PHASING_1    (-0.70711f - 0.70711f*_Complex_I)

//#define DEBUG

// internal
//void framegen64_encode_header(unsigned char * _header_dec, unsigned char * _header_enc);
void framegen64_byte_to_syms(unsigned char _byte, unsigned char * _syms);

struct framegen64_s {
    modem mod;
    fec enc;

    // buffers: preamble (BPSK)
    float complex ramp_up[FRAME64_RAMP_UP_LEN];
    float complex phasing[FRAME64_PHASING_LEN];
    float complex pn_sequence[FRAME64_PN_LEN];
    float complex ramp_dn[FRAME64_RAMP_DN_LEN];

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
        fg->ramp_up[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
        fg->ramp_up[i] *= kaiser(i,2*FRAME64_RAMP_UP_LEN,8.0f);
    }

    // generate ramp_dn
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        fg->ramp_dn[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
        fg->ramp_dn[i] *= kaiser(i+FRAME64_RAMP_DN_LEN,2*FRAME64_RAMP_DN_LEN,8.0f);
    }

    // generate phasing pattern
    for (i=0; i<FRAME64_PHASING_LEN; i++)
        fg->phasing[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;

    // generate pn sequence
    msequence ms = msequence_create(6);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
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
    modem_destroy(_fg->mod);
    free(_fg);
}

void framegen64_print(framegen64 _fg)
{
    printf("framegen:\n");
}

void framegen64_execute(framegen64 _fg, unsigned char * _header, unsigned char * _payload, float complex * _y)
{
    unsigned int i;
    memcpy(_fg->header, _header, 24);

    memcpy(_fg->payload, _payload, 64);
#ifdef DEBUG
    printf("payload (tx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fg->payload[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

    // compute crc32 on payload, append to header
    unsigned int payload_key = crc32_generate_key(_fg->payload, 64);
    //printf("tx: payload_key: 0x%8x\n", payload_key);
    _fg->header[24] = (payload_key >> 24) & 0xff;
    _fg->header[25] = (payload_key >> 16) & 0xff;
    _fg->header[26] = (payload_key >>  8) & 0xff;
    _fg->header[27] = (payload_key      ) & 0xff;

    // scramble payload data
    scramble_data(_fg->payload, 64);

    // encode payload
    fec_encode(_fg->enc, 64, _fg->payload, _fg->payload_enc);

    // compute crc32 on header, append
    unsigned int header_key = crc32_generate_key(_fg->header, 28);
    //printf("tx: header_key:  0x%8x\n", header_key);
    _fg->header[28] = (header_key >> 24) & 0xff;
    _fg->header[29] = (header_key >> 16) & 0xff;
    _fg->header[30] = (header_key >>  8) & 0xff;
    _fg->header[31] = (header_key      ) & 0xff;

#ifdef DEBUG
    printf("header (tx):\n");
    for (i=0; i<32; i++) {
        printf("%2x ", _fg->header[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

    // scramble header data
    scramble_data(_fg->header, 32);

    // encode header
    fec_encode(_fg->enc, 32, _fg->header, _fg->header_enc);

#ifdef DEBUG
    printf("header ENCODED (tx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fg->header_enc[i]);
        if (!((i+1)%16)) printf("\n");
    }
    printf("\n");
#endif

    // generate header symbols
    //printf("header tx syms:\n");
    for (i=0; i<64; i++) {
        framegen64_byte_to_syms(_fg->header_enc[i], &(_fg->header_sym[4*i]));
        /*
        printf("%4u:",i);
        printf("  %2x ", _fg->header_sym[4*i+0]);
        printf("  %2x ", _fg->header_sym[4*i+1]);
        printf("  %2x ", _fg->header_sym[4*i+2]);
        printf("  %2x ", _fg->header_sym[4*i+3]);
        printf("\n");
        */
    }


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

