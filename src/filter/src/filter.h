//
//
//

#ifndef __LIQUID_FILTER_H__
#define __LIQUID_FILTER_H__

#include <complex.h>

#define FILTER_CONCAT(prefix,name) prefix ## name

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

#define FIR_FILTER_MANGLE_FLOAT(name)  FILTER_CONCAT(fir_filter,name)
#define FIR_FILTER_MANGLE_CFLOAT(name) FILTER_CONCAT(cfir_filter,name)

// Macro:
//  X   : name-mangling macro
//  T   : coefficients type
#define LIQUID_FIR_FILTER_DEFINE_API(X,T) \
typedef struct X(_s) * X();   \
X() X(_create)(T * _h, unsigned int _n); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_push)(X() _f, T _x); \
void X(_execute)(X() _f, T *_y); \
unsigned int X(_get_length)(X() _f);

LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER_MANGLE_FLOAT, float)
LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER_MANGLE_CFLOAT, float complex)


//
// Infinite impulse response filter
//

#define IIR_FILTER_MANGLE_FLOAT(name)  FILTER_CONCAT(iir_filter,name)
#define IIR_FILTER_MANGLE_CFLOAT(name) FILTER_CONCAT(ciir_filter,name)

// Macro:
//  X   : name-mangling macro
//  T   : coefficients type
#define LIQUID_IIR_FILTER_DEFINE_API(X,T) \
typedef struct X(_s) * X();   \
X() X(_create)(T * _b, unsigned int _nb, T * _a,  unsigned int _na); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_clear)(X() _f); \
void X(_execute)(X() _f, T _x, T *_y); \
unsigned int X(_get_length)(X() _f);

LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER_MANGLE_FLOAT, float)
LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER_MANGLE_CFLOAT, float complex)


//
// FIR Polyphase filter bank
//
#define FIRPFB_MANGLE_FLOAT(name)  FILTER_CONCAT(firpfb,name)
#define FIRPFB_MANGLE_CFLOAT(name) FILTER_CONCAT(cfirpfb,name)

#define LIQUID_FIRPFB_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _num_filters, T * _h, unsigned int _h_len); \
void X(_destroy)(X() _b); \
void X(_print)(X() _b); \
void X(_push)(X() _b, T _x); \
void X(_execute)(X() _b, unsigned int _i, T *_y);

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_FLOAT, float)
LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CFLOAT, float complex)

// 
// Interpolator
//
#define INTERP_MANGLE_FLOAT(name)  FILTER_CONCAT(interp,name)
#define INTERP_MANGLE_CFLOAT(name) FILTER_CONCAT(cinterp,name)

#define LIQUID_INTERP_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _M, T *_h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, T _x, T *_y);

LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_FLOAT, float)
LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_CFLOAT, float complex)

// 
// Decimator (basic)
//
#define DECIM_MANGLE_FLOAT(name)    FILTER_CONCAT(decim,name)
#define DECIM_MANGLE_CFLOAT(name)   FILTER_CONCAT(cdecim,name)

#define LIQUID_DECIM_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _D, T *_h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, T *_x, T *_y, unsigned int _index);

LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_FLOAT, float)
LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_CFLOAT, float complex)

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


#endif // __LIQUID_FILTER_H__

