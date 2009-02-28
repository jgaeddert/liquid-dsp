//
//
//

#include <stdlib.h>
#include <stdio.h>

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
    c->firpfb_crcf = firpfb_crcf_create(c->num_channels,h,h_len);

    // allocate memory for buffers
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // create fft plan
    // TODO: FIXME!
    float complex * fft_in=NULL;
    float complex * fft_out=NULL;
    if (_dir == FIRPFBCH_ANALYZER) {
        c->dir = FFT_REVERSE;
    } else if (_dir == FIRPFBCH_SYNTHESIZER) {
        c->dir = FFT_FORWARD;
    } else {
        printf("error: firpfbch_create(), unknown channelizer type\n");
        exit(0);
    }
    c->fft = fftplan_create(c->num_channels, fft_in, fft_out, c->dir);

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    free(_c->x);
    free(_c->X);
    free(_c);
}

void firpfbch_print(firpfbch _c)
{

}

void firpfbch_synthesizer_execute(float complex * _x, float complex * _X)
{

}

void firpfbch_analyzer_execute(float complex * _X, float complex * _x)
{

}



