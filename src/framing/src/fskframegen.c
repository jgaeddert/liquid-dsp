#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>


#include "liquid.internal.h"

struct bitreader_s {
    unsigned char * buffer;
    unsigned int buffer_index;
    unsigned int buffer_len;
    unsigned char current;
    unsigned int current_len;
};

struct bitreader_s * bitreader_create()
{
    struct bitreader_s * r = (struct bitreader_s *)calloc(1, sizeof(struct bitreader_s));

    return r;
}

void bitreader_set_source(struct bitreader_s * _r,
                          unsigned char      * _buffer,
                          unsigned int         _buffer_len)
{

    _r->buffer = _buffer;
    _r->buffer_len = _buffer_len;
    _r->buffer_index = 0;
    _r->current = _buffer[0];
    _r->current_len = 8;
}

void bitreader_destroy(struct bitreader_s * _r)
{
    free(_r);
}

unsigned char bitreader_read(struct bitreader_s * _r,
                              unsigned int          _len)
{
    // we will only read up to 8 bits, so we will only read
    // one or two bytes of _r->buffer

    unsigned char res = 0;

    if (_r->current_len < _len) {
        // take the rest of the current byte
        res = _r->current & ((1 << _r->current_len) - 1);

        _r->buffer_index += 1;
        _r->current = _r->buffer[_r->buffer_index];

        _len -= _r->current_len;
        _r->current_len = 8;

        // make space for the rest of the read
        res <<= _len;
    }

    unsigned int shift = _r->current_len - _len;
    unsigned char mask = ((1 << _len) - 1) << shift;

    res |= (_r->current & mask) >> (_r->current_len - _len);
    _r->current_len -= _len;

    return res;
}

unsigned int bitreader_length(struct bitreader_s * _r)
{
    return _r->buffer_len;
}

// fskframegen
unsigned int fskframegen_write_preamble(fskframegen _q, liquid_float_complex * _y, unsigned int _len);
unsigned int fskframegen_write_header(  fskframegen _q, liquid_float_complex * _y, unsigned int _len);
unsigned int fskframegen_write_payload( fskframegen _q, liquid_float_complex * _y, unsigned int _len);
void fskframegen_reconfigure(   fskframegen _q);
void fskframegen_reconfigure_header(fskframegen _q);

static fskframegenprops_s fskframegenprops_default = {
    LIQUID_CRC_16,
    LIQUID_FEC_NONE,
    LIQUID_FEC_NONE,
    1,
    2,
};

static fskframegenprops_s fskframegenprops_header_default = {
    FSKFRAME_H_CRC,
    FSKFRAME_H_FEC0,
    FSKFRAME_H_FEC1,
    FSKFRAME_H_BITS_PER_SYMBOL,
    FSKFRAME_H_SAMPLES_PER_SYMBOL,
};

enum state {
    STATE_PREAMBLE,         // preamble
    STATE_HEADER,           // header
    STATE_PAYLOAD,          // payload (frame)
};

// fskframe object structure
struct fskframegen_s {
    float bandwidth;  // bandwidth for all fsk mods

    // preamble
    msequence preamble_ms;      // preamble p/n sequence
    fskmod    preamble_mod;
    liquid_float_complex * preamble_samples;  // modulated preamble symbol [FSKFRAME_PRE_K]
    unsigned int ramp_len;  // ramp up length
    unsigned int preamble_len;  // length of preamble in samples

    // header
    unsigned int header_user_len;  // unencoded header user length
    unsigned int header_enc_len;  // encoded header length (user + reserved)
    unsigned char * header_dec; // uncoded header [header_user_len + FSKFRAME_H_DEC]
    unsigned char * header_enc; // encoded header [header_enc_len]
    struct bitreader_s * header_reader;  // encoded header reader
    fskframegenprops_s header_props;
    packetizer header_packetizer;        // header packetizer
    fskmod header_mod;
    liquid_float_complex * header_samples;  // modulated header symbol [header_props.samples_per_symbol]
    unsigned int header_len;  // length of header in samples

