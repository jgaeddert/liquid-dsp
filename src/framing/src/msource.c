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

// internal structure (single source)
struct QSOURCE(_s) {
    int id; // unique id

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
            SYMSTREAM() symstream;
        } linmod;
    } source;
    
    enum {
        QSOURCE_TONE,
        QSOURCE_NOISE,
        QSOURCE_MODEM,
    } type;

    nco_crcf mixer;
    float    gain;
    //int      enabled;
};

QSOURCE() QSOURCE(_create_tone)(int _id);

QSOURCE() QSOURCE(_create_noise)(int _id, float _bandwidth);

QSOURCE() QSOURCE(_create_modem)(int          _id,
                                 int          _ms,
                                 unsigned int _k,
                                 unsigned int _m,
                                 float        _beta);

void QSOURCE(_destroy)(QSOURCE() _q);

void QSOURCE(_print)(QSOURCE() _q);

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

    int id_counter;
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
    q->id_counter  = 0;

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

// reset msource internal state
void MSOURCE(_reset)(MSOURCE() _q)
{
}

// print
void MSOURCE(_print)(MSOURCE() _q)
{
    printf("msource%s:\n", EXTENSION);
    unsigned int i;
    for (i=0; i<_q->num_sources; i++)
        QSOURCE(_print)(_q->sources[i]);
}

// add tone source
int MSOURCE(_add_tone)(MSOURCE() _q)
{
    int id = _q->id_counter;
    _q->id_counter++;
    QSOURCE() s = QSOURCE(_create_tone)(id);
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

    int id = _q->id_counter;
    _q->id_counter++;
    QSOURCE() s = QSOURCE(_create_noise)(id, _bandwidth);
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

    int id = _q->id_counter;
    _q->id_counter++;
    QSOURCE() s = QSOURCE(_create_modem)(id, _ms, _k, _m, _beta);
    return MSOURCE(_add_source)(_q, s);
}

// remove signal
void MSOURCE(_remove)(MSOURCE() _q,
                      int       _id)
{
    // find source object matching id
    unsigned int i;
    int id_found = 0;
    for (i=0; i<_q->num_sources; i++) {
        if (_q->sources[i]->id == _id) {
            id_found = 1;
            break;
        }
    }

    // check to see if id was found
    if (!id_found) {
        fprintf(stderr,"error: qsource%s_remove(), signal id (%d) not found\n",
                EXTENSION, _id);
        exit(1);
    }

    // delete source
    //printf("deleting source with id %d (requested %d)\n", _q->sources[i]->id, _id);
    QSOURCE(_destroy)(_q->sources[i]);

    //
    _q->num_sources--;

    // shift sources down
    for (; i<_q->num_sources; i++)
        _q->sources[i] = _q->sources[i+1];
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

// get source by id
QSOURCE() MSOURCE(_get_source)(MSOURCE() _q,
                               int       _id)
{
    unsigned int i;
    for (i=0; i<_q->num_sources; i++) {
        if (_q->sources[i]->id == _id)
            return _q->sources[i];
    }

    return NULL;
}


//
// internal qsource
//
QSOURCE() QSOURCE(_create_tone)(int _id)
{
    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->id   = _id;
    q->type = QSOURCE_TONE;

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

QSOURCE() QSOURCE(_create_noise)(int   _id,
                                 float _bandwidth)
{
    // TODO: validate input

    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->id   = _id;
    q->type = QSOURCE_NOISE;

    unsigned int order = 7;
    q->source.noise.filter = IIRFILT(_create_prototype)(LIQUID_IIRDES_ELLIP,
                                                        LIQUID_IIRDES_LOWPASS,
                                                        LIQUID_IIRDES_SOS,
                                                        order,
                                                        0.5*_bandwidth, 0.0f,
                                                        0.1f, 80.0f);

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

QSOURCE() QSOURCE(_create_modem)(int          _id,
                                 int          _ms,
                                 unsigned int _k,
                                 unsigned int _m,
                                 float        _beta)
{
    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->id   = _id;
    q->type = QSOURCE_MODEM;

    q->source.linmod.symstream=SYMSTREAM(_create_linear)(LIQUID_FIRFILT_ARKAISER,_k,_m,_beta,_ms);

    q->mixer = NCO(_create)(LIQUID_VCO);
    q->gain  = 1;

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

void QSOURCE(_print)(QSOURCE() _q)
{
    printf("  qsource%s[%3d] : ", EXTENSION, _q->id);
    // print type-specific parameters
    switch (_q->type) {
    case QSOURCE_TONE:  printf("tone\n");   break;
    case QSOURCE_NOISE: printf("noise\n");  break;
    case QSOURCE_MODEM: printf("modem\n");  break;
    default:
        fprintf(stderr,"error: qsource%s_print(), internal logic error\n", EXTENSION);
        exit(1);
    }
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
        SYMSTREAM(_destroy)(_q->source.linmod.symstream);
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
        SYMSTREAM(_write_samples)(_q->source.linmod.symstream, &sample, 1);
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

