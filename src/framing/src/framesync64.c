//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define FRAMESYNC64_SYMSYNC_BW_0    (0.01f)
#define FRAMESYNC64_SYMSYNC_BW_1    (0.001f)

#define FRAMESYNC64_AGC_BW_0        (1e-3f)
#define FRAMESYNC64_AGC_BW_1        (1e-6f)

#define FRAMESYNC64_PLL_BW_0        (1e-2f)
#define FRAMESYNC64_PLL_BW_1        (1e-4f)

struct framesync64_s {
    modem demod;
    fec dec;

    // synchronizer objects
    agc agc_rx;
    symsync_crcf mfsync;
    pll pll_rx;
    nco nco_rx;
    pnsync_crcf fsync;

    framesync64_callback *callback;
};

framesync64 framesync64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta,
    framesync64_callback _callback)
{
    framesync64 fg = (framesync64) malloc(sizeof(struct framesync64_s));
    //fg->callback = _callback;

    //
    fg->agc_rx = agc_create(1.0f, FRAMESYNC64_AGC_BW_0);
    fg->pll_rx = pll_create();
    fg->nco_rx = nco_create();
    pll_set_bandwidth(fg->pll_rx, FRAMESYNC64_PLL_BW_0);

    // pnsync
    unsigned int i;
    msequence ms = msequence_create(6);
    float pn_sequence[64];
    for (i=0; i<64; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fg->fsync = pnsync_crcf_create(pn_sequence, 64);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 16;
    unsigned int H_len = 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,_m,_beta,0,H);
    fg->mfsync =  symsync_crcf_create(2, npfb, H, H_len-1);
    symsync_crcf_set_lf_bw(fg->mfsync, FRAMESYNC64_SYMSYNC_BW_0);

    // create decoder
    fg->dec = fec_create(FEC_HAMMING74, NULL);

    // create demod
    fg->demod = modem_create(MOD_QPSK, 2);

    return fg;
}

void framesync64_destroy(framesync64 _fg)
{
    symsync_crcf_destroy(_fg->mfsync);
    fec_destroy(_fg->dec);
    agc_destroy(_fg->agc_rx);
    pll_destroy(_fg->pll_rx);
    nco_destroy(_fg->nco_rx);
    pnsync_crcf_destroy(_fg->fsync);
    free(_fg->demod);
    free(_fg);
}

void framesync64_print(framesync64 _fg)
{
    printf("framesync:\n");
}



