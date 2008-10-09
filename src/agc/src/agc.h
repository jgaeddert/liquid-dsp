// 
// Automatic gain control
//

#ifndef __LIQUID_AGC_H__
#define __LIQUID_AGC_H__

#include <complex.h>

typedef struct agc_s * agc;

agc agc_create(float _etarget, float _BT);
void agc_free(agc _agc);

// Initialize AGC object
void agc_init(agc _agc);

// Set target energy
void agc_set_target(agc _agc, float _e_target);

// Set loop filter bandwidth; attack/release time
void agc_set_bandwidth(agc _agc, float _BT);

// Apply gain to input, update tracking loop
void agc_apply_gain(
    agc _agc,
    float complex _x,
    float complex *_y);

// Return signal level in dB relative to target
float agc_get_signal_level(agc _agc);

// Return gain in dB relative to target energy
float agc_get_gain(agc _agc);

#endif // __LIQUID_AGC_H__


