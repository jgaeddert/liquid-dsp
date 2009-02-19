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
    symsync_crcf mfdecim;
    pll pll_rx;
    nco nco_rx;
    pnsync_crcf fsync;

    // status variables
    bool frame_detected;
    unsigned int num_symbols_collected;

    framesync64_callback *callback;
};

framesync64 framesync64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta,
    framesync64_callback _callback)
{
    framesync64 fs = (framesync64) malloc(sizeof(struct framesync64_s));
    //fs->callback = _callback;

    //
    fs->agc_rx = agc_create(1.0f, FRAMESYNC64_AGC_BW_0);
    fs->pll_rx = pll_create();
    fs->nco_rx = nco_create();
    pll_set_bandwidth(fs->pll_rx, FRAMESYNC64_PLL_BW_0);

    // pnsync
    unsigned int i;
    msequence ms = msequence_create(6);
    float pn_sequence[64];
    for (i=0; i<64; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = pnsync_crcf_create(64, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 16;
    unsigned int H_len = 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,_m,_beta,0,H);
    fs->mfdecim =  symsync_crcf_create(2, npfb, H, H_len-1);
    symsync_crcf_set_lf_bw(fs->mfdecim, FRAMESYNC64_SYMSYNC_BW_0);

    // create decoder
    fs->dec = fec_create(FEC_HAMMING74, NULL);

    // create demod
    fs->demod = modem_create(MOD_QPSK, 2);

    // set status flags
    fs->frame_detected = false;
    fs->num_symbols_collected = 0;

    return fs;
}

void framesync64_destroy(framesync64 _fs)
{
    symsync_crcf_destroy(_fs->mfdecim);
    fec_destroy(_fs->dec);
    agc_destroy(_fs->agc_rx);
    pll_destroy(_fs->pll_rx);
    nco_destroy(_fs->nco_rx);
    pnsync_crcf_destroy(_fs->fsync);
    free(_fs->demod);
    free(_fs);
}

void framesync64_print(framesync64 _fs)
{
    printf("framesync:\n");
}

void framesync64_execute(framesync64 _fs, float complex *_x, unsigned int _n)
{

}

