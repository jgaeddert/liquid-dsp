//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.internal.h"

#define FBASC_DEBUG 0
#define DEBUG_FILE  stdout

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
    } else if (q->type = FBASC_DECODER) {
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
    q->bytes_per_frame = 512;

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
    unsigned int i;
#if FBASC_DEBUG
    for (i=0; i<10; i++)
        DEBUG_PRINTF_FLOAT(DEBUG_FILE,"x",i,_audio[i]);
#endif

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
        _q->channel_energy[i] = sqrtf(_q->channel_energy[i] / _q->samples_per_channel);
#if FBASC_DEBUG
    printf("channel energy:\n");
    for (i=0; i<_q->num_channels; i++)
        DEBUG_PRINTF_FLOAT(DEBUG_FILE,"e",i, _q->channel_energy[i]);
#endif

    // encode using basic quantizer
    float z;
    unsigned int bi, bq;
    for (i=0; i<_q->samples_per_frame; i++) {
        // compress using mu-law encoder
        // TODO: ensure proper scaling
        z = compress_mulaw(_q->X[i], _q->mu);

        // quantize
        bi = quantize_adc(z, 8);

        _frame[i] = bi;
    }
}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{
    unsigned int i;

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    // decode using basic quantizer
    float z;
    unsigned int bi, bq;
    for (i=0; i<_q->samples_per_frame; i++) {
        // quantize
        bi = _frame[i];

        z = quantize_dac(bi,8);

        // expand using mu-law decoder
        _q->X[i] = expand_mulaw(z, _q->mu);
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
        _q->channel_energy[i] = sqrtf(_q->channel_energy[i] / _q->samples_per_channel);
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

