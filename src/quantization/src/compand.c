//
//
//

#include <math.h>

#include "liquid.internal.h"

#define LIQUID_VALIDATE_INPUT

float compress_mulaw(float _x, float _mu)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (fabsf(_x) > 1.0f) {
        printf("error: compress_mulaw(), input out of range\n");
        exit(1);
    } else if ( _mu <= 0.0f || _mu >= 1.0f ) {
        printf("error: compress_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    float y = logf(1 + _mu*logf(fabsf(_x))) / logf(1 + _mu);
    return copysignf(y, _x);
}

float expand_mulaw(float _y, float _mu)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (fabsf(_y) > 1.0f) {
        printf("error: expand_mulaw(), input out of range\n");
        exit(1);
    } else if ( _mu <= 0.0f || _mu >= 1.0f ) {
        printf("error: expand_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    float x = (1/_mu)*( powf(1+_mu,fabsf(_y)) - 1);
    return copysign(x, _y);
}

/*
float compress_alaw(float _x, float _a)
{

}

float expand_alaw(float _x, float _a)
{

}
*/