    // payload
    unsigned int payload_msg_len;  // unencoded payload length
    unsigned int payload_enc_len;  // encoded payload length
    unsigned char * payload_enc;  // encoded payload
    struct bitreader_s * payload_reader;  // encoded payload reader
    fskframegenprops_s payload_props;
    packetizer payload_packetizer;       // payload packetizer
    fskmod payload_mod;
    liquid_float_complex * payload_samples;  // modulated payload symbol [payload_k]
    unsigned int payload_k;  // samples/symbol of payload
    unsigned int payload_m;  // bits/symbol of payload
    unsigned int payload_len;  // length of payload in samples

    // framing state
    enum state state;
    int frame_assembled;         // frame assembled flag
    int frame_complete;          // frame completed flag
    unsigned int tail_len;       // number of tail samples to write
    unsigned int sample_counter; // per-state sample progress tracker
};

fskframegen fskframegen_create(fskframegenprops_s * _props,
                               float _bandwidth)
{
    fskframegen q = (fskframegen)calloc(1, sizeof(struct fskframegen_s));
    fskframegen_reset(q);

    q->bandwidth = _bandwidth;

    q->ramp_len = 12;
    q->tail_len = 12;

    q->preamble_ms = msequence_create(6, 0x6d, 1);
    q->preamble_mod = fskmod_create(1, FSKFRAME_PRE_K, q->bandwidth);
    q->preamble_samples = (liquid_float_complex *)malloc(FSKFRAME_PRE_K * sizeof(liquid_float_complex));
    q->preamble_len = 63 * FSKFRAME_PRE_K;

    q->header_reader = bitreader_create();
    q->header_mod = NULL;
    q->header_dec = NULL;
    q->header_enc = NULL;
    q->header_samples = NULL;
    q->header_packetizer = NULL;

    fskframegen_set_header_props(q, NULL);
    fskframegen_set_header_len(q, FSKFRAME_H_USER_DEFAULT);

    q->payload_reader = bitreader_create();
    q->payload_enc = NULL;
    q->payload_packetizer = NULL;
    q->payload_mod = NULL;
    q->payload_samples = NULL;

    fskframegen_setprops(q, _props);

    return q;
}

void fskframegen_destroy(fskframegen _q)
{
    if (!_q) {
        return;
    }

    msequence_destroy(_q->preamble_ms);
    fskmod_destroy(_q->preamble_mod);
    free(_q->preamble_samples);

    free(_q->header_dec);
    free(_q->header_enc);
    bitreader_destroy(_q->header_reader);
    packetizer_destroy(_q->header_packetizer);
    fskmod_destroy(_q->header_mod);
    free(_q->header_samples);

    free(_q->payload_enc);
    bitreader_destroy(_q->payload_reader);
    packetizer_destroy(_q->payload_packetizer);
    fskmod_destroy(_q->payload_mod);
    free(_q->payload_samples);

    free(_q);
}

void fskframegen_reset(fskframegen _q)
{
    _q->sample_counter = 0;
    _q->frame_assembled = 0;
    _q->frame_complete = 0;
    _q->state = STATE_PREAMBLE;
}

int fskframegen_is_assembled(fskframegen _q)
{
    return _q->frame_assembled;
}

void fskframegen_getprops(fskframegen _q, fskframegenprops_s * _props)
{
    memmove(_props, &_q->payload_props, sizeof(fskframegenprops_s));
}

