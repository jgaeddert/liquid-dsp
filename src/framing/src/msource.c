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
// Generic source generator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// forward declaration of internal single source object and methods
typedef struct QSOURCE(_s) * QSOURCE();
QSOURCE() QSOURCE(_create_tone)();

QSOURCE() QSOURCE(_create_noise)(float _bandwidth);

QSOURCE() QSOURCE(_create_modem)(int          _ms,
                                 unsigned int _k,
                                 unsigned int _m,
                                 float        _beta);

void QSOURCE(_destroy)(QSOURCE() _q);

void QSOURCE(_reset)(QSOURCE() _q);

void QSOURCE(_set_gain)(QSOURCE() _q,
                        float     _gain_dB);

void QSOURCE(_set_frequency)(QSOURCE() _q,
                             float     _dphi);

void QSOURCE(_gen_sample)(QSOURCE() _q,
                          TO *      _v);

// internal structure
struct MSOURCE(_s)
{
    QSOURCE() *  sources;
    unsigned int num_sources;
};

//
// internal methods
//

// add source to list
int MSOURCE(_add_source)(MSOURCE() _q,
                         QSOURCE() _s);

// create msource object with linear modulation
MSOURCE() MSOURCE(_create)(void)
{
    // allocate memory for main object
    MSOURCE() q = (MSOURCE()) malloc( sizeof(struct MSOURCE(_s)) );

    //
    q->sources = NULL;
    q->num_sources = 0;

    // reset and return main object
    MSOURCE(_reset)(q);
    return q;
}

// destroy msource object, freeing all internal memory
void MSOURCE(_destroy)(MSOURCE() _q)
{
    // destroy internal objects
    unsigned int i;
    for (i=0; i<_q->num_sources; i++)
        QSOURCE(_destroy)(_q->sources[i]);

    // free list of sources
    free(_q->sources);

    // free main object
    free(_q);
}

// add tone source
int MSOURCE(_add_tone)(MSOURCE() _q)
{
    QSOURCE() s = QSOURCE(_create_tone)();
    return MSOURCE(_add_source)(_q, s);
}

// add noise source
int MSOURCE(_add_noise)(MSOURCE() _q,
                        float     _bandwidth)
{
    // validate input
    if (_bandwidth <= 0.0f || _bandwidth > 1.0f) {
        fprintf(stderr,"error: msource%s_add_noise(), noise bandwidth must be in (0,1.0]\n", EXTENSION);
        exit(1);
    } else if (_bandwidth >= 0.9995f) {
        _bandwidth = 0.9995f;
    }

    QSOURCE() s = QSOURCE(_create_noise)(_bandwidth);
    return MSOURCE(_add_source)(_q, s);
}

// add modem source
int MSOURCE(_add_modem)(MSOURCE()    _q,
                        int          _ms,
                        unsigned int _k,
                        unsigned int _m,
                        float        _beta)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: msource%s_create(), samples/symbol must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: msource%s_create(), filter delay must be greater than zero\n", EXTENSION);
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: msource%s_create(), filter excess bandwidth must be in (0,1]\n", EXTENSION);
        exit(1);
    } else if (_ms == LIQUID_MODEM_UNKNOWN || _ms >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: msource%s_create(), invalid modulation scheme\n", EXTENSION);
        exit(1);
    }

    QSOURCE() s = QSOURCE(_create_modem)(_ms, _k, _m, _beta);
    return MSOURCE(_add_source)(_q, s);
}
// print msource object's parameters
void MSOURCE(_print)(MSOURCE() _q)
{
    printf("msource_%s:\n", EXTENSION);
}

// reset msource internal state
void MSOURCE(_reset)(MSOURCE() _q)
{
}

// remove signal
void MSOURCE(_remove)(MSOURCE() _q,
                      int       _id)
{
}

// enable/disable signal
void MSOURCE(_enable)(MSOURCE() _q,
                      int       _id)
{
}

void MSOURCE(_disable)(MSOURCE() _q,
                       int       _id)
{
}

// set signal gain
//  _q      :   msource object
//  _id     :   source id
//  _gain_dB:   signal gain in dB
void MSOURCE(_set_gain)(MSOURCE() _q,
                        int       _id,
                        float     _gain_dB)
{
    // validate input
    if (_id > _q->num_sources) {
        fprintf(stderr,"error: qsource%s_set_gain(), signal id (%d) out of range (%u)\n",
                EXTENSION, _id, _q->num_sources);
        exit(1);
    }

    // set source gain
    QSOURCE(_set_gain)(_q->sources[_id], _gain_dB);
}

// set carrier offset to signal
//  _q      :   msource object
//  _id     :   source id
//  _fc     :   carrier offset, fc in [-0.5,0.5]
void MSOURCE(_set_frequency)(MSOURCE() _q,
                             int       _id,
                             float     _dphi)
{
    // validate input
    if (_id > _q->num_sources) {
        fprintf(stderr,"error: qsource%s_set_frequency(), signal id (%d) out of range (%u)\n",
                EXTENSION, _id, _q->num_sources);
        exit(1);
    
    }
    // set source frequency
    QSOURCE(_set_frequency)(_q->sources[_id], _dphi);
}

// write block of samples to output buffer
//  _q      : synchronizer object
//  _buf    : output buffer [size: _buf_len x 1]
//  _buf_len: output buffer size
void MSOURCE(_write_samples)(MSOURCE()    _q,
                             TO *         _buf,
                             unsigned int _buf_len)
{
    TO sample;
    TO accumulation;
    unsigned int i;
    unsigned int j;
    for (i=0; i<_buf_len; i++) {
        accumulation = 0;

        for (j=0; j<_q->num_sources; j++) {
            QSOURCE(_gen_sample)(_q->sources[j], &sample);
            accumulation += sample;
        }

        _buf[i] = accumulation;
    }
}

