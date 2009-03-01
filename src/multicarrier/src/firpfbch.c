//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

struct firpfbch_s {
    unsigned int num_channels;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer
    
    firpfb_crcf bank;
    fftplan fft;
    unsigned int type;  // synthesis/analysis
    unsigned int ftype; // filter type (nyquist, etc.)
};

firpfbch firpfbch_create(unsigned int _num_channels, float _slsl, int _nyquist, int _dir)
{
    firpfbch c = (firpfbch) malloc(sizeof(struct firpfbch_s));
    c->num_channels = _num_channels;

    // design filter
    unsigned int h_len;
#if 0
    if (_nyquist==FIRPFBCH_NYQUIST) {
        c->type = FIRPFBCH_NYQUIST;
    } else if (_nyquist==FIRPFBCH_ROOTNYQUIST) {
        c->type = FIRPFBCH_ROOTNYQUIST;

        // use root-raised cosine for now
        unsigned int m=4;
        float beta=0.7f;
    } else {
        printf("error: firpfbch_create(), unknown filter type\n");
        exit(0);
    }
#else
    // design filter using kaiser window and be done with it
    // TODO: use filter prototype object
    h_len = (c->num_channels)*4;
    float h[h_len];
    float fc = 1/(float)(c->num_channels);
    fir_kaiser_window(h_len, fc, -60, h);
#endif

    // create firpfb object
    c->bank = firpfb_crcf_create(c->num_channels,h,h_len);

    // allocate memory for buffers
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // create fft plan
    // TODO: FIXME!
    if (_dir == FIRPFBCH_ANALYZER) {
        c->fft = fft_create_plan(c->num_channels, c->X, c->x, FFT_REVERSE);
    } else if (_dir == FIRPFBCH_SYNTHESIZER) {
        c->fft = fft_create_plan(c->num_channels, c->x, c->X, FFT_REVERSE);
    } else {
        printf("error: firpfbch_create(), unknown channelizer type\n");
        exit(0);
    }

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    firpfb_crcf_destroy(_c->bank);
    fft_destroy_plan(_c->fft);
    free(_c->x);
    free(_c->X);
    free(_c);
}

void firpfbch_print(firpfbch _c)
{
    printf("firpfbch: [%u taps]\n", 0);
}


void firpfbch_execute(firpfbch _c, float complex * _x, float complex * _y)
{

}

void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _X)
{
    unsigned int i;

    // copy samples into time-domain buffer (_c->x)
    memmove(_c->x, _x, (_c->num_channels)*sizeof(float complex));

    // execute fft, store in freq-domain buffer (_c->X)
    fft_execute(_c->fft);

    // push samples into filter bank
    for (i=0; i<_c->num_channels; i++)
        firpfb_crcf_push(_c->bank, _c->X[i]);

    // execute filterbank, putting samples into output buffer
    for (i=0; i<_c->num_channels; i++)
        firpfb_crcf_execute(_c->bank, i, &(_X[i]));
}

void firpfbch_analyzer_execute(firpfbch _c, float complex * _X, float complex * _x)
{
    unsigned int i;

    // push samples into filter bank
    for (i=0; i<_c->num_channels; i++)
        firpfb_crcf_push(_c->bank, _X[i]);

    // execute filterbank, putting samples into freq-domain buffer (_c->X) in
    // reverse order
    for (i=0; i<_c->num_channels; i++)
        firpfb_crcf_execute(_c->bank, _c->num_channels-i-1, &(_c->X[i]));
    
    // execute inverse fft, store in time-domain buffer (_c->x)
    fft_execute(_c->fft);

    // copy results to output buffer
    memmove(_x, _c->x, (_c->num_channels)*sizeof(float complex));
}


