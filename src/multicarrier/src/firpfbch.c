//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

//#define DEBUG

#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_crcf,name)

struct firpfbch_s {
    unsigned int num_channels;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer
    
    FIR_FILTER() * bank;
    fftplan fft;
    unsigned int type;  // synthesis/analysis
};

firpfbch firpfbch_create(unsigned int _num_channels, float _slsl, int _nyquist, int _type)
{
    firpfbch c = (firpfbch) malloc(sizeof(struct firpfbch_s));
    c->num_channels = _num_channels;

    // create bank of filters
    c->bank = (FIR_FILTER()*) malloc((c->num_channels)*sizeof(FIR_FILTER()));

    // design filter
    unsigned int h_len;

    // design filter using kaiser window and be done with it
    // TODO: use filter prototype object
    h_len = (c->num_channels)*4;
    float h[h_len+1];
    float fc = 1/(float)(c->num_channels);  // cutoff frequency
    fir_kaiser_window(h_len+1, fc, 60, h);

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = h_len / c->num_channels;
    float h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<c->num_channels; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = h[i + n*(c->num_channels)];
        }   

        c->bank[i] = FIR_FILTER(_create)(h_sub, h_sub_len);
    }   

#ifdef DEBUG
    for (i=0; i<h_len+1; i++)
        printf("h(%4u) = %12.4e;\n", i+1, h[i]);
#endif

    // allocate memory for buffers
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // create fft plan
    c->type = _type;
    if (c->type == FIRPFBCH_ANALYZER) {
        c->fft = fft_create_plan(c->num_channels, c->X, c->x, FFT_FORWARD);
    } else if (c->type == FIRPFBCH_SYNTHESIZER) {
        c->fft = fft_create_plan(c->num_channels, c->x, c->X, FFT_REVERSE);
    } else {
        printf("error: firpfbch_create(), unknown channelizer type\n");
        exit(0);
    }

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++)
        fir_filter_crcf_destroy(_c->bank[i]);
    free(_c->bank);

    fft_destroy_plan(_c->fft);
    free(_c->x);
    free(_c->X);
    free(_c);
}

void firpfbch_print(firpfbch _c)
{
    printf("firpfbch: [%u taps]\n", 0);
}


void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _X)
{
    unsigned int i;

    // copy samples into time-domain buffer (_c->x)
    memmove(_c->x, _x, (_c->num_channels)*sizeof(float complex));

    // execute fft, store in freq-domain buffer (_c->X)
    fft_execute(_c->fft);

    // push samples into filter bank
    // execute filterbank, putting samples into output buffer
    for (i=0; i<_c->num_channels; i++) {
        fir_filter_crcf_push(_c->bank[i], _c->X[i]);
        fir_filter_crcf_execute(_c->bank[i], &(_X[i]));
    }
}

void firpfbch_analyzer_execute(firpfbch _c, float complex * _X, float complex * _x)
{
    unsigned int i, b;

    // push samples into filter bank
    // execute filterbank, putting samples into freq-domain buffer (_c->X) in
    // reverse order
    for (i=0; i<_c->num_channels; i++) {
        b = _c->num_channels-i-1;
        fir_filter_crcf_push(_c->bank[b], _X[i]);
        fir_filter_crcf_execute(_c->bank[b], &(_c->X[i]));
    }
    
    // execute inverse fft, store in time-domain buffer (_c->x)
    fft_execute(_c->fft);

    // copy results to output buffer
    memmove(_x, _c->x, (_c->num_channels)*sizeof(float complex));
}

void firpfbch_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    if (_c->type == FIRPFBCH_ANALYZER)
        firpfbch_analyzer_execute(_c,_x,_y);
    else
        firpfbch_synthesizer_execute(_c,_x,_y);
}

