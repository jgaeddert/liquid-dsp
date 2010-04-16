/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// fbasc : filterbank audio synthesizer codec
// 
// The fbasc audio codec implements an AAC-like compression
// algorithm, using the modified discrete cosine transform as a
// loss-less channelizer.  The resulting channelized data are
// then quantized based on their spectral energy levels and then
// packed into a frame which the decoder can then interpret.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "liquid.internal.h"

#define FBASC_DEBUG     0

struct fbasc_s {
    int type;                       // encoder/decoder
    unsigned int num_channels;      // MDCT size (half-length)
    unsigned int samples_per_frame; // input audio samples per frame
    unsigned int bytes_per_frame;   // number of bytes per frame

    // derived values
    unsigned int symbols_per_frame; // samples_per_frame/num_channels

    // common objects
    float * w;                      // MDCT window [size: 2*num_channels]
    float * buffer;                 // MDCT buffer [size: 2*num_channels]
    float * X;                      // channelized matrix (size: num_channels x symbols_per_frame)
    unsigned int * bk;              // bits per subchannel
    float * gk;                     // subchannel gain
    unsigned char * data;           // quantized frame data (bytes)
    //unsigned char * packed_data;    // packed quantized data

    // analysis/synthesis
    float * channel_energy;
    float mu;
};

// create options
//  _type               :   analysis/synthesis (encoder/decoder)
//  _num_channels       :   number of filterbank channels
//  _samples_per_frame  :   number of real samples per frame (must be even multiple of _num_channels)
//  _bytes_per_frame    :   number of encoded data bytes per frame
fbasc fbasc_create(
        int _type,
        unsigned int _num_channels,
        unsigned int _samples_per_frame,
        unsigned int _bytes_per_frame)
{
    fbasc q = (fbasc) malloc(sizeof(struct fbasc_s));

    // initialize parametric values/lengths
    q->type = _type;
    q->num_channels = _num_channels;
    q->samples_per_frame = _samples_per_frame;
    q->bytes_per_frame = _bytes_per_frame;

    // validate input
    if (q->type == FBASC_ENCODER) {
    } else if (q->type == FBASC_DECODER) {
    } else {
        printf("error: fbasc_create(), unknown type: %d\n", _type);
        exit(1);
    }

    // initialize derived values/lengths
    q->symbols_per_frame = (q->samples_per_frame) / (q->num_channels);

    if ( q->symbols_per_frame * q->num_channels != q->samples_per_frame) {
        fprintf(stderr,"error: fbasc_create(), _num_channels must evenly divide _samples_per_frame\n");
        exit(1);
    }

    // analysis/synthesis
    q->w =              (float*) malloc( 2*(q->num_channels)*sizeof(float) );
    q->buffer =         (float*) malloc( 2*(q->num_channels)*sizeof(float) );
    q->X =              (float*) malloc( (q->samples_per_frame)*sizeof(float) );
    q->channel_energy = (float*) malloc( (q->num_channels)*sizeof(float) );
    q->gk =             (float*) malloc( (q->num_channels)*sizeof(float) );
    q->mu = 255.0f;

    // data
    q->bk = (unsigned int *) malloc( (q->num_channels)*sizeof(unsigned int));
    q->data = (unsigned char *) malloc( (q->samples_per_frame)*sizeof(unsigned char));

    unsigned int i;

    // initialize window
    for (i=0; i<2*q->num_channels; i++)
        q->w[i] = liquid_kbd_window(i,2*q->num_channels,10.0f);

    // reset buffer
    for (i=0; i<2*q->num_channels; i++)
        q->buffer[i] = 0.0f;

    return q;
}

void fbasc_destroy(fbasc _q)
{
    // free common arrays
    free(_q->X);        // channelized samples
    free(_q->w);        // windowing function for MDCT
    free(_q->buffer);   // buffer for MDCT
    free(_q->data);
    free(_q->bk);
    free(_q->gk);

    // free memory structure
    free(_q);
}

