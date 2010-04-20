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
// The result is a lossy encoder (as a result of quantization)
// whose compression/quality levels can be easily varied.
// 
// More information available in src/audio/readme.fbasc.txt
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "liquid.internal.h"

#define FBASC_DEBUG     1

// fbasc object structure
struct fbasc_s {
    int type;                       // encoder/decoder
    unsigned int num_channels;      // MDCT size (half-length)
    unsigned int samples_per_frame; // input audio samples per frame
    unsigned int bytes_per_frame;   // number of bytes per frame

    // derived values
    unsigned int symbols_per_frame; // samples_per_frame/num_channels
    unsigned int header_len;        // header length (bytes)
    unsigned int bits_per_frame;
    unsigned int bits_per_symbol;
    unsigned int max_bits_per_sample;

    // common objects
    float * w;                      // MDCT window [size: 2*num_channels x 1]
    float * buffer;                 // MDCT buffer [size: 2*num_channels x 1]
    float * X;                      // channelized matrix [size: num_channels x symbols_per_frame]
    unsigned int * bk;              // bits per channel [size: num_channels x 1]
    float * gk;                     // channel gain [size: num_channels x 1]
    unsigned char * data;           // quantized frame data (bytes) [size: num_channels x symbols_per_frame]
    // TODO : extend 'data' variable to unsigned short to allow more than 8 bits / sample
    //unsigned char * packed_data;    // packed quantized data

    // analysis/synthesis
    float * channel_energy;         // signal variance on each channel [size: num_channels x 1]
    float gain;                     // nominal gain
    float mu;                       // compression factor (see module:quantization)
};

// compute length of header
unsigned int fbasc_compute_header_length(unsigned int _num_channels,
                                         unsigned int _samples_per_frame,
                                         unsigned int _bytes_per_frame)
{
    return _num_channels + 1;
}

// create options
//  _type               :   analysis/synthesis (encoder/decoder)
//  _num_channels       :   number of filterbank channels
//  _samples_per_frame  :   number of real samples per frame (must be even multiple of _num_channels)
//  _bytes_per_frame    :   number of encoded data bytes per frame
fbasc fbasc_create(int _type,
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

#if 1
    if (q->samples_per_frame != q->bytes_per_frame) {
        fprintf(stderr,"error: fbasc_create(), [bug] samples_per_frame must be equal to bytes_per_frame\n");
        exit(1);
    }
#endif

    // validate input
    if (q->type == FBASC_ENCODER) {
    } else if (q->type == FBASC_DECODER) {
    } else {
        fprintf(stderr,"error: fbasc_create(), unknown type: %d\n", _type);
        exit(1);
    }

    // initialize derived values/lengths
    q->symbols_per_frame = (q->samples_per_frame) / (q->num_channels);
    q->bits_per_frame = 8*q->bytes_per_frame;
    q->bits_per_symbol = q->bits_per_frame / q->symbols_per_frame;
    q->max_bits_per_sample = 8;

    // ensure num_channels evenly divides samples_per_frame
    if ( q->symbols_per_frame * q->num_channels != q->samples_per_frame) {
        fprintf(stderr,"error: fbasc_create(), _num_channels must evenly divide _samples_per_frame\n");
        exit(1);
    }

    // ensure num_channels evenly divides samples_per_frame
    unsigned int bytes_per_symbol = q->bytes_per_frame / q->num_channels;
    if ( bytes_per_symbol * q->num_channels != q->bytes_per_frame) {
        fprintf(stderr,"error: fbasc_create(), _num_channels must evenly divide _bytes_per_frame\n");
        exit(1);
    }

    // compute header length (bytes)
    q->header_len = fbasc_compute_header_length(q->num_channels,
                                                q->samples_per_frame,
                                                q->bytes_per_frame);

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
    printf("    header length:  %u bytes\n", _q->header_len);
}

// encode frame of audio
//  _q      :   fbasc object
//  _audio  :   audio samples [size: samples_per_frame x 1]
//  _header :   encoded header [size: ???]
//  _frame  :   encoded frame bytes [size: bytes_per_frame x 1]
void fbasc_encode(fbasc _q,
                  float * _audio,
                  unsigned char * _header,
                  unsigned char * _frame)
{
    // run analyzer
    fbasc_encoder_run_analyzer(_q, _audio, _q->X);

    // compute channel energy
    fbasc_encoder_compute_channel_energy(_q);

    // compute bit partitioning
    fbasc_compute_bit_allocation(_q->num_channels,
                                 _q->channel_energy,
                                 _q->bits_per_symbol,
                                 _q->max_bits_per_sample,
                                 _q->bk);

    // compute metrics for encoding
    fbasc_encoder_compute_metrics(_q);

    // quantize samples
    fbasc_encoder_quantize_samples(_q);

#if 0
    // pack data
    fbasc_encoder_pack_frame(_q);

    // TODO : encode header
#else
    // write raw data to header
    unsigned int i;
    _header[0] = 0; // nominal gain
    printf("encoder:\n");
    for (i=0; i<_q->num_channels; i++) {
        _header[i+1] = _q->bk[i];
        printf("  %3u : b=%u\n", i, _header[i+1]);
    }

    // write raw samples to framedata
    memmove(_frame, _q->data, _q->samples_per_frame*sizeof(unsigned char));
#endif

}

