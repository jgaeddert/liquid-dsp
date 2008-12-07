//
//
//

#ifndef __LIQUID_FILTER_INTERNAL_H__
#define __LIQUID_FILTER_INTERNAL_H__

#include "filter.h"
#include "../../buffer/src/window.h"

//
// Finite impulse response filter
//

struct fir_filter_s {
    float * h;
    unsigned int h_len;

    fwindow w;

    fir_prototype p;
};


//
// FIR Polyphase filter bank
//
struct firpfb_s {
    fir_filter * f;
    unsigned int num_filters;
    unsigned int h_len;
};

// 
// Interpolator
//
struct interp_s {
    float * h;
    unsigned int h_len;
    unsigned int M;

    fir_prototype p;
    fwindow w;
};

// 
// Decimator (basic)
//
struct decim_s {
    float * h;
    unsigned int h_len;
    unsigned int D;

    fir_prototype p;
    fwindow w;
};

//
// 2nd-Order Loop Filter
//
// generate filter coefficients
void lf2_generate_filter(lf2 _f);


#endif // __LIQUID_FILTER_INTERNAL_H__

