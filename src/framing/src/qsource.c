/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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
    float           As;         // channelizer filter stop-band suppression (dB)
    float           fc;         // signal normalized center frequency
    float           bw;         // signal normalized bandwidth
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
    uint64_t        num_samples;// total number of output samples generated

    // signal type
    enum {
        QSOURCE_UNKNOWN=0,
        QSOURCE_USER,
        QSOURCE_TONE,
        QSOURCE_CHIRP,
        QSOURCE_NOISE,
        QSOURCE_MODEM,
        QSOURCE_FSK,
        QSOURCE_GMSK,
    } type;

    // memory object for specific signal sources
    union {
        struct { void * userdata; MSOURCE(_callback) callback; } user;
        struct { } tone;
        struct { NCO() nco; float df; int negate, single; uint64_t num, timer; } chirp;
        struct { } noise;
        struct { SYMSTREAM() symstream; } linmod;
        struct { fskmod mod; float complex * buf; unsigned int len, index, mask; } fsk;
        struct { gmskmod mod; float complex buf[2]; int index; } gmsk;
    } source;
};

QSOURCE() QSOURCE(_create)(unsigned int _M,
                           unsigned int _m,
                           float        _As,
                           float        _fc,
                           float        _bw,
                           float        _gain)
{
    // validate input
    if (_M < 2 || (_M % 2))
        return liquid_error_config("qsource%s_create(), invalid channelizer size (%u); must be even and greater than 1",EXTENSION,_M);
    if (_fc < -0.5f || _fc > 0.5f)
        return liquid_error_config("qsource%s_create(), invalid frequency offset (%f); must be in [-0.5,0.5]",EXTENSION,_fc);

    // allocate memory for main object
    QSOURCE() q = (QSOURCE()) malloc( sizeof(struct QSOURCE(_s)) );

    // initialize state
    q->id          = -1;                        // default identifier
    q->type        = QSOURCE_UNKNOWN;           // default type
    q->gain        = powf(10.0f, _gain/20.0f);  // user-defined gain
    q->enabled     = 1;                         // enabled by default
    q->num_samples = 0;                         // no output samples generated yet
    q->fc          = _fc;                       // center frequency (relative to sample rate)
    q->bw          = _bw;                       // bandwidth (relative to sample rate)

    // set channelizer values appropriately
    q->M = _M;
    q->P = 2*(unsigned int)ceilf( 0.5 * _bw * _M );
    q->P = max(2, q->P);
    // allow P to exceed M for cases where wider bandwidth is needed (e.g. modem)
    q->m = _m;
    q->As= _As;

    // create resampler to correct for rate offset
    float rate = _bw == 0 ? 1.0f : _bw * (float)(q->M) / (float)(q->P);
    q->resamp = resamp_crcf_create(rate, 12, 0.45f, q->As, 64);

    // create mixer for frequency offset correction
    q->index = (unsigned int)roundf((_fc < 0.0f ? _fc + 1.0f : _fc) * q->M) % q->M;
    q->mixer = NCO(_create)(LIQUID_VCO);
    // compute frequency applied by channelizer alignment
    float fc_index = (float)(q->index) / (float)(q->M) + (q->index < q->M/2 ? 0 : -1);
    // compute residual frequency needed by mixer
    float fc_mixer = _fc - fc_index;
    // apply mixer frequency (in radians), scaled by resampling ratio
    NCO(_set_frequency)(q->mixer, 2*M_PI*fc_mixer * (float)(q->M) / (float)(q->P));
#if 0
    printf("fc : %12.8f (index: %4u, P:%4u, M:%4u), actual : %12.8f = %12.8f + %12.8f, e : %12.8f\n",
            _fc, q->index, q->P, q->M, fc_index+fc_mixer, fc_index, fc_mixer, _fc-(fc_index+fc_mixer));
#endif

    // create buffers
    q->buf_len  = 64;
    q->buf      = (float complex*) malloc(q->buf_len * sizeof(float complex));
    q->buf_time = (float complex*) malloc(q->P/2     * sizeof(float complex));
    q->buf_freq = (float complex*) malloc(q->P       * sizeof(float complex));

    // create channelizer
    q->ch = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER, q->P, q->m, q->As);

    // channelizer gain correction
    q->gain_ch = sqrtf((float)(q->P)/(float)(q->M));

    // reset and return main object
    QSOURCE(_reset)(q);
    return q;
}

