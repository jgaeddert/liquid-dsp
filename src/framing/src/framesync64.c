//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"

struct framesync64_s {
    modem demod;

    // synchronizer objects
    // agc
    // symsync_crcf
    // pll
    // pnsync_crcf
};

framesync64 framesync64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta,
    framesync64_callback _callback)
{
    framesync64 fg = (framesync64) malloc(sizeof(struct framesync64_s));

    // design symsync (k=2)
    unsigned int npfb = 64;
    unsigned int H_len = 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,_m,_beta,0,H);
    // symsync_crcf_create(...)

    return fg;

}

void framesync64_destroy(framesync64 _fg)
{
    free(_fg);
}

void framesync64_print(framesync64 _fg)
{
    printf("framesync:\n");
}



