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
// spgram (spectral periodogram)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <complex.h>
#include "liquid.internal.h"

struct SPGRAM(_s) {
    // options
    unsigned int    nfft;           // FFT length
    int             wtype;          // window type
    unsigned int    window_len;     // window length
    unsigned int    delay;          // delay between transforms [samples]
    float           alpha;          // spectrum smoothing filter: feedforward parameter
    float           gamma;          // spectrum smoothing filter: feedback parameter
    int             accumulate;     // accumulate? or use time-average

    WINDOW()        buffer;         // input buffer
    TC *            buf_time;       // pointer to input array (allocated)
    TC *            buf_freq;       // output fft (allocated)
    T  *            w;              // tapering window [size: window_len x 1]
    FFT_PLAN        fft;            // FFT plan

    // psd accumulation
    T *             psd;                    // accumulated power spectral density estimate (linear)
    unsigned int    sample_timer;           // countdown to transform
    uint64_t        num_samples;            // total number of samples since reset
    uint64_t        num_samples_total;      // total number of samples since start
    uint64_t        num_transforms;         // total number of transforms since reset
    uint64_t        num_transforms_total;   // total number of transforms since start

    // parameters for display purposes only
    float           frequency;      // center frequency [Hz]
    float           sample_rate;    // sample rate [Hz]
};

//
// internal methods
//

// compute spectral periodogram output (complex values)
// from current buffer contents
void SPGRAM(_step)(SPGRAM() _q);

// create spgram object
//  _nfft       : FFT size
//  _wtype      : window type, e.g. LIQUID_WINDOW_HAMMING
//  _window_len : window length
//  _delay      : delay between transforms, _delay > 0
SPGRAM() SPGRAM(_create)(unsigned int _nfft,
                         int          _wtype,
                         unsigned int _window_len,
                         unsigned int _delay)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram%s_create(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram%s_create(), window size cannot exceed fft size\n", EXTENSION);
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spgram%s_create(), window size must be greater than zero\n", EXTENSION);
        exit(1);
    } else if (_wtype == LIQUID_WINDOW_KBD && _window_len % 2) {
        fprintf(stderr,"error: spgram%s_create(), KBD window length must be even\n", EXTENSION);
        exit(1);
    } else if (_delay == 0) {
        fprintf(stderr,"error: spgram%s_create(), delay must be greater than 0\n", EXTENSION);
        exit(1);
    }

    // allocate memory for main object
    SPGRAM() q = (SPGRAM()) malloc(sizeof(struct SPGRAM(_s)));

    // set input parameters
    q->nfft       = _nfft;
    q->wtype      = _wtype;
    q->window_len = _window_len;
    q->delay      = _delay;
    q->frequency  =  0;
    q->sample_rate= -1;

    // set object for full accumulation
    SPGRAM(_set_alpha)(q, -1.0f);

    // create FFT arrays, object
    q->buf_time = (TC*) malloc((q->nfft)*sizeof(TC));
    q->buf_freq = (TC*) malloc((q->nfft)*sizeof(TC));
    q->psd      = (T *) malloc((q->nfft)*sizeof(T ));
    q->fft      = FFT_CREATE_PLAN(q->nfft, q->buf_time, q->buf_freq, FFT_DIR_FORWARD, FFT_METHOD);

    // create buffer
    q->buffer = WINDOW(_create)(q->window_len);

    // create window
    q->w = (T*) malloc((q->window_len)*sizeof(T));
    unsigned int i;
    unsigned int n = q->window_len;
    float beta = 10.0f;
    float zeta =  3.0f;
    for (i=0; i<n; i++) {
        switch (q->wtype) {
        case LIQUID_WINDOW_HAMMING:         q->w[i] = hamming(i,n);         break;
        case LIQUID_WINDOW_HANN:            q->w[i] = hann(i,n);            break;
        case LIQUID_WINDOW_BLACKMANHARRIS:  q->w[i] = blackmanharris(i,n);  break;
        case LIQUID_WINDOW_BLACKMANHARRIS7: q->w[i] = blackmanharris7(i,n); break;
        case LIQUID_WINDOW_KAISER:          q->w[i] = kaiser(i,n,beta,0);   break;
        case LIQUID_WINDOW_FLATTOP:         q->w[i] = flattop(i,n);         break;
        case LIQUID_WINDOW_TRIANGULAR:      q->w[i] = triangular(i,n,n);    break;
        case LIQUID_WINDOW_RCOSTAPER:       q->w[i] = liquid_rcostaper_windowf(i,n/3,n); break;
        case LIQUID_WINDOW_KBD:             q->w[i] = liquid_kbd(i,n,zeta); break;
        default:
            fprintf(stderr,"error: spgram%s_create(), invalid window\n", EXTENSION);
            exit(1);
        }
    }

    // scale by window magnitude, FFT size
    float g = 0.0f;
    for (i=0; i<q->window_len; i++)
        g += q->w[i] * q->w[i];
    g = M_SQRT2 / ( sqrtf(g / q->window_len) * sqrtf((float)(q->nfft)) );

    // scale window and copy
    for (i=0; i<q->window_len; i++)
        q->w[i] = g * q->w[i];

    // reset the spgram object
    q->num_samples_total    = 0;
    q->num_transforms_total = 0;
    SPGRAM(_reset)(q);

    // return new object
    return q;
}

