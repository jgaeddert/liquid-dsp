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
    float complex * x0; // time-domain buffer
    float complex * x1; // time-domain buffer

    float complex * X0; // freq-domain buffer
    float complex * X1; // freq-domain buffer

    float complex * x_prime;
    
    firpfbch c0;
    firpfbch c1;

    unsigned int type;  // synthesis/analysis
};

ofdmoqam ofdmoqam_create(unsigned int _num_channels, unsigned int _m, int _type)
{
    ofdmoqam c = (ofdmoqam) malloc(sizeof(struct ofdmoqam_s));
    c->num_channels = _num_channels;

    // allocate memory for time-domain buffers
    c->x0 = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->x1 = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // allocate memory for freq-domain buffers
    c->X0 = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X1 = (float complex*) malloc((c->num_channels)*sizeof(float complex));

    // allocate memory for time-delay buffer
    c->x_prime = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    unsigned int i;
    for (i=0; i<c->num_channels; i++)
        c->x_prime[i] = 0;

    // create filterbank channelizers
    c->type = _type;
    // TODO: use actual prototype (get rid of _slsl input)
    c->c0 = firpfbch_create(_num_channels, 60.0f, FIRPFBCH_ROOTNYQUIST, _type);
    c->c1 = firpfbch_create(_num_channels, 60.0f, FIRPFBCH_ROOTNYQUIST, _type);

    return c;
}

void ofdmoqam_destroy(ofdmoqam _c)
{
    free(_c->c0);
    free(_c->x0);
    free(_c->X0);

    free(_c->c1);
    free(_c->x1);
    free(_c->X1);

    free(_c);
}

void ofdmoqam_print(ofdmoqam _c)
{
    printf("ofdmoqam: [%u taps]\n", 0);
}


void ofdmoqam_synthesizer_execute(ofdmoqam _c, float complex * _X, float complex * _x)
{
    unsigned int i;

    // prepare signal
    for (i=0; i<_c->num_channels; i+=2) {
        // even channels
        _c->X0[i]   = cimagf(_X[i])*_Complex_I;
        _c->X1[i]   = crealf(_X[i]);

        // odd channels
        _c->X0[i+1] = crealf(_X[i+1]);
        _c->X1[i+1] = cimagf(_X[i+1])*_Complex_I;
    }

    // execute synthesis filter banks
    firpfbch_execute(_c->c0, _c->X0, _c->x0);
    firpfbch_execute(_c->c1, _c->X1, _c->x1);

    // delay the upper branch
    memmove(_c->x_prime + _c->num_channels/2, _c->x0, (_c->num_channels/2)*sizeof(float complex));

    for (i=0; i<_c->num_channels; i++)
        _x[i] = _c->x_prime[i] + _c->x1[i];

    // finish delay operation
    memmove(_c->x_prime, _c->x0 + _c->num_channels/2, (_c->num_channels/2)*sizeof(float complex));
}

void ofdmoqam_analyzer_execute(ofdmoqam _c, float complex * _x, float complex * _X)
{
    unsigned int i;
    memmove(_c->x0, _x, (_c->num_channels)*sizeof(float complex));

    // delay the lower branch
    memmove(_c->x_prime + _c->num_channels/2, _x, (_c->num_channels/2)*sizeof(float complex));

    // copy delayed lower branch partition
    memmove(_c->x1, _c->x_prime, (_c->num_channels)*sizeof(float complex));

    // finish delay operation
    memmove(_c->x_prime, _x + _c->num_channels/2, (_c->num_channels/2)*sizeof(float complex));

    // execute analysis filter banks
    firpfbch_execute(_c->c0, _c->x0, _c->X0);
    firpfbch_execute(_c->c1, _c->x1, _c->X1);

    for (i=0; i<_c->num_channels; i+=2) {
        _X[i]   = crealf(_c->X0[i])
                + cimagf(_c->X1[i])*_Complex_I;

        _X[i+1] = cimagf(_c->X0[i+1])*_Complex_I
                + crealf(_c->X1[i+1]);
    }
}

void ofdmoqam_execute(ofdmoqam _c, float complex * _x, float complex * _y)
{
    if (_c->type == OFDMOQAM_ANALYZER)
        ofdmoqam_analyzer_execute(_c,_x,_y);
    else
        ofdmoqam_synthesizer_execute(_c,_x,_y);
}