int fskframegen_setprops(fskframegen _q, fskframegenprops_s * _props)
{
    if (_q->frame_assembled) {
        fprintf(
            stderr,
            "warning: fskframegen_setprops(), frame is already assembled; must reset() first\n");
        return -1;
    }

    if (_props == NULL) {
        fskframegen_setprops(_q, &fskframegenprops_default);
        return 0;
    }

    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: fskframegen_setprops(), invalid/unsupported CRC scheme\n");
        exit(1);
    }

    if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: fskframegen_setprops(), invalid/unsupported FEC scheme\n");
        exit(1);
    }

    if (_props->bits_per_symbol < 1 || _props->bits_per_symbol > 8) {
        fprintf(stderr, "error: fskframegen_setprops(), invalid/unsupported bits per symbol\n");
        exit(1);
    }

    if (_props->samples_per_symbol < (1 << _props->bits_per_symbol) || _props->samples_per_symbol > 2048) {
        fprintf(stderr, "error: fskframegen_setprops(), invalid/unsupported samples per symbol\n");
        exit(1);
    }

    // copy properties to internal structure
    memmove(&_q->payload_props, _props, sizeof(fskframegenprops_s));

    // reconfigure payload buffers (reallocate as necessary)
    fskframegen_reconfigure(_q);

    return 0;
}

void fskframegen_set_header_len(fskframegen _q, unsigned int _len)
{
    if (_q->frame_assembled) {
        fprintf(stderr,
                "warning: fskframegen_set_header_len(), frame is already assembled; must reset() "
                "first\n");
        return;
    }

    _q->header_user_len = _len;
    unsigned int dec_len  = FSKFRAME_H_DEC + _q->header_user_len;
    _q->header_dec = (unsigned char *)realloc(_q->header_dec, dec_len * sizeof(unsigned char));

    fskframegen_reconfigure_header(_q);
}

int fskframegen_set_header_props(fskframegen _q, fskframegenprops_s * _props)
{
    if (_q->frame_assembled) {
        fprintf(stderr,
                "warning: fskframegen_set_header_props(), frmae is already assembled; must "
                "reset() first\n");
        return -1;
    }

    if (_props == NULL) {
        _props = &fskframegenprops_header_default;
    }

    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: fskframegen_set_header_props(), invalid/unsupported CRC scheme\n");
        exit(1);
    }

    if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: fskframegen_set_header_props(), invalid/unsupported FEC scheme\n");
        exit(1);
    }

    if (_props->bits_per_symbol < 1 || _props->bits_per_symbol > 8) {
        fprintf(stderr, "error: fskframegen_set_header_props(), invalid/unsupported bits per symbol\n");
        exit(1);
    }

    if (_props->samples_per_symbol < (1 << _props->bits_per_symbol) || _props->samples_per_symbol > 2048) {
        fprintf(stderr, "error: fskframegen_set_header_props(), invalid/unsupported samples per symbol\n");
        exit(1);
    }

    memmove(&_q->header_props, _props, sizeof(fskframegenprops_s));

    fskframegen_reconfigure_header(_q);

    return 0;
}

unsigned int fskframegen_getframelen(fskframegen _q)
{
    if (!_q->frame_assembled) {
        fprintf(stderr, "warning: fskframegen_getframelen(), frame not assembled\n");
        return 0;
    }

    return _q->preamble_len + _q->header_len + _q->payload_len;
}

void fskframegen_reconfigure(fskframegen _q)
{
    fskmod_destroy(_q->payload_mod);
    _q->payload_mod = fskmod_create(_q->payload_props.bits_per_symbol,
                                    _q->payload_props.samples_per_symbol,
                                    _q->bandwidth);

    _q->payload_packetizer = packetizer_recreate(_q->payload_packetizer,
                                                 _q->payload_msg_len,
                                                 _q->payload_props.check,
                                                 _q->payload_props.fec0,
                                                 _q->payload_props.fec1);

    _q->payload_enc_len = packetizer_get_enc_msg_len(_q->payload_packetizer);
    _q->payload_enc = (unsigned char *)realloc(_q->payload_enc, _q->payload_enc_len * sizeof(unsigned char));
    bitreader_set_source(_q->payload_reader, _q->payload_enc, _q->payload_enc_len);

    _q->payload_samples = (liquid_float_complex *)realloc(_q->payload_samples,
                                                          _q->payload_props.samples_per_symbol * sizeof(liquid_float_complex));

    unsigned int num_payload_symbols = _q->payload_enc_len / _q->payload_props.bits_per_symbol;
    if (_q->payload_enc_len % _q->payload_props.bits_per_symbol) {
        num_payload_symbols++;
    }
    _q->payload_len = _q->payload_props.samples_per_symbol * num_payload_symbols;
}

