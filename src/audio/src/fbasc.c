//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "liquid.internal.h"

#define FBASC_DEBUG 1
#define DEBUG_FILE  stdout

#define FBASC_COMPRESS 1

//  description         value   units
//  -----------         -----   -----
//  sample rate         16      kHz
//  frame length        64      ms
//  samples/frame       1024    samp.
//
//  num. channels       32      -
//  samp./ch./frame     32      samp.
//
//  av. bit rate        16      kbps
//  num. used channels  16      ch.
//  bits/u. ch./frame   ?       bits
//

struct fbasc_s {
    int type;                       // encoder/decoder
    unsigned int num_channels;      // 16
    unsigned int samples_per_frame; // 256
    unsigned int bytes_per_header;  // 8 (num_channels/2)
    unsigned int bytes_per_frame;   // <fixed>

    // derived values
    unsigned int samples_per_channel;   // samples_per_frame/num_channels (16)

    // common objects
    int channelizer_type;
    itqmfb_rrrf channelizer;        // channelizer
    float * X;              // channelized matrix (length: samples_per_frame)
    unsigned char * data;   // frame data (bytes)

    // analysis/synthesis
    float * channel_energy;
    float mu;
};

// create options
//  _type               :   analysis/synthesis (encoder/decoder)
//  _num_channels       :   number of filterbank channels
//  _samples_per_frame  :   number of real samples per frame
//  _bytes_per_frame    :   number of encoded data bytes per frame
//
// FIXME: only build objects necessary for codec type (analysis vs. synthesis)
fbasc fbasc_create(
        int _type,
        unsigned int _num_channels,
        unsigned int _samples_per_frame,
        unsigned int _bytes_per_frame)
{
    fbasc q = (fbasc) malloc(sizeof(struct fbasc_s));
    q->type = _type;

    if (q->type == FBASC_ENCODER) {
        q->channelizer_type = LIQUID_ITQMFB_ANALYZER;
    } else if (q->type == FBASC_DECODER) {
        q->channelizer_type = LIQUID_ITQMFB_SYNTHESIZER;
    } else {
        printf("error: fbasc_create(), unknown type: %d\n", _type);
        exit(1);
    }

    // initialize parametric values/lengths
    q->num_channels = _num_channels;
    q->samples_per_frame = _samples_per_frame;
    q->bytes_per_frame = _bytes_per_frame;

    // override to default values
    q->num_channels = 16;
    q->samples_per_frame = 512;
    q->bytes_per_header = q->num_channels;
    q->bytes_per_frame = q->samples_per_frame + q->bytes_per_header;

    // initialize derived values/lengths
    q->samples_per_channel = (q->samples_per_frame) / (q->num_channels);

    // create polyphase filterbank channelizers
    //    num_channels      : 16 = 2^4
    //    filter length     : 20
    //    filter bandwidth  : 0.3
    q->channelizer = itqmfb_rrrf_create(4, 20, 0.3f, q->channelizer_type);

    // analysis/synthesis
    q->X = (float*) malloc( (q->samples_per_frame)*sizeof(float) );
    q->channel_energy = (float*) malloc( (q->num_channels)*sizeof(float) );
    q->mu = 255.0f;

    return q;
}

