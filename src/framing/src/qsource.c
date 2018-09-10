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

//
// Generic single signal source generator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    int      enabled;
};

QSOURCE() QSOURCE(_create_tone)(int _id)
{
    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    q->id   = _id;
    q->type = QSOURCE_TONE;

    q->mixer   = NCO(_create)(LIQUID_VCO);
    q->gain    = 1;
    q->enabled = 1;

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

    q->mixer   = NCO(_create)(LIQUID_VCO);
    q->gain    = 1;
    q->enabled = 1;

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

    q->mixer   = NCO(_create)(LIQUID_VCO);
    q->gain    = 1;
    q->enabled = 1;

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


int QSOURCE(_get_id)(QSOURCE() _q)
{
    return _q->id;
}

void QSOURCE(_enable)(QSOURCE() _q)
{
    _q->enabled = 1;
}

void QSOURCE(_disable)(QSOURCE() _q)
{
    _q->enabled = 0;
}

void QSOURCE(_set_gain)(QSOURCE() _q,
                        float     _gain_dB)
{
    // convert from dB
    _q->gain = powf(10.0f, _gain_dB/20.0f);
}

float QSOURCE(_get_gain)(QSOURCE() _q)
{
    return 20*log10f(_q->gain);
}

void QSOURCE(_set_frequency)(QSOURCE() _q,
                             float     _dphi)
{
    NCO(_set_frequency)(_q->mixer, _dphi);
}

float QSOURCE(_get_frequency)(QSOURCE() _q)
{
    return NCO(_get_frequency)(_q->mixer);
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
    
    if (!_q->enabled)
        sample = 0.0f;

    // apply gain
    sample *= _q->gain;

    // mix sample up
    NCO(_mix_up)(_q->mixer, sample, _v);

    // step mixer
    NCO(_step)(_q->mixer);
}