void fskframegen_reconfigure_header(fskframegen _q)
{
    fskmod_destroy(_q->header_mod);
    _q->header_mod = fskmod_create(_q->header_props.bits_per_symbol,
                                   _q->header_props.samples_per_symbol,
                                   _q->bandwidth);

    unsigned dec_len = FSKFRAME_H_DEC + _q->header_user_len;
    _q->header_packetizer = packetizer_recreate(_q->header_packetizer,
                                                dec_len,
                                                _q->header_props.check,
                                                _q->header_props.fec0,
                                                _q->header_props.fec1);

    _q->header_enc_len = packetizer_get_enc_msg_len(_q->header_packetizer);
    _q->header_enc = (unsigned char *)realloc(_q->header_enc, _q->header_enc_len * sizeof(unsigned char));
    bitreader_set_source(_q->header_reader, _q->header_enc, _q->header_enc_len);

    _q->header_samples = (liquid_float_complex *)realloc(_q->header_samples,
                                                         _q->header_props.samples_per_symbol * sizeof(liquid_float_complex));
    unsigned int num_header_symbols = _q->header_enc_len / _q->header_props.bits_per_symbol;
    if (_q->header_enc_len % _q->header_props.bits_per_symbol) {
        num_header_symbols++;
    }
    _q->header_len = _q->header_props.samples_per_symbol * num_header_symbols;
}

void fskframegen_assemble(fskframegen           _q,
                          const unsigned char * _header,
                          const unsigned char * _payload,
                          unsigned int          _payload_len)
{
    fskframegen_reset(_q);

    _q->payload_msg_len = _payload_len;

    if (_header == NULL) {
        memset(_q->header_dec, 0x00, _q->header_user_len * sizeof(unsigned char));
    } else {
        memmove(_q->header_dec, _header, _q->header_user_len * sizeof(unsigned char));
    }

    unsigned int n = _q->header_user_len;

    _q->header_dec[n + 0] = FSKFRAME_PROTOCOL;
    _q->header_dec[n + 1] = (_q->payload_msg_len >> 8) & 0xff;
    _q->header_dec[n + 2] = (_q->payload_msg_len) & 0xff;
    _q->header_dec[n + 3] = (_q->payload_props.check & 0x07) << 5;
    _q->header_dec[n + 3] |= (_q->payload_props.fec0) & 0x1f;
    _q->header_dec[n + 4] = (_q->payload_props.bits_per_symbol & 0x07) << 5;
    _q->header_dec[n + 4] |= (_q->payload_props.fec1) & 0x1f;
    _q->header_dec[n + 5] = (_q->payload_props.samples_per_symbol) & 0xff;

    packetizer_encode(_q->header_packetizer, _q->header_dec, _q->header_enc);

    fskframegen_reconfigure(_q);

    packetizer_encode(_q->payload_packetizer, _payload, _q->payload_enc);

    _q->frame_assembled = 1;
    printf("frame assembled\n");
    printf("bandwidth %.2f\n", _q->bandwidth);
    printf("header bits/symbol %d\n", _q->header_props.bits_per_symbol);
    printf("header samples/symbol %d\n", _q->header_props.samples_per_symbol);
    printf("payload bits/symbol %d\n", _q->payload_props.bits_per_symbol);
    printf("payload samples/symbol %d\n", _q->payload_props.samples_per_symbol);
    printf("preamble length %d\n", _q->preamble_len);
    printf("header length %d\n", _q->header_len);
    printf("payload length %d\n", _q->payload_len);
}

