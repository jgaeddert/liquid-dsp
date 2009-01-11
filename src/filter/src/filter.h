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
// FIR Hilbert transform
//  2:1 real-to-complex decimator
//  1:2 complex-to-real interpolator
//

#define FIRHILB_MANGLE_FLOAT(name)  FILTER_CONCAT(firhilb, name)
//#define FIRHILB_MANGLE_DOUBLE(name) FILTER_CONCAT(dfirhilb, name)

// NOTES:
//   Although firhilb is a placeholder for both the decimation and
//   interpolation, separate objects should be used for each task.
#define LIQUID_FIRHILB_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _h_len); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_clear)(X() _f); \
void X(_decim_execute)(X() _f, T * _x, T complex * _y); \
void X(_interp_execute)(X() _f, T complex _x, T * _y);

LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_FLOAT, float)
//LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_DOUBLE, double)

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
void X(_execute)(X() _b, unsigned int _i, T *_y); \
void X(_clear)(X() _b);

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
// Decimator
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
// Half-band resampler
//
#define RESAMP2_MANGLE_FLOAT(name)   FILTER_CONCAT(resamp2,name)
#define RESAMP2_MANGLE_CFLOAT(name)  FILTER_CONCAT(cresamp2,name)

#define LIQUID_RESAMP2_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_decim_execute)(X() _f, T * _x, T * _y); \
void X(_interp_execute)(X() _f, T _x, T * _y);

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_FLOAT, float)
LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CFLOAT, float complex)


// 
// Arbitrary resampler
//
#define RESAMP_MANGLE_FLOAT(name)   FILTER_CONCAT(resamp,name)
#define RESAMP_MANGLE_CFLOAT(name)  FILTER_CONCAT(cresamp,name)

#define LIQUID_RESAMP_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(float _r); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q);

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_FLOAT, float)
LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CFLOAT, float complex)

// 
// Symbol timing recovery (symbol synchronizer)
//
#define SYMSYNC_MANGLE_FLOAT(name)  FILTER_CONCAT(symsync,name)
#define SYMSYNC_MANGLE_CFLOAT(name) FILTER_CONCAT(csymsync,name)

#define LIQUID_SYMSYNC_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _k, unsigned int _num_filters, T * _h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, T * _x, unsigned int _nx, T * _y, unsigned int *_ny); \
void X(_set_lf_bw)(X() _q, float _bt); \
void X(_clear)(X() _q); \
void X(_estimate_timing)(X() _q, T * _x, unsigned int _n);

LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_FLOAT, float)
LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CFLOAT, float complex)


// 
// Symbol timing recovery (symbol synchronizer, 2 samples/symbol in/out)
//
#define SYMSYNC2_MANGLE_FLOAT(name)  FILTER_CONCAT(symsync2,name)
#define SYMSYNC2_MANGLE_CFLOAT(name) FILTER_CONCAT(csymsync2,name)

#define LIQUID_SYMSYNC2_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _num_filters, T * _h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, T * _x, unsigned int _nx, T * _y, unsigned int *_ny); \
void X(_set_lf_bw)(X() _q, float _bt); \
void X(_clear)(X() _q); \
void X(_estimate_timing)(X() _q, T * _x, unsigned int _n);

LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_FLOAT, float)
LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_CFLOAT, float complex)

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

