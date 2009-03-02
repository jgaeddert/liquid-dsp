//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

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
    unsigned int num_channels;          // 32
    unsigned int samples_per_frame;     // 1024
    int type;

    // derived values
    unsigned int cplx_samples_per_frame;// 512
    unsigned int samples_per_channel;   // 32
    unsigned int cplx_samples_per_channel;   // complex samples/ch.

    // common objects
    firhilb hilbert_transform;
    int channelizer_type;
    firpfbch channelizer;
    float complex * X; // channelized matrix
    unsigned char * data;   // 

    // analysis
    float complex * x;
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
fbasc fbasc_create(int _type)
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
    q->num_channels = 32;           // num filters in bank

    q->samples_per_frame = 1024;    // number of real samples per frame (needs
                                    // to be even number and even multiple of
                                    // num_channels)

    // initialize derived values/lengths
    q->samples_per_channel = (q->samples_per_frame) / (q->num_channels);
    q->cplx_samples_per_channel = (q->samples_per_channel) / 2;

    // create Hilbert transform
    q->hilbert_transform = firhilb_create(37);

    // create polyphase filterbank channelizers
    q->channelizer = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    // analysis
    q->cplx_samples_per_frame = (q->samples_per_frame)/2;
    q->X = (float complex*) malloc( (q->samples_per_frame)*sizeof(float complex) );
    q->x = (float complex*) malloc( (q->samples_per_frame)*sizeof(float complex) );
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

    // 1. push samples through Hilbert transform (decimator)
    for (i=0; i<_q->cplx_samples_per_frame; i++)
        firhilb_decim_execute(_q->hilbert_transform, &_audio[2*i], &(_q->ht_decim_out[i]));

    // 2. channelize complex input
    for (i=0; i<_q->cplx_samples_per_frame; i+=_q->cplx_samples_per_channel)
        firpfbch_execute(_q->channelizer, &(_q->ht_decim_out[i]), &(_q->X[i]));

    // 3. compute energy on each channel
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = 0.0f;
    for (i=0; i<_q->cplx_samples_per_frame; i++)
        _q->channel_energy[i%(_q->num_channels)] += (_q->X[i]) * conj(_q->X[i]);
    for (i=0; i<_q->num_channels; i++)
        _q->channel_energy[i] = sqrtf(_q->channel_energy[i]);
}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{

}


// internal

void fbasc_encode_run_analyzer(fbasc _q)
{

}

