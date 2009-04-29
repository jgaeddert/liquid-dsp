//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

//#define DEBUG

struct ofdmoqam_s {
    unsigned int num_channels;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer

    float complex * x0;
    float complex * x1;
    
    firpfbch c0;
    firpfbch c1;

    unsigned int type;  // synthesis/analysis
};

ofdmoqam ofdmoqam_create(unsigned int _num_channels, unsigned int _m, int _type)
{
    ofdmoqam c = (ofdmoqam) malloc(sizeof(struct ofdmoqam_s));
    c->num_channels = _num_channels;

    // allocate memory for buffers
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // create filterbank channelizers
    c->type = _type;
    c->c0 = firpfbch_create(_num_channels, 60.0f, FIRPFBCH_ROOTNYQUIST, _type);
    c->c1 = firpfbch_create(_num_channels, 60.0f, FIRPFBCH_ROOTNYQUIST, _type);

    return c;
}

void ofdmoqam_destroy(ofdmoqam _c)
{
    free(_c->x);
    free(_c->X);
    free(_c->c0);
    free(_c->c1);
    free(_c);
}

void ofdmoqam_print(ofdmoqam _c)
{
    printf("ofdmoqam: [%u taps]\n", 0);
}


void ofdmoqam_synthesizer_execute(ofdmoqam _c, float complex * _x, float complex * _X)
{
}

void ofdmoqam_analyzer_execute(ofdmoqam _c, float complex * _X, float complex * _x)
{
}

void ofdmoqam_execute(ofdmoqam _c, float complex * _x, float complex * _y)
{
    if (_c->type == OFDMOQAM_ANALYZER)
        ofdmoqam_analyzer_execute(_c,_x,_y);
    else
        ofdmoqam_synthesizer_execute(_c,_x,_y);
}