void fbasc_print(fbasc _q)
{
    printf("filterbank audio synthesizer codec:\n");
    printf("    channels:       %u\n", _q->num_channels);
    printf("    type:           %s\n", 
        _q->type == FBASC_ENCODER ? "encoder" : "decoder");
    printf("    samples/frame:  %u\n", _q->samples_per_frame);
    printf("    symbols/frame:  %u\n", _q->symbols_per_frame);
    printf("    bytes/frame:    %u\n", _q->bytes_per_frame);
}

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame)
{
    unsigned int i, j;

    // run analyzer

    // compute channel energy
   
    // compute bit allocation

    // quantize samples

    // pack data

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    unsigned int n=0;   // input sample counter
    for (i=0; i<_q->symbols_per_frame; i++) {
        // channelize time series
        //itqmfb_rrrf_execute(_q->channelizer, _audio + n, _q->X + n);

        // compute energy on each channel
        for (j=0; j<_q->num_channels; j++)
            _q->channel_energy[j] += (_q->X[n+j])*(_q->X[n+j]);

        n += _q->num_channels;
    }

#if 0
    // normalize channel energy
    float max_var = 0.0f;
    for (i=0; i<_q->num_channels; i++) {
        _q->channel_energy[i] = _q->channel_energy[i] / _q->symbols_per_frame;
        max_var = _q->channel_energy[i] > max_var ? _q->channel_energy[i] : max_var;
    }
    //printf("max variance: %16.12f\n", max_var);

    // compute nominal gain
    int gi = (int)(-log2f(max_var)) - 16;
    gi = gi > 255 ? 255 : gi;
    gi = gi <   0 ?   0 : gi;
    float g = (float)(1<<gi);
#if FBASC_DEBUG
    printf("  enc: nominal gain : %12.4e (gi = %3u)\n", g, gi);
#endif

#if FBASC_DEBUG
    printf("channel energy:\n");
    for (i=0; i<_q->num_channels; i++)
        printf("  e[%3u] = %12.8f\n", i, _q->channel_energy[i]);
#endif

    // compute bit partitioning
    fbasc_compute_bit_allocation(_q->num_channels,
                                 _q->channel_energy,
                                 _q->bits_per_block,
                                 _q->max_bits_per_sample,
                                 _q->bk);

    // write partition to header
    unsigned int s=0;
    _frame[s++] = gi;
    unsigned int k_max=0;
    for (i=0; i<_q->bytes_per_header; i++) {
        _frame[s++] = _q->bk[i];
        k_max = (_q->bk[i] > k_max) ? _q->bk[i] : k_max;
    }

    // compute scaling factor: gk = 2^(max(bk) - bk)
    for (i=0; i<_q->num_channels; i++)
        _q->gk[i] = (float)(1<<(k_max-_q->bk[i]));

    // encode using basic quantizer
    float sample, z;
#if FBASC_DEBUG
    float max_sample=0.0f;
#endif
    unsigned int b;
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {

            if (_q->bk[j] > 0) {
                // acquire sample, applying proper gain
                sample = _q->X[i*(_q->num_channels)+j] * _q->gk[j] * g;

                // compress using mu-law encoder
                z = compress_mulaw(sample, _q->mu);

#if FBASC_DEBUG
                if (fabsf(z) > max_sample)
                    max_sample = fabsf(z);
#endif
                // quantize
                b = quantize_adc(z, _q->bk[j]);
            } else {
                b = 0;
            }

            _frame[s] = b;
            s++;
        }
    }
#if FBASC_DEBUG
    printf("max sample: %12.8f\n", max_sample);
#endif

    // TODO: pack frame

#endif

}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{
    unsigned int i, j;

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    // get bit partitioning
    unsigned int s=0;
    // compute nominal gain
    unsigned int gi = _frame[s++];
    float g = (float)(1<<gi);
#if FBASC_DEBUG
    printf("  dec: nominal gain : %12.4e (gi = %3u)\n", g, gi);
#endif
    unsigned int k_max=0;
    for (i=0; i<_q->num_channels; i++) {
        _q->bk[i] = _frame[s];
        s++;
        k_max = (_q->bk[i] > k_max) ? _q->bk[i] : k_max;
    }

    // compute scaling factor: gk = 2^-(max(bk) - bk)
    for (i=0; i<_q->num_channels; i++)
        _q->gk[i] = 1.0f / (float)(1<<(k_max-_q->bk[i]));

    // decode using basic quantizer
    float sample, z;
    unsigned int b;
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {
            if (_q->bk[j] > 0) {
                // quantize
                b = _frame[s];

                z = quantize_dac(b,_q->bk[j]);
            } else {
                z = 0.0f;
            }

            s++;

            // expand using mu-law decoder
            sample = expand_mulaw(z, _q->mu);

            // store sample, applying proper gain
            _q->X[i*(_q->num_channels)+j] = sample * _q->gk[j] / g;
        }
    }

    unsigned int n=0;   // output sample counter
    for (i=0; i<_q->symbols_per_frame; i++) {
        // run synthesizer
        //itqmfb_rrrf_execute(_q->channelizer, _q->X + n, _audio + n);

        n += _q->num_channels;
    }
}


// internal

// run analyzer
void fbasc_encoder_run_analyzer(fbasc _q,
                                float * _x,
                                float * _X)
{
    unsigned int i;

    for (i=0; i<_q->symbols_per_frame; i++) {
        // copy last half of buffer to first half
        memmove(_q->buffer, &_q->buffer[_q->num_channels], _q->num_channels*sizeof(float));

        // copy input block to last half of buffer
        memmove(&_q->buffer[_q->num_channels], &_x[i*_q->num_channels], _q->num_channels*sizeof(float));

        // run transform
        mdct(_q->buffer, &_X[i*_q->num_channels], _q->w, _q->num_channels);
    }
}