int QSOURCE(_destroy)(QSOURCE() _q)
{
    // free internal type-specific objects
    switch (_q->type) {
    case QSOURCE_UNKNOWN:   break;
    case QSOURCE_USER:      break;
    case QSOURCE_TONE:      break;
    case QSOURCE_CHIRP:
        NCO(_destroy)(_q->source.chirp.nco);
        break;
    case QSOURCE_NOISE:     break;
    case QSOURCE_MODEM:
        SYMSTREAM(_destroy)(_q->source.linmod.symstream);
        break;
    case QSOURCE_FSK:
        fskmod_destroy(_q->source.fsk.mod);
        free(_q->source.fsk.buf);
        break;
    case QSOURCE_GMSK:
        gmskmod_destroy(_q->source.gmsk.mod);
        break;
    default:
        return liquid_error(LIQUID_EINT,"qsource%s_destroy(), invalid internal state",EXTENSION);
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
    return LIQUID_OK;
}

// initialize source with user-defined callback for generating samples
int QSOURCE(_init_user)(QSOURCE() _q,
                        void *    _userdata,
                        void *    _callback)
{
    _q->type = QSOURCE_USER;
    _q->source.user.userdata = _userdata;
    _q->source.user.callback = (MSOURCE(_callback))_callback;
    return LIQUID_OK;
}

// initialize source with tone source
int QSOURCE(_init_tone)(QSOURCE() _q)
{
    _q->type = QSOURCE_TONE;
    return LIQUID_OK;
}

// Add chirp to signal generator, returning id of signal
//  _q          : signal source object
//  _duration   : duration of chirp [samples]
//  _negate     : negate frequency direction
//  _single     : run single chirp? or repeatedly
int QSOURCE(_init_chirp)(QSOURCE() _q,
                         float     _duration,
                         int       _negate,
                         int       _single)
{
    _q->type                = QSOURCE_CHIRP;
    _q->source.chirp.nco    = NCO(_create)(LIQUID_VCO);
    _q->source.chirp.negate = _negate ? 1 : 0;
    _q->source.chirp.single = _single ? 1 : 0;
    _q->source.chirp.num    = (uint64_t) roundf(_duration * _q->bw);
    _q->source.chirp.df     = 2*M_PI / (float)(_q->source.chirp.num) * (_q->source.chirp.negate ? -1 : 1);

    // initialize properties
    NCO(_set_frequency)(_q->source.chirp.nco, _q->source.chirp.negate ? M_PI : -M_PI);
    _q->source.chirp.timer = _q->source.chirp.num;
    return LIQUID_OK;
}

// initialize source with noise source
int QSOURCE(_init_noise)(QSOURCE() _q)
{
    _q->type = QSOURCE_NOISE;
    return LIQUID_OK;
}

// initialize source with linear modulation generator
int QSOURCE(_init_modem)(QSOURCE()    _q,
                         int          _ms,
                         unsigned int _m,
                         float        _beta)
{
    _q->type = QSOURCE_MODEM;
    _q->source.linmod.symstream=SYMSTREAM(_create_linear)(LIQUID_FIRFILT_ARKAISER,2,_m,_beta,_ms);
    return LIQUID_OK;
}

// initialize source with frequency-shift keying modem
int QSOURCE(_init_fsk)(QSOURCE()    _q,
                       unsigned int _m,
                       unsigned int _k)
{
    _q->type            = QSOURCE_FSK;
    _q->source.fsk.mod  = fskmod_create(_m, _k, 0.25f);
    _q->source.fsk.len  = _k;   // buffer length
    _q->source.fsk.buf  = (float complex*)malloc(_k*sizeof(float complex));
    _q->source.fsk.mask = (1 << _m) - 1;
    _q->source.fsk.index= 0;
    return LIQUID_OK;
}

// initialize source with Gauss minimum-shift keying modem
int QSOURCE(_init_gmsk)(QSOURCE()    _q,
                        unsigned int _m,
                        float        _bt)
{
    _q->type = QSOURCE_GMSK;
    _q->source.gmsk.mod = gmskmod_create(2,_m,_bt);
    _q->source.gmsk.index = 0;
    return LIQUID_OK;
}

// print basic info to stdout
int QSOURCE(_print)(QSOURCE() _q)
{
    // TODO: print generic parameters
    printf("  qsource%s[%3d] : ", EXTENSION, _q->id);
    // print type-specific parameters
    float bw = _q->bw;
    switch (_q->type) {
    case QSOURCE_USER:  printf("user ");             break;
    case QSOURCE_TONE:  printf("tone ");             break;
    case QSOURCE_CHIRP: printf("chirp");             break;
    case QSOURCE_NOISE: printf("noise");             break;
    case QSOURCE_MODEM: printf("modem"); bw *= 0.5f; break;
    case QSOURCE_FSK:   printf("fsk  "); bw *= 0.5f; break;
    case QSOURCE_GMSK:  printf("gmsk "); bw *= 0.5f; break;
    default:
        return liquid_error(LIQUID_EINT,"qsource%s_print(), invalid internal state",EXTENSION);
    }
    printf(" : fc=%6.3f, bw=%5.3f, P=%4u, m=%2u, As=%5.1f dB, gain=%5.1f dB %c\n",
            _q->fc, bw, _q->P, _q->m, _q->As, QSOURCE(_get_gain)(_q), _q->enabled ? '*' : ' ');
    return LIQUID_OK;
}

// reset object internals
// NOTE: placeholder for future funcitonality
int QSOURCE(_reset)(QSOURCE() _q)
{
    return LIQUID_OK;
}

// set internal object identifier
int QSOURCE(_set_id)(QSOURCE() _q,
                     int       _id)
{
    _q->id = _id;
    return LIQUID_OK;
}

// get internal object identifier
int QSOURCE(_get_id)(QSOURCE() _q)
{
    return _q->id;
}

// enable source generation
int QSOURCE(_enable)(QSOURCE() _q)
{
    _q->enabled = 1;
    return LIQUID_OK;
}

// disable source generation
int QSOURCE(_disable)(QSOURCE() _q)
{
    _q->enabled = 0;
    return LIQUID_OK;
}

// set signal gain in dB
int QSOURCE(_set_gain)(QSOURCE() _q,
                       float     _gain_dB)
{
    // convert from dB
    _q->gain = powf(10.0f, _gain_dB/20.0f);
    return LIQUID_OK;
}

// get signal gain in dB
float QSOURCE(_get_gain)(QSOURCE() _q)
{
    return 20*log10f(_q->gain);
}

// get number of samples generated by the object so far
uint64_t QSOURCE(_get_num_samples)(QSOURCE() _q)
{
    return _q->num_samples;
}

// set mixer frequency
int QSOURCE(_set_frequency)(QSOURCE() _q,
                            float     _dphi)
{
    NCO(_set_frequency)(_q->mixer, _dphi);
    return LIQUID_OK;
}

// get mixer frequency
float QSOURCE(_get_frequency)(QSOURCE() _q)
{
    return NCO(_get_frequency)(_q->mixer);
}

// generate a single sample
int QSOURCE(_generate)(QSOURCE() _q,
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
    case QSOURCE_CHIRP:
        NCO(_cexpf)           (_q->source.chirp.nco, &sample);
        NCO(_adjust_frequency)(_q->source.chirp.nco, _q->source.chirp.df);
        NCO(_step)            (_q->source.chirp.nco);
        _q->source.chirp.timer--;
        if (_q->source.chirp.timer==0) {
            _q->source.chirp.timer = _q->source.chirp.num;  // reset timer
            // disable for just one instance
            if (_q->source.chirp.single)
                QSOURCE(_disable)(_q);
            // reset NCO frequency
            NCO(_set_frequency)(_q->source.chirp.nco, _q->source.chirp.negate ? M_PI : -M_PI);
        }
        break;
    case QSOURCE_NOISE:
        sample = (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
        break;
    case QSOURCE_MODEM:
        SYMSTREAM(_write_samples)(_q->source.linmod.symstream, &sample, 1);
        sample *= M_SQRT1_2; // compensate for 2 samples/symbol
        break;
    case QSOURCE_FSK:
        // fill buffer when necessary
        if (_q->source.fsk.index==0)
            fskmod_modulate(_q->source.fsk.mod, rand() & _q->source.fsk.mask, _q->source.fsk.buf);

        // compensate for k samples/symbol
        sample = _q->source.fsk.buf[ _q->source.fsk.index++ ]; // *  M_SQRT1_2;
        _q->source.fsk.index %= _q->source.fsk.len; // reset index every k samples
        break;
    case QSOURCE_GMSK:
        // fill buffer when necessary
        if (_q->source.gmsk.index==0)
            gmskmod_modulate(_q->source.gmsk.mod, rand() & 1, _q->source.gmsk.buf);

        // compensate for 2 samples/symbol
        sample = _q->source.gmsk.buf[ _q->source.gmsk.index++ ] *  M_SQRT1_2;
        _q->source.gmsk.index &= 1; // reset index every 2 samples
        break;
    default:
        return liquid_error(LIQUID_EINT,"qsource%s_generate(), invalid internal state",EXTENSION);
    }
    
    if (!_q->enabled)
        sample = 0.0f;

    // TODO: push through resampler

    // mix sample up
    NCO(_mix_up)(_q->mixer, sample, _v);

    // step mixer
    NCO(_step)(_q->mixer);
    return LIQUID_OK;
}

// generate a block of samples, convert to frequency domain, and write
// result into parent channelizer buffer at appropriate frequency location
int QSOURCE(_generate_into)(QSOURCE() _q,
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
    return LIQUID_OK;
}

