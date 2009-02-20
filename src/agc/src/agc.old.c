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

    unsigned int h_len = 11;
    float h[h_len];

    unsigned int i;
    float sum=0.0f;
    for (i=0; i<h_len; i++) {
        h[i] = kaiser(i,h_len,5.0f);
        sum += h[i];
    }

    //for (i=0; i<h_len; i++)
    //    h[i] /= sum;

    for (i=0; i<h_len; i++)
        printf("agc:h(%4u) = %8.4f;\n", i+1, h[i]);

    _agc->f = fir_filter_rrrf_create(h,h_len);

    for (i=0; i<h_len; i++)
        fir_filter_rrrf_push(_agc->f, 1.0f);

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
    fir_filter_rrrf_push(_agc->f, e);
    float e_hat;
    fir_filter_rrrf_execute(_agc->f, &e_hat);
    //e_hat *= 10;

#if 0
    // generate error signal
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
    // ideal gain
    float g = _agc->e_target / e_hat;

    _agc->g = 0.9f*(_agc->g) + 0.1f*g;
#endif

    // limit gain
    if ( _agc->g > _agc->g_max )
        _agc->g = _agc->g_max;
    else if ( _agc->g < _agc->g_min )
        _agc->g = _agc->g_min;

    // apply gain to input
    *_y = _x * _agc->g;
}

#if 0
void xagc_execute(agc _agc, float complex _x, float complex *_y)
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
#endif

float agc_get_signal_level(agc _agc)
{
    return (_agc->e_target / _agc->g);
}

float agc_get_gain(agc _agc)
{
    return _agc->g;
}

