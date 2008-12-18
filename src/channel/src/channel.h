//
//
//

#ifndef __LIQUID_CHANNEL_H__
#define __LIQUID_CHANNEL_H__

#include <complex.h>
#include "../../filter/src/filter.h"

typedef struct awgn_channel_s * awgn_channel;
typedef struct ricek_channel_s * ricek_channel;
typedef struct lognorm_channel_s * lognorm_channel;

typedef struct channel_s * channel;
struct channel_s {
    float K;        // Rice-K fading
    //float omega;    // Mean power (omega=1)
    float fd;       // Maximum doppler frequency
    float theta;    // Line-of-sight angle of arrival
    float std;      // log-normal shadowing std. dev.
    float n0;       // AWGN std. dev.

    cfir_filter f_ricek;     // doppler filter (Rice-K fading)
    fir_filter f_lognorm;   // doppler filter (Log-normal shadowing)

    // internal
    float s, sig;
};

channel channel_create();
void channel_destroy(channel _c);
void channel_print(channel _c);

void channel_execute(channel _c, float complex _x, float complex *_y);

#endif // __LIQUID_CHANNEL_H__

