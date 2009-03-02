//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct fbasc_s {
    unsigned int num_channels;
    unsigned int frame_length;

    firhilb ht_interp;
    firhilb ht_decim;

    firpfbch analyzer;
    firpfbch synthesizer;

    // analysis buffers
    float complex * x;
};

fbasc fbasc_create()
{
    fbasc q = (fbasc) malloc(sizeof(struct fbasc_s));

    // create Hilbert transforms
    q->ht_interp    = firhilb_create(37);
    q->ht_decim     = firhilb_create(37);

    // create polyphase filterbank channelizers
    q->num_channels = 64;
    q->analyzer     = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, FIRPFBCH_ANALYZER);
    q->synthesizer  = firpfbch_create(q->num_channels, 60.0f, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    // allocate memory for buffers
    q->frame_length = 64;
    q->x = (float complex*) malloc( (q->frame_length)*sizeof(float complex) );

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

    // free buffer
    free(_q->x);

    // free memory structure
    free(_q);
}

void fbasc_print(fbasc _q)
{
    printf("filterbank audio synthesizer codec: [%u channels]\n", _q->num_channels);
}

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame)
{

}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{

}