// decode frame of audio
//  _q      :   fbasc object
//  _header :   encoded header [size: ???]
//  _frame  :   encoded frame bytes [size: bytes_per_frame x 1]
//  _audio  :   decoded audio samples [size: samples_per_frame x 1]
void fbasc_decode(fbasc _q,
                  unsigned char * _header,
                  unsigned char * _frame,
                  float * _audio)
{
    unsigned int i;

#if 0
    // unpack data
    fbasc_decoder_unpack_frame(_q);

    // TODO : decode header
#else
    // read raw data from header
    //_q->gi = _header[0]; // nominal gain
    printf("decoder:\n");
    for (i=0; i<_q->num_channels; i++) {
        _q->bk[i] = _header[i+1];
        printf("  %3u : b=%u\n", i, _q->bk[i]);
    }

    // read raw samples from framedata
    memmove(_q->data, _frame, _q->samples_per_frame*sizeof(unsigned char));
#endif

    // compute metrics for decoding
    printf("**** DECODER METRICS\n");
    fbasc_encoder_compute_metrics(_q);

    // de-quantize samples
    fbasc_decoder_dequantize_samples(_q);

    // run synthesizer
    fbasc_decoder_run_synthesizer(_q, _q->X, _audio);
}


// 
// internal methods
//

// fbasc_encoder_run_analyzer()
//
// run analyzer on a frame of data
//  _q      :   fbasc object
//  _x      :   input audio samples [size: samples_per_frame x 1]
//  _X      :   output channelized samples [size: num_channels x symbols_per_frame]
//  NOTE: num_channels * symbols_per_frame = samples_per_frame
void fbasc_encoder_run_analyzer(fbasc _q,
                                float * _x,
                                float * _X)
{
    unsigned int i;

    for (i=0; i<_q->symbols_per_frame; i++) {
        // copy last half of buffer to first half
        memmove(_q->buffer,
                &_q->buffer[_q->num_channels],
                _q->num_channels*sizeof(float));

        // copy input block [size: num_channels x 1] to last half of buffer
        memmove(&_q->buffer[_q->num_channels],
                &_x[i*_q->num_channels],
                _q->num_channels*sizeof(float));

        // run transform on internal buffer, store result in output
        mdct(_q->buffer,
             &_X[i*_q->num_channels],
             _q->w,
             _q->num_channels);
    }
}

// fbasc_decoder_run_synthesizer()
//
// run synthesizer on a frame of data
//  _q      :   fbasc object
//  _X      :   intput channelized samples [size: num_channels x symbols_per_frame]
//  _x      :   output audio samples [size: samples_per_frame x 1]
//  NOTE: num_channels * symbols_per_frame = samples_per_frame
void fbasc_decoder_run_synthesizer(fbasc _q,
                                   float * _X,
                                   float * _x)
{
    unsigned int i,j;

    // copy last half of buffer to beginning of output; this
    // preserves continuity between frames
    memmove(_x,
            &_q->buffer[_q->num_channels],
            _q->num_channels*sizeof(float));

    for (i=0; i<_q->symbols_per_frame; i++) {
        // run inverse transform on input [size: num_channels x 1]
        imdct(&_X[i*_q->num_channels],
              _q->buffer,
              _q->w,
              _q->num_channels);

        // accumulate first half of buffer to output
        for (j=0; j<_q->num_channels; j++)
            _x[i*_q->num_channels + j] += _q->buffer[j];

        // copy last half of buffer to output (only if the
        // index isn't on the last symbol)
        if (i==_q->symbols_per_frame-1)
            continue;
        memmove(&_x[(i+1)*_q->num_channels],
                &_q->buffer[_q->num_channels],
                _q->num_channels*sizeof(float));
    }
}

// compute normalized channel energy
void fbasc_encoder_compute_channel_energy(fbasc _q)
{
    unsigned int i,j;

    // clear channel energy array
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    // compute channel energy, maximum amplitude
    float max_amp = 0.0f;
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {
            // strip sample from channelized data
            float v = _q->X[i*_q->num_channels+j];

            // accumulate energy on channel
            _q->channel_energy[j] += v*v;

            if ( fabsf(v) > max_amp || (i==0 && j==0) )
                max_amp = fabsf(v);
        }
    }

    // normalize channel energy by number of symbols per frame
    // TODO : determine if this is really necessary
    float max_var = 0.0f;
    for (i=0; i<_q->num_channels; i++) {
        _q->channel_energy[i] = _q->channel_energy[i] / _q->symbols_per_frame;
        max_var = _q->channel_energy[i] > max_var ? _q->channel_energy[i] : max_var;
    }
    printf("max variance:  %16.12f\n", max_var);
    printf("max amplitude: %16.12f\n", max_amp);
}