// create default spgram object (Kaiser-Bessel window)
SPGRAM() SPGRAM(_create_default)(unsigned int _nfft)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram%s_create_default(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    }

    return SPGRAM(_create)(_nfft, LIQUID_WINDOW_KAISER, _nfft/2, _nfft/4);
}

// destroy spgram object
void SPGRAM(_destroy)(SPGRAM() _q)
{
    // free allocated memory
    free(_q->buf_time);
    free(_q->buf_freq);
    free(_q->w);
    free(_q->psd);
    WINDOW(_destroy)(_q->buffer);
    FFT_DESTROY_PLAN(_q->fft);

    // free main object
    free(_q);
}

// clears the internal state of the spgram object, but not
// the internal buffer
void SPGRAM(_clear)(SPGRAM() _q)
{
    // clear FFT input
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->buf_time[i] = 0.0f;

    // reset counters
    _q->sample_timer   = _q->delay;
    _q->num_transforms = 0;
    _q->num_samples    = 0;

    // clear PSD accumulation
    for (i=0; i<_q->nfft; i++)
        _q->psd[i] = 0.0f;
}

// reset the spgram object to its original state completely
void SPGRAM(_reset)(SPGRAM() _q)
{
    // reset spgram object except for the window buffer
    SPGRAM(_clear)(_q);

    // clear the window buffer
    WINDOW(_reset)(_q->buffer);
}

// prints the spgram object's parameters
void SPGRAM(_print)(SPGRAM() _q)
{
    printf("spgram%s: nfft=%u, window=%u, delay=%u\n",
            EXTENSION, _q->nfft, _q->window_len, _q->delay);
}

// set forgetting factor
int SPGRAM(_set_alpha)(SPGRAM() _q,
                       float    _alpha)
{
    // validate input
    if (_alpha != -1 && (_alpha < 0.0f || _alpha > 1.0f)) {
        fprintf(stderr,"warning: spgram%s_set_alpha(), alpha must be in {-1,[0,1]}\n", EXTENSION);
        return -1;
    }

    // set accumulation flag appropriately
    _q->accumulate = (_alpha == -1.0f) ? 1 : 0;

    if (_q->accumulate) {
        _q->alpha = 1.0f;
        _q->gamma = 1.0f;
    } else {
        _q->alpha = _alpha;
        _q->gamma = 1.0f - _q->alpha;
    }
    return 0;
}

// set center freuqncy
int SPGRAM(_set_freq)(SPGRAM() _q,
                      float    _freq)
{
    _q->frequency = _freq;
    return 0;
}

// set sample rate
int SPGRAM(_set_rate)(SPGRAM() _q,
                      float    _rate)
{
    // validate input
    if (_rate <= 0.0f) {
        fprintf(stderr,"error: spgram%s_set_rate(), sample rate must be greater than zero\n", EXTENSION);
        return -1;
    }
    _q->sample_rate = _rate;
    return 0;
}

// get FFT size
unsigned int SPGRAM(_get_nfft)(SPGRAM() _q)
{
    return _q->nfft;
}

// get window length
unsigned int SPGRAM(_get_window_len)(SPGRAM() _q)
{
    return _q->window_len;
}

// get delay between transforms
unsigned int SPGRAM(_get_delay)(SPGRAM() _q)
{
    return _q->delay;
}

// get number of samples processed since reset
uint64_t SPGRAM(_get_num_samples)(SPGRAM() _q)
{
    return _q->num_samples;
}

// get number of samples processed since start
uint64_t SPGRAM(_get_num_samples_total)(SPGRAM() _q)
{
    return _q->num_samples_total;
}

// get number of transforms processed since reset
uint64_t SPGRAM(_get_num_transforms)(SPGRAM() _q)
{
    return _q->num_transforms;
}

// get number of transforms processed since start
uint64_t SPGRAM(_get_num_transforms_total)(SPGRAM() _q)
{
    return _q->num_transforms_total;
}

// push a single sample into the spgram object
//  _q      :   spgram object
//  _x      :   input sample
void SPGRAM(_push)(SPGRAM() _q,
                   TI       _x)
{
    // push sample into internal window
    WINDOW(_push)(_q->buffer, _x);

    // update counters
    _q->num_samples++;
    _q->num_samples_total++;

    // adjust timer
    _q->sample_timer--;

    if (_q->sample_timer)
        return;

    // reset timer and step through computation
    _q->sample_timer = _q->delay;
    SPGRAM(_step)(_q);
}

// write a block of samples to the spgram object
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void SPGRAM(_write)(SPGRAM()     _q,
                    TI *         _x,
                    unsigned int _n)
{
#if 0
    // write a block of samples to the internal window
    WINDOW(_write)(_q->buffer, _x, _n);
#else
    // TODO: be smarter about how to write and execute samples
    unsigned int i;
    for (i=0; i<_n; i++)
        SPGRAM(_push)(_q, _x[i]);
#endif
}