// run synthesizer
void fbasc_decoder_run_synthesizer(fbasc _q,
                                   float * _X,
                                   float * _x)
{
    unsigned int i,j;

    // copy last half of buffer to beginning of output; this
    // preserves continuity between frames
    memmove(_x, &_q->buffer[_q->num_channels], _q->num_channels*sizeof(float));

    for (i=0; i<_q->symbols_per_frame; i++) {
        // run inverse transform
        imdct(&_X[i*_q->num_channels], _q->buffer, _q->w, _q->num_channels);

        // accumulate first half of buffer to output
        for (j=0; j<_q->num_channels; j++)
            _x[i*_q->num_channels + j] += _q->buffer[j];

        // copy last half of buffer to output (only if the
        // index isn't on the last symbol)
        if (i==_q->symbols_per_frame-1) continue;
        memmove(&_x[(i+1)*_q->num_channels], &_q->buffer[_q->num_channels], _q->num_channels*sizeof(float));
    }
}

// TODO: document this method
void fbasc_compute_bit_allocation(unsigned int _n,
                                  float * _e,
                                  unsigned int _num_bits,
                                  unsigned int _max_bits,
                                  unsigned int * _k)
{
    unsigned int idx[_n];   // sorted indices

    unsigned int i, j;
    for (i=0; i<_n; i++)
        idx[i] = i;

    // sort (inefficient, but easy to implement)
    unsigned int i_tmp;
    for (i=0; i<_n; i++) {
        for (j=0; j<_n; j++) {
            if ( (i!=j) && (_e[idx[i]] > _e[idx[j]]) ) {
                // swap values
                i_tmp = idx[i];
                idx[i] = idx[j];
                idx[j] = i_tmp;
            }
        }
    }

    // compute bit allocation
    float log2p;
    int bk;
    float bkf;
    unsigned int n=_n;
    unsigned int available_bits = _num_bits;
    float b;
    for (i=0; i<_n; i++) {
        log2p = 0.0f;
        b = (float)(available_bits) / (float)(n);
        // compute 'entropy' metric
        for (j=i; j<_n; j++)
            log2p += (_e[idx[j]] == 0.0f) ? -60.0f : log2f(_e[idx[j]]);
        log2p /= n;

        bkf = (_e[idx[i]]==0.0f) ? 1.0f : b + 0.5f*log2f(_e[idx[i]]) - 0.5f*log2p;
        bk  = (int)roundf(bkf);

        bk = (bk > _max_bits)       ? _max_bits         : bk;
        bk = (bk > available_bits)  ? available_bits    : bk;
        bk = (bk < 0)               ? 0                 : bk;

#if FBASC_DEBUG
        printf("e[%3u] = %12.8f, b = %8.4f, log2p = %12.8f, bk = %8.4f(%3d)\n",
               idx[i], _e[idx[i]], b, log2p, bkf, bk);
#endif
        _k[idx[i]] = bk;

        available_bits -= bk;
        n--;
    }

}

// quantize channelized data
void fbasc_encoder_quantize_samples(fbasc _q)
{
    unsigned int i;     // symbol counter
    unsigned int j;     // sub-channel counter
    unsigned int s=0;   // output sample counter
    float sample;       // channelized sample
    float z;            // compressed sample
    unsigned int b;     // quantized sample

#if 0
    // find bk_max
    unsigned int bk_max=0;
    for (i=0; i<_q->num_channels; i++)
        bk_max = _q->bk[i] > bk_max ? _q->bk[i] : bk_max;

    // compute scaling factor: gk = 2^(max(bk) - bk)
    for (i=0; i<_q->num_channels; i++)
        _q->gk[i] = (float)(1<<(bk_max-_q->bk[i]));
#endif

    // cycle through symbols in each sub-channel and quantize
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {
            if (_q->bk[j] > 0) {
                // acquire sample, applying proper gain
                //sample = _q->X[i*(_q->num_channels)+j] * _q->gk[j] * g;
                sample = _q->X[i*(_q->num_channels)+j];

                // compress using mu-law encoder
                z = compress_mulaw(sample, _q->mu);

                // quantize
                b = quantize_adc(z, _q->bk[j]);
            } else {
                b = 0;
            }
            _q->data[s++] = b;
        }
    }
}

// de-quantize channelized data
void fbasc_decoder_deqauntize_samples(fbasc _q)
{
}

// pack frame
void fbasc_encoder_pack_frame(fbasc _q)
{
}

// unpack frame
void fbasc_decoder_unpack_frame(fbasc _q)
{
}