int fskframegen_write_samples(fskframegen           _q,
                              liquid_float_complex * _buffer,
                              unsigned int           _buffer_len)
{
    unsigned int i;
    for (i = 0; i < _buffer_len; ) {
        unsigned int written;
        switch (_q->state) {
        case STATE_PREAMBLE:
            written = fskframegen_write_preamble(_q, _buffer, _buffer_len - i);
            break;
        case STATE_HEADER:
            written = fskframegen_write_header(_q, _buffer, _buffer_len - i);
            break;
        case STATE_PAYLOAD:
            written = fskframegen_write_payload(_q, _buffer, _buffer_len - i);
            break;
        default:
            fprintf(stderr,"error: fskframegen_write_samples(), unknown/unsupported internal state\n");
            exit(1);
        }
        i += written;
        _buffer += written;
    }

    return _q->frame_complete;
}

unsigned int fskframegen_write_preamble(fskframegen            _q,
                                        liquid_float_complex * _buffer,
                                        unsigned int           _buffer_len)
{
    unsigned int i;
    unsigned int num_samples = _q->preamble_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    for (i = 0; i < num_samples; i++) {
        if (_q->sample_counter % FSKFRAME_PRE_K == 0) {
            unsigned int symbol = msequence_advance(_q->preamble_ms);
            fskmod_modulate(_q->preamble_mod, symbol, _q->preamble_samples);
        }

        _buffer[i] = _q->preamble_samples[_q->sample_counter % FSKFRAME_PRE_K];

        if (_q->sample_counter < _q->ramp_len) {
            _buffer[i] *= hamming(_q->sample_counter, 2*_q->ramp_len);
        }

        _q->sample_counter++;
    }

    if (_q->sample_counter == _q->preamble_len) {
        msequence_reset(_q->preamble_ms);
        _q->sample_counter = 0;
        _q->state = STATE_HEADER;
    }

    return num_samples;
}

unsigned int fskframegen_write_header(fskframegen            _q,
                                      liquid_float_complex * _buffer,
                                      unsigned int           _buffer_len)
{
    unsigned int i;
    unsigned int num_samples = _q->header_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    for (i = 0; i < num_samples; i++) {
        if (_q->sample_counter % _q->header_props.samples_per_symbol == 0) {
            unsigned int symbol = bitreader_read(_q->header_reader, _q->header_props.bits_per_symbol);
            fskmod_modulate(_q->header_mod, symbol, _q->header_samples);
        }

        _buffer[i] = _q->header_samples[_q->sample_counter % _q->header_props.samples_per_symbol];

        _q->sample_counter++;
    }

    if (_q->sample_counter == _q->header_len) {
        _q->sample_counter = 0;
        _q->state = STATE_PAYLOAD;
    }

    return num_samples;
}

unsigned int fskframegen_write_payload(fskframegen            _q,
                                       liquid_float_complex * _buffer,
                                       unsigned int           _buffer_len)
{
    unsigned int i;
    unsigned int num_samples = _q->payload_len - _q->sample_counter;
    if (num_samples > _buffer_len) {
        num_samples = _buffer_len;
    }

    for (i = 0; i < num_samples; i++) {
        if (_q->sample_counter % _q->payload_props.samples_per_symbol == 0) {
            unsigned int symbol = bitreader_read(_q->payload_reader, _q->payload_props.bits_per_symbol);
            fskmod_modulate(_q->payload_mod, symbol, _q->payload_samples);
        }

        _buffer[i] = _q->payload_samples[_q->sample_counter % _q->payload_props.samples_per_symbol];

        if (_q->sample_counter >= _q->payload_len - _q->tail_len) {
            _buffer[i] *= hamming(_q->payload_len - _q->sample_counter, 2*_q->tail_len);
        }

        _q->sample_counter++;
    }

    if (_q->sample_counter == _q->payload_len) {
        _q->sample_counter = 0;
        _q->frame_complete = 1;
        _q->frame_assembled = 0;
    }

    return num_samples;
}