// compute spectral periodogram output from current buffer contents
//  _q      :   spgram object
void SPGRAM(_step)(SPGRAM() _q)
{
    unsigned int i;

    // read buffer, copy to FFT input (applying window)
    // TODO: use SIMD extensions to speed this up
    TI * rc;
    WINDOW(_read)(_q->buffer, &rc);
    for (i=0; i<_q->window_len; i++)
        _q->buf_time[i] = rc[i] * _q->w[i];

    // execute fft on _q->buf_time and store result in _q->buf_freq
    FFT_EXECUTE(_q->fft);

    // accumulate output
    // TODO: vectorize this operation
    for (i=0; i<_q->nfft; i++) {
        T v = crealf( _q->buf_freq[i] * conjf(_q->buf_freq[i]) );
        if (_q->num_transforms == 0)
            _q->psd[i] = v;
        else
            _q->psd[i] = _q->gamma*_q->psd[i] + _q->alpha*v;
    }

    _q->num_transforms++;
    _q->num_transforms_total++;
}

// compute spectral periodogram output (fft-shifted values
// in dB) from current buffer contents
//  _q      :   spgram object
//  _X      :   output spectrum [size: _nfft x 1]
void SPGRAM(_get_psd)(SPGRAM() _q,
                      T *      _X)
{
    // compute magnitude in dB and run FFT shift
    unsigned int i;
    unsigned int nfft_2 = _q->nfft / 2;
    T scale = _q->accumulate ? -10*log10f(_q->num_transforms) : 0.0f;
    // TODO: adjust scale if infinite integration
    for (i=0; i<_q->nfft; i++) {
        unsigned int k = (i + nfft_2) % _q->nfft;
        _X[i] = 10*log10f(_q->psd[k]+1e-12f) + scale;
    }
}

// export gnuplot file
//  _q        : spgram object
//  _filename : input buffer [size: _n x 1]
int SPGRAM(_export_gnuplot)(SPGRAM()     _q,
                            const char * _filename)
{
    FILE * fid = fopen(_filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: spgram%s_export_gnuplot(), could not open '%s' for writing\n",
                EXTENSION, _filename);
        return -1;
    }
    fprintf(fid,"# %s : auto-generated file\n", _filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal png size 1200,800 enhanced font 'Verdana,10'\n");
    fprintf(fid,"set output '%s.png'\n", _filename);
    fprintf(fid,"set autoscale y\n");
    fprintf(fid,"set ylabel 'Power Spectral Density'\n");
    fprintf(fid,"set style line 12 lc rgb '#404040' lt 0 lw 1\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid front ls 12\n");
    //fprintf(fid,"set style fill transparent solid 0.2\n");
    const char plot_with[] = "lines"; // "filledcurves x1"
    fprintf(fid,"set nokey\n");
    if (_q->sample_rate < 0) {
        fprintf(fid,"set xrange [-0.5:0.5]\n");
        fprintf(fid,"set xlabel 'Noramlized Frequency'\n");
        fprintf(fid,"plot '-' w %s lt 1 lw 2 lc rgb '#004080'\n", plot_with);
    } else {
        char unit = ' ';
        float g   = 1.0f;
        liquid_get_scale(_q->frequency, &unit, &g);
        fprintf(fid,"set xlabel 'Frequency [%cHz]'\n", unit);
        fprintf(fid,"set xrange [%f:%f]\n", g*(_q->frequency-0.5*_q->sample_rate), g*(_q->frequency+0.5*_q->sample_rate));
        fprintf(fid,"plot '-' u ($1*%f+%f):2 w %s lt 1 lw 2 lc rgb '#004080'\n",
                g*(_q->sample_rate < 0 ? 1 : _q->sample_rate), g*_q->frequency, plot_with);
    }

    // export spectrum data
    T * psd = (T*) malloc(_q->nfft * sizeof(T));
    SPGRAM(_get_psd)(_q, psd);
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        fprintf(fid,"  %12.8f %12.8f\n", (float)i/(float)(_q->nfft)-0.5f, (float)(psd[i]));
    free(psd);
    fprintf(fid,"e\n");

    // close it up
    fclose(fid);

    return 0;
}

//
// object-independent methods
//

// estimate spectrum on input signal
//  _nfft   :   FFT size
//  _x      :   input signal [size: _n x 1]
//  _n      :   input signal length
//  _psd    :   output spectrum, [size: _nfft x 1]
void SPGRAM(_estimate_psd)(unsigned int _nfft,
                           TI *         _x,
                           unsigned int _n,
                           T *          _psd)
{
    // create object
    SPGRAM() q = SPGRAM(_create_default)(_nfft);

    // run spectral estimate on entire sequence
    SPGRAM(_write)(q, _x, _n);

    // get PSD estimate
    SPGRAM(_get_psd)(q, _psd);

    // destroy object
    SPGRAM(_destroy)(q);
}