void fbasc_destroy(fbasc _q)
{
    // destroy channelizer
    itqmfb_rrrf_destroy(_q->channelizer);

    // free common buffers
    free(_q->X);

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
    printf("    bytes/frame:    %u\n", _q->bytes_per_frame);
}

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame)
{
    unsigned int i, j;

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    unsigned int b;     // block counter
    unsigned int n=0;   // input sample counter
    for (b=0; b<_q->samples_per_channel; b++) {
        // channelize time series
        itqmfb_rrrf_execute(_q->channelizer, _audio + n, _q->X + n);

        // compute energy on each channel
        for (i=0; i<_q->num_channels; i++)
            _q->channel_energy[i] += (_q->X[n+i])*(_q->X[n+i]);

        n += _q->num_channels;
    }

    // normalize channel energy
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = _q->channel_energy[i] / _q->samples_per_channel;

    // find maximum
    unsigned int i_max=0;
    float e_max=0.0f;
    for (i=0; i<_q->num_channels; i++) {
        if (i==0 || _q->channel_energy[i] > e_max) {
            i_max = i;
            e_max = _q->channel_energy[i];
        }
    }

#if FBASC_DEBUG
    printf("channel energy:\n");
    for (i=0; i<_q->num_channels; i++)
        printf("  e[%3u] = %12.8f\n", i, _q->channel_energy[i]);
    //    DEBUG_PRINTF_FLOAT(DEBUG_FILE,"e",i, _q->channel_energy[i]);
    printf("max channel energy: %12.8f\n", e_max);
#endif

    // compute bit partitioning
    unsigned int k[_q->num_channels];
    fbasc_compute_bit_allocation(_q->num_channels,
                                 _q->channel_energy,
                                 16,
                                 8,
                                 k);

    // write partition to header
    unsigned int s=0;
    for (i=0; i<_q->bytes_per_header; i++) {
        _frame[s] = k[i];
        s++;
    }

    // compute scaling factor
    float g[_q->num_channels];
    for (i=0; i<_q->num_channels; i++) {
        g[i] = 1.0f;
        printf("g[%3u] = %12.8f\n", i, g[i]);
    }

    // encode using basic quantizer
    float sample, z;
    unsigned int bi, bq;
    for (i=0; i<_q->samples_per_channel; i++) {
        for (j=0; j<_q->num_channels; j++) {

            if (k[j] > 1) {
                // compress using mu-law encoder
                // TODO: ensure proper scaling
                sample = _q->X[i*(_q->num_channels)+j] * g[j];
                if (fabsf(sample) > 1)
                    printf("sample = %12.8f, k[j] = %3u\n", sample, k[j]);
#if FBASC_COMPRESS
                z = compress_mulaw(sample, _q->mu);
#else
                z = sample;
#endif
                // quantize
                bi = quantize_adc(z, k[j]);
            } else {
                bi = 0;
            }

            _frame[s] = bi;
            s++;
        }
    }
}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{
    unsigned int i, j;

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    // get bit partitioning
    unsigned int k[_q->num_channels];
    unsigned int s=0;
    for (i=0; i<_q->num_channels; i++) {
        k[s] = _frame[i];
        s++;
        printf("rx b[%3u] = %3u\n", i, _frame[i]);
    }

    // compute scaling factor
    float g[_q->num_channels];
    for (i=0; i<_q->num_channels; i++)
        g[i] = 1.0f;

    // decode using basic quantizer
    float sample, z;
    unsigned int bi, bq;
    for (i=0; i<_q->samples_per_channel; i++) {
        for (j=0; j<_q->num_channels; j++) {
            if (k[j] > 1) {
                // quantize
                bi = _frame[s];

                z = quantize_dac(bi,k[j]);
            } else {
                z = 0.0f;
            }

            s++;
#if FBASC_COMPRESS
            // expand using mu-law decoder
            sample = expand_mulaw(z, _q->mu) / g[j];
#else
            sample = z;
#endif
            _q->X[i*(_q->num_channels)+j] = sample;
        }
    }

    unsigned int b;     // block counter
    unsigned int n=0;   // output sample counter
    for (b=0; b<_q->samples_per_channel; b++) {
        // compute energy
        for (i=0; i<_q->num_channels; i++)
            _q->channel_energy[i] += (_q->X[n+i]) * (_q->X[n+i]);

        // run synthesizer
        itqmfb_rrrf_execute(_q->channelizer, _q->X + n, _audio + n);

        n += _q->num_channels;
    }

    // normalize channel energy
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = _q->channel_energy[i] / _q->samples_per_channel;
#ifdef DEBUG
    printf("decoder: channel energy:\n");
    for (i=0; i<_q->num_channels; i++)
        DEBUG_PRINTF_FLOAT(DEBUG_FILE,"e",i, _q->channel_energy[i]);
#endif

}


// internal

void fbasc_encode_run_analyzer(fbasc _q)
{

}

void fbasc_compute_bit_allocation(unsigned int _n,
                                  float * _e,
                                  unsigned int _num_bits,
                                  unsigned int _max_bits,
                                  unsigned int * _k)
{
    float e[_n];            // sorted energy variances
    unsigned int idx[_n];   // sorted indices

    unsigned int i, j;
    for (i=0; i<_n; i++) {
        e[i] = _e[i];
        idx[i] = i;
    }

    // sort (inefficient, but easy to implement)
    float e_tmp;
    unsigned int i_tmp;
    for (i=0; i<_n; i++) {
        for (j=0; j<_n; j++) {
            if ( (i!=j) && (e[i] > e[j]) ) {
                // swap values
                e_tmp = e[i];
                e[i] = e[j];
                e[j] = e_tmp;

                i_tmp = idx[i];
                idx[i] = idx[j];
                idx[j] = i_tmp;
            }
        }
    }

#if 0
    printf("original:\n");
    for (i=0; i<_n; i++)
        printf("e[%3u] = %12.8f\n",i,_e[i]);
    printf("\n\n");

    printf("sorted:\n");
    for (i=0; i<_n; i++)
        printf("e[%3u] = %12.8f\n", idx[i], e[i]);
    printf("\n\n");
#endif

    // compute bit partitions
    float log2p;
    int bk;
    unsigned int n=_n;
    unsigned int num_bits = _num_bits;
    float b;
    for (i=0; i<_n; i++) {
        log2p = 0.0f;
        b = (float)(num_bits) / (float)(n);
        // compute 'entropy' metric
        for (j=i; j<_n; j++)
            log2p += (e[j] == 0.0f) ? -60.0f : log2(e[j]);
        log2p /= n;

        bk = (int)roundf(b + 0.5f*e[i] - 0.5f*log2p);

        bk = (bk > _max_bits)    ? _max_bits    : bk;
        bk = (bk > num_bits)     ? num_bits     : bk;
        bk = (bk < 0)            ? 0            : bk;

#if FBASC_DEBUG
        printf("e[%3u] = %12.8f, b = %8.4f, log2p = %12.8f, bk = %3d\n",
               idx[i], e[i], b, log2p, bk);
#endif
        _k[idx[i]] = bk;

        num_bits -= bk;
        n--;
    }

}