//
// internal msource methods
//

// add source to list
int MSOURCE(_add_source)(MSOURCE() _q,
                         QSOURCE() _s)
{
    if (_s == NULL)
        return -1;

    // reallocate
    if (_q->num_sources == 0) {
        _q->sources = (QSOURCE()*) malloc(sizeof(QSOURCE()));
    } else {
        _q->sources = (QSOURCE()*) realloc(_q->sources,
                                           (_q->num_sources+1)*sizeof(QSOURCE()));
    }

    // append new object to end of list
    _q->sources[_q->num_sources] = _s;

    //
    _q->num_sources++;

    //
    return _q->num_sources-1;
}

//
// internal structure (single source)
//
struct QSOURCE(_s) {
    union {
        // tone
        struct {
            int x;
        } tone;

        // wide-band noise
        struct {
            IIRFILT() filter;
        } noise;

        // linear modulation
        struct {
            MODEM()      mod;
            FIRINTERP()  interp;
            unsigned int k;
        } linmod;
    } source;
    
    enum {
        QSOURCE_TONE,
        QSOURCE_NOISE,
        QSOURCE_MODEM,
    } type;

    unsigned int    buf_len;
    float complex * buf;
    unsigned int    buf_index;

    nco_crcf mixer;
    float    gain;
};

QSOURCE() QSOURCE(_create_tone)()
{
    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->type = QSOURCE_TONE;

    // sample buffer
    q->buf_len = 0;
    q->buf = NULL;
    q->buf_index = 0;

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

QSOURCE() QSOURCE(_create_noise)(float _bandwidth)
{
    // TODO: validate input

    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->type = QSOURCE_NOISE;

    unsigned int order = 7;
    q->source.noise.filter = IIRFILT(_create_prototype)(LIQUID_IIRDES_ELLIP,
                                                        LIQUID_IIRDES_LOWPASS,
                                                        LIQUID_IIRDES_SOS,
                                                        order,
                                                        0.5*_bandwidth, 0.0f,
                                                        0.1f, 80.0f);

    // sample buffer
    q->buf_len = 0;
    q->buf = NULL;
    q->buf_index = 0;

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

QSOURCE() QSOURCE(_create_modem)(int          _ms,
                                 unsigned int _k,
                                 unsigned int _m,
                                 float        _beta)
{
    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->type = QSOURCE_MODEM;

    q->source.linmod.mod    = MODEM(_create)(_ms);
    q->source.linmod.interp = FIRINTERP(_create_rnyquist)(LIQUID_FIRFILT_ARKAISER, _k, _m, _beta, 0.0f);
    q->source.linmod.k      = _k;

    // sample buffer
    q->buf_len = _k;
    q->buf = (TO*) malloc(_k*sizeof(TO));
    q->buf_index = 0;

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

void QSOURCE(_destroy)(QSOURCE() _q)
{
    // free internal type-specific objects
    switch (_q->type) {
    case QSOURCE_TONE: break;
    case QSOURCE_NOISE:
        IIRFILT(_destroy)(_q->source.noise.filter);
        break;
    case QSOURCE_MODEM:
        MODEM    (_destroy)(_q->source.linmod.mod);
        FIRINTERP(_destroy)(_q->source.linmod.interp);
        // free buffer
        free(_q->buf);
        break;
    default:
        fprintf(stderr,"error: qsource%s_destroy(), internal logic error\n", EXTENSION);
        exit(1);
    }

    // destroy mixer object
    NCO(_destroy)(_q->mixer);

    // free main object memory
    free(_q);
}

void QSOURCE(_reset)(QSOURCE() _q)
{
    _q->buf_index = 0;
}

void QSOURCE(_set_gain)(QSOURCE() _q,
                        float     _gain_dB)
{
    // convert from dB
    _q->gain = powf(10.0f, _gain_dB/20.0f);
}

void QSOURCE(_set_frequency)(QSOURCE() _q,
                             float     _dphi)
{
    NCO(_set_frequency)(_q->mixer, _dphi);
}

void QSOURCE(_gen_sample)(QSOURCE() _q,
                          TO *      _v)
{
    TO sample;

    // free internal type-specific objects
    switch (_q->type) {
    case QSOURCE_TONE:
        sample = 1.0f;
        break;
    case QSOURCE_NOISE:
        IIRFILT(_execute)(_q->source.noise.filter, (randnf() + _Complex_I*randnf())*M_SQRT1_2, &sample);
        break;
    case QSOURCE_MODEM:
        if (_q->buf_index == 0) {
            // generate more samples if necessary
            //MODEM    (_destroy)(_q->source.linmod.mod);
            //FIRINTERP(_destroy)(_q->source.linmod.interp);
            FIRINTERP(_execute)(_q->source.linmod.interp,
                                rand() % 2 ? 1.0f : -1.0f, 
                                _q->buf);
        }
        sample = _q->buf[_q->buf_index];
        _q->buf_index++;
        _q->buf_index %= _q->buf_len;
        break;
    default:
        fprintf(stderr,"error: qsource%s_gen_sample(), internal logic error\n", EXTENSION);
        exit(1);
    }

    // apply gain
    sample *= _q->gain;

    // mix sample up
    NCO(_mix_up)(_q->mixer, sample, _v);

    // step mixer
    NCO(_step)(_q->mixer);
}

