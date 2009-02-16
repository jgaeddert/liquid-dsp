//
// Automatic gain control
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "agc_internal.h"

#define AGC_LOG

#define AGC_RECURSIVE   0
#define AGC_COMPARATIVE 1

#define AGC_TYPE        AGC_RECURSIVE

agc agc_create(float _etarget, float _BT)
{
    agc _agc = (agc) malloc(sizeof(struct agc_s));
    agc_init(_agc);
    agc_set_target(_agc, _etarget);
    agc_set_bandwidth(_agc, _BT);
    return _agc;
}

void agc_destroy(agc _agc)
{
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
    float theta = _agc->BT * M_PI * 0.5f;

    // calculate coefficients from first-order butterworth
    // prototype using bilinear z-transform
    float sin_theta = sinf(theta);
    float cos_theta = cosf(theta);
    _agc->beta = sin_theta / (sin_theta + cos_theta);
    _agc->alpha = (sin_theta - cos_theta)/(sin_theta + cos_theta);

    // reduce feedback parameter by emperical value to prevent ringing
#ifdef AGC_LOG
    _agc->alpha *= 1-expf( logf(_agc->BT)*0.5f + 0.95f );
#else
    _agc->alpha *= 1-expf( logf(_agc->BT)*0.5f + 0.70f );
#endif
}

void agc_execute(agc _agc, float complex _x, float complex *_y)
{
    // estimate normalized energy, should be equal to 1.0 when locked
    float e = cabsf(_x) * (_agc->g) / (_agc->e_target);
    if ( e <= 0.0f ) {
        printf("warning! agc_apply_gain(), input level not valid!\n");
        *_y = _x * _agc->g;
        return;
    }

#if AGC_TYPE == AGC_RECURSIVE
    // generate error signal
#  ifdef AGC_LOG
    _agc->e = logf( e );
#  else
    _agc->e = e - 1;
#  endif

    // filter estimate using first-order loop filter
    _agc->e_prime = _agc->e - _agc->alpha * _agc->tmp2;
    _agc->e_hat = (_agc->e_prime + _agc->tmp2) * _agc->beta;
    _agc->tmp2 = _agc->e_prime;

    // compute new gain value
    _agc->g *= expf( -_agc->e_hat );

#elif AGC_TYPE == AGC_COMPARATIVE
    // generate error signal
    float e_hat = (_agc->g)*cabsf(_x);
    if ( e_hat > _agc->e_target ) {
        // attack
        _agc->g *= 1 - 0.1f*( (e_hat-_agc->e_target) / _agc->e_target );
        //_agc->g *= 0.9f;
    } else {
        // release
        _agc->g *= 1 + 0.1f*( (_agc->e_target-e_hat) / e_hat);
        //_agc->g *= 1.1f;
    }
#else
#  error "invalid AGC_TYPE macro"
#endif

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

