//
//
//

#ifndef __LIQUID_FILTER_H__
#define __LIQUID_FILTER_H__

#include "../../buffer/src/window.h"

//
// Finite impulse response filter
//

// FIR filter prototypes
typedef enum {
    FIR_NONE=0,
    FIR_RRCOS,
    FIR_RCOS,
    FIR_GAUSS
} fir_prototype;

//  filter type     : out   : coeff : in
//  ----------------+-------+-------+-----
//  fir_filter_rrr  : REAL  : REAL  : REAL
//  fir_filter_ccc  : CPLX  : CPLX  : CPLX
//  fir_filter_ccr  : CPLX  : CPLX  : REAL

typedef struct fir_filter_s * fir_filter;

fir_filter fir_filter_create(float * _h, unsigned int _n);

//fir_filter fir_filter_create_prototype(unsigned int _n);

void fir_filter_destroy(fir_filter _f);

void fir_filter_print(fir_filter _f);

//float fir_filter_execute(fir_filter _f, float * _v);
float fir_filter_execute(fir_filter _f, float _x);

// accessor functions
unsigned int fir_filter_get_length(fir_filter _f);

// Macro:
//  X   : name-mangling macro
//  O   : output type
//  C   : coefficients type
//  I   : input type
// DEFINE_FIR_FITER_API(X,O,C,I)

//
// internal
//
struct fir_filter_s {
    float * h;
    unsigned int h_len;

    fwindow w;

    fir_prototype p;
};

// 
// Interpolator
//
typedef struct interp_s * interp;
struct interp_s {
    float * h;
    unsigned int h_len;
    unsigned int M;

    fir_prototype p;
    fwindow w;
};

interp interp_create(unsigned int _M, float *_h, unsigned int _h_len);
void interp_destroy(interp _q);
void interp_print(interp _q);
void interp_execute(interp _q, float _x, float *_y);

// 
// Decimator (basic)
//
typedef struct decim_s * decim;
decim decim_create(unsigned int _D, float *_h, unsigned int _h_len);
struct decim_s {
    float * h;
    unsigned int h_len;
    unsigned int D;

    fir_prototype p;
    fwindow w;
};

decim decim_create(unsigned int _M, float *_h, unsigned int _h_len);
void decim_destroy(decim _q);
void decim_print(decim _q);
void decim_execute(decim _q, float *_x, float *_y, unsigned int _index);

//
// 2nd-Order Loop Filter
//

typedef struct lf2_s * lf2;

lf2 lf2_create(float _bt);
void lf2_destroy(lf2 _f);
void lf2_print(lf2 _f);

// reset filter state
void lf2_reset(lf2 _f);

// set bandwidth-time product
void lf2_set_damping_factor(lf2 _f, float _xi);

// set bandwidth-time product
void lf2_set_bandwidth(lf2 _f, float _bt);

// push input value and compute output
void lf2_advance(lf2 _f, float _v, float *_v_hat);

// 
// internal
//

// generate filter coefficients
void lf2_generate_filter(lf2 _f);


#endif // __FIR_FILTER_H__

