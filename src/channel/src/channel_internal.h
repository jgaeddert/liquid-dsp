//
//
//

#ifndef __LIQUID_CHANNEL_INTERNAL_H__
#define __LIQUID_CHANNEL_INTERNAL_H__

#include "liquid.internal.h"

struct awgn_channel_s {
    float nvar;
    float nstd;
};

struct ricek_channel_s {
    unsigned int h_len;
    float K;
    float omega;
    float sig;
    float s;
    float fd;
    float theta;
    firfilt_cccf f;
};

struct channel_s {
    float K;        // Rice-K fading
    //float omega;    // Mean power (omega=1)
    float fd;       // Maximum doppler frequency
    float theta;    // Line-of-sight angle of arrival
    float std;      // log-normal shadowing std. dev.
    float n0;       // AWGN std. dev.

    firfilt_cccf f_ricek;     // doppler filter (Rice-K fading)
    firfilt_rrrf f_lognorm;   // doppler filter (Log-normal shadowing)

    // internal
    float s, sig;
};

#endif // __LIQUID_CHANNEL_INTERNAL_H__

