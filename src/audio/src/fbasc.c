//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.internal.h"

#define DEBUG
#define DEBUG_FILE stdout

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
    int type;
    unsigned int num_channels;      // 32
                                    // number of real samples per frame (needs
                                    // to be even number and even multiple of
                                    // num_channels)
    unsigned int samples_per_frame;     // 1024
    unsigned int bytes_per_frame;

    // derived values
    unsigned int cplx_samples_per_frame;// 512
    unsigned int samples_per_channel;   // 32
    unsigned int cplx_samples_per_channel;   // complex samples/ch.
    unsigned int blocks_per_frame;      // 512/32 = 16

    // common objects
    firhilb hilbert_transform;
    int channelizer_type;
    firpfbch channelizer;
    float complex * X; // channelized matrix (length: cplx_samples_per_frame)
    unsigned char * data;   // 

    // analysis
    float complex * x;  // time-domain (length: num_channels)
    float complex * y;  // freq-domain (length: num_channels)
    float complex * ht_decim_out;
    float * channel_energy;

    // synthesis

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
    switch (q->type) {
    case FBASC_ENCODER:
        q->channelizer_type = FIRPFBCH_ANALYZER;
        break;
    case FBASC_DECODER:
        q->channelizer_type = FIRPFBCH_SYNTHESIZER;
        break;
    default:
        printf("error: fbasc_create(), unknown type: %d\n", _type);
        exit(1);
    }

    // initialize parametric values/lengths
    q->num_channels = _num_channels;
    q->samples_per_frame = _samples_per_frame;
    q->bytes_per_frame = _bytes_per_frame;

    // initialize derived values/lengths
    q->samples_per_channel = (q->samples_per_frame) / (q->num_channels);
    q->cplx_samples_per_channel = (q->samples_per_channel) / 2;
    q->cplx_samples_per_frame = (q->samples_per_frame)/2;
    q->blocks_per_frame = (q->cplx_samples_per_frame) / (q->num_channels);

    // create Hilbert transform
    q->hilbert_transform = firhilb_create(37);

    // create polyphase filterbank channelizers
    q->channelizer = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, q->channelizer_type);

    // analysis
    q->X = (float complex*) malloc( (q->samples_per_frame)*sizeof(float complex) );
    q->x = (float complex*) malloc( (q->num_channels)*sizeof(float complex) );
    q->y = (float complex*) malloc( (q->num_channels)*sizeof(float complex) );
    q->ht_decim_out = (float complex*) malloc( (q->cplx_samples_per_frame)*sizeof(float complex) );
    q->channel_energy = (float*) malloc( (q->num_channels)*sizeof(float) );

    return q;
}

void fbasc_destroy(fbasc _q)
{
    // destroy Hilbert transforms
    firhilb_destroy(_q->hilbert_transform);

    // destroy polyphase filterbank channelizers
    firpfbch_destroy(_q->channelizer);

    // free common buffers
    free(_q->X);

    // analysis
    free(_q->x);
    free(_q->y);
    free(_q->ht_decim_out);
    free(_q->channel_energy);

    // synthesis

    // free memory structure
    free(_q);
}

void fbasc_print(fbasc _q)
{
    printf("filterbank audio synthesizer codec: [%u channels]\n", _q->num_channels);
}

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame)
{
    unsigned int i;
#ifdef DEBUG
    for (i=0; i<10; i++)
        DEBUG_PRINTF_FLOAT(DEBUG_FILE,"x",i,_audio[i]);
#endif

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    unsigned int b;     // block counter
    unsigned int n=0;   // input sample counter
    for (b=0; b<_q->blocks_per_frame; b++) {
        // 1. push samples through Hilbert transform (decimator)
        for (i=0; i<_q->num_channels; i++) {
            firhilb_decim_execute(_q->hilbert_transform, &_audio[n], &(_q->x[i]));
            n+=2;
        }

        // 2. channelize complex input
        firpfbch_execute(_q->channelizer, _q->x, _q->y);

        // scale by number of channels
        for (i=0; i<_q->num_channels; i++)
            _q->y[i] /= (float)(_q->num_channels);

        // 3. compute energy on each channel
        for (i=0; i<_q->num_channels; i++)
            _q->channel_energy[i] += crealf( _q->y[i] * conj(_q->y[i]) );

        // 4. move channelized data to matrix
        memmove(&_q->X[b*(_q->num_channels)], _q->y, (_q->num_channels)*sizeof(float complex));
    }

    // normalize channel energy
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = sqrtf(_q->channel_energy[i] / _q->cplx_samples_per_channel);
#ifdef DEBUG
    printf("channel energy:\n");
    for (i=0; i<_q->num_channels; i++)
        DEBUG_PRINTF_FLOAT(DEBUG_FILE,"e",i, _q->channel_energy[i]);
#endif

    // encode using basic quantizer (4 bits for in-phase, quadrature)
    float complex z;
    unsigned int bi, bq;
    for (i=0; i<_q->cplx_samples_per_frame; i++) {
        // compress using mu-law encoder
        compress_cf_mulaw(_q->X[i], 255.0f, &z);

        // quantize
        bi = quantize_adc(crealf(z), 4);
        bq = quantize_adc(cimagf(z), 4);

        _frame[i] = ((bi & 0x0f) << 4) | (bq & 0x0f);
    }
}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{
    unsigned int i;

    // clear energy...
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;

    // decode using basic quantizer (4 bits for in-phase, quadrature)
    float complex z;
    unsigned int bi, bq;
    for (i=0; i<_q->cplx_samples_per_frame; i++) {
        // quantize
        bi = (_frame[i] >> 4) & 0x0f;
        bq = (_frame[i]     ) & 0x0f;

        z = quantize_dac(bi,4) + _Complex_I*quantize_dac(bq,4);

        expand_cf_mulaw(z, 255.0f, &_q->X[i]);
    }

    unsigned int b;     // block counter
    unsigned int n=0;   // output sample counter
    for (b=0; b<_q->blocks_per_frame; b++) {
        // move channelized data input freq-domain buffer
        memmove(_q->y, &_q->X[b*(_q->num_channels)], (_q->num_channels)*sizeof(float complex));

        // compute energy
        for (i=0; i<_q->num_channels; i++)
            _q->channel_energy[i] += crealf( _q->y[i] * conj(_q->y[i]) );

        // run reverse channelizer
        firpfbch_execute(_q->channelizer, _q->y, _q->x);

        // run through Hilbert transform (interpolator)
        for (i=0; i<_q->num_channels; i++) {
            firhilb_interp_execute(_q->hilbert_transform, _q->x[i], &_audio[n]);
            n+=2;
        }
    }

    // normalize channel energy
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = sqrtf(_q->channel_energy[i] / _q->cplx_samples_per_channel);
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

