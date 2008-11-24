// 
// Numerically-controlled oscillator
//

#ifndef __LIQUID_NCO_H__
#define __LIQUID_NCO_H__

#include <math.h>
#include <complex.h>

// Numerically-controlled oscillator, floating point phase precision
struct nco_s {
    float theta;        // NCO phase
    float d_theta;      // NCO frequency
};

typedef struct nco_s * nco;

nco nco_create();
void nco_destroy(nco _nco);

void nco_init(nco _nco);

static inline void nco_set_frequency(nco _nco, float _f) {
    _nco->d_theta = _f;
}

static inline void nco_adjust_frequency(nco _nco, float _df) {
    _nco->d_theta += _df;
}

void nco_set_phase(nco _nco, float _phi);
void nco_adjust_phase(nco _nco, float _dphi);

void nco_step(nco _nco);
void nco_constrain_phase(nco _nco);

#define nco_sin(_nco) (sinf(_nco->theta))
#define nco_cos(_nco) (cosf(_nco->theta))
//static inline float nco_sin(nco _nco) {return sinf(_nco->theta);}
//static inline float nco_cos(nco _nco) {return cosf(_nco->theta);}

static inline void nco_sincos(nco _nco, float* _s, float* _c) {
    *_s = sinf(_nco->theta);
    *_c = cosf(_nco->theta);
}

#define nco_cexpf(_nco) (cexpf(_Complex_I*(_nco->theta)))

// mixing functions

// Rotate input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
static inline void
nco_mix_up(nco _nco, complex float _x, complex float *_y) {
    *_y = _x * cexpf(_Complex_I*(_nco->theta));
}

// Rotate input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
static inline void
nco_mix_down(nco _nco, complex float _x, complex float *_y) {
    *_y = _x * cexpf(-_Complex_I*(_nco->theta));
}

// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_up(
    nco _nco,
    complex float *_x,
    complex float *_y,
    unsigned int _N);

void nco_mix_block_down(
    nco _nco,
    complex float *_x,
    complex float *_y,
    unsigned int _N);

//
// Phase-locked loop
//

typedef struct pll_s * pll;
pll pll_create(void);
void pll_destroy(pll _p);
void pll_print(pll _p);

void pll_set_bandwidth(pll _p, float _bt);
//void pll_execute(pll _p, float complex _x, float complex *_y, float _e);
void pll_step(pll _p, nco _nco, float _e);

#endif // __LIQUID_NCO_H__