// TODO: document this method
// fbasc_compute_bit_allocation()
//
// computes optimal bit allocation based on energy data
//
//  _n          :   number of channels
//  _e          :   energy array [size: _n x 1]
//  _num_bits   :   total number of bits per symbol
//  _max_bits   :   maximum number of bits per channel
//  _k          :   resulting bit allocation per channel [size: _n x 1]
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
        printf("e[%3u] = %12.8f, b = %8.4f, log2p = %12.4f, bk = %8.4f(%3d)\n",
               idx[i], _e[idx[i]], b, log2p, bkf, bk);
#endif
        _k[idx[i]] = bk;

        available_bits -= bk;
        n--;
    }

}

// compute encoder metrics
void fbasc_encoder_compute_metrics(fbasc _q)
{
    unsigned int i;

    // compute nominal gain
#if 0
    int gi = (int)(-log2f(max_var)) - 16;   // use variance
    //int gi = (int)(-log2f(max_amp)) - 1;    // use peak amplitude
    gi = gi > 255 ? 255 : gi;
    gi = gi <   0 ?   0 : gi;
    _q->gain = (float)(1<<gi);
#else
    // TODO FIXME : compute nominal gain properly
    int gi = 0;
    _q->gain = 1.0f;
#endif

    // find maximum bit allocation
    unsigned int bk_max = 0;
    for (i=0; i<_q->num_channels; i++) {
        if (_q->bk[i] > bk_max || i==0)
            bk_max = _q->bk[i];
    }

    // compute relative gains: gk = 2^(max(bk) - bk)
    for (i=0; i<_q->num_channels; i++)
        _q->gk[i] = (float)(1<<(bk_max-_q->bk[i])) * _q->gain;

#if FBASC_DEBUG
    printf("encoder metrics:\n");
    printf("    nominal gain : %12.4e (gi = %3u)\n", _q->gain, gi);
    for (i=0; i<_q->num_channels; i++) {
        if (_q->bk[i] > 0)
            printf("  %3u : e = %12.8f, b = %3u, g=%12.4f\n", i, _q->channel_energy[i],_q->bk[i], _q->gk[i]);
        else
            printf("  %3u : e = %12.8f, b = %3u\n",           i, _q->channel_energy[i],_q->bk[i]);
    }
#endif
}


// quantize channelized data
void fbasc_encoder_quantize_samples(fbasc _q)
{
    unsigned int i;     // symbol counter
    unsigned int j;     // channel counter
    unsigned int s=0;   // output sample counter
    float sample;       // channelized sample
    float z;            // compressed sample
    unsigned int b;     // quantized sample

    // cycle through symbols in each channel and quantize
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {
            if (_q->bk[j] > 0) {
                // acquire sample, applying proper gain
                sample = _q->X[i*(_q->num_channels)+j] * _q->gk[j];

                // compress using mu-law encoder
                z = compress_mulaw(sample, _q->mu);

                // quantize
                b = quantize_adc(z, _q->bk[j]);
#if 0
                if (s < 10)
                    printf("  %3u : b=0x%4.4x, z=%12.8f, sample=%12.8f\n", s, b,z,sample);
#endif
            } else {
                b = 0;
            }
            _q->data[s] = b;

            s++;
        }
    }

    printf("encoder data...\n");
    for (i=0; i<10; i++)
        printf("  %3u : %3u\n", i, _q->data[i]);

}

// de-quantize channelized data
void fbasc_decoder_dequantize_samples(fbasc _q)
{
    unsigned int i;     // symbol counter
    unsigned int j;     // channel counter
    unsigned int s=0;   // input sample counter
    float sample;       // channelized sample
    float z;            // compressed sample
    unsigned int b;     // quantized sample

    printf("decoder data...\n");
    for (i=0; i<10; i++)
        printf("  %3u : %3u\n", i, _q->data[i]);

    // cycle through symbols in each channel and quantize
    for (i=0; i<_q->symbols_per_frame; i++) {
        for (j=0; j<_q->num_channels; j++) {
            if (_q->bk[j] > 0) {
                // acquire digital sample
                b = _q->data[s];

                // quantize, digital-to-analog conversion
                z = quantize_dac(b, _q->bk[j]);

                // de-compress (expand) using mu-law decoder
                sample = expand_mulaw(z, _q->mu) / _q->gk[j];

#if 0
                if (s < 10)
                    printf("  %3u : b=0x%4.4x, z=%12.8f, sample=%12.8f\n", s, b,z,sample);
#endif
            } else {
                sample = 0.0f;
            }
            _q->X[i*(_q->num_channels)+j] = sample;
            s++;
        }
    }
}

// pack frame
void fbasc_encoder_pack_frame(fbasc _q)
{
}

// unpack frame
void fbasc_decoder_unpack_frame(fbasc _q)
{
}


