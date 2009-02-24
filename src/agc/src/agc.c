//
// Automatic gain control
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "agc_internal.h"

agc agc_create(float _etarget, float _BT)
{
    agc _agc = (agc) malloc(sizeof(struct agc_s));
    agc_init(_agc);
    agc_set_target(_agc, _etarget);
    agc_set_bandwidth(_agc, _BT);

    // normalized windowing function
    float w[11] = {
       0.014792,
       0.042634,
       0.081587,
       0.122933,
       0.154722,
       0.166667,
       0.154722,
       0.122933,
       0.081587,
       0.042634,
       0.014792
    };

    unsigned int i;
    //for (i=0; i<11; i++)
    //    printf("w(%4u) = %8.4f;\n", i+1, w[i]);

    _agc->f = fir_filter_rrrf_create(w,11);

    for (i=0; i<11; i++)
        fir_filter_rrrf_push(_agc->f, 0.0f);

    return _agc;
}

void agc_destroy(agc _agc)
{
    fir_filter_rrrf_destroy(_agc->f);
    free(_agc);
}

void agc_print(agc _agc)
{
    printf("agc [rssi: %12.4fdB]:\n", 10*log10(_agc->e_target / _agc->g));
}

void agc_init(agc _agc)
{
    //_agc->e = 1.0f;
    _agc->e_target = 1.0f;

    // set gain variables
    _agc->g = 1.0f;
    _agc->g_min = 1e-6f;
    _agc->g_max = 1e+6f;

    // prototype loop filter
    agc_set_bandwidth(_agc, 0.01f);

    // initialize loop filter state variables
    _agc->e_prime = 1.0f;
    _agc->e_hat = 1.0f;
    _agc->tmp2 = 1.0f;
}

void agc_set_target(agc _agc, float _e_target)
{
    // check to ensure _e_target is reasonable

    _agc->e_target = _e_target;

    ///\todo auto-adjust gain to compensate?
}

void agc_set_gain_limits(agc _agc, float _g_min, float _g_max)
{
    if (_g_min > _g_max) {
        printf("error: agc_set_gain_limits(), _g_min < _g_max\n");
        exit(0);
    }

    _agc->g_min = _g_min;
    _agc->g_max = _g_max;
}

void agc_set_bandwidth(agc _agc, float _BT)
{
    // check to ensure _BT is reasonable
    if ( _BT <= 0 ) {
        perror("\n");
        exit(-1);
    } else if ( _BT > 0.5f ) {
        perror("\n");
        exit(-1);
    }

    _agc->BT = _BT;
    _agc->alpha = sqrtf(_agc->BT);
    _agc->beta = 1 - _agc->alpha;
}

void agc_execute(agc _agc, float complex _x, float complex *_y)
{
    // estimate normalized energy, should be equal to 1.0 when locked
    float e2 = crealf(_x * conj(_x)); // NOTE: crealf used for roundoff error
    fir_filter_rrrf_push(_agc->f, e2);
    float e_hat;
    fir_filter_rrrf_execute(_agc->f, &e_hat);
    e_hat = sqrtf(e_hat);// * (_agc->g) / (_agc->e_target);

    // ideal gain
    float g = _agc->e_target / e_hat;

    // accumulated gain
    _agc->g = (_agc->beta)*(_agc->g) + (_agc->alpha)*g;
    //_agc->g = g;

    // limit gain
    if ( _agc->g > _agc->g_max )
        _agc->g = _agc->g_max;
    else if ( _agc->g < _agc->g_min )
        _agc->g = _agc->g_min;

    // apply gain to input
    *_y = _x * _agc->g;
}

float agc_get_signal_level(agc _agc)
{
    return (_agc->e_target / _agc->g);
}

float agc_get_gain(agc _agc)
{
    return _agc->g;
}

