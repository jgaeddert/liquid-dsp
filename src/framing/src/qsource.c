/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
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

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

// internal structure (single source)
struct QSOURCE(_s)
{
    // common parameters
    int             id;         // unique id
    unsigned int    M;          // number of channels in parent object's synthesis channelizer
    unsigned int    P;          // number of channels in this object's analysis channelizer
    unsigned int    m;          // channelizer filter semi-length
    unsigned int    index;      // base index
    resamp_crcf     resamp;     // arbitrary rate resampler
    nco_crcf        mixer;      // fine frequency adjustment
    float           gain;       // signal gain (user defined)
    float           gain_ch;    // channelizer gain
    unsigned int    buf_len;    // temporary buffer for resampler output
    float complex * buf;        // sample buffer (resamp output), [size: buf_len x 1]
    float complex * buf_time;   // channelizer input buffer, [size: P/2 x 1]
    float complex * buf_freq;   // channelizer input buffer, [size: P   x 1]
    firpfbch2_crcf  ch;         // analysis channelizer
    int             enabled;    // signal enabled?
    uint64_t        num_samples;// total number of samples generated

    enum {
        QSOURCE_UNKNOWN=0,
        QSOURCE_USER,
        QSOURCE_TONE,
        QSOURCE_NOISE,
        QSOURCE_MODEM,
    } type;

    union {
        struct { void * userdata; MSOURCE(_callback) callback; } user;
        struct { } tone;
        struct { } noise;
        struct { SYMSTREAM() symstream; } linmod;
    } source;
};

QSOURCE() QSOURCE(_create)(unsigned int _M,
                           unsigned int _m,
                           float        _fc,
                           float        _bw,
                           float        _gain)
{
    // validate input
    if (_M < 2 || (_M%2)) {
        fprintf(stderr,"error: %s:%u, qsource%s_create(), invalid channelizer size (%u); must be even and greater than 1\n",
                __FILE__,__LINE__,EXTENSION,_M);
        exit(1);
    } else if (_fc < -0.5f || _fc > 0.5f) {
        fprintf(stderr,"error: %s:%u, qsource%s_create(), invalid frequency offset (%f); must be in [-0.5,0.5]\n",
                __FILE__,__LINE__,EXTENSION,_fc);
        exit(1);
    }

    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    // initialize state
    q->id      = -1;
    q->type    = QSOURCE_UNKNOWN;
    q->gain    = powf(10.0f, _gain/20.0f);
    q->enabled = 1;
    q->num_samples = 0;

    // set channelizer values appropriately
    q->M = _M;
    q->P = 2*(unsigned int)ceilf( 0.5 * _bw * _M );
    q->P = max(2, q->P);
    // allow P to exceed M for cases where wider bandwidth is needed (e.g. modem)
    q->m = _m;

    // create resampler to correct for rate offset
    float rate = _bw == 0 ? 1.0f : _bw * (float)(q->M) / (float)(q->P);
    q->resamp = resamp_crcf_create(rate, 12, 0.45f, 60.0f, 64);

    // create mixer for frequency offset correction
    q->index = (unsigned int)roundf((_fc < 0.0f ? _fc + 1.0f : _fc) * q->M) % q->M;
    q->mixer = NCO(_create)(LIQUID_VCO);
    // TODO: set frequency correction
#if 0
    float fc_prime = roundf(_fc * (float)(q->M)) / (float)(q->M);
    NCO(_set_frequency)(q->mixer, 2*M_PI*(_fc-fc_prime));
#endif

    // create buffers
    q->buf_len  = 64;
    q->buf      = (float complex*) malloc(q->buf_len * sizeof(float complex));
    q->buf_time = (float complex*) malloc(q->P/2     * sizeof(float complex));
    q->buf_freq = (float complex*) malloc(q->P       * sizeof(float complex));

    // create channelizer
    q->ch = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER, q->P, q->m, 60.0f);

    // channelizer gain correction
    // TODO: adjust this appropriately
    q->gain_ch = sqrtf((float)(q->P)/(float)(q->M));

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

