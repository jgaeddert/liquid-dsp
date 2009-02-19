//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"

struct framegen64_s {
    modem mod_preamble;
    modem mod;

    // preamble (BPSK)
    float ramp_up[32];
    float phasing[64];
    float pn_sequence[64];
    float ramp_dn[32];

    // header (QPSK)
    
    // payload (QPSK)

    // pulse-shaping filter
    interp_crcf interp;
};

framegen64 framegen64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta)
{
    framegen64 fg = (framegen64) malloc(sizeof(struct framegen64_s));

    // generate pn sequence
    msequence ms = msequence_create(6);
    unsigned int i;
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create pulse-shaping filter (k=2)
    unsigned int h_len = 2*2*_m + 1;
    float h[h_len];
    design_rrc_filter(2,_m,_beta,0,h);
    fg->interp = interp_crcf_create(2, h, h_len);

    return fg;
}

void framegen64_destroy(framegen64 _fg)
{
    interp_crcf_destroy(_fg->interp);
    free(_fg);
}

void framegen64_print(framegen64 _fg)
{
    printf("framegen:\n");
}

void framegen64_execute(framegen64 _fg, unsigned char * _payload, float complex * _y)
{
    //
}

void framegen64_flush(framegen64 _fg, unsigned int _n, float complex * _y)
{
    if (_n % 2) {
        printf("error: framegen64_flush(), _n must be even\n");
        exit(-1);
    }

    unsigned int i;
    for (i=0; i<_n; i+=2)
        interp_crcf_execute(_fg->interp, 0, &_y[i]);
}


