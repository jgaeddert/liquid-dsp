//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>

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
    unsigned int frame_len;             // 1024 real samples
    unsigned int ht_len;                // 512  complex samples
    unsigned int samples_per_channel;   // 32

    firhilb ht_interp;
    firhilb ht_decim;

    firpfbch analyzer;
    firpfbch synthesizer;

    // analysis buffers
    float complex * x;
    float complex * ht_decim_out;
    float * energy;
};

fbasc fbasc_create()
{
    fbasc q = (fbasc) malloc(sizeof(struct fbasc_s));

    // create Hilbert transforms
    q->ht_interp    = firhilb_create(37);
    q->ht_decim     = firhilb_create(37);

    // create polyphase filterbank channelizers
    q->num_channels = 32;
    q->analyzer     = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, FIRPFBCH_ANALYZER);
    q->synthesizer  = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    // compute lengths
    q->frame_len = 1024;  // needs to be (even) integer multiple of q->num_channels
    q->ht_len = (q->frame_len)/2;
    q->samples_per_channel = (q->ht_len) / (q->num_channels);

    // allocate memory for buffers
    q->x = (float complex*) malloc( (q->frame_len)*sizeof(float complex) );
    q->ht_decim_out = (float complex*) malloc( (q->ht_len)*sizeof(float complex) );
    q->energy = (float*) malloc( (q->num_channels)*sizeof(float) );

    return q;
}

void fbasc_destroy(fbasc _q)
{
    // destroy Hilbert transforms
    firhilb_destroy(_q->ht_interp);
    firhilb_destroy(_q->ht_decim);

    // destroy polyphase filterbank channelizers
    firpfbch_destroy(_q->analyzer);
    firpfbch_destroy(_q->synthesizer);

    // free buffers
    free(_q->x);
    free(_q->ht_decim_out);
    free(_q->energy);

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

    // push samples through Hilbert transform (decimator)
    for (i=0; i<_q->ht_len; i++)
        firhilb_decim_execute(_q->ht_decim, &_audio[2*i], &(_q->ht_decim_out[i]));

    // push samples into filterbank channelizer
    for (i=0; i<_q->samples_per_channel; i+=_q->num_channels)
        firpfbch_execute(_q->analyzer, &_q->ht_decim_out[i], &_q->x[i]);

}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{

}


