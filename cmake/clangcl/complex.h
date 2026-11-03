// complex.h shim for clang-cl/MSVC-ABI builds.
//
// The UCRT <complex.h> predates C99 _Complex support in MSVC: it provides
// struct-based types with no `complex` macro, so sources written against
// C99 complex cannot use it. When building with clang-cl (which implements
// _Complex fully), this header stands in for <complex.h> via -I.
//
// Real/imaginary parts, conjugate, magnitude, and phase decompose to
// inline code and scalar CRT math. Transcendentals forward to the UCRT
// struct-based routines, reached through asm-labeled declarations so the
// wrappers never share a name with the call they lower to. Deliberately
// uses no complex builtins: a builtin that lowers to a library call would
// resolve to the wrapper itself and recurse.
//
// Only what liquid-dsp uses is provided: creal/cimag/conj/cabs/carg and
// cexp/csqrt/csin/ccos in float and double flavors.
#ifndef _LIQUID_CLANG_COMPLEX_H
#define _LIQUID_CLANG_COMPLEX_H

#include <math.h>
#include <string.h>

// The UCRT defines `complex` as its legacy struct type (via <math.h>);
// replace it so the C99 definition below wins regardless of order.
#undef complex
#undef I
#undef _Complex_I

#define complex _Complex
#define _Complex_I ((float _Complex){0.0f, 1.0f})
#define I _Complex_I

// UCRT struct-based complex types (same layout as <complex.h>) and
// routines, reached under distinct names so the wrappers below can never
// resolve back to themselves.
typedef struct { float _Val[2]; } _Fcomplex;
typedef struct { double _Val[2]; } _Dcomplex;
// x86 decorates C symbols with a leading underscore; asm labels bypass
// decoration, so spell it explicitly there.
#ifdef _M_IX86
#define _LIQUID_UCRT_SYM(name) "_" name
#else
#define _LIQUID_UCRT_SYM(name) name
#endif
_Fcomplex _liquid_ucrt_cexpf(_Fcomplex) __asm__(_LIQUID_UCRT_SYM("cexpf"));
_Dcomplex _liquid_ucrt_cexp(_Dcomplex) __asm__(_LIQUID_UCRT_SYM("cexp"));
_Fcomplex _liquid_ucrt_csqrtf(_Fcomplex) __asm__(_LIQUID_UCRT_SYM("csqrtf"));
_Dcomplex _liquid_ucrt_csqrt(_Dcomplex) __asm__(_LIQUID_UCRT_SYM("csqrt"));
_Fcomplex _liquid_ucrt_csinf(_Fcomplex) __asm__(_LIQUID_UCRT_SYM("csinf"));
_Dcomplex _liquid_ucrt_csin(_Dcomplex) __asm__(_LIQUID_UCRT_SYM("csin"));
_Fcomplex _liquid_ucrt_ccosf(_Fcomplex) __asm__(_LIQUID_UCRT_SYM("ccosf"));
_Dcomplex _liquid_ucrt_ccos(_Dcomplex) __asm__(_LIQUID_UCRT_SYM("ccos"));

// Real/imaginary parts and conjugate decompose to inline code.
static inline float crealf(float _Complex _z) { return __real__ _z; }
static inline double creal(double _Complex _z) { return __real__ _z; }
static inline float cimagf(float _Complex _z) { return __imag__ _z; }
static inline double cimag(double _Complex _z) { return __imag__ _z; }
static inline float _Complex conjf(float _Complex _z) { return __real__ _z - _Complex_I * __imag__ _z; }
static inline double _Complex conj(double _Complex _z) { return __real__ _z - (double _Complex)_Complex_I * __imag__ _z; }

// Magnitude and phase via scalar CRT math.
static inline float cabsf(float _Complex _z) { return hypotf(__real__ _z, __imag__ _z); }
static inline double cabs(double _Complex _z) { return hypot(__real__ _z, __imag__ _z); }
static inline float cargf(float _Complex _z) { return atan2f(__imag__ _z, __real__ _z); }
static inline double carg(double _Complex _z) { return atan2(__imag__ _z, __real__ _z); }

// Transcendentals forward to the UCRT routines; _Complex shares their
// two-contiguous-reals layout, so the argument shuttles by value copy.
static inline float _Complex _liquid_cexpf(float _Complex _z) {
    _Fcomplex _in;
    _Fcomplex _out;
    float _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_cexpf(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline double _Complex _liquid_cexp(double _Complex _z) {
    _Dcomplex _in;
    _Dcomplex _out;
    double _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_cexp(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline float _Complex _liquid_csqrtf(float _Complex _z) {
    _Fcomplex _in;
    _Fcomplex _out;
    float _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_csqrtf(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline double _Complex _liquid_csqrt(double _Complex _z) {
    _Dcomplex _in;
    _Dcomplex _out;
    double _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_csqrt(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline float _Complex _liquid_csinf(float _Complex _z) {
    _Fcomplex _in;
    _Fcomplex _out;
    float _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_csinf(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline double _Complex _liquid_csin(double _Complex _z) {
    _Dcomplex _in;
    _Dcomplex _out;
    double _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_csin(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline float _Complex _liquid_ccosf(float _Complex _z) {
    _Fcomplex _in;
    _Fcomplex _out;
    float _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_ccosf(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}
static inline double _Complex _liquid_ccos(double _Complex _z) {
    _Dcomplex _in;
    _Dcomplex _out;
    double _Complex _r;
    memcpy(&_in, &_z, sizeof(_in));
    _out = _liquid_ucrt_ccos(_in);
    memcpy(&_r, &_out, sizeof(_r));
    return _r;
}

// Function-like macros route calls to the wrappers; the compiler never
// sees a direct call to a complex builtin name, so nothing can lower
// back into a wrapper.
#define cexpf(_z)  _liquid_cexpf(_z)
#define cexp(_z)   _liquid_cexp(_z)
#define csqrtf(_z) _liquid_csqrtf(_z)
#define csqrt(_z)  _liquid_csqrt(_z)
#define csinf(_z)  _liquid_csinf(_z)
#define csin(_z)   _liquid_csin(_z)
#define ccosf(_z)  _liquid_ccosf(_z)
#define ccos(_z)   _liquid_ccos(_z)

#endif