void QSOURCE(_destroy)(QSOURCE() _q)
{
    // free internal type-specific objects
    switch (_q->type) {
    case QSOURCE_UNKNOWN:   break;
    case QSOURCE_USER:      break;
    case QSOURCE_TONE:      break;
    case QSOURCE_NOISE:     break;
    case QSOURCE_MODEM:
        SYMSTREAM(_destroy)(_q->source.linmod.symstream);
        break;
    default:
        fprintf(stderr,"error: qsource%s_destroy(), internal logic error\n", EXTENSION);
        exit(1);
    }
    // free buffers
    free(_q->buf);
    free(_q->buf_time);
    free(_q->buf_freq);

    // destroy main internal objects
    firpfbch2_crcf_destroy(_q->ch);
    resamp_crcf_destroy   (_q->resamp);
    NCO(_destroy)         (_q->mixer);

    // free main object memory
    free(_q);
}

void QSOURCE(_init_user)(QSOURCE() _q,
                         void *    _userdata,
                         void *    _callback)
{
    _q->type = QSOURCE_USER;
    _q->source.user.userdata = _userdata;
    _q->source.user.callback = (MSOURCE(_callback))_callback;
}

void QSOURCE(_init_tone)(QSOURCE() _q)
{
    _q->type = QSOURCE_TONE;
}

void QSOURCE(_init_noise)(QSOURCE() _q)
{
    _q->type = QSOURCE_NOISE;
}

void QSOURCE(_init_modem)(QSOURCE()    _q,
                          int          _ms,
                          unsigned int _m,
                          float        _beta)
{
    _q->type = QSOURCE_MODEM;
    _q->source.linmod.symstream=SYMSTREAM(_create_linear)(LIQUID_FIRFILT_ARKAISER,2,_m,_beta,_ms);
    // TODO: adjust rate
}

void QSOURCE(_print)(QSOURCE() _q)
{
    // TODO: print generic parameters
    printf("  qsource%s[%3d] : ", EXTENSION, _q->id);
    // print type-specific parameters
    switch (_q->type) {
    case QSOURCE_USER:  printf("user\n");   break;
    case QSOURCE_TONE:  printf("tone\n");   break;
    case QSOURCE_NOISE: printf("noise\n");  break;
    case QSOURCE_MODEM: printf("modem\n");  break;
    default:
        fprintf(stderr,"error: qsource%s_print(), internal logic error\n", EXTENSION);
        exit(1);
    }
}

void QSOURCE(_reset)(QSOURCE() _q)
{
}

void QSOURCE(_set_id)(QSOURCE() _q,
                      int       _id)
{
    _q->id = _id;
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

// Get number of samples generated by the object so far
uint64_t QSOURCE(_get_num_samples)(QSOURCE() _q)
{
    return _q->num_samples;
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

void QSOURCE(_generate)(QSOURCE() _q,
                        TO *      _v)
{
    // generate type-specific sample
    TO sample;
    switch (_q->type) {
    case QSOURCE_USER:
        _q->source.user.callback(_q->source.user.userdata, &sample, 1);
        break;
    case QSOURCE_TONE:
        sample = 1.0f;
        break;
    case QSOURCE_NOISE:
        sample = (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
        break;
    case QSOURCE_MODEM:
        SYMSTREAM(_write_samples)(_q->source.linmod.symstream, &sample, 1);
        sample *= M_SQRT1_2; // compensate for 2 samples/symbol
        break;
    default:
        fprintf(stderr,"error: qsource%s_generate(), internal logic error\n", EXTENSION);
        exit(1);
    }
    
    if (!_q->enabled)
        sample = 0.0f;

    // TODO: push through resampler

    // mix sample up
    NCO(_mix_up)(_q->mixer, sample, _v);

    // step mixer
    NCO(_step)(_q->mixer);
}

void QSOURCE(_generate_into)(QSOURCE() _q,
                             TO *      _buf)
{
    // add into output buffer, applying appropriate scaling
    unsigned int i;
    unsigned int P2 = _q->P/2;

    // fill input buffer for channelizer
    for (i=0; i<P2; i++)
        QSOURCE(_generate)(_q, _q->buf_time+i);

    // run analysis channelizer
    firpfbch2_crcf_execute(_q->ch, _q->buf_time, _q->buf_freq);

    // aggregate gain
    float g = _q->gain * _q->gain_ch;

    // copy upper frequency band (base index = _q->index)
    unsigned int base_index = _q->index;
    for (i=0; i<P2; i++)
        _buf[ (base_index+i) % _q->M ] += _q->buf_freq[i] * g;

    // copy lower frequency band (base index = _q->index-P/2)
    base_index = _q->index;
    while (base_index <= P2)
        base_index += _q->M;
    base_index -= P2;
    for (i=0; i<P2; i++)
        _buf[ (base_index+i) % _q->M ] += _q->buf_freq[i+P2] * g;
    
    _q->num_samples += P2;
}

