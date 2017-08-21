/*
 * Copyright (c) 2007 - 2017 Joseph Gaeddert
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
// spwaterfall (spectral periodogram waterfall)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <complex.h>
#include "liquid.internal.h"

struct SPWATERFALL(_s) {
    // options
    unsigned int    nfft;           // FFT length
    unsigned int    time;           // minimum time buffer
    SPGRAM()        periodogram;    // spectral periodogram object

    // buffers
    T *             psd;            // time/frequency buffer [nfft x 2*time]
    unsigned int    index_time;     // time index for writing to buffer
    unsigned int    rollover;       // number of FFTs to take before writing to output
};

//
// internal methods
//

// compute spectral periodogram output (complex values) from internal periodogram object
void SPWATERFALL(_step)(SPWATERFALL() _q);

// consolidate buffer by taking log-average of two separate spectral estimates in time
void SPWATERFALL(_consolidate_buffer)(SPWATERFALL() _q);

// export files
int SPWATERFALL(_export_bin)(SPWATERFALL() _q, const char * _filename_base);
int SPWATERFALL(_export_gnu)(SPWATERFALL() _q, const char * _filename_base);

// create spwaterfall object
//  _nfft       : FFT size
//  _wtype      : window type, e.g. LIQUID_WINDOW_HAMMING
//  _window_len : window length
//  _delay      : delay between transforms, _delay > 0
SPWATERFALL() SPWATERFALL(_create)(unsigned int _nfft,
                                   int          _wtype,
                                   unsigned int _window_len,
                                   unsigned int _delay,
                                   unsigned int _time)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spwaterfall%s_create(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spwaterfall%s_create(), window size cannot exceed fft size\n", EXTENSION);
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spwaterfall%s_create(), window size must be greater than zero\n", EXTENSION);
        exit(1);
    } else if (_wtype == LIQUID_WINDOW_KBD && _window_len % 2) {
        fprintf(stderr,"error: spwaterfall%s_create(), KBD window length must be even\n", EXTENSION);
        exit(1);
    } else if (_delay == 0) {
        fprintf(stderr,"error: spwaterfall%s_create(), delay must be greater than 0\n", EXTENSION);
        exit(1);
    } else if (_time == 0) {
        fprintf(stderr,"error: spwaterfall%s_create(), time must be greater than 0\n", EXTENSION);
        exit(1);
    }

    // allocate memory for main object
    SPWATERFALL() q = (SPWATERFALL()) malloc(sizeof(struct SPWATERFALL(_s)));

    // set input parameters
    q->nfft = _nfft;
    q->time = _time;

    // create buffer to hold aggregated power spectral density
    // NOTE: the buffer is two-dimensional time/frequency grid that is two times
    //       'nfft' and 'time' to account for log-average consolidation each time
    //       the buffer gets filled
    q->psd = (T*) malloc( 2 * q->nfft * q->time * sizeof(T));

    // create spectral periodogram object
    q->periodogram = SPGRAM(_create)(_nfft, _wtype, _window_len, _delay);

    // reset the object
    SPWATERFALL(_reset)(q);

    // return new object
    return q;
}

// create default spwaterfall object (Kaiser-Bessel window)
SPWATERFALL() SPWATERFALL(_create_default)(unsigned int _nfft,
                                           unsigned int _time)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spwaterfall%s_create_default(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_time < 2) {
        fprintf(stderr,"error: spwaterfall%s_create_default(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    }

    return SPWATERFALL(_create)(_nfft, LIQUID_WINDOW_KAISER, _nfft/2, _nfft/4, _time);
}

// destroy spwaterfall object
void SPWATERFALL(_destroy)(SPWATERFALL() _q)
{
    // free allocated memory
    free(_q->psd);

    // destroy internal spectral periodogram object
    SPGRAM(_destroy)(_q->periodogram);

    // free main object
    free(_q);
}

// clears the internal state of the spwaterfall object, but not
// the internal buffer
void SPWATERFALL(_clear)(SPWATERFALL() _q)
{
    memset(_q->psd, 0x00, 2*_q->nfft*_q->time*sizeof(T));
    _q->index_time = 0;
}

// reset the spwaterfall object to its original state completely
void SPWATERFALL(_reset)(SPWATERFALL() _q)
{
    SPWATERFALL(_clear)(_q);
    _q->rollover = 1;
}

// prints the spwaterfall object's parameters
void SPWATERFALL(_print)(SPWATERFALL() _q)
{
    printf("spwaterfall%s: nfft=%u, time=%u\n", EXTENSION, _q->nfft, _q->time);
}

// push a single sample into the spwaterfall object
//  _q      :   spwaterfall object
//  _x      :   input sample
void SPWATERFALL(_push)(SPWATERFALL() _q,
                        TI            _x)
{
    SPGRAM(_push)(_q->periodogram, _x);
    SPWATERFALL(_step)(_q);
}

// write a block of samples to the spwaterfall object
//  _q      :   spwaterfall object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void SPWATERFALL(_write)(SPWATERFALL() _q,
                         TI *          _x,
                         unsigned int  _n)
{
    // TODO: be smarter about how to write and execute samples
    unsigned int i;
    for (i=0; i<_n; i++)
        SPWATERFALL(_push)(_q, _x[i]);
}

// export output files
//  _q             : spwaterfall object
//  _filename_base : base filename
int SPWATERFALL(_export)(SPWATERFALL() _q,
                         const char *  _filename_base)
{
    return
    SPWATERFALL(_export_bin)(_q, _filename_base) +
    SPWATERFALL(_export_gnu)(_q, _filename_base);
}

// compute spectral periodogram output from current buffer contents
//  _q : spwaterfall object
void SPWATERFALL(_step)(SPWATERFALL() _q)
{
    // determine if we need to extract PSD estimate from periodogram
    if (SPGRAM(_get_num_transforms)(_q->periodogram) >= _q->rollover) {
        //printf("index : %u\n", _q->index_time);
        // get PSD estimate from periodogram object, placing result in
        // proper location in internal buffer
        SPGRAM(_get_psd)(_q->periodogram, _q->psd + _q->nfft*_q->index_time);

        // soft reset of internal state, counters
        SPGRAM(_clear)(_q->periodogram);

        // increment buffer counter
        _q->index_time++;

        // determine if buffer is full and we need to consolidate buffer
        if (_q->index_time == 2*_q->time)
            SPWATERFALL(_consolidate_buffer)(_q);
    }
}

// consolidate buffer by taking log-average of two separate spectral estimates in time
//  _q : spwaterfall object
void SPWATERFALL(_consolidate_buffer)(SPWATERFALL() _q)
{
    // assert(_q->index_time == 2*_q->time);
    printf("consolidating... (rollover = %10u, total samples : %16llu, index : %u)\n",
            _q->rollover, SPGRAM(_get_num_samples_total)(_q->periodogram), _q->index_time);
    unsigned int i; // time index
    unsigned int k; // freq index
    for (i=0; i<_q->time; i++) {
        for (k=0; k<_q->nfft; k++) {
            // compute median
            T v0  = _q->psd[ (2*i + 0)*_q->nfft + k ];
            T v1  = _q->psd[ (2*i + 1)*_q->nfft + k ];

            // keep log average (only need double buffer for this, not triple buffer)
            _q->psd[ i*_q->nfft + k ] = logf(0.5f*(expf(v0) + expf(v1)));
        }
    }

    // update time index
    _q->index_time = _q->time;

    // update rollover counter
    _q->rollover *= 2;
}

// export gnuplot file
//  _q        : spwaterfall object
//  _filename : input buffer [size: _n x 1]
int SPWATERFALL(_export_bin)(SPWATERFALL() _q,
                             const char *  _filename_base)
{
    // add '.bin' extension to base
    int n = strlen(_filename_base);
    char filename[n+5];
    sprintf(filename,"%s.bin", _filename_base);

    // open output file for writing
    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: spwaterfall%s_export_bin(), could not open '%s' for writing\n",
                EXTENSION, filename);
        return -1;
    }

    unsigned int i;
    
    // write header
    float nfftf = (float)(_q->nfft);
    fwrite(&nfftf, sizeof(float), 1, fid);
    for (i=0; i<_q->nfft; i++) {
        float f = (float)i/nfftf - 0.5f;
        fwrite(&f, sizeof(float), 1, fid);
    }
    
    // write output spectral estimate
    // TODO: force converstion from type 'T' to type 'float'
    uint64_t total_samples = SPGRAM(_get_num_samples_total)(_q->periodogram);
    for (i=0; i<_q->index_time; i++) {
        float n = (float)i / (float)(_q->index_time) * (float)total_samples;
        fwrite(&n, sizeof(float), 1, fid);
        fwrite(&_q->psd[i*_q->nfft], sizeof(float), _q->nfft, fid);
    }

    // close it up
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}

// export gnuplot file
//  _q        : spwaterfall object
//  _filename : input buffer [size: _n x 1]
int SPWATERFALL(_export_gnu)(SPWATERFALL() _q,
                             const char *  _filename_base)
{
    // add '.bin' extension to base
    int n = strlen(_filename_base);
    char filename[n+5];
    sprintf(filename,"%s.gnu", _filename_base);

    // open output file for writing
    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: spwaterfall%s_export_gnu(), could not open '%s' for writing\n",
                EXTENSION, filename);
        return -1;
    }
    
    // scale to thousands, millions, billions (etc.) automatically
    uint64_t total_samples = SPGRAM(_get_num_samples_total)(_q->periodogram);
    char units  = ' ';
    float scale = 1.0f;
    if      (total_samples < 4e3 ) { units = ' '; scale = 1e-0f;  }
    else if (total_samples < 4e6 ) { units = 'k'; scale = 1e-3f;  }
    else if (total_samples < 4e9 ) { units = 'M'; scale = 1e-6f;  }
    else if (total_samples < 4e12) { units = 'G'; scale = 1e-9f;  }
    else if (total_samples < 4e15) { units = 'T'; scale = 1e-12f; }
    else                           { units = 'P'; scale = 1e-15f; }

    fprintf(fid,"#!/usr/bin/gnuplot\n");
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal png size 800,800 enhanced font 'Verdana,10'\n");
    fprintf(fid,"set output '%s.png'\n", _filename_base);
    fprintf(fid,"unset key\n");
    fprintf(fid,"set style line 11 lc rgb '#808080' lt 1\n");
    fprintf(fid,"set border 3 front ls 11\n");
    fprintf(fid,"set style line 12 lc rgb '#888888' lt 0 lw 1\n");
    fprintf(fid,"set grid front ls 12\n");
    fprintf(fid,"set tics nomirror out scale 0.75\n");
    fprintf(fid,"set xrange [-0.5:0.5]\n");
    fprintf(fid,"set yrange [0:%f]\n", (float)(total_samples-1)*scale);
    fprintf(fid,"set xlabel 'Normalized Frequency [f/F_s]'\n");
    fprintf(fid,"set ylabel 'Sample Index'\n");
    fprintf(fid,"set format y '%%.0f %c'\n", units);
    fprintf(fid,"# disable colorbar tics\n");
    fprintf(fid,"set cbtics scale 0\n");
    fprintf(fid,"set palette negative defined ( \\\n");
    fprintf(fid,"    0 '#D53E4F',\\\n");
    fprintf(fid,"    1 '#F46D43',\\\n");
    fprintf(fid,"    2 '#FDAE61',\\\n");
    fprintf(fid,"    3 '#FEE08B',\\\n");
    fprintf(fid,"    4 '#E6F598',\\\n");
    fprintf(fid,"    5 '#ABDDA4',\\\n");
    fprintf(fid,"    6 '#66C2A5',\\\n");
    fprintf(fid,"    7 '#3288BD' )\n");
    fprintf(fid,"\n");
    fprintf(fid,"plot '%s.bin' u 1:($2*%e):3 binary matrix with image\n", _filename_base, scale);
    fclose(fid);

    // close it up
    printf("results written to %s\n", filename);
    printf("index time       : %u\n", _q->index_time);
    printf("rollover         : %u\n", _q->rollover);
    printf("total transforms : %llu\n", SPGRAM(_get_num_transforms_total)(_q->periodogram));
    return 0;
}

