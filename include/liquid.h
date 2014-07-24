/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __LIQUID_H__
#define __LIQUID_H__

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4003 )
#endif


#ifdef __cplusplus
extern "C" {
#   define LIQUID_USE_COMPLEX_H 0
#else
#   define LIQUID_USE_COMPLEX_H 1
#endif // __cplusplus

//
// Make sure the version and version number macros weren't defined by
// some prevoiusly included header file.
//
#ifdef LIQUID_VERSION
#  undef LIQUID_VERSION
#endif
#ifdef LIQUID_VERSION_NUMBER
#  undef LIQUID_VERSION_NUMBER
#endif

//
// Compile-time version numbers
// 
// LIQUID_VERSION = "X.Y.Z"
// LIQUID_VERSION_NUMBER = (X*1000000 + Y*1000 + Z)
//
#define LIQUID_VERSION          "1.2.0"
#define LIQUID_VERSION_NUMBER   1002000

//
// Run-time library version numbers
//
extern const char liquid_version[];
const char * liquid_libversion(void);
int liquid_libversion_number(void);

// run-time library validation
#define LIQUID_VALIDATE_LIBVERSION                              \
  if (LIQUID_VERSION_NUMBER != liquid_libversion_number()) {    \
    fprintf(stderr,"%s:%u: ", __FILE__,__LINE__);               \
    fprintf(stderr,"error: invalid liquid runtime library\n");  \
    exit(1);                                                    \
  }                                                             \

#define LIQUID_CONCAT(prefix, name) prefix ## name
#define LIQUID_VALIDATE_INPUT

/* 
 * Compile-time complex data type definitions
 *
 * Default: use the C99 complex data type, otherwise
 * define complex type compatible with the C++ complex standard,
 * otherwise resort to defining binary compatible array.
 */
#if LIQUID_USE_COMPLEX_H==1
#   include <complex.h>
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef R _Complex C
#elif defined _GLIBCXX_COMPLEX || defined _LIBCPP_COMPLEX
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef std::complex<R> C
#else
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef struct {R real; R imag;} C;
#endif
//#   define LIQUID_DEFINE_COMPLEX(R,C) typedef R C[2]

LIQUID_DEFINE_COMPLEX(float,  liquid_float_complex);
LIQUID_DEFINE_COMPLEX(double, liquid_double_complex);

// 
// MODULE : agc (automatic gain control)
//

#define AGC_MANGLE_CRCF(name)   LIQUID_CONCAT(agc_crcf, name)
#define AGC_MANGLE_RRRF(name)   LIQUID_CONCAT(agc_rrrf, name)

// large macro
//   AGC    : name-mangling macro
//   T      : primitive data type
//   TC     : input/output data type
#define LIQUID_AGC_DEFINE_API(AGC,T,TC)                         \
typedef struct AGC(_s) * AGC();                                 \
                                                                \
/* create automatic gain control object                     */  \
AGC() AGC(_create)(void);                                       \
                                                                \
/* destroy object, freeing all internally-allocated memory  */  \
void AGC(_destroy)(AGC() _q);                                   \
                                                                \
/* print object properties to stdout                        */  \
void AGC(_print)(AGC() _q);                                     \
                                                                \
/* reset object's internal state                            */  \
void AGC(_reset)(AGC() _q);                                     \
                                                                \
/* execute automatic gain control on an single input sample */  \
/*  _q      : automatic gain control object                 */  \
/*  _x      : input sample                                  */  \
/*  _y      : output sample                                 */  \
void AGC(_execute)(AGC() _q,                                    \
                   TC    _x,                                    \
                   TC *  _y);                                   \
                                                                \
/* execute automatic gain control on block of samples       */  \
/*  _q      : automatic gain control object                 */  \
/*  _x      : input data array, [size: _n x 1]              */  \
/*  _n      : number of input, output samples               */  \
/*  _y      : output data array, [szie: _n x 1]             */  \
void AGC(_execute_block)(AGC()          _q,                     \
                         TC *           _x,                     \
                         unsigned int   _n,                     \
                         TC *           _y);                    \
                                                                \
/* lock/unlock gain control */                                  \
void AGC(_lock)(  AGC() _q);                                    \
void AGC(_unlock)(AGC() _q);                                    \
                                                                \
/* get/set loop filter bandwidth; attack/release time       */  \
float AGC(_get_bandwidth)(AGC() _q);                            \
void  AGC(_set_bandwidth)(AGC() _q, float _bt);                 \
                                                                \
/* get/set signal level (linear) relative to unity energy   */  \
float AGC(_get_signal_level)(AGC() _q);                         \
void  AGC(_set_signal_level)(AGC() _q, float _signal_level);    \
                                                                \
/* get/set signal level (dB) relative to unity energy       */  \
float AGC(_get_rssi)(AGC() _q);                                 \
void  AGC(_set_rssi)(AGC() _q, float _rssi);                    \
                                                                \
/* get/set gain value (linear) relative to unity energy     */  \
float AGC(_get_gain)(AGC() _q);                                 \
void  AGC(_set_gain)(AGC() _q, float _gain);                    \
                                                                \
/* initialize internal gain on input array                  */  \
/*  _q      : automatic gain control object                 */  \
/*  _x      : input data array, [size: _n x 1]              */  \
/*  _n      : number of input, output samples               */  \
void AGC(_init)(AGC()        _q,                                \
                TC *         _x,                                \
                unsigned int _n);                               \

// Define agc APIs
LIQUID_AGC_DEFINE_API(AGC_MANGLE_CRCF, float, liquid_float_complex)
LIQUID_AGC_DEFINE_API(AGC_MANGLE_RRRF, float, float)



//
// MODULE : audio
//

// CVSD: continuously variable slope delta
typedef struct cvsd_s * cvsd;

// create cvsd object
//  _num_bits   :   number of adjacent bits to observe (4 recommended)
//  _zeta       :   slope adjustment multiplier (1.5 recommended)
//  _alpha      :   pre-/post-emphasis filter coefficient (0.9 recommended)
// NOTE: _alpha must be in [0,1]
cvsd cvsd_create(unsigned int _num_bits,
                 float _zeta,
                 float _alpha);

// destroy cvsd object
void cvsd_destroy(cvsd _q);

// print cvsd object parameters
void cvsd_print(cvsd _q);

// encode/decode single sample
unsigned char   cvsd_encode(cvsd _q, float _audio_sample);
float           cvsd_decode(cvsd _q, unsigned char _bit);

// encode/decode 8 samples at a time
void cvsd_encode8(cvsd _q, float * _audio, unsigned char * _data);
void cvsd_decode8(cvsd _q, unsigned char _data, float * _audio);


//
// MODULE : buffer
//

// circular buffer
#define CBUFFER_MANGLE_FLOAT(name)  LIQUID_CONCAT(cbufferf,  name)
#define CBUFFER_MANGLE_CFLOAT(name) LIQUID_CONCAT(cbuffercf, name)

// large macro
//   CBUFFER : name-mangling macro
//   T       : data type
#define LIQUID_CBUFFER_DEFINE_API(CBUFFER,T)                    \
typedef struct CBUFFER(_s) * CBUFFER();                         \
                                                                \
/* create circular buffer object of a particular size       */  \
CBUFFER() CBUFFER(_create)(unsigned int _max_size);             \
                                                                \
/* create circular buffer object of a particular size and   */  \
/* specify the maximum number of elements that can be read  */  \
/* at any given time.                                       */  \
CBUFFER() CBUFFER(_create_max)(unsigned int _max_size,          \
                               unsigned int _max_read);         \
                                                                \
/* destroy cbuffer object, freeing all internal memory      */  \
void CBUFFER(_destroy)(CBUFFER() _q);                           \
                                                                \
/* print cbuffer object properties                          */  \
void CBUFFER(_print)(CBUFFER() _q);                             \
                                                                \
/* print cbuffer object properties and internal state       */  \
void CBUFFER(_debug_print)(CBUFFER() _q);                       \
                                                                \
/* clear internal buffer                                    */  \
void CBUFFER(_clear)(CBUFFER() _q);                             \
                                                                \
/* get the number of elements currently in the buffer       */  \
unsigned int CBUFFER(_size)(CBUFFER() _q);                      \
                                                                \
/* get the maximum number of elements the buffer can hold   */  \
unsigned int CBUFFER(_max_size)(CBUFFER() _q);                  \
                                                                \
/* get the maximum number of elements you may read at once  */  \
unsigned int CBUFFER(_max_read)(CBUFFER() _q);                  \
                                                                \
/* get the number of available slots (max_size - size)      */  \
unsigned int CBUFFER(_space_available)(CBUFFER() _q);           \
                                                                \
/* is buffer full?                                          */  \
int CBUFFER(_is_full)(CBUFFER() _q);                            \
                                                                \
/* write a single sample into the buffer                    */  \
/*  _q  : circular buffer object                            */  \
/*  _v  : input sample                                      */  \
void CBUFFER(_push)(CBUFFER() _q,                               \
                    T         _v);                              \
                                                                \
/* write samples to the buffer                              */  \
/*  _q  : circular buffer object                            */  \
/*  _v  : output array                                      */  \
/*  _n  : number of samples to write                        */  \
void CBUFFER(_write)(CBUFFER()    _q,                           \
                     T *          _v,                           \
                     unsigned int _n);                          \
                                                                \
/* remove and return a single element from the buffer       */  \
/*  _q  : circular buffer object                            */  \
/*  _v  : pointer to sample output                          */  \
void CBUFFER(_pop)(CBUFFER() _q,                                \
                   T *       _v);                               \
                                                                \
/* read buffer contents                                     */  \
/*  _q              : circular buffer object                */  \
/*  _num_requested  : number of elements requested          */  \
/*  _v              : output pointer                        */  \
/*  _nr             : number of elements referenced by _v   */  \
void CBUFFER(_read)(CBUFFER()      _q,                          \
                    unsigned int   _num_requested,              \
                    T **           _v,                          \
                    unsigned int * _num_read);                  \
                                                                \
/* release _n samples from the buffer                       */  \
void CBUFFER(_release)(CBUFFER()    _q,                         \
                       unsigned int _n);                        \

// Define buffer APIs
LIQUID_CBUFFER_DEFINE_API(CBUFFER_MANGLE_FLOAT,  float)
LIQUID_CBUFFER_DEFINE_API(CBUFFER_MANGLE_CFLOAT, liquid_float_complex)



// Windowing functions
#define WINDOW_MANGLE_FLOAT(name)  LIQUID_CONCAT(windowf,  name)
#define WINDOW_MANGLE_CFLOAT(name) LIQUID_CONCAT(windowcf, name)

// large macro
//   WINDOW : name-mangling macro
//   T      : data type
#define LIQUID_WINDOW_DEFINE_API(WINDOW,T)                      \
                                                                \
typedef struct WINDOW(_s) * WINDOW();                           \
                                                                \
/* create window buffer object of length _n                 */  \
WINDOW() WINDOW(_create)(unsigned int _n);                      \
                                                                \
/* recreate window buffer object with new length            */  \
/*  _q      : old window object                             */  \
/*  _n      : new window length                             */  \
WINDOW() WINDOW(_recreate)(WINDOW() _q, unsigned int _n);       \
                                                                \
/* destroy window object, freeing all internally memory     */  \
void WINDOW(_destroy)(WINDOW() _q);                             \
                                                                \
/* print window object to stdout                            */  \
void WINDOW(_print)(WINDOW() _q);                               \
                                                                \
/* print window object to stdout (with extra information)   */  \
void WINDOW(_debug_print)(WINDOW() _q);                         \
                                                                \
/* clear/reset window object (initialize to zeros)          */  \
void WINDOW(_clear)(WINDOW() _q);                               \
                                                                \
/* read window buffer contents                              */  \
/*  _q      : window object                                 */  \
/*  _v      : output pointer (set to internal array)        */  \
void WINDOW(_read)(WINDOW() _q, T ** _v);                       \
                                                                \
/* index single element in buffer at a particular index     */  \
/*  _q      : window object                                 */  \
/*  _i      : index of element to read                      */  \
/*  _v      : output value pointer                          */  \
void WINDOW(_index)(WINDOW()     _q,                            \
                    unsigned int _i,                            \
                    T *          _v);                           \
                                                                \
/* push single element onto window buffer                   */  \
/*  _q      : window object                                 */  \
/*  _v      : single input element                          */  \
void WINDOW(_push)(WINDOW() _q,                                 \
                   T        _v);                                \
                                                                \
/* write array of elements onto window buffer               */  \
/*  _q      : window object                                 */  \
/*  _v      : input array of values to write                */  \
/*  _n      : number of input values to write               */  \
void WINDOW(_write)(WINDOW()     _q,                            \
                    T *          _v,                            \
                    unsigned int _n);                           \

// Define window APIs
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_FLOAT,  float)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_CFLOAT, liquid_float_complex)
//LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_UINT,   unsigned int)


// wdelay functions : windowed-delay
// Implements an efficient z^-k delay with minimal memory
#define WDELAY_MANGLE_FLOAT(name)   LIQUID_CONCAT(wdelayf,  name)
#define WDELAY_MANGLE_CFLOAT(name)  LIQUID_CONCAT(wdelaycf, name)
#define WDELAY_MANGLE_UINT(name)    LIQUID_CONCAT(wdelayui, name)

// large macro
//   WDELAY : name-mangling macro
//   T      : data type
#define LIQUID_WDELAY_DEFINE_API(WDELAY,T)                      \
typedef struct WDELAY(_s) * WDELAY();                           \
                                                                \
/* create delay buffer object with '_delay' samples         */  \
WDELAY() WDELAY(_create)(unsigned int _delay);                  \
                                                                \
/* re-create delay buffer object with '_delay' samples      */  \
/*  _q      :   old delay buffer object                     */  \
/*  _delay  :   delay for new object                        */  \
WDELAY() WDELAY(_recreate)(WDELAY()     _q,                     \
                           unsigned int _delay);                \
                                                                \
/* destroy delay buffer object, freeing internal memory     */  \
void WDELAY(_destroy)(WDELAY() _q);                             \
                                                                \
/* print delay buffer object's state to stdout              */  \
void WDELAY(_print)(WDELAY() _q);                               \
                                                                \
/* clear/reset state of object                              */  \
void WDELAY(_clear)(WDELAY() _q);                               \
                                                                \
/* read delayed sample from delay buffer object             */  \
/*  _q  :   delay buffer object                             */  \
/*  _v  :   value of delayed element                        */  \
void WDELAY(_read)(WDELAY() _q,                                 \
                   T *      _v);                                \
                                                                \
/* push new sample into delay buffer object                 */  \
/*  _q  :   delay buffer object                             */  \
/*  _v  :   new value to be added to buffer                 */  \
void WDELAY(_push)(WDELAY() _q,                                 \
                   T        _v);                                \

// Define wdelay APIs
LIQUID_WDELAY_DEFINE_API(WDELAY_MANGLE_FLOAT,  float)
LIQUID_WDELAY_DEFINE_API(WDELAY_MANGLE_CFLOAT, liquid_float_complex)
//LIQUID_WDELAY_DEFINE_API(WDELAY_MANGLE_UINT,   unsigned int)



//
// MODULE : dotprod (vector dot product)
//

#define DOTPROD_MANGLE_RRRF(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define DOTPROD_MANGLE_CCCF(name)   LIQUID_CONCAT(dotprod_cccf,name)
#define DOTPROD_MANGLE_CRCF(name)   LIQUID_CONCAT(dotprod_crcf,name)

// large macro
//   DOTPROD    : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_DOTPROD_DEFINE_API(DOTPROD,TO,TC,TI)             \
                                                                \
/* run dot product without creating object [unrolled loop]  */  \
/*  _v      : coefficients array [size: _n x 1]             */  \
/*  _x      : input array [size: _n x 1]                    */  \
/*  _n      : dotprod length, _n > 0                        */  \
/*  _y      : output sample pointer                         */  \
void DOTPROD(_run)( TC *_v, TI *_x, unsigned int _n, TO *_y);   \
void DOTPROD(_run4)(TC *_v, TI *_x, unsigned int _n, TO *_y);   \
                                                                \
typedef struct DOTPROD(_s) * DOTPROD();                         \
                                                                \
/* create dot product object                                */  \
/*  _v      : coefficients array [size: _n x 1]             */  \
/*  _n      : dotprod length, _n > 0                        */  \
DOTPROD() DOTPROD(_create)(TC *         _v,                     \
                           unsigned int _n);                    \
                                                                \
/* re-create dot product object                             */  \
/*  _q      : old dotprod object                            */  \
/*  _v      : coefficients array [size: _n x 1]             */  \
/*  _n      : dotprod length, _n > 0                        */  \
DOTPROD() DOTPROD(_recreate)(DOTPROD()    _q,                   \
                             TC *         _v,                   \
                             unsigned int _n);                  \
                                                                \
/* destroy dotprod object, freeing all internal memory      */  \
void DOTPROD(_destroy)(DOTPROD() _q);                           \
                                                                \
/* print dotprod object internals to standard output        */  \
void DOTPROD(_print)(DOTPROD() _q);                             \
                                                                \
/* execute dot product                                      */  \
/*  _q      : dotprod object                                */  \
/*  _x      : input array [size: _n x 1]                    */  \
/*  _y      : output sample pointer                         */  \
void DOTPROD(_execute)(DOTPROD() _q,                            \
                       TI *      _x,                            \
                       TO *      _y);                           \

LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)

LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

// 
// sum squared methods
//

float liquid_sumsqf(float *      _v,
                    unsigned int _n);

float liquid_sumsqcf(liquid_float_complex * _v,
                     unsigned int           _n);


//
// MODULE : equalization
//

// least mean-squares (LMS)
#define EQLMS_MANGLE_RRRF(name)     LIQUID_CONCAT(eqlms_rrrf,name)
#define EQLMS_MANGLE_CCCF(name)     LIQUID_CONCAT(eqlms_cccf,name)

// large macro
//   EQLMS  : name-mangling macro
//   T      : data type
#define LIQUID_EQLMS_DEFINE_API(EQLMS,T)                        \
typedef struct EQLMS(_s) * EQLMS();                             \
                                                                \
/* create LMS EQ initialized with external coefficients     */  \
/*  _h      : filter coefficients (NULL for {1,0,0...})     */  \
/*  _p      : filter length                                 */  \
EQLMS() EQLMS(_create)(T *          _h,                         \
                       unsigned int _p);                        \
                                                                \
/* create LMS EQ initialized with square-root Nyquist       */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _k      : samples/symbol                                */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _beta   : rolloff factor (0 < beta <= 1)                */  \
/*  _dt     : fractional sample delay                       */  \
EQLMS() EQLMS(_create_rnyquist)(int          _type,             \
                                unsigned int _k,                \
                                unsigned int _m,                \
                                float        _beta,             \
                                float        _dt);              \
                                                                \
/* re-create EQ initialized with external coefficients      */  \
/*  _q      :   equalizer object                            */  \
/*  _h      :   filter coefficients (NULL for {1,0,0...})   */  \
/*  _p      :   filter length                               */  \
EQLMS() EQLMS(_recreate)(EQLMS()      _q,                       \
                         T *          _h,                       \
                         unsigned int _p);                      \
                                                                \
/* destroy equalizer object, freeing all internal memory    */  \
void EQLMS(_destroy)(EQLMS() _q);                               \
                                                                \
/* reset equalizer object, clearing internal state          */  \
void EQLMS(_reset)(EQLMS() _q);                                 \
                                                                \
/* print equalizer internal state                           */  \
void EQLMS(_print)(EQLMS() _q);                                 \
                                                                \
/* get/set equalizer learning rate                          */  \
float EQLMS(_get_bw)(EQLMS() _q);                               \
void  EQLMS(_set_bw)(EQLMS() _q,                                \
                     float   _lambda);                          \
                                                                \
/* push sample into equalizer internal buffer               */  \
void EQLMS(_push)(EQLMS() _q,                                   \
                  T       _x);                                  \
                                                                \
/* execute internal dot product and return result           */  \
/*  _q      :   equalizer object                            */  \
/*  _y      :   output sample                               */  \
void EQLMS(_execute)(EQLMS() _q,                                \
                     T *     _y);                               \
                                                                \
/* step through one cycle of equalizer training             */  \
/*  _q      :   equalizer object                            */  \
/*  _d      :   desired output                              */  \
/*  _d_hat  :   actual output                               */  \
void EQLMS(_step)(EQLMS() _q,                                   \
                  T       _d,                                   \
                  T       _d_hat);                              \
                                                                \
/* get equalizer's internal coefficients                    */  \
/*  _q      :   equalizer object                            */  \
/*  _w      :   weights [size: _p x 1]                      */  \
void EQLMS(_get_weights)(EQLMS() _q,                            \
                         T *     _w);                           \
                                                                \
/* train equalizer object on group of samples               */  \
/*  _q      :   equalizer object                            */  \
/*  _w      :   input/output weights   [size: _p x 1]       */  \
/*  _x      :   received sample vector [size: _n x 1]       */  \
/*  _d      :   desired output vector  [size: _n x 1]       */  \
/*  _n      :   input, output vector length                 */  \
void EQLMS(_train)(EQLMS()      _q,                             \
                   T *          _w,                             \
                   T *          _x,                             \
                   T *          _d,                             \
                   unsigned int _n);                            \

LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_RRRF, float);
LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_CCCF, liquid_float_complex);


// recursive least-squares (RLS)
#define EQRLS_MANGLE_RRRF(name)     LIQUID_CONCAT(eqrls_rrrf,name)
#define EQRLS_MANGLE_CCCF(name)     LIQUID_CONCAT(eqrls_cccf,name)

// large macro
//   EQRLS  : name-mangling macro
//   T      : data type
#define LIQUID_EQRLS_DEFINE_API(EQRLS,T)                        \
typedef struct EQRLS(_s) * EQRLS();                             \
                                                                \
/* create RLS EQ initialized with external coefficients     */  \
/*  _h  : filter coefficients (NULL for {1,0,0...})         */  \
/*  _p  : filter length                                     */  \
EQRLS() EQRLS(_create)(T *          _h,                         \
                       unsigned int _p);                        \
                                                                \
/* re-create RLS EQ initialized with external coefficients  */  \
/*  _q  : initial equalizer object                          */  \
/*  _h  : filter coefficients (NULL for {1,0,0...})         */  \
/*  _p  : filter length                                     */  \
EQRLS() EQRLS(_recreate)(EQRLS()      _q,                       \
                         T *          _h,                       \
                         unsigned int _p);                      \
                                                                \
/* destroy equalizer object, freeing all internal memory    */  \
void EQRLS(_destroy)(EQRLS() _q);                               \
                                                                \
/* print equalizer internal state                           */  \
void EQRLS(_print)(EQRLS() _q);                                 \
                                                                \
/* reset equalizer object, clearing internal state          */  \
void EQRLS(_reset)(EQRLS() _q);                                 \
                                                                \
/* get/set equalizer learning rate                          */  \
float EQRLS(_get_bw)(EQRLS() _q);                               \
void  EQRLS(_set_bw)(EQRLS() _q,                                \
                     float   _mu);                              \
                                                                \
/* push sample into equalizer internal buffer               */  \
void EQRLS(_push)(EQRLS() _q, T _x);                            \
                                                                \
/* execute internal dot product and return result           */  \
/*  _q      :   equalizer object                            */  \
/*  _y      :   output sample                               */  \
void EQRLS(_execute)(EQRLS() _q, T * _y);                       \
                                                                \
/* step through one cycle of equalizer training             */  \
/*  _q      :   equalizer object                            */  \
/*  _d      :   desired output                              */  \
/*  _d_hat  :   actual output                               */  \
void EQRLS(_step)(EQRLS() _q, T _d, T _d_hat);                  \
                                                                \
/* retrieve internal filter coefficients                    */  \
/*  _q      :   equalizer object                            */  \
/*  _w      :   weights [size: _p x 1]                      */  \
void EQRLS(_get_weights)(EQRLS() _q,                            \
                         T *     _w);                           \
                                                                \
/* train equalizer object on group of samples               */  \
/*  _q      :   equalizer object                            */  \
/*  _w      :   input/output weights   [size: _p x 1]       */  \
/*  _x      :   received sample vector [size: _n x 1]       */  \
/*  _d      :   desired output vector  [size: _n x 1]       */  \
/*  _n      :   input, output vector length                 */  \
void EQRLS(_train)(EQRLS()      _q,                             \
                   T *          _w,                             \
                   T *          _x,                             \
                   T *          _d,                             \
                   unsigned int _n);

LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_RRRF, float);
LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_CCCF, liquid_float_complex);




//
// MODULE : fec (forward error-correction)
//

// soft bit values
#define LIQUID_SOFTBIT_0        (0)
#define LIQUID_SOFTBIT_1        (255)
#define LIQUID_SOFTBIT_ERASURE  (127)

// available CRC schemes
#define LIQUID_CRC_NUM_SCHEMES  7
typedef enum {
    LIQUID_CRC_UNKNOWN=0,   // unknown/unavailable CRC scheme
    LIQUID_CRC_NONE,        // no error-detection
    LIQUID_CRC_CHECKSUM,    // 8-bit checksum
    LIQUID_CRC_8,           // 8-bit CRC
    LIQUID_CRC_16,          // 16-bit CRC
    LIQUID_CRC_24,          // 24-bit CRC
    LIQUID_CRC_32           // 32-bit CRC
} crc_scheme;

// pretty names for crc schemes
extern const char * crc_scheme_str[LIQUID_CRC_NUM_SCHEMES][2];

// Print compact list of existing and available CRC schemes
void liquid_print_crc_schemes();

// returns crc_scheme based on input string
crc_scheme liquid_getopt_str2crc(const char * _str);

// get length of CRC (bytes)
unsigned int crc_get_length(crc_scheme _scheme);

// generate error-detection key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
unsigned int crc_generate_key(crc_scheme _scheme,
                              unsigned char * _msg,
                              unsigned int _n);

// validate message using error-detection key
//
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
//  _key        :   error-detection key
int crc_validate_message(crc_scheme _scheme,
                         unsigned char * _msg,
                         unsigned int _n,
                         unsigned int _key);


// available FEC schemes
#define LIQUID_FEC_NUM_SCHEMES  28
typedef enum {
    LIQUID_FEC_UNKNOWN=0,       // unknown/unsupported scheme
    LIQUID_FEC_NONE,            // no error-correction
    LIQUID_FEC_REP3,            // simple repeat code, r1/3
    LIQUID_FEC_REP5,            // simple repeat code, r1/5
    LIQUID_FEC_HAMMING74,       // Hamming (7,4) block code, r1/2 (really 4/7)
    LIQUID_FEC_HAMMING84,       // Hamming (7,4) with extra parity bit, r1/2
    LIQUID_FEC_HAMMING128,      // Hamming (12,8) block code, r2/3
    
    LIQUID_FEC_GOLAY2412,       // Golay (24,12) block code, r1/2
    LIQUID_FEC_SECDED2216,      // SEC-DED (22,16) block code, r8/11
    LIQUID_FEC_SECDED3932,      // SEC-DED (39,32) block code
    LIQUID_FEC_SECDED7264,      // SEC-DED (72,64) block code, r8/9

    // codecs not defined internally (see http://www.ka9q.net/code/fec/)
    LIQUID_FEC_CONV_V27,        // r1/2, K=7, dfree=10
    LIQUID_FEC_CONV_V29,        // r1/2, K=9, dfree=12
    LIQUID_FEC_CONV_V39,        // r1/3, K=9, dfree=18
    LIQUID_FEC_CONV_V615,       // r1/6, K=15, dfree<=57 (Heller 1968)

    // punctured (perforated) codes
    LIQUID_FEC_CONV_V27P23,     // r2/3, K=7, dfree=6
    LIQUID_FEC_CONV_V27P34,     // r3/4, K=7, dfree=5
    LIQUID_FEC_CONV_V27P45,     // r4/5, K=7, dfree=4
    LIQUID_FEC_CONV_V27P56,     // r5/6, K=7, dfree=4
    LIQUID_FEC_CONV_V27P67,     // r6/7, K=7, dfree=3
    LIQUID_FEC_CONV_V27P78,     // r7/8, K=7, dfree=3

    LIQUID_FEC_CONV_V29P23,     // r2/3, K=9, dfree=7
    LIQUID_FEC_CONV_V29P34,     // r3/4, K=9, dfree=6
    LIQUID_FEC_CONV_V29P45,     // r4/5, K=9, dfree=5
    LIQUID_FEC_CONV_V29P56,     // r5/6, K=9, dfree=5
    LIQUID_FEC_CONV_V29P67,     // r6/7, K=9, dfree=4
    LIQUID_FEC_CONV_V29P78,     // r7/8, K=9, dfree=4

    // Reed-Solomon codes
    LIQUID_FEC_RS_M8            // m=8, n=255, k=223
} fec_scheme;

// pretty names for fec schemes
extern const char * fec_scheme_str[LIQUID_FEC_NUM_SCHEMES][2];

// Print compact list of existing and available FEC schemes
void liquid_print_fec_schemes();

// returns fec_scheme based on input string
fec_scheme liquid_getopt_str2fec(const char * _str);

// fec object (pointer to fec structure)
typedef struct fec_s * fec;

// return the encoded message length using a particular error-
// correction scheme (object-independent method)
//  _scheme     :   forward error-correction scheme
//  _msg_len    :   raw, uncoded message length
unsigned int fec_get_enc_msg_length(fec_scheme _scheme,
                                    unsigned int _msg_len);

// get the theoretical rate of a particular forward error-
// correction scheme (object-independent method)
float fec_get_rate(fec_scheme _scheme);

// create a fec object of a particular scheme
//  _scheme     :   error-correction scheme
//  _opts       :   (ignored)
fec fec_create(fec_scheme _scheme,
               void *_opts);

// recreate fec object
//  _q          :   old fec object
//  _scheme     :   new error-correction scheme
//  _opts       :   (ignored)
fec fec_recreate(fec _q,
                 fec_scheme _scheme,
                 void *_opts);

// destroy fec object
void fec_destroy(fec _q);

// print fec object internals
void fec_print(fec _q);

// encode a block of data using a fec scheme
//  _q              :   fec object
//  _dec_msg_len    :   decoded message length
//  _msg_dec        :   decoded message
//  _msg_enc        :   encoded message
void fec_encode(fec _q,
                unsigned int _dec_msg_len,
                unsigned char * _msg_dec,
                unsigned char * _msg_enc);

// decode a block of data using a fec scheme
//  _q              :   fec object
//  _dec_msg_len    :   decoded message length
//  _msg_enc        :   encoded message
//  _msg_dec        :   decoded message
void fec_decode(fec _q,
                unsigned int _dec_msg_len,
                unsigned char * _msg_enc,
                unsigned char * _msg_dec);

// decode a block of data using a fec scheme (soft decision)
//  _q              :   fec object
//  _dec_msg_len    :   decoded message length
//  _msg_enc        :   encoded message (soft bits)
//  _msg_dec        :   decoded message
void fec_decode_soft(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_enc,
                     unsigned char * _msg_dec);

// 
// Packetizer
//

// computes the number of encoded bytes after packetizing
//
//  _n      :   number of uncoded input bytes
//  _crc    :   error-detecting scheme
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
unsigned int packetizer_compute_enc_msg_len(unsigned int _n,
                                            int _crc,
                                            int _fec0,
                                            int _fec1);

// computes the number of decoded bytes before packetizing
//
//  _k      :   number of encoded bytes
//  _crc    :   error-detecting scheme
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
unsigned int packetizer_compute_dec_msg_len(unsigned int _k,
                                            int _crc,
                                            int _fec0,
                                            int _fec1);

typedef struct packetizer_s * packetizer;

// create packetizer object
//
//  _n      :   number of uncoded input bytes
//  _crc    :   error-detecting scheme
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
packetizer packetizer_create(unsigned int _dec_msg_len,
                             int _crc,
                             int _fec0,
                             int _fec1);

// re-create packetizer object
//
//  _p      :   initialz packetizer object
//  _n      :   number of uncoded input bytes
//  _crc    :   error-detecting scheme
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
packetizer packetizer_recreate(packetizer _p,
                               unsigned int _dec_msg_len,
                               int _crc,
                               int _fec0,
                               int _fec1);

// destroy packetizer object
void packetizer_destroy(packetizer _p);

// print packetizer object internals
void packetizer_print(packetizer _p);

unsigned int packetizer_get_dec_msg_len(packetizer _p);
unsigned int packetizer_get_enc_msg_len(packetizer _p);


// packetizer_encode()
//
// Execute the packetizer on an input message
//
//  _p      :   packetizer object
//  _msg    :   input message (uncoded bytes)
//  _pkt    :   encoded output message
void packetizer_encode(packetizer _p,
                       unsigned char * _msg,
                       unsigned char * _pkt);

// packetizer_decode()
//
// Execute the packetizer to decode an input message, return validity
// check of resulting data
//
//  _p      :   packetizer object
//  _pkt    :   input message (coded bytes)
//  _msg    :   decoded output message
int  packetizer_decode(packetizer _p,
                       unsigned char * _pkt,
                       unsigned char * _msg);

// Execute the packetizer to decode an input message, return validity
// check of resulting data
//
//  _p      :   packetizer object
//  _pkt    :   input message (coded soft bits)
//  _msg    :   decoded output message
int  packetizer_decode_soft(packetizer _p,
                            unsigned char * _pkt,
                            unsigned char * _msg);


//
// interleaver
//
typedef struct interleaver_s * interleaver;

// create interleaver
//   _n     : number of bytes
interleaver interleaver_create(unsigned int _n);

// destroy interleaver object
void interleaver_destroy(interleaver _q);

// print interleaver object internals
void interleaver_print(interleaver _q);

// set depth (number of internal iterations)
//  _q      :   interleaver object
//  _depth  :   depth
void interleaver_set_depth(interleaver _q,
                           unsigned int _depth);

// execute forward interleaver (encoder)
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode(interleaver _q,
                        unsigned char * _msg_dec,
                        unsigned char * _msg_enc);

// execute forward interleaver (encoder) on soft bits
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode_soft(interleaver _q,
                             unsigned char * _msg_dec,
                             unsigned char * _msg_enc);

// execute reverse interleaver (decoder)
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode(interleaver _q,
                        unsigned char * _msg_enc,
                        unsigned char * _msg_dec);

// execute reverse interleaver (decoder) on soft bits
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode_soft(interleaver _q,
                             unsigned char * _msg_enc,
                             unsigned char * _msg_dec);



//
// MODULE : fft (fast Fourier transform)
//

// type of transform
typedef enum {
    LIQUID_FFT_UNKNOWN  =   0,  // unknown transform type

    // regular complex one-dimensional transforms
    LIQUID_FFT_FORWARD  =  +1,  // complex one-dimensional FFT 
    LIQUID_FFT_BACKWARD =  -1,  // complex one-dimensional inverse FFT 

    // discrete cosine transforms
    LIQUID_FFT_REDFT00  =  10,  // real one-dimensional DCT-I
    LIQUID_FFT_REDFT10  =  11,  // real one-dimensional DCT-II
    LIQUID_FFT_REDFT01  =  12,  // real one-dimensional DCT-III
    LIQUID_FFT_REDFT11  =  13,  // real one-dimensional DCT-IV

    // discrete sine transforms
    LIQUID_FFT_RODFT00  =  20,  // real one-dimensional DST-I
    LIQUID_FFT_RODFT10  =  21,  // real one-dimensional DST-II
    LIQUID_FFT_RODFT01  =  22,  // real one-dimensional DST-III
    LIQUID_FFT_RODFT11  =  23,  // real one-dimensional DST-IV

    // modified discrete cosine transform
    LIQUID_FFT_MDCT     =  30,  // MDCT
    LIQUID_FFT_IMDCT    =  31,  // IMDCT
} liquid_fft_type;

#define LIQUID_FFT_MANGLE_FLOAT(name)   LIQUID_CONCAT(fft,name)

// Macro    :   FFT
//  FFT     :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
#define LIQUID_FFT_DEFINE_API(FFT,T,TC)                         \
                                                                \
typedef struct FFT(plan_s) * FFT(plan);                         \
                                                                \
/* create regular complex one-dimensional transform         */  \
/*  _n      :   transform size                              */  \
/*  _x      :   pointer to input array  [size: _n x 1]      */  \
/*  _y      :   pointer to output array [size: _n x 1]      */  \
/*  _dir    :   direction (e.g. LIQUID_FFT_FORWARD)         */  \
/*  _flags  :   options, optimization                       */  \
FFT(plan) FFT(_create_plan)(unsigned int _n,                    \
                            TC *         _x,                    \
                            TC *         _y,                    \
                            int          _dir,                  \
                            int          _flags);               \
                                                                \
/* create real-to-real transform                            */  \
/*  _n      :   transform size                              */  \
/*  _x      :   pointer to input array  [size: _n x 1]      */  \
/*  _y      :   pointer to output array [size: _n x 1]      */  \
/*  _type   :   transform type (e.g. LIQUID_FFT_REDFT00)    */  \
/*  _flags  :   options, optimization                       */  \
FFT(plan) FFT(_create_plan_r2r_1d)(unsigned int _n,             \
                                   T *          _x,             \
                                   T *          _y,             \
                                   int          _type,          \
                                   int          _flags);        \
                                                                \
/* destroy transform                                        */  \
void FFT(_destroy_plan)(FFT(plan) _p);                          \
                                                                \
/* print transform plan and internal strategy               */  \
void FFT(_print_plan)(FFT(plan) _p);                            \
                                                                \
/* run the transform                                        */  \
void FFT(_execute)(FFT(plan) _p);                               \
                                                                \
/* object-independent methods */                                \
                                                                \
/* perform n-point FFT allocating plan internally           */  \
/*  _nfft   : fft size                                      */  \
/*  _x      : input array [size: _nfft x 1]                 */  \
/*  _y      : output array [size: _nfft x 1]                */  \
/*  _dir    : fft direction: LIQUID_FFT_{FORWARD,BACKWARD}  */  \
/*  _flags  : fft flags                                     */  \
void FFT(_run)(unsigned int _n,                                 \
               TC *         _x,                                 \
               TC *         _y,                                 \
               int          _dir,                               \
               int          _flags);                            \
                                                                \
/* perform n-point real FFT allocating plan internally      */  \
/*  _nfft   : fft size                                      */  \
/*  _x      : input array [size: _nfft x 1]                 */  \
/*  _y      : output array [size: _nfft x 1]                */  \
/*  _type   : fft type, e.g. LIQUID_FFT_REDFT10             */  \
/*  _flags  : fft flags                                     */  \
void FFT(_r2r_1d_run)(unsigned int _n,                          \
                      T *          _x,                          \
                      T *          _y,                          \
                      int          _type,                       \
                      int          _flags);                     \
                                                                \
/* perform _n-point fft shift                               */  \
void FFT(_shift)(TC *         _x,                               \
                 unsigned int _n);                              \


LIQUID_FFT_DEFINE_API(LIQUID_FFT_MANGLE_FLOAT,float,liquid_float_complex)

// antiquated fft methods
// FFT(plan) FFT(_create_plan_mdct)(unsigned int _n,
//                                  T * _x,
//                                  T * _y,
//                                  int _kind,
//                                  int _flags);


// 
// spectral periodogram
//

#define LIQUID_SPGRAM_MANGLE_CFLOAT(name) LIQUID_CONCAT(spgramcf,name)
#define LIQUID_SPGRAM_MANGLE_FLOAT(name)  LIQUID_CONCAT(spgramf, name)

// Macro    :   SPGRAM
//  SPGRAM  :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
//  TI      :   primitive data type (input)
#define LIQUID_SPGRAM_DEFINE_API(SPGRAM,T,TC,TI)                \
                                                                \
typedef struct SPGRAM(_s) * SPGRAM();                           \
                                                                \
/* create spgram object                                     */  \
/*  _nfft       :   FFT size                                */  \
/*  _window     :   window [size: _window_len x 1]          */  \
/*  _window_len :   window length                           */  \
SPGRAM() SPGRAM(_create)(unsigned int _nfft,                    \
                         float *      _window,                  \
                         unsigned int _window_len);             \
                                                                \
/* create spgram object with Kaiser-Bessel window           */  \
/*  _nfft       :   FFT size                                */  \
/*  _window_len :   window length                           */  \
/*  _beta       :   Kaiser-Bessel parameter (_beta > 0)     */  \
SPGRAM() SPGRAM(_create_kaiser)(unsigned int _nfft,             \
                                unsigned int _window_len,       \
                                float        _beta);            \
                                                                \
/* destroy spgram object                                    */  \
void SPGRAM(_destroy)(SPGRAM() _q);                             \
                                                                \
/* resets the internal state of the spgram object           */  \
void SPGRAM(_reset)(SPGRAM() _q);                               \
                                                                \
/* push samples into spgram object                          */  \
/*  _q      :   spgram object                               */  \
/*  _x      :   input buffer [size: _n x 1]                 */  \
/*  _n      :   input buffer length                         */  \
void SPGRAM(_push)(SPGRAM()     _q,                             \
                   TI *         _x,                             \
                   unsigned int _n);                            \
                                                                \
/* compute spectral periodogram output from current buffer  */  \
/* contents                                                 */  \
/*  _q      :   spgram object                               */  \
/*  _X      :   output complex spectrum [size: _nfft x 1]   */  \
void SPGRAM(_execute)(SPGRAM() _q,                              \
                      TC *     _X);                             \
                                                                \
/* accumulate power spectral density                        */  \
/*  _q      :   spgram object                               */  \
/*  _x      :   input buffer [size: _n x 1]                 */  \
/*  _n      :   input buffer length                         */  \
void SPGRAM(_accumulate_psd)(SPGRAM()       _q,                 \
                             TI *           _x,                 \
                             unsigned int   _n);                \
                                                                \
/* write accumulated psd                                    */  \
/*  _q      :   spgram object                               */  \
/*  _x      :   input buffer [size: _n x 1]                 */  \
/*  _n      :   input buffer length [size: _nfft x 1]       */  \
void SPGRAM(_write_accumulation)(SPGRAM() _q,                   \
                                 T *      _x);                  \
                                                                \
/* estimate spectrum on input signal                        */  \
/*  _q      :   spgram object                               */  \
/*  _x      :   input signal [size: _n x 1]                 */  \
/*  _n      :   input signal length                         */  \
/*  _psd    :   output spectrum, [size: _nfft x 1]          */  \
void SPGRAM(_estimate_psd)(SPGRAM()     _q,                     \
                           TI *         _x,                     \
                           unsigned int _n,                     \
                           T *          _psd);                  \

LIQUID_SPGRAM_DEFINE_API(LIQUID_SPGRAM_MANGLE_CFLOAT,
                         float,
                         liquid_float_complex,
                         liquid_float_complex)

LIQUID_SPGRAM_DEFINE_API(LIQUID_SPGRAM_MANGLE_FLOAT,
                         float,
                         liquid_float_complex,
                         float)


// ascii spectrogram
typedef struct asgram_s * asgram;

// create asgram object
//  _nfft       :   FFT size
asgram asgram_create(unsigned int _nfft);
void asgram_destroy(asgram _q);
void asgram_reset(asgram _q);
void asgram_set_scale(asgram _q, float _offset, float _scale);

// push samples into asgram object
//  _q      :   asgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void asgram_push(asgram                 _q,
                 liquid_float_complex * _x,
                 unsigned int           _n);

// execute asgram
//  _q          :   asgram object
//  _ascii      :   ASCII character output buffer [size: _nfft x 1]
//  _peakval    :   peak PSD value [dB]
//  _peakfreq   :   normalized frequency of peak PSD value
void asgram_execute(asgram  _q,
                    char *  _ascii,
                    float * _peakval,
                    float * _peakfreq);

//
// MODULE : filter
//

//
// firdes: finite impulse response filter design
//

// prototypes
typedef enum {
    LIQUID_FIRFILT_UNKNOWN=0,   // unknown filter type

    // Nyquist filter prototypes
    LIQUID_FIRFILT_KAISER,      // Nyquist Kaiser filter
    LIQUID_FIRFILT_PM,          // Parks-McClellan filter
    LIQUID_FIRFILT_RCOS,        // raised-cosine filter
    LIQUID_FIRFILT_FEXP,        // flipped exponential
    LIQUID_FIRFILT_FSECH,       // flipped hyperbolic secant
    LIQUID_FIRFILT_FARCSECH,    // flipped arc-hyperbolic secant

    // root-Nyquist filter prototypes
    LIQUID_FIRFILT_ARKAISER,    // root-Nyquist Kaiser (approximate optimum)
    LIQUID_FIRFILT_RKAISER,     // root-Nyquist Kaiser (true optimum)
    LIQUID_FIRFILT_RRC,         // root raised-cosine
    LIQUID_FIRFILT_hM3,         // harris-Moerder-3 filter
    LIQUID_FIRFILT_GMSKTX,      // GMSK transmit filter
    LIQUID_FIRFILT_GMSKRX,      // GMSK receive filter
    LIQUID_FIRFILT_RFEXP,       // flipped exponential
    LIQUID_FIRFILT_RFSECH,      // flipped hyperbolic secant
    LIQUID_FIRFILT_RFARCSECH,   // flipped arc-hyperbolic secant
} liquid_firfilt_type;

// returns filter type based on input string
int liquid_getopt_str2firfilt(const char * _str);

// estimate required filter length given
//  _df     :   transition bandwidth (0 < _b < 0.5)
//  _As     :   stop-band attenuation [dB], _As > 0
unsigned int estimate_req_filter_len(float _df,
                                     float _As);

// estimate filter stop-band attenuation given
//  _df     :   transition bandwidth (0 < _b < 0.5)
//  _N      :   filter length
float estimate_req_filter_As(float _df,
                             unsigned int _N);

// estimate filter transition bandwidth given
//  _As     :   stop-band attenuation [dB], _As > 0
//  _N      :   filter length
float estimate_req_filter_df(float _As,
                             unsigned int _N);


// returns the Kaiser window beta factor give the filter's target
// stop-band attenuation (As) [Vaidyanathan:1993]
//  _As     :   target filter's stop-band attenuation [dB], _As > 0
float kaiser_beta_As(float _As);


// Design Nyquist filter
//  _type   : filter type (e.g. LIQUID_FIRFILT_RCOS)
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : excess bandwidth factor, _beta in [0,1]
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_nyquist(liquid_firfilt_type _type,
                           unsigned int        _k,
                           unsigned int        _m,
                           float               _beta,
                           float               _dt,
                           float *             _h);


// Design FIR filter using Parks-McClellan algorithm

// band type specifier
typedef enum {
    LIQUID_FIRDESPM_BANDPASS=0,     // regular band-pass filter
    LIQUID_FIRDESPM_DIFFERENTIATOR, // differentiating filter
    LIQUID_FIRDESPM_HILBERT         // Hilbert transform
} liquid_firdespm_btype;

// weighting type specifier
typedef enum {
    LIQUID_FIRDESPM_FLATWEIGHT=0,   // flat weighting
    LIQUID_FIRDESPM_EXPWEIGHT,      // exponential weighting
    LIQUID_FIRDESPM_LINWEIGHT,      // linear weighting
} liquid_firdespm_wtype;

// run filter design (full life cycle of object)
//  _h_len      :   length of filter (number of taps)
//  _num_bands  :   number of frequency bands
//  _bands      :   band edges, f in [0,0.5], [size: _num_bands x 2]
//  _des        :   desired response [size: _num_bands x 1]
//  _weights    :   response weighting [size: _num_bands x 1]
//  _wtype      :   weight types (e.g. LIQUID_FIRDESPM_FLATWEIGHT) [size: _num_bands x 1]
//  _btype      :   band type (e.g. LIQUID_FIRDESPM_BANDPASS)
//  _h          :   output coefficients array [size: _h_len x 1]
void firdespm_run(unsigned int _h_len,
                  unsigned int _num_bands,
                  float * _bands,
                  float * _des,
                  float * _weights,
                  liquid_firdespm_wtype * _wtype,
                  liquid_firdespm_btype _btype,
                  float * _h);

// structured object
typedef struct firdespm_s * firdespm;

// create firdespm object
//  _h_len      :   length of filter (number of taps)
//  _num_bands  :   number of frequency bands
//  _bands      :   band edges, f in [0,0.5], [size: _num_bands x 2]
//  _des        :   desired response [size: _num_bands x 1]
//  _weights    :   response weighting [size: _num_bands x 1]
//  _wtype      :   weight types (e.g. LIQUID_FIRDESPM_FLATWEIGHT) [size: _num_bands x 1]
//  _btype      :   band type (e.g. LIQUID_FIRDESPM_BANDPASS)
firdespm firdespm_create(unsigned int _h_len,
                         unsigned int _num_bands,
                         float * _bands,
                         float * _des,
                         float * _weights,
                         liquid_firdespm_wtype * _wtype,
                         liquid_firdespm_btype _btype);

// destroy firdespm object
void firdespm_destroy(firdespm _q);

// print firdespm object internals
void firdespm_print(firdespm _q);

// execute filter design, storing result in _h
void firdespm_execute(firdespm _q, float * _h);


// Design FIR using kaiser window
//  _n      : filter length, _n > 0
//  _fc     : cutoff frequency, 0 < _fc < 0.5
//  _As     : stop-band attenuation [dB], _As > 0
//  _mu     : fractional sample offset, -0.5 < _mu < 0.5
//  _h      : output coefficient buffer, [size: _n x 1]
void liquid_firdes_kaiser(unsigned int _n,
                          float _fc,
                          float _As,
                          float _mu,
                          float *_h);

// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _theta  : LoS component angle of arrival
//  _h      : output coefficient buffer
void liquid_firdes_doppler(unsigned int _n,
                           float _fd,
                           float _K,
                           float _theta,
                           float *_h);


// Design Nyquist raised-cosine filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_rcos(unsigned int _k,
                        unsigned int _m,
                        float _beta,
                        float _dt,
                        float * _h);

// Design root-Nyquist filter
//  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)
//  _k      : samples/symbol,          _k > 1
//  _m      : symbol delay,            _m > 0
//  _beta   : excess bandwidth factor, _beta in [0,1)
//  _dt     : fractional sample delay, _dt in [-1,1]
//  _h      : output coefficient buffer (length: 2*_k*_m+1)
void liquid_firdes_rnyquist(liquid_firfilt_type _type,
                            unsigned int        _k,
                            unsigned int        _m,
                            float               _beta,
                            float               _dt,
                            float *             _h);

// Design root-Nyquist raised-cosine filter
void liquid_firdes_rrcos(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design root-Nyquist Kaiser filter
void liquid_firdes_rkaiser(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design (approximate) root-Nyquist Kaiser filter
void liquid_firdes_arkaiser(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design root-Nyquist harris-Moerder filter
void liquid_firdes_hM3(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design GMSK transmit and receive filters
void liquid_firdes_gmsktx(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);
void liquid_firdes_gmskrx(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design flipped exponential Nyquist/root-Nyquist filters
void liquid_firdes_fexp( unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);
void liquid_firdes_rfexp(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design flipped hyperbolic secand Nyquist/root-Nyquist filters
void liquid_firdes_fsech( unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);
void liquid_firdes_rfsech(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Design flipped arc-hyperbolic secand Nyquist/root-Nyquist filters
void liquid_firdes_farcsech( unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);
void liquid_firdes_rfarcsech(unsigned int _k, unsigned int _m, float _beta, float _dt, float * _h);

// Compute group delay for an FIR filter
//  _h      : filter coefficients array
//  _n      : filter length
//  _fc     : frequency at which delay is evaluated (-0.5 < _fc < 0.5)
float fir_group_delay(float * _h,
                      unsigned int _n,
                      float _fc);

// Compute group delay for an IIR filter
//  _b      : filter numerator coefficients
//  _nb     : filter numerator length
//  _a      : filter denominator coefficients
//  _na     : filter denominator length
//  _fc     : frequency at which delay is evaluated (-0.5 < _fc < 0.5)
float iir_group_delay(float * _b,
                      unsigned int _nb,
                      float * _a,
                      unsigned int _na,
                      float _fc);


// liquid_filter_autocorr()
//
// Compute auto-correlation of filter at a specific lag.
//
//  _h      :   filter coefficients [size: _h_len x 1]
//  _h_len  :   filter length
//  _lag    :   auto-correlation lag (samples)
float liquid_filter_autocorr(float * _h,
                             unsigned int _h_len,
                             int _lag);

// liquid_filter_crosscorr()
//
// Compute cross-correlation of two filters at a specific lag.
//
//  _h      :   filter coefficients [size: _h_len]
//  _h_len  :   filter length
//  _g      :   filter coefficients [size: _g_len]
//  _g_len  :   filter length
//  _lag    :   cross-correlation lag (samples)
float liquid_filter_crosscorr(float * _h,
                              unsigned int _h_len,
                              float * _g,
                              unsigned int _g_len,
                              int _lag);

// liquid_filter_isi()
//
// Compute inter-symbol interference (ISI)--both RMS and
// maximum--for the filter _h.
//
//  _h      :   filter coefficients [size: 2*_k*_m+1 x 1]
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _rms    :   output root mean-squared ISI
//  _max    :   maximum ISI
void liquid_filter_isi(float * _h,
                       unsigned int _k,
                       unsigned int _m,
                       float * _rms,
                       float * _max);

// Compute relative out-of-band energy
//
//  _h      :   filter coefficients [size: _h_len x 1]
//  _h_len  :   filter length
//  _fc     :   analysis cut-off frequency
//  _nfft   :   fft size
float liquid_filter_energy(float * _h,
                           unsigned int _h_len,
                           float _fc,
                           unsigned int _nfft);


//
// IIR filter design
//

// IIR filter design filter type
typedef enum {
    LIQUID_IIRDES_BUTTER=0,
    LIQUID_IIRDES_CHEBY1,
    LIQUID_IIRDES_CHEBY2,
    LIQUID_IIRDES_ELLIP,
    LIQUID_IIRDES_BESSEL
} liquid_iirdes_filtertype;

// IIR filter design band type
typedef enum {
    LIQUID_IIRDES_LOWPASS=0,
    LIQUID_IIRDES_HIGHPASS,
    LIQUID_IIRDES_BANDPASS,
    LIQUID_IIRDES_BANDSTOP
} liquid_iirdes_bandtype;

// IIR filter design coefficients format
typedef enum {
    LIQUID_IIRDES_SOS=0,
    LIQUID_IIRDES_TF
} liquid_iirdes_format;

// IIR filter design template
//  _ftype      :   filter type (e.g. LIQUID_IIRDES_BUTTER)
//  _btype      :   band type (e.g. LIQUID_IIRDES_BANDPASS)
//  _format     :   coefficients format (e.g. LIQUID_IIRDES_SOS)
//  _n          :   filter order
//  _fc         :   low-pass prototype cut-off frequency
//  _f0         :   center frequency (band-pass, band-stop)
//  _Ap         :   pass-band ripple in dB
//  _As         :   stop-band ripple in dB
//  _B          :   numerator
//  _A          :   denominator
void liquid_iirdes(liquid_iirdes_filtertype _ftype,
                   liquid_iirdes_bandtype   _btype,
                   liquid_iirdes_format     _format,
                   unsigned int _n,
                   float _fc,
                   float _f0,
                   float _Ap,
                   float _As,
                   float * _B,
                   float * _A);

// compute analog zeros, poles, gain for specific filter types
void butter_azpkf(unsigned int _n,
                  liquid_float_complex * _za,
                  liquid_float_complex * _pa,
                  liquid_float_complex * _ka);
void cheby1_azpkf(unsigned int _n,
                  float _ep,
                  liquid_float_complex * _z,
                  liquid_float_complex * _p,
                  liquid_float_complex * _k);
void cheby2_azpkf(unsigned int _n,
                  float _es,
                  liquid_float_complex * _z,
                  liquid_float_complex * _p,
                  liquid_float_complex * _k);
void ellip_azpkf(unsigned int _n,
                 float _ep,
                 float _es,
                 liquid_float_complex * _z,
                 liquid_float_complex * _p,
                 liquid_float_complex * _k);
void bessel_azpkf(unsigned int _n,
                  liquid_float_complex * _z,
                  liquid_float_complex * _p,
                  liquid_float_complex * _k);

// compute frequency pre-warping factor
float iirdes_freqprewarp(liquid_iirdes_bandtype _btype,
                         float _fc,
                         float _f0);

// convert analog z/p/k form to discrete z/p/k form (bilinear z-transform)
//  _za     :   analog zeros [length: _nza]
//  _nza    :   number of analog zeros
//  _pa     :   analog poles [length: _npa]
//  _npa    :   number of analog poles
//  _m      :   frequency pre-warping factor
//  _zd     :   output digital zeros [length: _npa]
//  _pd     :   output digital poles [length: _npa]
//  _kd     :   output digital gain (should actually be real-valued)
void bilinear_zpkf(liquid_float_complex * _za,
                   unsigned int _nza,
                   liquid_float_complex * _pa,
                   unsigned int _npa,
                   liquid_float_complex _ka,
                   float _m,
                   liquid_float_complex * _zd,
                   liquid_float_complex * _pd,
                   liquid_float_complex * _kd);

// digital z/p/k low-pass to high-pass
//  _zd     :   digital zeros (low-pass prototype), [length: _n]
//  _pd     :   digital poles (low-pass prototype), [length: _n]
//  _n      :   low-pass filter order
//  _zdt    :   output digital zeros transformed [length: _n]
//  _pdt    :   output digital poles transformed [length: _n]
void iirdes_dzpk_lp2hp(liquid_float_complex * _zd,
                       liquid_float_complex * _pd,
                       unsigned int _n,
                       liquid_float_complex * _zdt,
                       liquid_float_complex * _pdt);

// digital z/p/k low-pass to band-pass
//  _zd     :   digital zeros (low-pass prototype), [length: _n]
//  _pd     :   digital poles (low-pass prototype), [length: _n]
//  _n      :   low-pass filter order
//  _f0     :   center frequency
//  _zdt    :   output digital zeros transformed [length: 2*_n]
//  _pdt    :   output digital poles transformed [length: 2*_n]
void iirdes_dzpk_lp2bp(liquid_float_complex * _zd,
                       liquid_float_complex * _pd,
                       unsigned int _n,
                       float _f0,
                       liquid_float_complex * _zdt,
                       liquid_float_complex * _pdt);

// convert discrete z/p/k form to transfer function
//  _zd     :   digital zeros [length: _n]
//  _pd     :   digital poles [length: _n]
//  _n      :   filter order
//  _kd     :   digital gain
//  _b      :   output numerator [length: _n+1]
//  _a      :   output denominator [length: _n+1]
void iirdes_dzpk2tff(liquid_float_complex * _zd,
                     liquid_float_complex * _pd,
                     unsigned int _n,
                     liquid_float_complex _kd,
                     float * _b,
                     float * _a);

// convert discrete z/p/k form to second-order sections
//  _zd     :   digital zeros [length: _n]
//  _pd     :   digital poles [length: _n]
//  _n      :   filter order
//  _kd     :   digital gain
//  _B      :   output numerator [size: 3 x L+r]
//  _A      :   output denominator [size: 3 x L+r]
//  where r = _n%2, L = (_n-r)/2
void iirdes_dzpk2sosf(liquid_float_complex * _zd,
                      liquid_float_complex * _pd,
                      unsigned int _n,
                      liquid_float_complex _kd,
                      float * _B,
                      float * _A);

// additional IIR filter design templates

// design 2nd-order IIR filter (active lag)
//          1 + t2 * s
//  F(s) = ------------
//          1 + t1 * s
//
//  _w      :   filter bandwidth
//  _zeta   :   damping factor (1/sqrt(2) suggested)
//  _K      :   loop gain (1000 suggested)
//  _b      :   output feed-forward coefficients [size: 3 x 1]
//  _a      :   output feed-back coefficients [size: 3 x 1]
void iirdes_pll_active_lag(float _w,
                           float _zeta,
                           float _K,
                           float * _b,
                           float * _a);

// design 2nd-order IIR filter (active PI)
//          1 + t2 * s
//  F(s) = ------------
//           t1 * s
//
//  _w      :   filter bandwidth
//  _zeta   :   damping factor (1/sqrt(2) suggested)
//  _K      :   loop gain (1000 suggested)
//  _b      :   output feed-forward coefficients [size: 3 x 1]
//  _a      :   output feed-back coefficients [size: 3 x 1]
void iirdes_pll_active_PI(float _w,
                          float _zeta,
                          float _K,
                          float * _b,
                          float * _a);

// checks stability of iir filter
//  _b      :   feed-forward coefficients [size: _n x 1]
//  _a      :   feed-back coefficients [size: _n x 1]
//  _n      :   number of coefficients
int iirdes_isstable(float * _b,
                    float * _a,
                    unsigned int _n);

//
// linear prediction
//

// compute the linear prediction coefficients for an input signal _x
//  _x      :   input signal [size: _n x 1]
//  _n      :   input signal length
//  _p      :   prediction filter order
//  _a      :   prediction filter [size: _p+1 x 1]
//  _e      :   prediction error variance [size: _p+1 x 1]
void liquid_lpc(float * _x,
                unsigned int _n,
                unsigned int _p,
                float * _a,
                float * _g);

// solve the Yule-Walker equations using Levinson-Durbin recursion
// for _symmetric_ autocorrelation
//  _r      :   autocorrelation array [size: _p+1 x 1]
//  _p      :   filter order
//  _a      :   output coefficients [size: _p+1 x 1]
//  _e      :   error variance [size: _p+1 x 1]
//
// NOTES:
//  By definition _a[0] = 1.0
void liquid_levinson(float * _r,
                     unsigned int _p,
                     float * _a,
                     float * _e);

//
// auto-correlator (delay cross-correlation)
//

#define AUTOCORR_MANGLE_CCCF(name)  LIQUID_CONCAT(autocorr_cccf,name)
#define AUTOCORR_MANGLE_RRRF(name)  LIQUID_CONCAT(autocorr_rrrf,name)

// Macro:
//   AUTOCORR   : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_AUTOCORR_DEFINE_API(AUTOCORR,TO,TC,TI)           \
                                                                \
typedef struct AUTOCORR(_s) * AUTOCORR();                       \
                                                                \
/* create auto-correlator object                            */  \
/*  _window_size    : size of the correlator window         */  \
/*  _delay          : correlator delay [samples]            */  \
AUTOCORR() AUTOCORR(_create)(unsigned int _window_size,         \
                             unsigned int _delay);              \
                                                                \
/* destroy auto-correlator object, freeing internal memory  */  \
void AUTOCORR(_destroy)(AUTOCORR() _q);                         \
                                                                \
/* reset auto-correlator object's internals                 */  \
void AUTOCORR(_reset)(AUTOCORR() _q);                           \
                                                                \
/* print auto-correlator parameters to stdout               */  \
void AUTOCORR(_print)(AUTOCORR() _q);                           \
                                                                \
/* push sample into auto-correlator object                  */  \
void AUTOCORR(_push)(AUTOCORR() _q,                             \
                     TI         _x);                            \
                                                                \
/* compute single auto-correlation output                   */  \
void AUTOCORR(_execute)(AUTOCORR() _q,                          \
                        TO *       _rxx);                       \
                                                                \
/* compute auto-correlation on block of samples; the input  */  \
/* and output arrays may have the same pointer              */  \
/*  _q      :   auto-correlation object                     */  \
/*  _x      :   input array [size: _n x 1]                  */  \
/*  _n      :   number of input, output samples             */  \
/*  _rxx    :   input array [size: _n x 1]                  */  \
void AUTOCORR(_execute_block)(AUTOCORR()   _q,                  \
                              TI *         _x,                  \
                              unsigned int _n,                  \
                              TO *         _rxx);               \
                                                                \
/* return sum of squares of buffered samples                */  \
float AUTOCORR(_get_energy)(AUTOCORR() _q);                     \

LIQUID_AUTOCORR_DEFINE_API(AUTOCORR_MANGLE_CCCF,
                           liquid_float_complex,
                           liquid_float_complex,
                           liquid_float_complex)

LIQUID_AUTOCORR_DEFINE_API(AUTOCORR_MANGLE_RRRF,
                           float,
                           float,
                           float)


//
// Finite impulse response filter
//

#define FIRFILT_MANGLE_RRRF(name)  LIQUID_CONCAT(firfilt_rrrf,name)
#define FIRFILT_MANGLE_CRCF(name)  LIQUID_CONCAT(firfilt_crcf,name)
#define FIRFILT_MANGLE_CCCF(name)  LIQUID_CONCAT(firfilt_cccf,name)

// Macro:
//   FIRFILT : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIRFILT_DEFINE_API(FIRFILT,TO,TC,TI)             \
typedef struct FIRFILT(_s) * FIRFILT();                         \
                                                                \
FIRFILT() FIRFILT(_create)(TC * _h, unsigned int _n);           \
                                                                \
/* create using Kaiser-Bessel windowed sinc method          */  \
/*  _n      : filter length, _n > 0                         */  \
/*  _fc     : filter cut-off frequency 0 < _fc < 0.5        */  \
/*  _As     : filter stop-band attenuation [dB], _As > 0    */  \
/*  _mu     : fractional sample offset, -0.5 < _mu < 0.5    */  \
FIRFILT() FIRFILT(_create_kaiser)(unsigned int _n,              \
                                  float        _fc,             \
                                  float        _As,             \
                                  float        _mu);            \
                                                                \
/* create from square-root Nyquist prototype                */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _k      : nominal samples/symbol, _k > 1                */  \
/*  _m      : filter delay [symbols], _m > 0                */  \
/*  _beta   : rolloff factor, 0 < beta <= 1                 */  \
/*  _mu     : fractional sample offset,-0.5 < _mu < 0.5     */  \
FIRFILT() FIRFILT(_create_rnyquist)(int          _type,         \
                                    unsigned int _k,            \
                                    unsigned int _m,            \
                                    float        _beta,         \
                                    float        _mu);          \
                                                                \
/* re-create filter                                         */  \
/*  _q      : original filter object                        */  \
/*  _h      : pointer to filter coefficients [size: _n x 1] */  \
/*  _n      : filter length, _n > 0                         */  \
FIRFILT() FIRFILT(_recreate)(FIRFILT()    _q,                   \
                             TC *         _h,                   \
                             unsigned int _n);                  \
                                                                \
/* destroy filter object and free all internal memory       */  \
void FIRFILT(_destroy)(FIRFILT() _q);                           \
                                                                \
/* reset filter object's internal buffer                    */  \
void FIRFILT(_reset)(FIRFILT() _q);                             \
                                                                \
/* print filter object information                          */  \
void FIRFILT(_print)(FIRFILT() _q);                             \
                                                                \
/* set output scaling for filter                            */  \
void FIRFILT(_set_scale)(FIRFILT() _q,                          \
                         TC        _g);                         \
                                                                \
/* push sample into filter object's internal buffer         */  \
/*  _q      : filter object                                 */  \
/*  _x      : single input sample                           */  \
void FIRFILT(_push)(FIRFILT() _q,                               \
                    TI        _x);                              \
                                                                \
/* execute the filter on internal buffer and coefficients   */  \
/*  _q      : filter object                                 */  \
/*  _y      : pointer to single output sample               */  \
void FIRFILT(_execute)(FIRFILT() _q,                            \
                       TO *      _y);                           \
                                                                \
/* execute the filter on a block of input samples; the      */  \
/* input and output buffers may be the same                 */  \
/*  _q      : filter object                                 */  \
/*  _x      : pointer to input array [size: _n x 1]         */  \
/*  _n      : number of input, output samples               */  \
/*  _y      : pointer to output array [size: _n x 1]        */  \
void FIRFILT(_execute_block)(FIRFILT()    _q,                   \
                             TI *         _x,                   \
                             unsigned int _n,                   \
                             TO *         _y);                  \
                                                                \
/* return length of filter object                           */  \
unsigned int FIRFILT(_get_length)(FIRFILT() _q);                \
                                                                \
/* compute complex frequency response of filter object      */  \
/*  _q      : filter object                                 */  \
/*  _fc     : frequency to evaluate                         */  \
/*  _H      : pointer to output complex frequency response  */  \
void FIRFILT(_freqresponse)(FIRFILT()              _q,          \
                            float                  _fc,         \
                            liquid_float_complex * _H);         \
                                                                \
/* compute and return group delay of filter object          */  \
/*  _q      : filter object                                 */  \
/*  _fc     : frequency to evaluate                         */  \
float FIRFILT(_groupdelay)(FIRFILT() _q,                        \
                           float     _fc);                      \

LIQUID_FIRFILT_DEFINE_API(FIRFILT_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_FIRFILT_DEFINE_API(FIRFILT_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

LIQUID_FIRFILT_DEFINE_API(FIRFILT_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)

//
// FIR Hilbert transform
//  2:1 real-to-complex decimator
//  1:2 complex-to-real interpolator
//

#define FIRHILB_MANGLE_FLOAT(name)  LIQUID_CONCAT(firhilbf, name)
//#define FIRHILB_MANGLE_DOUBLE(name) LIQUID_CONCAT(firhilb, name)

// NOTES:
//   Although firhilb is a placeholder for both decimation and
//   interpolation, separate objects should be used for each task.
#define LIQUID_FIRHILB_DEFINE_API(FIRHILB,T,TC)                 \
typedef struct FIRHILB(_s) * FIRHILB();                         \
                                                                \
/* create finite impulse reponse Hilbert transform          */  \
/*  _m      : filter semi-length, delay is 2*m+1            */  \
/*  _As     : filter stop-band attenuation [dB]             */  \
FIRHILB() FIRHILB(_create)(unsigned int _m,                     \
                           float        _As);                   \
                                                                \
/* destroy finite impulse reponse Hilbert transform         */  \
void FIRHILB(_destroy)(FIRHILB() _q);                           \
                                                                \
/* print firhilb object internals to stdout                 */  \
void FIRHILB(_print)(FIRHILB() _q);                             \
                                                                \
/* reset firhilb object internal state                      */  \
void FIRHILB(_reset)(FIRHILB() _q);                             \
                                                                \
/* execute Hilbert transform (real to complex)              */  \
/*  _q      :   Hilbert transform object                    */  \
/*  _x      :   real-valued input sample                    */  \
/*  _y      :   complex-valued output sample                */  \
void FIRHILB(_r2c_execute)(FIRHILB() _q,                        \
                           T         _x,                        \
                           TC *      _y);                       \
                                                                \
/* execute Hilbert transform (complex to real)              */  \
/*  _q      :   Hilbert transform object                    */  \
/*  _x      :   complex-valued input sample                 */  \
/*  _y      :   real-valued output sample                   */  \
void FIRHILB(_c2r_execute)(FIRHILB() _q,                        \
                           TC        _x,                        \
                           T *       _y);                       \
                                                                \
/* execute Hilbert transform decimator (real to complex)    */  \
/*  _q      :   Hilbert transform object                    */  \
/*  _x      :   real-valued input array [size: 2 x 1]       */  \
/*  _y      :   complex-valued output sample                */  \
void FIRHILB(_decim_execute)(FIRHILB() _q,                      \
                             T *       _x,                      \
                             TC *      _y);                     \
                                                                \
/* execute Hilbert transform interpolator (real to complex) */  \
/*  _q      :   Hilbert transform object                    */  \
/*  _x      :   complex-valued input sample                 */  \
/*  _y      :   real-valued output array [size: 2 x 1]      */  \
void FIRHILB(_interp_execute)(FIRHILB() _q,                     \
                              TC        _x,                     \
                              T *       _y);                    \

LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_FLOAT, float, liquid_float_complex)
//LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_DOUBLE, double, liquid_double_complex)


//
// FFT-based finite impulse response filter
//

#define FFTFILT_MANGLE_RRRF(name)  LIQUID_CONCAT(fftfilt_rrrf,name)
#define FFTFILT_MANGLE_CRCF(name)  LIQUID_CONCAT(fftfilt_crcf,name)
#define FFTFILT_MANGLE_CCCF(name)  LIQUID_CONCAT(fftfilt_cccf,name)

// Macro:
//   FFTFILT : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FFTFILT_DEFINE_API(FFTFILT,TO,TC,TI)             \
typedef struct FFTFILT(_s) * FFTFILT();                         \
                                                                \
/* create FFT-based FIR filter using external coefficients  */  \
/*  _h      : filter coefficients [size: _h_len x 1]        */  \
/*  _h_len  : filter length, _h_len > 0                     */  \
/*  _n      : block size = nfft/2, at least _h_len-1        */  \
FFTFILT() FFTFILT(_create)(TC *         _h,                     \
                           unsigned int _h_len,                 \
                           unsigned int _n);                    \
                                                                \
/* destroy filter object and free all internal memory       */  \
void FFTFILT(_destroy)(FFTFILT() _q);                           \
                                                                \
/* reset filter object's internal buffer                    */  \
void FFTFILT(_reset)(FFTFILT() _q);                             \
                                                                \
/* print filter object information                          */  \
void FFTFILT(_print)(FFTFILT() _q);                             \
                                                                \
/* set output scaling for filter                            */  \
void FFTFILT(_set_scale)(FFTFILT() _q,                          \
                         TC        _g);                         \
                                                                \
/* execute the filter on internal buffer and coefficients   */  \
/*  _q      : filter object                                 */  \
/*  _x      : pointer to input data array  [size: _n x 1]   */  \
/*  _y      : pointer to output data array [size: _n x 1]   */  \
void FFTFILT(_execute)(FFTFILT() _q,                            \
                       TI *      _x,                            \
                       TO *      _y);                           \
                                                                \
/* return length of filter object's internal coefficients   */  \
unsigned int FFTFILT(_get_length)(FFTFILT() _q);                \

LIQUID_FFTFILT_DEFINE_API(FFTFILT_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_FFTFILT_DEFINE_API(FFTFILT_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

LIQUID_FFTFILT_DEFINE_API(FFTFILT_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)


//
// Infinite impulse response filter
//

#define IIRFILT_MANGLE_RRRF(name)  LIQUID_CONCAT(iirfilt_rrrf,name)
#define IIRFILT_MANGLE_CRCF(name)  LIQUID_CONCAT(iirfilt_crcf,name)
#define IIRFILT_MANGLE_CCCF(name)  LIQUID_CONCAT(iirfilt_cccf,name)

// Macro:
//   IIRFILT : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_IIRFILT_DEFINE_API(IIRFILT,TO,TC,TI)             \
                                                                \
typedef struct IIRFILT(_s) * IIRFILT();                         \
                                                                \
/* create infinite impulse reponse filter                   */  \
/*  _b      : feed-forward coefficients [size: _nb x 1]     */  \
/*  _nb     : number of feed-forward coefficients           */  \
/*  _a      : feed-back coefficients [size: _na x 1]        */  \
/*  _na     : number of feed-back coefficients              */  \
IIRFILT() IIRFILT(_create)(TC *         _b,                     \
                           unsigned int _nb,                    \
                           TC *         _a,                     \
                           unsigned int _na);                   \
                                                                \
/* create IIR filter using 2nd-order secitons               */  \
/*  _B      : feed-forward coefficients [size: _nsos x 3]   */  \
/*  _A      : feed-back coefficients    [size: _nsos x 3]   */  \
IIRFILT() IIRFILT(_create_sos)(TC *         _B,                 \
                               TC *         _A,                 \
                               unsigned int _nsos);             \
                                                                \
/* create IIR filter from design template                   */  \
/*  _ftype  : filter type (e.g. LIQUID_IIRDES_BUTTER)       */  \
/*  _btype  : band type (e.g. LIQUID_IIRDES_BANDPASS)       */  \
/*  _format : coefficients format (e.g. LIQUID_IIRDES_SOS)  */  \
/*  _n      : filter order                                  */  \
/*  _fc     : low-pass prototype cut-off frequency          */  \
/*  _f0     : center frequency (band-pass, band-stop)       */  \
/*  _Ap     : pass-band ripple in dB                        */  \
/*  _As     : stop-band ripple in dB                        */  \
IIRFILT() IIRFILT(_create_prototype)(                           \
            liquid_iirdes_filtertype _ftype,                    \
            liquid_iirdes_bandtype   _btype,                    \
            liquid_iirdes_format     _format,                   \
            unsigned int             _order,                    \
            float _fc,                                          \
            float _f0,                                          \
            float _Ap,                                          \
            float _As);                                         \
                                                                \
/* create 8th-order integrator filter                       */  \
IIRFILT() IIRFILT(_create_integrator)();                        \
                                                                \
/* create 8th-order differentiator filter                   */  \
IIRFILT() IIRFILT(_create_differentiator)();                    \
                                                                \
/* create simple DC-blocking filter                         */  \
IIRFILT() IIRFILT(_create_dc_blocker)(float _alpha);            \
                                                                \
/* create phase-locked loop iirfilt object                  */  \
/*  _w      : filter bandwidth                              */  \
/*  _zeta   : damping factor (1/sqrt(2) suggested)          */  \
/*  _K      : loop gain (1000 suggested)                    */  \
IIRFILT() IIRFILT(_create_pll)(float _w,                        \
                               float _zeta,                     \
                               float _K);                       \
                                                                \
/* destroy iirfilt object, freeing all internal memory      */  \
void IIRFILT(_destroy)(IIRFILT() _q);                           \
                                                                \
/* print iirfilt object properties to stdout                */  \
void IIRFILT(_print)(IIRFILT() _q);                             \
                                                                \
/* clear/reset iirfilt object internals                     */  \
void IIRFILT(_reset)(IIRFILT() _q);                             \
                                                                \
/* compute filter output                                    */  \
/*  _q      : iirfilt object                                */  \
/*  _x      : input sample                                  */  \
/*  _y      : output sample pointer                         */  \
void IIRFILT(_execute)(IIRFILT() _q,                            \
                       TI        _x,                            \
                       TO *      _y);                           \
                                                                \
/* execute the filter on a block of input samples; the      */  \
/* input and output buffers may be the same                 */  \
/*  _q      : filter object                                 */  \
/*  _x      : pointer to input array [size: _n x 1]         */  \
/*  _n      : number of input, output samples               */  \
/*  _y      : pointer to output array [size: _n x 1]        */  \
void IIRFILT(_execute_block)(IIRFILT()    _q,                   \
                             TI *         _x,                   \
                             unsigned int _n,                   \
                             TO *         _y);                  \
                                                                \
/* return iirfilt object's filter length (order + 1)        */  \
unsigned int IIRFILT(_get_length)(IIRFILT() _q);                \
                                                                \
/* compute complex frequency response of filter object      */  \
/*  _q      : filter object                                 */  \
/*  _fc     : frequency to evaluate                         */  \
/*  _H      : pointer to output complex frequency response  */  \
void IIRFILT(_freqresponse)(IIRFILT()              _q,          \
                            float                  _fc,         \
                            liquid_float_complex * _H);         \
                                                                \
/* compute and return group delay of filter object          */  \
/*  _q      : filter object                                 */  \
/*  _fc     : frequency to evaluate                         */  \
float IIRFILT(_groupdelay)(IIRFILT() _q, float _fc);            \

LIQUID_IIRFILT_DEFINE_API(IIRFILT_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_IIRFILT_DEFINE_API(IIRFILT_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

LIQUID_IIRFILT_DEFINE_API(IIRFILT_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)


//
// FIR Polyphase filter bank
//
#define FIRPFB_MANGLE_RRRF(name)  LIQUID_CONCAT(firpfb_rrrf,name)
#define FIRPFB_MANGLE_CRCF(name)  LIQUID_CONCAT(firpfb_crcf,name)
#define FIRPFB_MANGLE_CCCF(name)  LIQUID_CONCAT(firpfb_cccf,name)

// Macro:
//   FIRPFB : name-mangling macro
//   TO     : output data type
//   TC     : coefficients data type
//   TI     : input data type
#define LIQUID_FIRPFB_DEFINE_API(FIRPFB,TO,TC,TI)               \
                                                                \
typedef struct FIRPFB(_s) * FIRPFB();                           \
                                                                \
/* create firpfb from external coefficients                 */  \
/*  _M      : number of filters in the bank                 */  \
/*  _h      : coefficients [size: _M*_h_len x 1]            */  \
/*  _h_len  : filter delay (symbols)                        */  \
FIRPFB() FIRPFB(_create)(unsigned int _M,                       \
                         TC *         _h,                       \
                         unsigned int _h_len);                  \
                                                                \
/* create firpfb from square-root Nyquist prototype         */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _npfb   : number of filters in the bank                 */  \
/*  _k      : nominal samples/symbol                        */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _beta   : rolloff factor (0 < beta <= 1)                */  \
FIRPFB() FIRPFB(_create_rnyquist)(int          _type,           \
                                  unsigned int _npfb,           \
                                  unsigned int _k,              \
                                  unsigned int _m,              \
                                  float        _beta);          \
                                                                \
/* create from square-root derivative Nyquist prototype     */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _npfb   : number of filters in the bank                 */  \
/*  _k      : nominal samples/symbol                        */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _beta   : rolloff factor (0 < beta <= 1)                */  \
FIRPFB() FIRPFB(_create_drnyquist)(int          _type,          \
                                   unsigned int _npfb,          \
                                   unsigned int _k,             \
                                   unsigned int _m,             \
                                   float        _beta);         \
                                                                \
/* re-create filterbank object                              */  \
/*  _q      : original firpfb object                        */  \
/*  _M      : number of filters in the bank                 */  \
/*  _h      : coefficients [size: _M x _h_len]              */  \
/*  _h_len  : length of each filter                         */  \
FIRPFB() FIRPFB(_recreate)(FIRPFB()     _q,                     \
                           unsigned int _M,                     \
                           TC *         _h,                     \
                           unsigned int _h_len);                \
                                                                \
/* destroy firpfb object, freeing all internal memory       */  \
void FIRPFB(_destroy)(FIRPFB() _q);                             \
                                                                \
/* print firpfb object's parameters                         */  \
void FIRPFB(_print)(FIRPFB() _q);                               \
                                                                \
/* clear/reset firpfb object internal state                 */  \
void FIRPFB(_reset)(FIRPFB() _q);                               \
                                                                \
/* push sample into firpfb internal buffer                  */  \
void FIRPFB(_push)(FIRPFB() _q, TI _x);                         \
                                                                \
/* execute the filter on internal buffer and coefficients   */  \
/*  _q      : firpfb object                                 */  \
/*  _i      : index of filter to use                        */  \
/*  _y      : pointer to output sample                      */  \
void FIRPFB(_execute)(FIRPFB()     _q,                          \
                      unsigned int _i,                          \
                      TO *         _y);                         \

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_RRRF,
                         float,
                         float,
                         float)

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CRCF,
                         liquid_float_complex,
                         float,
                         liquid_float_complex)

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CCCF,
                         liquid_float_complex,
                         liquid_float_complex,
                         liquid_float_complex)

// 
// Interpolators
//

// firinterp : finite impulse response interpolator
#define FIRINTERP_MANGLE_RRRF(name)  LIQUID_CONCAT(firinterp_rrrf,name)
#define FIRINTERP_MANGLE_CRCF(name)  LIQUID_CONCAT(firinterp_crcf,name)
#define FIRINTERP_MANGLE_CCCF(name)  LIQUID_CONCAT(firinterp_cccf,name)

#define LIQUID_FIRINTERP_DEFINE_API(FIRINTERP,TO,TC,TI)         \
                                                                \
typedef struct FIRINTERP(_s) * FIRINTERP();                     \
                                                                \
/* create interpolator from external coefficients           */  \
/*  _M      : interpolation factor                          */  \
/*  _h      : filter coefficients [size: _h_len x 1]        */  \
/*  _h_len  : filter length                                 */  \
FIRINTERP() FIRINTERP(_create)(unsigned int _M,                 \
                               TC *         _h,                 \
                               unsigned int _h_len);            \
                                                                \
/* create interpolator from prototype                       */  \
/*  _M      : interpolation factor                          */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _As     : stop-band attenuation [dB]                    */  \
FIRINTERP() FIRINTERP(_create_prototype)(unsigned int _M,       \
                                         unsigned int _m,       \
                                         float        _As);     \
                                                                \
/* create Nyquist interpolator                              */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RCOS)        */  \
/*  _k      :   samples/symbol,          _k > 1             */  \
/*  _m      :   filter delay (symbols),  _m > 0             */  \
/*  _beta   :   excess bandwidth factor, _beta < 1          */  \
/*  _dt     :   fractional sample delay, _dt in (-1, 1)     */  \
FIRINTERP() FIRINTERP(_create_nyquist)(int          _type,      \
                                       unsigned int _k,         \
                                       unsigned int _m,         \
                                       float        _beta,      \
                                       float        _dt);       \
                                                                \
/* create square-root Nyquist interpolator                  */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _k      :   samples/symbol,          _k > 1             */  \
/*  _m      :   filter delay (symbols),  _m > 0             */  \
/*  _beta   :   excess bandwidth factor, _beta < 1          */  \
/*  _dt     :   fractional sample delay, _dt in (-1, 1)     */  \
FIRINTERP() FIRINTERP(_create_rnyquist)(int          _type,     \
                                        unsigned int _k,        \
                                        unsigned int _m,        \
                                        float        _beta,     \
                                        float        _dt);      \
                                                                \
/* destroy firinterp object, freeing all internal memory    */  \
void FIRINTERP(_destroy)(FIRINTERP() _q);                       \
                                                                \
/* print firinterp object's internal properties to stdout   */  \
void FIRINTERP(_print)(FIRINTERP() _q);                         \
                                                                \
/* reset internal state                                     */  \
void FIRINTERP(_reset)(FIRINTERP() _q);                         \
                                                                \
/* execute interpolation on single input sample             */  \
/*  _q      : firinterp object                              */  \
/*  _x      : input sample                                  */  \
/*  _y      : output sample array [size: _M x 1]            */  \
void FIRINTERP(_execute)(FIRINTERP() _q,                        \
                         TI          _x,                        \
                         TO *        _y);                       \
                                                                \
/* execute interpolation on block of input samples          */  \
/*  _q      : firinterp object                              */  \
/*  _x      : input array [size: _n x 1]                    */  \
/*  _n      : size of input array                           */  \
/*  _y      : output sample array [size: _M*_n x 1]         */  \
void FIRINTERP(_execute_block)(FIRINTERP()  _q,                 \
                               TI *         _x,                 \
                               unsigned int _n,                 \
                               TO *         _y);                \

LIQUID_FIRINTERP_DEFINE_API(FIRINTERP_MANGLE_RRRF,
                            float,
                            float,
                            float)

LIQUID_FIRINTERP_DEFINE_API(FIRINTERP_MANGLE_CRCF,
                            liquid_float_complex,
                            float,
                            liquid_float_complex)

LIQUID_FIRINTERP_DEFINE_API(FIRINTERP_MANGLE_CCCF,
                            liquid_float_complex,
                            liquid_float_complex,
                            liquid_float_complex)

// iirinterp : infinite impulse response interpolator
#define IIRINTERP_MANGLE_RRRF(name)  LIQUID_CONCAT(iirinterp_rrrf,name)
#define IIRINTERP_MANGLE_CRCF(name)  LIQUID_CONCAT(iirinterp_crcf,name)
#define IIRINTERP_MANGLE_CCCF(name)  LIQUID_CONCAT(iirinterp_cccf,name)

#define LIQUID_IIRINTERP_DEFINE_API(IIRINTERP,TO,TC,TI)         \
typedef struct IIRINTERP(_s) * IIRINTERP();                     \
                                                                \
/* create interpolator from external coefficients           */  \
/*  _M      : interpolation factor                          */  \
/*  _b      : feed-back coefficients [size: _nb x 1]        */  \
/*  _nb     : feed-back coefficients length                 */  \
/*  _a      : feed-forward coefficients [size: _na x 1]     */  \
/*  _na     : feed-forward coefficients length              */  \
IIRINTERP() IIRINTERP(_create)(unsigned int _M,                 \
                               TC *         _b,                 \
                               unsigned int _nb,                \
                               TC *         _a,                 \
                               unsigned int _na);               \
                                                                \
/* create interpolator from prototype                       */  \
/*  _M      : interpolation factor                          */  \
IIRINTERP() IIRINTERP(_create_prototype)(                       \
                unsigned int _M,                                \
                liquid_iirdes_filtertype _ftype,                \
                liquid_iirdes_bandtype   _btype,                \
                liquid_iirdes_format     _format,               \
                unsigned int _order,                            \
                float _fc,                                      \
                float _f0,                                      \
                float _Ap,                                      \
                float _As);                                     \
                                                                \
/* destroy interpolator object and free internal memory     */  \
void IIRINTERP(_destroy)(IIRINTERP() _q);                       \
                                                                \
/* print interpolator object internals                      */  \
void IIRINTERP(_print)(IIRINTERP() _q);                         \
                                                                \
/* reset interpolator object                                */  \
void IIRINTERP(_reset)(IIRINTERP() _q);                         \
                                                                \
/* execute interpolation on single input sample             */  \
/*  _q      : iirinterp object                              */  \
/*  _x      : input sample                                  */  \
/*  _y      : output sample array [size: _M x 1]            */  \
void IIRINTERP(_execute)(IIRINTERP() _q,                        \
                         TI          _x,                        \
                         TO *        _y);                       \
                                                                \
/* execute interpolation on block of input samples          */  \
/*  _q      : iirinterp object                              */  \
/*  _x      : input array [size: _n x 1]                    */  \
/*  _n      : size of input array                           */  \
/*  _y      : output sample array [size: _M*_n x 1]         */  \
void IIRINTERP(_execute_block)(IIRINTERP()  _q,                 \
                               TI *         _x,                 \
                               unsigned int _n,                 \
                               TO *         _y);                \
                                                                \
/* get system group delay at frequency _fc                  */  \
float IIRINTERP(_groupdelay)(IIRINTERP() _q, float _fc);        \

LIQUID_IIRINTERP_DEFINE_API(IIRINTERP_MANGLE_RRRF,
                            float,
                            float,
                            float)

LIQUID_IIRINTERP_DEFINE_API(IIRINTERP_MANGLE_CRCF,
                            liquid_float_complex,
                            float,
                            liquid_float_complex)

LIQUID_IIRINTERP_DEFINE_API(IIRINTERP_MANGLE_CCCF,
                            liquid_float_complex,
                            liquid_float_complex,
                            liquid_float_complex)

// 
// Decimators
//

// firdecim : finite impulse response decimator
#define FIRDECIM_MANGLE_RRRF(name) LIQUID_CONCAT(firdecim_rrrf,name)
#define FIRDECIM_MANGLE_CRCF(name) LIQUID_CONCAT(firdecim_crcf,name)
#define FIRDECIM_MANGLE_CCCF(name) LIQUID_CONCAT(firdecim_cccf,name)

#define LIQUID_FIRDECIM_DEFINE_API(FIRDECIM,TO,TC,TI)           \
typedef struct FIRDECIM(_s) * FIRDECIM();                       \
                                                                \
/* create decimator from external coefficients              */  \
/*  _M      : decimation factor                             */  \
/*  _h      : filter coefficients [size: _h_len x 1]        */  \
/*  _h_len  : filter coefficients length                    */  \
FIRDECIM() FIRDECIM(_create)(unsigned int _M,                   \
                             TC *         _h,                   \
                             unsigned int _h_len);              \
                                                                \
/* create decimator from prototype                          */  \
/*  _M      : decimation factor                             */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _As     : stop-band attenuation [dB]                    */  \
FIRDECIM() FIRDECIM(_create_prototype)(unsigned int _M,         \
                                       unsigned int _m,         \
                                       float        _As);       \
                                                                \
/* create square-root Nyquist decimator                     */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _M      : samples/symbol (decimation factor)            */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _beta   : rolloff factor (0 < beta <= 1)                */  \
/*  _dt     : fractional sample delay                       */  \
FIRDECIM() FIRDECIM(_create_rnyquist)(int          _type,       \
                                      unsigned int _M,          \
                                      unsigned int _m,          \
                                      float        _beta,       \
                                      float        _dt);        \
                                                                \
/* destroy decimator object                                 */  \
void FIRDECIM(_destroy)(FIRDECIM() _q);                         \
                                                                \
/* print decimator object propreties to stdout              */  \
void FIRDECIM(_print)(FIRDECIM() _q);                           \
                                                                \
/* reset decimator object internal state                    */  \
void FIRDECIM(_clear)(FIRDECIM() _q);                           \
                                                                \
/* execute decimator on _M input samples                    */  \
/*  _q      : decimator object                              */  \
/*  _x      : input samples [size: _M x 1]                  */  \
/*  _y      : output sample pointer                         */  \
void FIRDECIM(_execute)(FIRDECIM() _q,                          \
                        TI *       _x,                          \
                        TO *       _y);                         \
                                                                \
/* execute decimator on block of _n*_M input samples        */  \
/*  _q      : decimator object                              */  \
/*  _x      : input array [size: _n*_M x 1]                 */  \
/*  _n      : number of _output_ samples                    */  \
/*  _y      : output array [_sze: _n x 1]                   */  \
void FIRDECIM(_execute_block)(FIRDECIM()   _q,                  \
                              TI *         _x,                  \
                              unsigned int _n,                  \
                              TO *         _y);                 \

LIQUID_FIRDECIM_DEFINE_API(FIRDECIM_MANGLE_RRRF,
                           float,
                           float,
                           float)

LIQUID_FIRDECIM_DEFINE_API(FIRDECIM_MANGLE_CRCF,
                           liquid_float_complex,
                           float,
                           liquid_float_complex)

LIQUID_FIRDECIM_DEFINE_API(FIRDECIM_MANGLE_CCCF,
                           liquid_float_complex,
                           liquid_float_complex,
                           liquid_float_complex)


// iirdecim : infinite impulse response decimator
#define IIRDECIM_MANGLE_RRRF(name)  LIQUID_CONCAT(iirdecim_rrrf,name)
#define IIRDECIM_MANGLE_CRCF(name)  LIQUID_CONCAT(iirdecim_crcf,name)
#define IIRDECIM_MANGLE_CCCF(name)  LIQUID_CONCAT(iirdecim_cccf,name)

#define LIQUID_IIRDECIM_DEFINE_API(IIRDECIM,TO,TC,TI)           \
typedef struct IIRDECIM(_s) * IIRDECIM();                       \
                                                                \
/* create decimator from external coefficients              */  \
/*  _M      : decimation factor                             */  \
/*  _b      : feed-back coefficients [size: _nb x 1]        */  \
/*  _nb     : feed-back coefficients length                 */  \
/*  _a      : feed-forward coefficients [size: _na x 1]     */  \
/*  _na     : feed-forward coefficients length              */  \
IIRDECIM() IIRDECIM(_create)(unsigned int _M,                   \
                             TC *         _b,                   \
                             unsigned int _nb,                  \
                             TC *         _a,                   \
                             unsigned int _na);                 \
                                                                \
/* create decimator from prototype                          */  \
/*  _M      : decimation factor                             */  \
IIRDECIM() IIRDECIM(_create_prototype)(                         \
                unsigned int _M,                                \
                liquid_iirdes_filtertype _ftype,                \
                liquid_iirdes_bandtype   _btype,                \
                liquid_iirdes_format     _format,               \
                unsigned int _order,                            \
                float _fc,                                      \
                float _f0,                                      \
                float _Ap,                                      \
                float _As);                                     \
                                                                \
/* destroy decimator object and free internal memory        */  \
void IIRDECIM(_destroy)(IIRDECIM() _q);                         \
                                                                \
/* print decimator object internals                         */  \
void IIRDECIM(_print)(IIRDECIM() _q);                           \
                                                                \
/* reset decimator object                                   */  \
void IIRDECIM(_reset)(IIRDECIM() _q);                           \
                                                                \
/* execute decimator on _M input samples                    */  \
/*  _q      : decimator object                              */  \
/*  _x      : input samples [size: _M x 1]                  */  \
/*  _y      : output sample pointer                         */  \
void IIRDECIM(_execute)(IIRDECIM() _q,                          \
                        TI *       _x,                          \
                        TO *       _y);                         \
                                                                \
/* execute decimator on block of _n*_M input samples        */  \
/*  _q      : decimator object                              */  \
/*  _x      : input array [size: _n*_M x 1]                 */  \
/*  _n      : number of _output_ samples                    */  \
/*  _y      : output array [_sze: _n x 1]                   */  \
void IIRDECIM(_execute_block)(IIRDECIM()   _q,                  \
                              TI *         _x,                  \
                              unsigned int _n,                  \
                              TO *         _y);                 \
                                                                \
/* get system group delay at frequency _fc                  */  \
float IIRDECIM(_groupdelay)(IIRDECIM() _q, float _fc);          \

LIQUID_IIRDECIM_DEFINE_API(IIRDECIM_MANGLE_RRRF,
                           float,
                           float,
                           float)

LIQUID_IIRDECIM_DEFINE_API(IIRDECIM_MANGLE_CRCF,
                           liquid_float_complex,
                           float,
                           liquid_float_complex)

LIQUID_IIRDECIM_DEFINE_API(IIRDECIM_MANGLE_CCCF,
                           liquid_float_complex,
                           liquid_float_complex,
                           liquid_float_complex)



// 
// Half-band resampler
//
#define RESAMP2_MANGLE_RRRF(name)   LIQUID_CONCAT(resamp2_rrrf,name)
#define RESAMP2_MANGLE_CRCF(name)   LIQUID_CONCAT(resamp2_crcf,name)
#define RESAMP2_MANGLE_CCCF(name)   LIQUID_CONCAT(resamp2_cccf,name)

#define LIQUID_RESAMP2_DEFINE_API(RESAMP2,TO,TC,TI)             \
typedef struct RESAMP2(_s) * RESAMP2();                         \
                                                                \
/* create half-band resampler                               */  \
/*  _m      :   filter semi-length (h_len = 4*m+1)          */  \
/*  _f0     :   filter center frequency                     */  \
/*  _As     :   stop-band attenuation [dB]                  */  \
RESAMP2() RESAMP2(_create)(unsigned int _m,                     \
                           float        _f0,                    \
                           float        _As);                   \
                                                                \
/* re-create half-band resampler with new properties        */  \
/*  _q      :   original half-band resampler object         */  \
/*  _m      :   filter semi-length (h_len = 4*m+1)          */  \
/*  _f0     :   filter center frequency                     */  \
/*  _As     :   stop-band attenuation [dB]                  */  \
RESAMP2() RESAMP2(_recreate)(RESAMP2()    _q,                   \
                             unsigned int _m,                   \
                             float        _f0,                  \
                             float        _As);                 \
                                                                \
/* destroy half-band resampler                              */  \
void RESAMP2(_destroy)(RESAMP2() _q);                           \
                                                                \
/* print resamp2 object's internals                         */  \
void RESAMP2(_print)(RESAMP2() _q);                             \
                                                                \
/* reset internal buffer                                    */  \
void RESAMP2(_clear)(RESAMP2() _q);                             \
                                                                \
/* get resampler filter delay (semi-length m)               */  \
unsigned int RESAMP2(_get_delay)(RESAMP2() _q);                 \
                                                                \
/* execute resamp2 as half-band filter                      */  \
/*  _q      :   resamp2 object                              */  \
/*  _x      :   input sample                                */  \
/*  _y0     :   output sample pointer (low frequency)       */  \
/*  _y1     :   output sample pointer (high frequency)      */  \
void RESAMP2(_filter_execute)(RESAMP2() _q,                     \
                              TI        _x,                     \
                              TO *      _y0,                    \
                              TO *      _y1);                   \
                                                                \
/* execute resamp2 as half-band analysis filterbank         */  \
/*  _q      :   resamp2 object                              */  \
/*  _x      :   input array  [size: 2 x 1]                  */  \
/*  _y      :   output array [size: 2 x 1]                  */  \
void RESAMP2(_analyzer_execute)(RESAMP2() _q,                   \
                                TI *      _x,                   \
                                TO *      _y);                  \
                                                                \
/* execute resamp2 as half-band synthesis filterbank        */  \
/*  _q      :   resamp2 object                              */  \
/*  _x      :   input array  [size: 2 x 1]                  */  \
/*  _y      :   output array [size: 2 x 1]                  */  \
void RESAMP2(_synthesizer_execute)(RESAMP2() _q,                \
                                   TI *      _x,                \
                                   TO *      _y);               \
                                                                \
/* execute resamp2 as half-band decimator                   */  \
/*  _q      :   resamp2 object                              */  \
/*  _x      :   input array  [size: 2 x 1]                  */  \
/*  _y      :   output sample pointer                       */  \
void RESAMP2(_decim_execute)(RESAMP2() _q,                      \
                             TI *      _x,                      \
                             TO *      _y);                     \
                                                                \
/* execute resamp2 as half-band interpolator                */  \
/*  _q      :   resamp2 object                              */  \
/*  _x      :   input sample                                */  \
/*  _y      :   output array [size: 2 x 1]                  */  \
void RESAMP2(_interp_execute)(RESAMP2() _q,                     \
                              TI        _x,                     \
                              TO *      _y);                    \

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)


// 
// Arbitrary resampler
//
#define RESAMP_MANGLE_RRRF(name)    LIQUID_CONCAT(resamp_rrrf,name)
#define RESAMP_MANGLE_CRCF(name)    LIQUID_CONCAT(resamp_crcf,name)
#define RESAMP_MANGLE_CCCF(name)    LIQUID_CONCAT(resamp_cccf,name)

#define LIQUID_RESAMP_DEFINE_API(RESAMP,TO,TC,TI)               \
typedef struct RESAMP(_s) * RESAMP();                           \
                                                                \
/* create arbitrary resampler object                        */  \
/*  _rate   : arbitrary resampling rate                     */  \
/*  _m      : filter semi-length (delay)                    */  \
/*  _fc     : filter cutoff frequency, 0 < _fc < 0.5        */  \
/*  _As     : filter stop-band attenuation [dB]             */  \
/*  _npfb   : number of filters in the bank                 */  \
RESAMP() RESAMP(_create)(float        _rate,                    \
                         unsigned int _m,                       \
                         float        _fc,                      \
                         float        _As,                      \
                         unsigned int _npfb);                   \
                                                                \
/* destroy arbitrary resampler object                       */  \
void RESAMP(_destroy)(RESAMP() _q);                             \
                                                                \
/* print resamp object internals to stdout                  */  \
void RESAMP(_print)(RESAMP() _q);                               \
                                                                \
/* reset resamp object internals                            */  \
void RESAMP(_reset)(RESAMP() _q);                               \
                                                                \
/* get resampler delay (output samples)                     */  \
unsigned int RESAMP(_get_delay)(RESAMP() _q);                   \
                                                                \
/* set rate of arbitrary resampler                          */  \
void RESAMP(_setrate)(RESAMP() _q, float _rate);                \
                                                                \
/* execute arbitrary resampler                              */  \
/*  _q              :   resamp object                       */  \
/*  _x              :   single input sample                 */  \
/*  _y              :   output sample array (pointer)       */  \
/*  _num_written    :   number of samples written to _y     */  \
void RESAMP(_execute)(RESAMP()       _q,                        \
                      TI             _x,                        \
                      TO *           _y,                        \
                      unsigned int * _num_written);             \
                                                                \
/* execute arbitrary resampler on a block of samples        */  \
/*  _q              :   resamp object                       */  \
/*  _x              :   input buffer [size: _nx x 1]        */  \
/*  _nx             :   input buffer                        */  \
/*  _y              :   output sample array (pointer)       */  \
/*  _ny             :   number of samples written to _y     */  \
void RESAMP(_execute_block)(RESAMP()       _q,                  \
                            TI *           _x,                  \
                            unsigned int   _nx,                 \
                            TO *           _y,                  \
                            unsigned int * _ny);                \

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_RRRF,
                         float,
                         float,
                         float)

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CRCF,
                         liquid_float_complex,
                         float,
                         liquid_float_complex)

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CCCF,
                         liquid_float_complex,
                         liquid_float_complex,
                         liquid_float_complex)


// 
// Multi-stage half-band resampler
//

// resampling type (interpolator/decimator)
typedef enum {
    LIQUID_RESAMP_INTERP=0, // interpolator
    LIQUID_RESAMP_DECIM,    // decimator
} liquid_resamp_type;

#define MSRESAMP2_MANGLE_RRRF(name) LIQUID_CONCAT(msresamp2_rrrf,name)
#define MSRESAMP2_MANGLE_CRCF(name) LIQUID_CONCAT(msresamp2_crcf,name)
#define MSRESAMP2_MANGLE_CCCF(name) LIQUID_CONCAT(msresamp2_cccf,name)

#define LIQUID_MSRESAMP2_DEFINE_API(MSRESAMP2,TO,TC,TI)         \
typedef struct MSRESAMP2(_s) * MSRESAMP2();                     \
                                                                \
/* create multi-stage half-band resampler                   */  \
/*  _type       : resampler type (e.g. LIQUID_RESAMP_DECIM) */  \
/*  _num_stages : number of resampling stages               */  \
/*  _fc         : filter cut-off frequency 0 < _fc < 0.5    */  \
/*  _f0         : filter center frequency                   */  \
/*  _As         : stop-band attenuation [dB]                */  \
MSRESAMP2() MSRESAMP2(_create)(int          _type,              \
                               unsigned int _num_stages,        \
                               float        _fc,                \
                               float        _f0,                \
                               float        _As);               \
                                                                \
/* destroy multi-stage half-bandresampler                   */  \
void MSRESAMP2(_destroy)(MSRESAMP2() _q);                       \
                                                                \
/* print msresamp object internals to stdout                */  \
void MSRESAMP2(_print)(MSRESAMP2() _q);                         \
                                                                \
/* reset msresamp object internal state                     */  \
void MSRESAMP2(_reset)(MSRESAMP2() _q);                         \
                                                                \
/* get group delay (number of output samples)               */  \
float MSRESAMP2(_get_delay)(MSRESAMP2() _q);                    \
                                                                \
/* execute multi-stage resampler, M = 2^num_stages          */  \
/*  LIQUID_RESAMP_INTERP:   input: 1,   output: M           */  \
/*  LIQUID_RESAMP_DECIM:    input: M,   output: 1           */  \
/*  _q      : msresamp object                               */  \
/*  _x      : input sample array                            */  \
/*  _y      : output sample array                           */  \
void MSRESAMP2(_execute)(MSRESAMP2() _q,                        \
                         TI *        _x,                        \
                         TO *        _y);                       \

LIQUID_MSRESAMP2_DEFINE_API(MSRESAMP2_MANGLE_RRRF,
                            float,
                            float,
                            float)

LIQUID_MSRESAMP2_DEFINE_API(MSRESAMP2_MANGLE_CRCF,
                            liquid_float_complex,
                            float,
                            liquid_float_complex)

LIQUID_MSRESAMP2_DEFINE_API(MSRESAMP2_MANGLE_CCCF,
                            liquid_float_complex,
                            liquid_float_complex,
                            liquid_float_complex)


// 
// Multi-stage arbitrary resampler
//
#define MSRESAMP_MANGLE_RRRF(name)    LIQUID_CONCAT(msresamp_rrrf,name)
#define MSRESAMP_MANGLE_CRCF(name)    LIQUID_CONCAT(msresamp_crcf,name)
#define MSRESAMP_MANGLE_CCCF(name)    LIQUID_CONCAT(msresamp_cccf,name)

#define LIQUID_MSRESAMP_DEFINE_API(MSRESAMP,TO,TC,TI)           \
typedef struct MSRESAMP(_s) * MSRESAMP();                       \
                                                                \
/* create multi-stage arbitrary resampler                   */  \
/*  _r      :   resampling rate [output/input]              */  \
/*  _As     :   stop-band attenuation [dB]                  */  \
MSRESAMP() MSRESAMP(_create)(float _r,                          \
                             float _As);                        \
                                                                \
/* destroy multi-stage arbitrary resampler                  */  \
void MSRESAMP(_destroy)(MSRESAMP() _q);                         \
                                                                \
/* print msresamp object internals to stdout                */  \
void MSRESAMP(_print)(MSRESAMP() _q);                           \
                                                                \
/* reset msresamp object internal state                     */  \
void MSRESAMP(_reset)(MSRESAMP() _q);                           \
                                                                \
/* get filter delay (output samples)                        */  \
float MSRESAMP(_get_delay)(MSRESAMP() _q);                      \
                                                                \
/* execute multi-stage resampler                            */  \
/*  _q      :   msresamp object                             */  \
/*  _x      :   input sample array  [size: _nx x 1]         */  \
/*  _nx     :   input sample array size                     */  \
/*  _y      :   output sample array [size: variable]        */  \
/*  _ny     :   number of samples written to _y             */  \
void MSRESAMP(_execute)(MSRESAMP()     _q,                      \
                        TI *           _x,                      \
                        unsigned int   _nx,                     \
                        TO *           _y,                      \
                        unsigned int * _ny);                    \

LIQUID_MSRESAMP_DEFINE_API(MSRESAMP_MANGLE_RRRF,
                           float,
                           float,
                           float)

LIQUID_MSRESAMP_DEFINE_API(MSRESAMP_MANGLE_CRCF,
                           liquid_float_complex,
                           float,
                           liquid_float_complex)

LIQUID_MSRESAMP_DEFINE_API(MSRESAMP_MANGLE_CCCF,
                           liquid_float_complex,
                           liquid_float_complex,
                           liquid_float_complex)


// 
// Symbol timing recovery (symbol synchronizer)
//
#define SYMSYNC_MANGLE_RRRF(name)   LIQUID_CONCAT(symsync_rrrf,name)
#define SYMSYNC_MANGLE_CRCF(name)   LIQUID_CONCAT(symsync_crcf,name)

#define LIQUID_SYMSYNC_DEFINE_API(SYMSYNC,TO,TC,TI)             \
                                                                \
typedef struct SYMSYNC(_s) * SYMSYNC();                         \
                                                                \
/* create synchronizer object from external coefficients    */  \
/*  _k      : samples per symbol                            */  \
/*  _M      : number of filters in the bank                 */  \
/*  _h      : matched filter coefficients [size:            */  \
/*  _h_len  : length of matched filter                      */  \
SYMSYNC() SYMSYNC(_create)(unsigned int _k,                     \
                           unsigned int _M,                     \
                           TC *         _h,                     \
                           unsigned int _h_len);                \
                                                                \
/* create square-root Nyquist symbol synchronizer           */  \
/*  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)         */  \
/*  _k      : samples/symbol                                */  \
/*  _m      : symbol delay                                  */  \
/*  _beta   : rolloff factor, beta in (0,1]                 */  \
/*  _M      : number of filters in the bank                 */  \
SYMSYNC() SYMSYNC(_create_rnyquist)(int          _type,         \
                                    unsigned int _k,            \
                                    unsigned int _m,            \
                                    float        _beta,         \
                                    unsigned int _M);           \
                                                                \
/* create symsync using Kaiser filter interpolator; useful  */  \
/* when the input signal has matched filter applied already */  \
/*  _k      : input samples/symbol                          */  \
/*  _m      : symbol delay                                  */  \
/*  _beta   : rolloff factor, beta in (0,1]                 */  \
/*  _M      : number of filters in the bank                 */  \
SYMSYNC() SYMSYNC(_create_kaiser)(unsigned int _k,              \
                                  unsigned int _m,              \
                                  float        _beta,           \
                                  unsigned int _M);             \
                                                                \
/* destroy symsync object, freeing all internal memory      */  \
void SYMSYNC(_destroy)(SYMSYNC() _q);                           \
                                                                \
/* print symsync object's parameters                        */  \
void SYMSYNC(_print)(SYMSYNC() _q);                             \
                                                                \
/* reset symsync internal state                             */  \
void SYMSYNC(_reset)(SYMSYNC() _q);                             \
                                                                \
/* lock/unlock loop control                                 */  \
void SYMSYNC(_lock)(  SYMSYNC() _q);                            \
void SYMSYNC(_unlock)(SYMSYNC() _q);                            \
                                                                \
/* set synchronizer output rate (samples/symbol)            */  \
/*  _q      : synchronizer object                           */  \
/*  _k_out  : output samples/symbol                         */  \
void SYMSYNC(_set_output_rate)(SYMSYNC()    _q,                 \
                               unsigned int _k_out);            \
                                                                \
/* set loop-filter bandwidth                                */  \
/*  _q      : synchronizer object                           */  \
/*  _bt     : loop bandwidth                                */  \
void SYMSYNC(_set_lf_bw)(SYMSYNC() _q,                          \
                         float     _bt);                        \
                                                                \
/* return instantaneous fractional timing offset estimate   */  \
float SYMSYNC(_get_tau)(SYMSYNC() _q);                          \
                                                                \
/* execute synchronizer on input data array                 */  \
/*  _q      : synchronizer object                           */  \
/*  _x      : input data array                              */  \
/*  _nx     : number of input samples                       */  \
/*  _y      : output data array                             */  \
/*  _ny     : number of samples written to output buffer    */  \
void SYMSYNC(_execute)(SYMSYNC()      _q,                       \
                       TI *           _x,                       \
                       unsigned int   _nx,                      \
                       TO *           _y,                       \
                       unsigned int * _ny);                     \

LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)


//
// Finite impulse response Farrow filter
//

#define FIRFARROW_MANGLE_RRRF(name)     LIQUID_CONCAT(firfarrow_rrrf,name)
#define FIRFARROW_MANGLE_CRCF(name)     LIQUID_CONCAT(firfarrow_crcf,name)
//#define FIRFARROW_MANGLE_CCCF(name)     LIQUID_CONCAT(firfarrow_cccf,name)

// Macro:
//   FIRFARROW  : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIRFARROW_DEFINE_API(FIRFARROW,TO,TC,TI)         \
                                                                \
typedef struct FIRFARROW(_s) * FIRFARROW();                     \
                                                                \
/* create firfarrow object                                  */  \
/*  _h_len      : filter length                             */  \
/*  _p          : polynomial order                          */  \
/*  _fc         : filter cutoff frequency                   */  \
/*  _As         : stopband attenuation [dB]                 */  \
FIRFARROW() FIRFARROW(_create)(unsigned int _h_len,             \
                               unsigned int _p,                 \
                               float        _fc,                \
                               float        _As);               \
                                                                \
/* destroy firfarrow object, freeing all internal memory    */  \
void FIRFARROW(_destroy)(FIRFARROW() _q);                       \
                                                                \
/* print firfarrow object's internal properties             */  \
void FIRFARROW(_print)(FIRFARROW() _q);                         \
                                                                \
/* reset firfarrow object's internal state                  */  \
void FIRFARROW(_reset)(FIRFARROW() _q);                         \
                                                                \
/* push sample into firfarrow object                        */  \
/*  _q      : firfarrow object                              */  \
/*  _x      : input sample                                  */  \
void FIRFARROW(_push)(FIRFARROW() _q,                           \
                      TI          _x);                          \
                                                                \
/* set fractional delay of firfarrow object                 */  \
/*  _q      : firfarrow object                              */  \
/*  _mu     : fractional sample delay                       */  \
void FIRFARROW(_set_delay)(FIRFARROW() _q,                      \
                           float       _mu);                    \
                                                                \
/* execute firfarrow internal dot product                   */  \
/*  _q      : firfarrow object                              */  \
/*  _y      : output sample pointer                         */  \
void FIRFARROW(_execute)(FIRFARROW() _q,                        \
                         TO *        _y);                       \
                                                                \
/* compute firfarrow filter on block of samples; the input  */  \
/* and output arrays may have the same pointer              */  \
/*  _q      : firfarrow object                              */  \
/*  _x      : input array [size: _n x 1]                    */  \
/*  _n      : input, output array size                      */  \
/*  _y      : output array [size: _n x 1]                   */  \
void FIRFARROW(_execute_block)(FIRFARROW()  _q,                 \
                               TI *         _x,                 \
                               unsigned int _n,                 \
                               TO *         _y);                \
                                                                \
/* get length of firfarrow object (number of filter taps)   */  \
unsigned int FIRFARROW(_get_length)(FIRFARROW() _q);            \
                                                                \
/* get coefficients of firfarrow object                     */  \
/*  _q      : firfarrow object                              */  \
/*  _h      : output coefficients pointer                   */  \
void FIRFARROW(_get_coefficients)(FIRFARROW() _q,               \
                                  float *     _h);              \
                                                                \
/* compute complex frequency response                       */  \
/*  _q      : filter object                                 */  \
/*  _fc     : frequency                                     */  \
/*  _H      : output frequency response                     */  \
void FIRFARROW(_freqresponse)(FIRFARROW()            _q,        \
                              float                  _fc,       \
                              liquid_float_complex * _H);       \
                                                                \
/* compute group delay [samples]                            */  \
/*  _q      :   filter object                               */  \
/*  _fc     :   frequency                                   */  \
float FIRFARROW(_groupdelay)(FIRFARROW() _q,                    \
                             float       _fc);                  \

LIQUID_FIRFARROW_DEFINE_API(FIRFARROW_MANGLE_RRRF,
                            float,
                            float,
                            float)

LIQUID_FIRFARROW_DEFINE_API(FIRFARROW_MANGLE_CRCF,
                            liquid_float_complex,
                            float,
                            liquid_float_complex)



//
// MODULE : framing
//

// framesyncstats : generic frame synchronizer statistic structure

typedef struct {
    // signal quality
    float evm;      // error vector magnitude [dB]
    float rssi;     // received signal strength indicator [dB]
    float cfo;      // carrier frequency offset (f/Fs)

    // demodulated frame symbols
    liquid_float_complex * framesyms;   // pointer to array [size: framesyms x 1]
    unsigned int num_framesyms;         // length of framesyms

    // modulation/coding scheme etc.
    unsigned int mod_scheme;    // modulation scheme
    unsigned int mod_bps;       // modulation depth (bits/symbol)
    unsigned int check;         // data validity check (crc, checksum)
    unsigned int fec0;          // forward error-correction (inner)
    unsigned int fec1;          // forward error-correction (outer)
} framesyncstats_s;

// external framesyncstats default object
extern framesyncstats_s framesyncstats_default;

// initialize framesyncstats object on default
void framesyncstats_init_default(framesyncstats_s * _stats);

// print framesyncstats object
void framesyncstats_print(framesyncstats_s * _stats);

// Generic frame synchronizer callback function type
//  _header         :   header data [size: 8 bytes]
//  _header_valid   :   is header valid? (0:no, 1:yes)
//  _payload        :   payload data [size: _payload_len]
//  _payload_len    :   length of payload (bytes)
//  _payload_valid  :   is payload valid? (0:no, 1:yes)
//  _stats          :   frame statistics object
//  _userdata       :   pointer to userdata
typedef int (*framesync_callback)(unsigned char *  _header,
                                  int              _header_valid,
                                  unsigned char *  _payload,
                                  unsigned int     _payload_len,
                                  int              _payload_valid,
                                  framesyncstats_s _stats,
                                  void *           _userdata);

// framesync csma callback functions invoked when signal levels is high or low
//  _userdata       :   user-defined data pointer
typedef void (*framesync_csma_callback)(void * _userdata);


//
// Basic frame generator (64 bytes data payload)
//

// frame length in samples
#define LIQUID_FRAME64_LEN (1340)

typedef struct framegen64_s * framegen64;

// create frame generator
framegen64 framegen64_create();

// destroy frame generator
void framegen64_destroy(framegen64 _q);

// print frame generator internal properties
void framegen64_print(framegen64 _q);

// generate frame
//  _q          :   frame generator object
//  _header     :   8-byte header data
//  _payload    :   64-byte payload data
//  _frame      :   output frame samples [size: LIQUID_FRAME64_LEN x 1]
void framegen64_execute(framegen64             _q,
                        unsigned char *        _header,
                        unsigned char *        _payload,
                        liquid_float_complex * _frame);

typedef struct framesync64_s * framesync64;

// create framesync64 object
//  _callback   :   callback function
//  _userdata   :   user data pointer passed to callback function
framesync64 framesync64_create(framesync_callback _callback,
                               void *             _userdata);

// destroy frame synchronizer
void framesync64_destroy(framesync64 _q);

// print frame synchronizer internal properties
void framesync64_print(framesync64 _q);

// reset frame synchronizer internal state
void framesync64_reset(framesync64 _q);

// push samples through frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input samples [size: _n x 1]
//  _n      :   number of input samples
void framesync64_execute(framesync64            _q,
                         liquid_float_complex * _x,
                         unsigned int           _n);

// enable/disable debugging
void framesync64_debug_enable(framesync64 _q);
void framesync64_debug_disable(framesync64 _q);
void framesync64_debug_print(framesync64 _q, const char * _filename);

#if 0
// advanced modes
void framesync64_set_csma_callbacks(framesync64             _q,
                                    framesync_csma_callback _csma_lock,
                                    framesync_csma_callback _csma_unlock,
                                    void *                  _csma_userdata);
#endif

//
// Flexible frame : adjustable payload, mod scheme, etc., but bring
//                  your own error correction, redundancy check
//

// frame generator
typedef struct {
    unsigned int check;         // data validity check
    unsigned int fec0;          // forward error-correction scheme (inner)
    unsigned int fec1;          // forward error-correction scheme (outer)
    unsigned int mod_scheme;    // modulation scheme
} flexframegenprops_s;

void flexframegenprops_init_default(flexframegenprops_s * _fgprops);

typedef struct flexframegen_s * flexframegen;

// create flexframegen object
//  _props  :   frame properties (modulation scheme, etc.)
flexframegen flexframegen_create(flexframegenprops_s * _props);

// destroy flexframegen object
void flexframegen_destroy(flexframegen _q);

// print flexframegen object internals
void flexframegen_print(flexframegen _q);

// reset flexframegen object internals
void flexframegen_reset(flexframegen _q);

// is frame assembled?
int flexframegen_is_assembled(flexframegen _q);

// get frame properties
void flexframegen_getprops(flexframegen _q, flexframegenprops_s * _props);

// set frame properties
void flexframegen_setprops(flexframegen _q, flexframegenprops_s * _props);

// get length of assembled frame (samples)
unsigned int flexframegen_getframelen(flexframegen _q);

// assemble a frame from an array of data
//  _q              :   frame generator object
//  _header         :   frame header
//  _payload        :   payload data [size: _payload_len x 1]
//  _payload_len    :   payload data length
void flexframegen_assemble(flexframegen    _q,
                           unsigned char * _header,
                           unsigned char * _payload,
                           unsigned int    _payload_len);

// write samples of assembled frame, two samples at a time, returning
// '1' when frame is complete, '0' otherwise
//  _q              :   frame generator object
//  _buffer         :   output buffer [size: 2 x 1]
int flexframegen_write_samples(flexframegen           _q,
                               liquid_float_complex * _buffer);

// frame synchronizer

typedef struct flexframesync_s * flexframesync;

// create flexframesync object
//  _callback   :   callback function
//  _userdata   :   user data pointer passed to callback function
flexframesync flexframesync_create(framesync_callback _callback,
                                   void *             _userdata);

// destroy frame synchronizer
void flexframesync_destroy(flexframesync _q);

// print frame synchronizer internal properties
void flexframesync_print(flexframesync _q);

// reset frame synchronizer internal state
void flexframesync_reset(flexframesync _q);

// push samples through frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input samples [size: _n x 1]
//  _n      :   number of input samples
void flexframesync_execute(flexframesync          _q,
                           liquid_float_complex * _x,
                           unsigned int           _n);

// enable/disable debugging
void flexframesync_debug_enable(flexframesync _q);
void flexframesync_debug_disable(flexframesync _q);
void flexframesync_debug_print(flexframesync _q,
                               const char *  _filename);
#if 0
// advanced modes
void flexframesync_set_csma_callbacks(flexframesync _fs,
                                      framesync_csma_callback _csma_lock,
                                      framesync_csma_callback _csma_unlock,
                                      void * _csma_userdata);
#endif


//
// bpacket : binary packet suitable for data streaming
//

// 
// bpacket generator/encoder
//
typedef struct bpacketgen_s * bpacketgen;

// create bpacketgen object
//  _m              :   p/n sequence length (ignored)
//  _dec_msg_len    :   decoded message length (original uncoded data)
//  _crc            :   data validity check (e.g. cyclic redundancy check)
//  _fec0           :   inner forward error-correction code scheme
//  _fec1           :   outer forward error-correction code scheme
bpacketgen bpacketgen_create(unsigned int _m,
                             unsigned int _dec_msg_len,
                             int _crc,
                             int _fec0,
                             int _fec1);

// re-create bpacketgen object from old object
//  _q              :   old bpacketgen object
//  _m              :   p/n sequence length (ignored)
//  _dec_msg_len    :   decoded message length (original uncoded data)
//  _crc            :   data validity check (e.g. cyclic redundancy check)
//  _fec0           :   inner forward error-correction code scheme
//  _fec1           :   outer forward error-correction code scheme
bpacketgen bpacketgen_recreate(bpacketgen _q,
                               unsigned int _m,
                               unsigned int _dec_msg_len,
                               int _crc,
                               int _fec0,
                               int _fec1);

// destroy bpacketgen object, freeing all internally-allocated memory
void bpacketgen_destroy(bpacketgen _q);

// print bpacketgen internals
void bpacketgen_print(bpacketgen _q);

// return length of full packet
unsigned int bpacketgen_get_packet_len(bpacketgen _q);

// encode packet
void bpacketgen_encode(bpacketgen _q,
                       unsigned char * _msg_dec,
                       unsigned char * _packet);

// 
// bpacket synchronizer/decoder
//
typedef struct bpacketsync_s * bpacketsync;
typedef int (*bpacketsync_callback)(unsigned char *  _payload,
                                    int              _payload_valid,
                                    unsigned int     _payload_len,
                                    framesyncstats_s _stats,
                                    void *           _userdata);
bpacketsync bpacketsync_create(unsigned int _m,
                               bpacketsync_callback _callback,
                               void * _userdata);
void bpacketsync_destroy(bpacketsync _q);
void bpacketsync_print(bpacketsync _q);
void bpacketsync_reset(bpacketsync _q);

// run synchronizer on array of input bytes
//  _q      :   bpacketsync object
//  _bytes  :   input data array [size: _n x 1]
//  _n      :   input array size
void bpacketsync_execute(bpacketsync _q,
                         unsigned char * _bytes,
                         unsigned int _n);

// run synchronizer on input byte
//  _q      :   bpacketsync object
//  _byte   :   input byte
void bpacketsync_execute_byte(bpacketsync _q,
                              unsigned char _byte);

// run synchronizer on input symbol
//  _q      :   bpacketsync object
//  _sym    :   input symbol with _bps significant bits
//  _bps    :   number of bits in input symbol
void bpacketsync_execute_sym(bpacketsync _q,
                             unsigned char _sym,
                             unsigned int _bps);

// execute one bit at a time
void bpacketsync_execute_bit(bpacketsync _q,
                             unsigned char _bit);

//
// GMSK frame generator
//

typedef struct gmskframegen_s * gmskframegen;

// create GMSK frame generator
gmskframegen gmskframegen_create();
void gmskframegen_destroy(gmskframegen _fg);
void gmskframegen_print(gmskframegen _fg);
void gmskframegen_reset(gmskframegen _fg);
void gmskframegen_assemble(gmskframegen    _fg,
                           unsigned char * _header,
                           unsigned char * _payload,
                           unsigned int    _payload_len,
                           crc_scheme      _check,
                           fec_scheme      _fec0,
                           fec_scheme      _fec1);
unsigned int gmskframegen_getframelen(gmskframegen _q);
int gmskframegen_write_samples(gmskframegen _fg,
                               liquid_float_complex * _y);


//
// GMSK frame synchronizer
//

typedef struct gmskframesync_s * gmskframesync;

// create GMSK frame synchronizer
//  _callback   :   callback function
//  _userdata   :   user data pointer passed to callback function
gmskframesync gmskframesync_create(framesync_callback _callback,
                                   void *             _userdata);
void gmskframesync_destroy(gmskframesync _q);
void gmskframesync_print(gmskframesync _q);
void gmskframesync_reset(gmskframesync _q);
void gmskframesync_execute(gmskframesync _q,
                           liquid_float_complex * _x,
                           unsigned int _n);

// debugging
void gmskframesync_debug_enable(gmskframesync _q);
void gmskframesync_debug_disable(gmskframesync _q);
void gmskframesync_debug_print(gmskframesync _q, const char * _filename);


// 
// OFDM flexframe generator
//

// ofdm frame generator properties
typedef struct {
    unsigned int check;         // data validity check
    unsigned int fec0;          // forward error-correction scheme (inner)
    unsigned int fec1;          // forward error-correction scheme (outer)
    unsigned int mod_scheme;    // modulation scheme
    //unsigned int block_size;  // framing block size
} ofdmflexframegenprops_s;
void ofdmflexframegenprops_init_default(ofdmflexframegenprops_s * _props);

typedef struct ofdmflexframegen_s * ofdmflexframegen;

// create OFDM flexible framing generator object
//  _M          :   number of subcarriers, >10 typical
//  _cp_len     :   cyclic prefix length
//  _taper_len  :   taper length (OFDM symbol overlap)
//  _p          :   subcarrier allocation (null, pilot, data), [size: _M x 1]
//  _fgprops    :   frame properties (modulation scheme, etc.)
ofdmflexframegen ofdmflexframegen_create(unsigned int              _M,
                                         unsigned int              _cp_len,
                                         unsigned int              _taper_len,
                                         unsigned char *           _p,
                                         ofdmflexframegenprops_s * _fgprops);

// destroy ofdmflexframegen object
void ofdmflexframegen_destroy(ofdmflexframegen _q);

// print parameters, properties, etc.
void ofdmflexframegen_print(ofdmflexframegen _q);

// reset ofdmflexframegen object internals
void ofdmflexframegen_reset(ofdmflexframegen _q);

// is frame assembled?
int ofdmflexframegen_is_assembled(ofdmflexframegen _q);

// get properties
void ofdmflexframegen_getprops(ofdmflexframegen _q,
                               ofdmflexframegenprops_s * _props);

// set properties
void ofdmflexframegen_setprops(ofdmflexframegen _q,
                               ofdmflexframegenprops_s * _props);

// get length of frame (symbols)
//  _q              :   OFDM frame generator object
unsigned int ofdmflexframegen_getframelen(ofdmflexframegen _q);

// assemble a frame from an array of data
//  _q              :   OFDM frame generator object
//  _header         :   frame header [8 bytes]
//  _payload        :   payload data [size: _payload_len x 1]
//  _payload_len    :   payload data length
void ofdmflexframegen_assemble(ofdmflexframegen _q,
                               unsigned char * _header,
                               unsigned char * _payload,
                               unsigned int    _payload_len);

// write symbols of assembled frame
//  _q              :   OFDM frame generator object
//  _buffer         :   output buffer [size: M+cp_len x 1]
int ofdmflexframegen_writesymbol(ofdmflexframegen _q,
                                 liquid_float_complex * _buffer);

// 
// OFDM flex frame synchronizer
//

typedef struct ofdmflexframesync_s * ofdmflexframesync;

// create OFDM flexible framing synchronizer object
//  _M          :   number of subcarriers
//  _cp_len     :   cyclic prefix length
//  _taper_len  :   taper length (OFDM symbol overlap)
//  _p          :   subcarrier allocation (null, pilot, data), [size: _M x 1]
//  _callback   :   user-defined callback function
//  _userdata   :   user-defined data pointer
ofdmflexframesync ofdmflexframesync_create(unsigned int       _M,
                                           unsigned int       _cp_len,
                                           unsigned int       _taper_len,
                                           unsigned char *    _p,
                                           framesync_callback _callback,
                                           void *             _userdata);

void ofdmflexframesync_destroy(ofdmflexframesync _q);
void ofdmflexframesync_print(ofdmflexframesync _q);
void ofdmflexframesync_reset(ofdmflexframesync _q);
void ofdmflexframesync_execute(ofdmflexframesync _q,
                               liquid_float_complex * _x,
                               unsigned int _n);

// query the received signal strength indication
float ofdmflexframesync_get_rssi(ofdmflexframesync _q);

// query the received carrier offset estimate
float ofdmflexframesync_get_cfo(ofdmflexframesync _q);

// enable/disable debugging
void ofdmflexframesync_debug_enable(ofdmflexframesync _q);
void ofdmflexframesync_debug_disable(ofdmflexframesync _q);
void ofdmflexframesync_debug_print(ofdmflexframesync _q,
                                   const char *      _filename);



//
// Binary P/N synchronizer
//
#define BSYNC_MANGLE_RRRF(name)     LIQUID_CONCAT(bsync_rrrf,name)
#define BSYNC_MANGLE_CRCF(name)     LIQUID_CONCAT(bsync_crcf,name)
#define BSYNC_MANGLE_CCCF(name)     LIQUID_CONCAT(bsync_cccf,name)

// Macro:
//   BSYNC  : name-mangling macro
//   TO     : output data type
//   TC     : coefficients data type
//   TI     : input data type
#define LIQUID_BSYNC_DEFINE_API(BSYNC,TO,TC,TI)                 \
typedef struct BSYNC(_s) * BSYNC();                             \
                                                                \
BSYNC() BSYNC(_create)(unsigned int _n, TC * _v);               \
                                                                \
/* create binary synchronizer from m-sequence               */  \
/*  _g      :   m-sequence generator polynomial             */  \
/*  _k      :   samples/symbol (over-sampling factor)       */  \
BSYNC() BSYNC(_create_msequence)(unsigned int _g,               \
                                 unsigned int _k);              \
void BSYNC(_destroy)(BSYNC() _fs);                              \
void BSYNC(_print)(BSYNC() _fs);                                \
void BSYNC(_correlate)(BSYNC() _fs, TI _sym, TO * _y);

LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_RRRF,
                        float,
                        float,
                        float)

LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_CRCF,
                        liquid_float_complex,
                        float,
                        liquid_float_complex)

LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_CCCF,
                        liquid_float_complex,
                        liquid_float_complex,
                        liquid_float_complex)


//
// Pre-demodulation synchronizers (binary and otherwise)
//
#define  PRESYNC_MANGLE_CCCF(name)  LIQUID_CONCAT( presync_cccf,name)
#define BPRESYNC_MANGLE_CCCF(name)  LIQUID_CONCAT(bpresync_cccf,name)

// Macro:
//   PRESYNC   : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_PRESYNC_DEFINE_API(PRESYNC,TO,TC,TI)             \
typedef struct PRESYNC(_s) * PRESYNC();                         \
                                                                \
/* create pre-demod synchronizer                            */  \
/*  _v          :   baseband sequence                       */  \
/*  _n          :   baseband sequence length                */  \
/*  _dphi_max   :   maximum absolute frequency deviation    */  \
/*  _m          :   number of correlators                   */  \
PRESYNC() PRESYNC(_create)(TC *         _v,                     \
                           unsigned int _n,                     \
                           float        _dphi_max,              \
                           unsigned int _m);                    \
                                                                \
/* destroy pre-demod synchronizer                           */  \
void PRESYNC(_destroy)(PRESYNC() _q);                           \
                                                                \
/* print pre-demod synchronizer internal state              */  \
void PRESYNC(_print)(PRESYNC() _q);                             \
                                                                \
/* reset pre-demod synchronizer internal state              */  \
void PRESYNC(_reset)(PRESYNC() _q);                             \
                                                                \
/* push input sample into pre-demod synchronizer            */  \
/*  _q          :   pre-demod synchronizer object           */  \
/*  _x          :   input sample                            */  \
void PRESYNC(_push)(PRESYNC() _q,                               \
                    TI        _x);                              \
                                                                \
/* correlate input sequence                                 */  \
/*  _q          :   pre-demod synchronizer object           */  \
/*  _rxy        :   output cross correlation                */  \
/*  _dphi_hat   :   output frequency offset estiamte        */  \
void PRESYNC(_correlate)(PRESYNC() _q,                          \
                         TO *      _rxy,                        \
                         float *   _dphi_hat);                  \

// non-binary pre-demodulation synchronizer
LIQUID_PRESYNC_DEFINE_API(PRESYNC_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)

// binary pre-demodulation synchronizer
LIQUID_PRESYNC_DEFINE_API(BPRESYNC_MANGLE_CCCF,
                          liquid_float_complex,
                          liquid_float_complex,
                          liquid_float_complex)

//
// Pre-demodulation detector
//

typedef struct detector_cccf_s * detector_cccf;

// create pre-demod detector
//  _s          :   sequence
//  _n          :   sequence length
//  _threshold  :   detection threshold (default: 0.7)
//  _dphi_max   :   maximum carrier offset
detector_cccf detector_cccf_create(liquid_float_complex * _s,
                                   unsigned int           _n,
                                   float                  _threshold,
                                   float                  _dphi_max);

// destroy pre-demo detector object
void detector_cccf_destroy(detector_cccf _q);

// print pre-demod detector internal state
void detector_cccf_print(detector_cccf _q);

// reset pre-demod detector internal state
void detector_cccf_reset(detector_cccf _q);

// Run sample through pre-demod detector's correlator.
// Returns '1' if signal was detected, '0' otherwise
//  _q          :   pre-demod detector
//  _x          :   input sample
//  _tau_hat    :   fractional sample offset estimate (set when detected)
//  _dphi_hat   :   carrier frequency offset estimate (set when detected)
//  _gamma_hat  :   channel gain estimate (set when detected)
int detector_cccf_correlate(detector_cccf        _q,
                            liquid_float_complex _x,
                            float *              _tau_hat,
                            float *              _dphi_hat,
                            float *              _gamma_hat);


//
// MODULE : math
//

// ln( Gamma(z) )
float liquid_lngammaf(float _z);

// Gamma(z)
float liquid_gammaf(float _z);

// ln( gamma(z,alpha) ) : lower incomplete gamma function
float liquid_lnlowergammaf(float _z, float _alpha);

// ln( Gamma(z,alpha) ) : upper incomplete gamma function
float liquid_lnuppergammaf(float _z, float _alpha);

// gamma(z,alpha) : lower incomplete gamma function
float liquid_lowergammaf(float _z, float _alpha);

// Gamma(z,alpha) : upper incomplete gamma function
float liquid_uppergammaf(float _z, float _alpha);

// n!
float liquid_factorialf(unsigned int _n);



// ln(I_v(z)) : log Modified Bessel function of the first kind
float liquid_lnbesselif(float _nu, float _z);

// I_v(z) : Modified Bessel function of the first kind
float liquid_besselif(float _nu, float _z);

// I_0(z) : Modified Bessel function of the first kind (order zero)
float liquid_besseli0f(float _z);

// J_v(z) : Bessel function of the first kind
float liquid_besseljf(float _nu, float _z);

// J_0(z) : Bessel function of the first kind (order zero)
float liquid_besselj0f(float _z);


// Q function
float liquid_Qf(float _z);

// Marcum Q-function
float liquid_MarcumQf(int _M,
                      float _alpha,
                      float _beta);

// Marcum Q-function (M=1)
float liquid_MarcumQ1f(float _alpha,
                       float _beta);

// sin(pi x) / (pi x)
float sincf(float _x);

// next power of 2 : y = ceil(log2(_x))
unsigned int liquid_nextpow2(unsigned int _x);

// (n choose k) = n! / ( k! (n-k)! )
float liquid_nchoosek(unsigned int _n, unsigned int _k);

// 
// Windowing functions
//

// Kaiser-Bessel derived window (single sample)
//  _n      :   index (0 <= _n < _N)
//  _N      :   length of filter (must be even)
//  _beta   :   Kaiser window parameter (_beta > 0)
float liquid_kbd(unsigned int _n, unsigned int _N, float _beta);

// Kaiser-Bessel derived window (full window)
//  _n      :   length of filter (must be even)
//  _beta   :   Kaiser window parameter (_beta > 0)
//  _w      :   resulting window
void liquid_kbd_window(unsigned int _n, float _beta, float * _w);

// Kaiser window
//  _n      :   window index
//  _N      :   full window length
//  _beta   :   Kaiser-Bessel window shape parameter
//  _dt     :   fractional sample offset
float kaiser(unsigned int _n,
             unsigned int _N,
             float _beta,
             float _dt);

// Hamming window
//  _n      :   window index
//  _N      :   full window length
float hamming(unsigned int _n, unsigned int _N);

// Hann window
//  _n      :   window index
//  _N      :   full window length
float hann(unsigned int _n, unsigned int _N);

// Blackman-harris window
//  _n      :   window index
//  _N      :   full window length
float blackmanharris(unsigned int _n, unsigned int _N);


// polynomials


#define POLY_MANGLE_DOUBLE(name)    LIQUID_CONCAT(poly,   name)
#define POLY_MANGLE_FLOAT(name)     LIQUID_CONCAT(polyf,  name)

#define POLY_MANGLE_CDOUBLE(name)   LIQUID_CONCAT(polyc,  name)
#define POLY_MANGLE_CFLOAT(name)    LIQUID_CONCAT(polycf, name)

// large macro
//   POLY   : name-mangling macro
//   T      : data type
//   TC     : data type (complex)
#define LIQUID_POLY_DEFINE_API(POLY,T,TC)                       \
/* evaluate polynomial _p (order _k-1) at value _x  */          \
T POLY(_val)(T * _p, unsigned int _k, T _x);                    \
                                                                \
/* least-squares polynomial fit (order _k-1) */                 \
void POLY(_fit)(T * _x,                                         \
                T * _y,                                         \
                unsigned int _n,                                \
                T * _p,                                         \
                unsigned int _k);                               \
                                                                \
/* Lagrange polynomial exact fit (order _n-1) */                \
void POLY(_fit_lagrange)(T * _x,                                \
                         T * _y,                                \
                         unsigned int _n,                       \
                         T * _p);                               \
                                                                \
/* Lagrange polynomial interpolation */                         \
T POLY(_interp_lagrange)(T * _x,                                \
                         T * _y,                                \
                         unsigned int _n,                       \
                         T   _x0);                              \
                                                                \
/* Lagrange polynomial fit (barycentric form) */                \
void POLY(_fit_lagrange_barycentric)(T * _x,                    \
                                     unsigned int _n,           \
                                     T * _w);                   \
                                                                \
/* Lagrange polynomial interpolation (barycentric form) */      \
T POLY(_val_lagrange_barycentric)(T * _x,                       \
                                  T * _y,                       \
                                  T * _w,                       \
                                  T   _x0,                      \
                                  unsigned int _n);             \
                                                                \
/* expands the polynomial:                                      \
 *  P_n(x) = (1+x)^n                                            \
 * as                                                           \
 *  P_n(x) = p[0] + p[1]*x + p[2]*x^2 + ... + p[n]x^n           \
 * NOTE: _p has order n=m+k (array is length n+1)               \
 */                                                             \
void POLY(_expandbinomial)(unsigned int _n,                     \
                           T * _p);                             \
                                                                \
/* expands the polynomial:                                      \
 *  P_n(x) = (1+x)^m * (1-x)^k                                  \
 * as                                                           \
 *  P_n(x) = p[0] + p[1]*x + p[2]*x^2 + ... + p[n]x^n           \
 * NOTE: _p has order n=m+k (array is length n+1)               \
 */                                                             \
void POLY(_expandbinomial_pm)(unsigned int _m,                  \
                              unsigned int _k,                  \
                              T * _p);                          \
                                                                \
/* expands the polynomial:                                      \
 *  P_n(x) = (x-r[0]) * (x-r[1]) * ... * (x-r[n-1])             \
 * as                                                           \
 *  P_n(x) = c[0] + c[1]*x + ... + c[n]*x^n                     \
 * where r[0],r[1],...,r[n-1] are the roots of P_n(x)           \
 * NOTE: _c has order _n (array is length _n+1)                 \
 */                                                             \
void POLY(_expandroots)(T * _a,                                 \
                        unsigned int _n,                        \
                        T * _c);                                \
                                                                \
/* expands the polynomial:                                      \
 *  P_n(x) =                                                    \
 *    (x*b[0]-a[0]) * (x*b[1]-a[1]) * ... * (x*b[n-1]-a[n-1])   \
 * as                                                           \
 *  P_n(x) = c[0] + c[1]*x + ... + c[n]*x^n                     \
 * NOTE: _c has order _n (array is length _n+1)                 \
 */                                                             \
void POLY(_expandroots2)(T * _a,                                \
                         T * _b,                                \
                         unsigned int _n,                       \
                         T * _c);                               \
                                                                \
/* find roots of the polynomial (complex)                   */  \
/*  _poly   : poly array, ascending powers [size: _k x 1]   */  \
/*  _k      : poly length (poly order = _k - 1)             */  \
/*  _roots  : resulting complex roots [size: _k-1 x 1]      */  \
void POLY(_findroots)(T *          _poly,                       \
                      unsigned int _n,                          \
                      TC *         _roots);                     \
                                                                \
/* find the complex roots of the polynomial using the       */  \
/* Durand-Kerner method                                     */  \
void POLY(_findroots_durandkerner)(T *          _poly,          \
                                   unsigned int _k,             \
                                   TC *         _roots);        \
                                                                \
/* find the complex roots of the polynomial using           */  \
/* Bairstow's method                                        */  \
void POLY(_findroots_bairstow)(T *          _poly,              \
                               unsigned int _k,                 \
                               TC *         _roots);            \
                                                                \
/* expands the multiplication of two polynomials */             \
void POLY(_mul)(T *          _a,                                \
                unsigned int _order_a,                          \
                T *          _b,                                \
                unsigned int _order_b,                          \
                T *          _c);                               \

LIQUID_POLY_DEFINE_API(POLY_MANGLE_DOUBLE,
                       double,
                       liquid_double_complex)

LIQUID_POLY_DEFINE_API(POLY_MANGLE_FLOAT,
                       float,
                       liquid_float_complex)

LIQUID_POLY_DEFINE_API(POLY_MANGLE_CDOUBLE,
                       liquid_double_complex,
                       liquid_double_complex)

LIQUID_POLY_DEFINE_API(POLY_MANGLE_CFLOAT,
                       liquid_float_complex,
                       liquid_float_complex)

#if 0
// expands the polynomial: (1+x)^n
void poly_binomial_expand(unsigned int _n, int * _c);

// expands the polynomial: (1+x)^k * (1-x)^(n-k)
void poly_binomial_expand_pm(unsigned int _n,
                             unsigned int _k,
                             int * _c);
#endif

// 
// modular arithmetic, etc.
//

// maximum number of factors
#define LIQUID_MAX_FACTORS (40)

// is number prime?
int liquid_is_prime(unsigned int _n);

// compute number's prime factors
//  _n          :   number to factor
//  _factors    :   pre-allocated array of factors [size: LIQUID_MAX_FACTORS x 1]
//  _num_factors:   number of factors found, sorted ascending
void liquid_factor(unsigned int   _n,
                   unsigned int * _factors,
                   unsigned int * _num_factors);

// compute number's unique prime factors
//  _n          :   number to factor
//  _factors    :   pre-allocated array of factors [size: LIQUID_MAX_FACTORS x 1]
//  _num_factors:   number of unique factors found, sorted ascending
void liquid_unique_factor(unsigned int   _n,
                          unsigned int * _factors,
                          unsigned int * _num_factors);

// compute c = base^exp (mod n)
unsigned int liquid_modpow(unsigned int _base,
                           unsigned int _exp,
                           unsigned int _n);

// find smallest primitive root of _n
unsigned int liquid_primitive_root(unsigned int _n);

// find smallest primitive root of _n, assuming _n is prime
unsigned int liquid_primitive_root_prime(unsigned int _n);

// Euler's totient function
unsigned int liquid_totient(unsigned int _n);


//
// MODULE : matrix
//

#define MATRIX_MANGLE_DOUBLE(name)  LIQUID_CONCAT(matrix,   name)
#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(matrixf,  name)

#define MATRIX_MANGLE_CDOUBLE(name) LIQUID_CONCAT(matrixc,  name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(matrixcf, name)

// large macro
//   MATRIX : name-mangling macro
//   T      : data type
#define LIQUID_MATRIX_DEFINE_API(MATRIX,T)                      \
void MATRIX(_print)(T * _x,                                     \
                    unsigned int _rx,                           \
                    unsigned int _cx);                          \
void MATRIX(_add)(T * _x,                                       \
                  T * _y,                                       \
                  T * _z,                                       \
                  unsigned int _r,                              \
                  unsigned int _c);                             \
void MATRIX(_sub)(T * _x,                                       \
                  T * _y,                                       \
                  T * _z,                                       \
                  unsigned int _r,                              \
                  unsigned int _c);                             \
void MATRIX(_pmul)(T * _x,                                      \
                   T * _y,                                      \
                   T * _z,                                      \
                   unsigned int _r,                             \
                   unsigned int _c);                            \
void MATRIX(_pdiv)(T * _x,                                      \
                   T * _y,                                      \
                   T * _z,                                      \
                   unsigned int _r,                             \
                   unsigned int _c);                            \
void MATRIX(_mul)(T * _x, unsigned int _rx, unsigned int _cx,   \
                  T * _y, unsigned int _ry, unsigned int _cy,   \
                  T * _z, unsigned int _rz, unsigned int _cz);  \
void MATRIX(_div)(T * _x, T * _y, T * _z, unsigned int _n);     \
T    MATRIX(_det)(T * _x, unsigned int _r, unsigned int _c);    \
void MATRIX(_trans)(T * _x, unsigned int _rx, unsigned int _cx);\
void MATRIX(_hermitian)(T * _x,                                 \
                        unsigned int _rx,                       \
                        unsigned int _cx);                      \
                                                                \
/* compute x*x' on [m x n] matrix, result: [m x m]          */  \
void MATRIX(_mul_transpose)(T * _x,                             \
                            unsigned int _m,                    \
                            unsigned int _n,                    \
                            T * _xxT);                          \
/* compute x'*x on [m x n] matrix, result: [n x n]          */  \
void MATRIX(_transpose_mul)(T * _x,                             \
                            unsigned int _m,                    \
                            unsigned int _n,                    \
                            T * _xTx);                          \
/* compute x*x.' on [m x n] matrix, result: [m x m]          */ \
void MATRIX(_mul_hermitian)(T * _x,                             \
                            unsigned int _m,                    \
                            unsigned int _n,                    \
                            T * _xxH);                          \
/* compute x.'*x on [m x n] matrix, result: [n x n]          */ \
void MATRIX(_hermitian_mul)(T * _x,                             \
                            unsigned int _m,                    \
                            unsigned int _n,                    \
                            T * _xHx);                          \
                                                                \
void MATRIX(_aug)(T * _x, unsigned int _rx, unsigned int _cx,   \
                  T * _y, unsigned int _ry, unsigned int _cy,   \
                  T * _z, unsigned int _rz, unsigned int _cz);  \
void MATRIX(_inv)(T * _x,                                       \
                  unsigned int _rx,                             \
                  unsigned int _cx);                            \
void MATRIX(_eye)(T * _x,                                       \
                  unsigned int _n);                             \
void MATRIX(_ones)(T * _x,                                      \
                   unsigned int _r,                             \
                   unsigned int _c);                            \
void MATRIX(_zeros)(T * _x,                                     \
                    unsigned int _r,                            \
                    unsigned int _c);                           \
void MATRIX(_gjelim)(T * _x,                                    \
                     unsigned int _rx,                          \
                     unsigned int _cx);                         \
void MATRIX(_pivot)(T * _x,                                     \
               unsigned int _rx,                                \
               unsigned int _cx,                                \
               unsigned int _r,                                 \
               unsigned int _c);                                \
void MATRIX(_swaprows)(T * _x,                                  \
                  unsigned int _rx,                             \
                  unsigned int _cx,                             \
                  unsigned int _r1,                             \
                  unsigned int _r2);                            \
void MATRIX(_linsolve)(T * _A,                                  \
                       unsigned int _r,                         \
                       T * _b,                                  \
                       T * _x,                                  \
                       void * _opts);                           \
void MATRIX(_cgsolve)(T * _A,                                   \
                      unsigned int _r,                          \
                      T * _b,                                   \
                      T * _x,                                   \
                      void * _opts);                            \
void MATRIX(_ludecomp_crout)(T * _x,                            \
                             unsigned int _rx,                  \
                             unsigned int _cx,                  \
                             T * _L,                            \
                             T * _U,                            \
                             T * _P);                           \
void MATRIX(_ludecomp_doolittle)(T * _x,                        \
                                 unsigned int _rx,              \
                                 unsigned int _cx,              \
                                 T * _L,                        \
                                 T * _U,                        \
                                 T * _P);                       \
void MATRIX(_gramschmidt)(T * _A,                               \
                          unsigned int _rx,                     \
                          unsigned int _cx,                     \
                          T * _U);                              \
void MATRIX(_qrdecomp_gramschmidt)(T * _x,                      \
                                   unsigned int _rx,            \
                                   unsigned int _cx,            \
                                   T * _Q,                      \
                                   T * _R);                     \
void MATRIX(_chol)(T * _A,                                      \
                   unsigned int _n,                             \
                   T * _L);                                     \

#define matrix_access(X,R,C,r,c) ((X)[(r)*(C)+(c)])

#define matrixc_access(X,R,C,r,c)   matrix_access(X,R,C,r,c)
#define matrixf_access(X,R,C,r,c)   matrix_access(X,R,C,r,c)
#define matrixcf_access(X,R,C,r,c)  matrix_access(X,R,C,r,c)

LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_FLOAT,   float)
LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_DOUBLE,  double)

LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_CFLOAT,  liquid_float_complex)
LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_CDOUBLE, liquid_double_complex)


#define SMATRIX_MANGLE_BOOL(name)   LIQUID_CONCAT(smatrixb,  name)
#define SMATRIX_MANGLE_FLOAT(name)  LIQUID_CONCAT(smatrixf,  name)
#define SMATRIX_MANGLE_INT(name)    LIQUID_CONCAT(smatrixi,  name)

// sparse 'alist' matrix type (similar to MacKay, Davey Lafferty convention)
// large macro
//   SMATRIX    : name-mangling macro
//   T          : primitive data type
#define LIQUID_SMATRIX_DEFINE_API(SMATRIX,T)                    \
typedef struct SMATRIX(_s) * SMATRIX();                         \
                                                                \
/* create _M x _N matrix, initialized with zeros */             \
SMATRIX() SMATRIX(_create)(unsigned int _M,                     \
                           unsigned int _N);                    \
                                                                \
/* create _M x _N matrix, initialized on array */               \
SMATRIX() SMATRIX(_create_array)(T *          _x,               \
                                 unsigned int _m,               \
                                 unsigned int _n);              \
                                                                \
/* destroy object */                                            \
void SMATRIX(_destroy)(SMATRIX() _q);                           \
                                                                \
/* print compact form */                                        \
void SMATRIX(_print)(SMATRIX() _q);                             \
                                                                \
/* print expanded form */                                       \
void SMATRIX(_print_expanded)(SMATRIX() _q);                    \
                                                                \
/* query properties methods */                                  \
void SMATRIX(_size)(SMATRIX()      _q,                          \
                    unsigned int * _m,                          \
                    unsigned int * _n);                         \
                                                                \
/* zero all elements */                                         \
void SMATRIX(_clear)(SMATRIX() _q); /* zero and keep memory  */ \
void SMATRIX(_reset)(SMATRIX() _q); /* zero and clear memory */ \
                                                                \
/* determine if value has been set (allocated memory) */        \
int SMATRIX(_isset)(SMATRIX()    _q,                            \
                    unsigned int _m,                            \
                    unsigned int _n);                           \
                                                                \
/* inserts/deletes element at index (memory allocation) */      \
void SMATRIX(_insert)(SMATRIX()    _q,                          \
                      unsigned int _m,                          \
                      unsigned int _n,                          \
                      T            _v);                         \
void SMATRIX(_delete)(SMATRIX()    _q,                          \
                      unsigned int _m,                          \
                      unsigned int _n);                         \
                                                                \
/* sets/gets the value (with memory allocation if needed) */    \
void SMATRIX(_set)(SMATRIX()    _q,                             \
                   unsigned int _m,                             \
                   unsigned int _n,                             \
                   T            _v);                            \
T SMATRIX(_get)(SMATRIX()    _q,                                \
                unsigned int _m,                                \
                unsigned int _n);                               \
                                                                \
/* initialize to identity matrix */                             \
void SMATRIX(_eye)(SMATRIX() _q);                               \
                                                                \
/* multiply two sparse binary matrices */                       \
void SMATRIX(_mul)(SMATRIX() _x,                                \
                   SMATRIX() _y,                                \
                   SMATRIX() _z);                               \
                                                                \
/* multiply sparse matrix by vector         */                  \
/*  _q  :   sparse matrix                   */                  \
/*  _x  :   input vector [size: _N x 1]     */                  \
/*  _y  :   output vector [size: _M x 1]    */                  \
void SMATRIX(_vmul)(SMATRIX() _q,                               \
                    T *       _x,                               \
                    T *       _y);                              \

LIQUID_SMATRIX_DEFINE_API(SMATRIX_MANGLE_BOOL,  unsigned char)
LIQUID_SMATRIX_DEFINE_API(SMATRIX_MANGLE_FLOAT, float)
LIQUID_SMATRIX_DEFINE_API(SMATRIX_MANGLE_INT,   short int)

// 
// smatrix cross methods
//

// multiply sparse binary matrix by floating-point matrix
//  _q  :   sparse matrix [size: A->M x A->N]
//  _x  :   input vector  [size:  mx  x  nx ]
//  _y  :   output vector [size:  my  x  ny ]
void smatrixb_mulf(smatrixb     _A,
                   float *      _x,
                   unsigned int _mx,
                   unsigned int _nx,
                   float *      _y,
                   unsigned int _my,
                   unsigned int _ny);

// multiply sparse binary matrix by floating-point vector
//  _q  :   sparse matrix
//  _x  :   input vector [size: _N x 1]
//  _y  :   output vector [size: _M x 1]
void smatrixb_vmulf(smatrixb _q,
                    float *  _x,
                    float *  _y);


//
// MODULE : modem (modulator/demodulator)
//

// Maximum number of allowed bits per symbol
#define MAX_MOD_BITS_PER_SYMBOL 8

// Modulation schemes available
#define LIQUID_MODEM_NUM_SCHEMES      (52)

typedef enum {
    LIQUID_MODEM_UNKNOWN=0, // Unknown modulation scheme

    // Phase-shift keying (PSK)
    LIQUID_MODEM_PSK2,      LIQUID_MODEM_PSK4,
    LIQUID_MODEM_PSK8,      LIQUID_MODEM_PSK16,
    LIQUID_MODEM_PSK32,     LIQUID_MODEM_PSK64,
    LIQUID_MODEM_PSK128,    LIQUID_MODEM_PSK256,

    // Differential phase-shift keying (DPSK)
    LIQUID_MODEM_DPSK2,     LIQUID_MODEM_DPSK4,
    LIQUID_MODEM_DPSK8,     LIQUID_MODEM_DPSK16,
    LIQUID_MODEM_DPSK32,    LIQUID_MODEM_DPSK64,
    LIQUID_MODEM_DPSK128,   LIQUID_MODEM_DPSK256,

    // amplitude-shift keying
    LIQUID_MODEM_ASK2,      LIQUID_MODEM_ASK4,
    LIQUID_MODEM_ASK8,      LIQUID_MODEM_ASK16,
    LIQUID_MODEM_ASK32,     LIQUID_MODEM_ASK64,
    LIQUID_MODEM_ASK128,    LIQUID_MODEM_ASK256,

    // rectangular quadrature amplitude-shift keying (QAM)
    LIQUID_MODEM_QAM4,
    LIQUID_MODEM_QAM8,      LIQUID_MODEM_QAM16,
    LIQUID_MODEM_QAM32,     LIQUID_MODEM_QAM64,
    LIQUID_MODEM_QAM128,    LIQUID_MODEM_QAM256,

    // amplitude phase-shift keying (APSK)
    LIQUID_MODEM_APSK4,
    LIQUID_MODEM_APSK8,     LIQUID_MODEM_APSK16,
    LIQUID_MODEM_APSK32,    LIQUID_MODEM_APSK64,
    LIQUID_MODEM_APSK128,   LIQUID_MODEM_APSK256,

    // specific modem types
    LIQUID_MODEM_BPSK,      // Specific: binary PSK
    LIQUID_MODEM_QPSK,      // specific: quaternary PSK
    LIQUID_MODEM_OOK,       // Specific: on/off keying
    LIQUID_MODEM_SQAM32,    // 'square' 32-QAM
    LIQUID_MODEM_SQAM128,   // 'square' 128-QAM
    LIQUID_MODEM_V29,       // V.29 star constellation
    LIQUID_MODEM_ARB16OPT,  // optimal 16-QAM
    LIQUID_MODEM_ARB32OPT,  // optimal 32-QAM
    LIQUID_MODEM_ARB64OPT,  // optimal 64-QAM
    LIQUID_MODEM_ARB128OPT, // optimal 128-QAM
    LIQUID_MODEM_ARB256OPT, // optimal 256-QAM
    LIQUID_MODEM_ARB64VT,   // Virginia Tech logo

    // arbitrary modem type
    LIQUID_MODEM_ARB        // arbitrary QAM
} modulation_scheme;

// structure for holding full modulation type descriptor
struct modulation_type_s {
    const char * name;          // short name (e.g. 'bpsk')
    const char * fullname;      // full name (e.g. 'binary phase-shift keying')
    modulation_scheme scheme;   // modulation scheme (e.g. LIQUID_MODEM_BPSK)
    unsigned int bps;           // modulation depth (e.g. 1)
};

// full modulation type descriptor
extern const struct modulation_type_s modulation_types[LIQUID_MODEM_NUM_SCHEMES];

// Print compact list of existing and available modulation schemes
void liquid_print_modulation_schemes();

// returns modulation_scheme based on input string
modulation_scheme liquid_getopt_str2mod(const char * _str);

// query basic modulation types
int liquid_modem_is_psk(modulation_scheme _ms);
int liquid_modem_is_dpsk(modulation_scheme _ms);
int liquid_modem_is_ask(modulation_scheme _ms);
int liquid_modem_is_qam(modulation_scheme _ms);
int liquid_modem_is_apsk(modulation_scheme _ms);

// useful functions

// counts the number of different bits between two symbols
unsigned int count_bit_errors(unsigned int _s1, unsigned int _s2);

// counts the number of different bits between two arrays of symbols
//  _msg0   :   original message [size: _n x 1]
//  _msg1   :   copy of original message [size: _n x 1]
//  _n      :   message size
unsigned int count_bit_errors_array(unsigned char * _msg0,
                                    unsigned char * _msg1,
                                    unsigned int _n);

// converts binary-coded decimal (BCD) to gray, ensuring successive values
// differ by exactly one bit
unsigned int gray_encode(unsigned int symbol_in);

// converts a gray-encoded symbol to binary-coded decimal (BCD)
unsigned int gray_decode(unsigned int symbol_in);

// pack soft bits into symbol
//  _soft_bits  :   soft input bits [size: _bps x 1]
//  _bps        :   bits per symbol
//  _sym_out    :   output symbol, value in [0,2^_bps)
void liquid_pack_soft_bits(unsigned char * _soft_bits,
                           unsigned int _bps,
                           unsigned int * _sym_out);

// unpack soft bits into symbol
//  _sym_in     :   input symbol, value in [0,2^_bps)
//  _bps        :   bits per symbol
//  _soft_bits  :   soft output bits [size: _bps x 1]
void liquid_unpack_soft_bits(unsigned int _sym_in,
                             unsigned int _bps,
                             unsigned char * _soft_bits);


//
// Linear modem
//

#define LIQUID_MODEM_MANGLE_FLOAT(name) LIQUID_CONCAT(modem,name)

// Macro    :   MODEM
//  MODEM   :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
#define LIQUID_MODEM_DEFINE_API(MODEM,T,TC)                     \
                                                                \
/* define struct pointer */                                     \
typedef struct MODEM(_s) * MODEM();                             \
                                                                \
/* create digital modem object, allocating memory as necessary */ \
MODEM() MODEM(_create)(modulation_scheme _scheme);                  \
                                                                \
/* create arbitrary digital modem object */                     \
MODEM() MODEM(_create_arbitrary)(TC * _table, unsigned int _M); \
                                                                \
/* recreate modulation scheme, re-allocating memory as necessary */ \
MODEM() MODEM(_recreate)(MODEM() _q,                            \
                         modulation_scheme _scheme);            \
                                                                \
void MODEM(_destroy)(MODEM() _q);                               \
void MODEM(_print)(  MODEM() _q);                               \
void MODEM(_reset)(  MODEM() _q);                               \
                                                                \
/* generate random symbol */                                    \
unsigned int MODEM(_gen_rand_sym)(MODEM() _q);                  \
                                                                \
/* Accessor functions */                                        \
unsigned int MODEM(_get_bps)(MODEM() _q);                       \
                                                                \
/* generic modulate function; simply queries modem scheme   */  \
/* and calls appropriate subroutine                         */  \
/*  _q  :   modem object                                    */  \
/*  _s  :   input symbol                                    */  \
/*  _x  :   output sample                                   */  \
void MODEM(_modulate)(MODEM() _q,                               \
                      unsigned int _s,                          \
                      TC *_y);                                  \
                                                                \
/* generic hard-decision demodulation function              */  \
/*  _q  :   modem object                                    */  \
/*  _x  :   input sample                                    */  \
/*  _s  :   output symbol                                   */  \
void MODEM(_demodulate)(MODEM() _q,                             \
                        TC _x,                                  \
                        unsigned int * _s);                     \
                                                                \
/* generic soft-decision demodulation function              */  \
/*  _q          :   modem object                            */  \
/*  _x          :   input sample                            */  \
/*  _s          :   output hard symbol                      */  \
/*  _soft_bits  :   output soft bits                        */  \
void MODEM(_demodulate_soft)(MODEM() _q,                        \
                             TC _x,                             \
                             unsigned int  * _s,                \
                             unsigned char * _soft_bits);       \
                                                                \
/* get demodulator's estimated transmit sample */               \
void MODEM(_get_demodulator_sample)(MODEM() _q,                 \
                                    TC * _x_hat);               \
                                                                \
/* get demodulator phase error */                               \
float MODEM(_get_demodulator_phase_error)(MODEM() _q);          \
                                                                \
/* get demodulator error vector magnitude */                    \
float MODEM(_get_demodulator_evm)(MODEM() _q);                  \

// define modem APIs
LIQUID_MODEM_DEFINE_API(LIQUID_MODEM_MANGLE_FLOAT,float,liquid_float_complex)


//
// continuous-phase modulation
//

// gmskmod : GMSK modulator
typedef struct gmskmod_s * gmskmod;

// create gmskmod object
//  _k      :   samples/symbol
//  _m      :   filter delay (symbols)
//  _BT     :   excess bandwidth factor
gmskmod gmskmod_create(unsigned int _k,
                       unsigned int _m,
                       float        _BT);
void gmskmod_destroy(gmskmod _q);
void gmskmod_print(gmskmod _q);
void gmskmod_reset(gmskmod _q);
void gmskmod_modulate(gmskmod _q,
                      unsigned int _sym,
                      liquid_float_complex * _y);


// gmskdem : GMSK demodulator
typedef struct gmskdem_s * gmskdem;

// create gmskdem object
//  _k      :   samples/symbol
//  _m      :   filter delay (symbols)
//  _BT     :   excess bandwidth factor
gmskdem gmskdem_create(unsigned int _k,
                       unsigned int _m,
                       float        _BT);
void gmskdem_destroy(gmskdem _q);
void gmskdem_print(gmskdem _q);
void gmskdem_reset(gmskdem _q);
void gmskdem_set_eq_bw(gmskdem _q, float _bw);
void gmskdem_demodulate(gmskdem _q,
                        liquid_float_complex * _y,
                        unsigned int * _sym);

// 
// Analog frequency modulator
//
#define LIQUID_FREQMOD_MANGLE_FLOAT(name) LIQUID_CONCAT(freqmod,name)

// Macro    :   FREQMOD (analog frequency modulator)
//  FREQMOD :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
#define LIQUID_FREQMOD_DEFINE_API(FREQMOD,T,TC)                 \
                                                                \
/* define struct pointer */                                     \
typedef struct FREQMOD(_s) * FREQMOD();                         \
                                                                \
/* create freqmod object (frequency modulator)              */  \
/*  _kf     :   modulation factor                           */  \
FREQMOD() FREQMOD(_create)(float _kf);                          \
                                                                \
/* destroy freqmod object                                   */  \
void FREQMOD(_destroy)(FREQMOD() _q);                           \
                                                                \
/* print freqmod object internals                           */  \
void FREQMOD(_print)(FREQMOD() _q);                             \
                                                                \
/* reset state                                              */  \
void FREQMOD(_reset)(FREQMOD() _q);                             \
                                                                \
/* modulate single sample                                   */  \
/*  _q      :   frequency modulator object                  */  \
/*  _m      :   message signal m(t)                         */  \
/*  _s      :   complex baseband signal s(t)                */  \
void FREQMOD(_modulate)(FREQMOD() _q,                           \
                        T         _m,                           \
                        TC *      _s);                          \
                                                                \
/* modulate block of samples                                */  \
/*  _q      :   frequency modulator object                  */  \
/*  _m      :   message signal m(t), [size: _n x 1]         */  \
/*  _n      :   number of input, output samples             */  \
/*  _s      :   complex baseband signal s(t) [size: _n x 1] */  \
void FREQMOD(_modulate_block)(FREQMOD()    _q,                  \
                              T *          _m,                  \
                              unsigned int _n,                  \
                              TC *         _s);                 \

// define freqmod APIs
LIQUID_FREQMOD_DEFINE_API(LIQUID_FREQMOD_MANGLE_FLOAT,float,liquid_float_complex)



// 
// Analog frequency demodulator
//

#define LIQUID_FREQDEM_MANGLE_FLOAT(name) LIQUID_CONCAT(freqdem,name)

// Macro    :   FREQDEM (analog frequency modulator)
//  FREQDEM :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
#define LIQUID_FREQDEM_DEFINE_API(FREQDEM,T,TC)                 \
                                                                \
/* define struct pointer */                                     \
typedef struct FREQDEM(_s) * FREQDEM();                         \
                                                                \
/* create freqdem object (frequency modulator)              */  \
/*  _kf      :   modulation factor                          */  \
FREQDEM() FREQDEM(_create)(float _kf);                          \
                                                                \
/* destroy freqdem object                                   */  \
void FREQDEM(_destroy)(FREQDEM() _q);                           \
                                                                \
/* print freqdem object internals                           */  \
void FREQDEM(_print)(FREQDEM() _q);                             \
                                                                \
/* reset state                                              */  \
void FREQDEM(_reset)(FREQDEM() _q);                             \
                                                                \
/* demodulate sample                                        */  \
/*  _q      :   frequency modulator object                  */  \
/*  _r      :   received signal r(t)                        */  \
/*  _m      :   output message signal m(t)                  */  \
void FREQDEM(_demodulate)(FREQDEM() _q,                         \
                          TC        _r,                         \
                          T *       _m);                        \
                                                                \
/* demodulate block of samples                              */  \
/*  _q      :   frequency demodulator object                */  \
/*  _r      :   received signal r(t) [size: _n x 1]         */  \
/*  _n      :   number of input, output samples             */  \
/*  _m      :   message signal m(t), [size: _n x 1]         */  \
void FREQDEM(_demodulate_block)(FREQDEM()    _q,                \
                                TC *         _r,                \
                                unsigned int _n,                \
                                T *          _m);               \

// define freqdem APIs
LIQUID_FREQDEM_DEFINE_API(LIQUID_FREQDEM_MANGLE_FLOAT,float,liquid_float_complex)



// amplitude modulation types
typedef enum {
    LIQUID_AMPMODEM_DSB=0,  // double side-band
    LIQUID_AMPMODEM_USB,    // single side-band (upper)
    LIQUID_AMPMODEM_LSB     // single side-band (lower)
} liquid_ampmodem_type;

typedef struct ampmodem_s * ampmodem;

// create ampmodem object
//  _m                  :   modulation index
//  _fc                 :   carrier frequency, range: [-0.5,0.5]
//  _type               :   AM type (e.g. LIQUID_AMPMODEM_DSB)
//  _suppressed_carrier :   carrier suppression flag
ampmodem ampmodem_create(float _m,
                         float _fc,
                         liquid_ampmodem_type _type,
                         int _suppressed_carrier);

// destroy ampmodem object
void ampmodem_destroy(ampmodem _fm);

// print ampmodem object internals
void ampmodem_print(ampmodem _fm);

// reset ampmodem object state
void ampmodem_reset(ampmodem _fm);

// modulate sample
void ampmodem_modulate(ampmodem _fm,
                       float _x,
                       liquid_float_complex *_y);

// demodulate sample
void ampmodem_demodulate(ampmodem _fm,
                         liquid_float_complex _y,
                         float *_x);


//
// MODULE : multichannel
//


#define FIRPFBCH_NYQUIST        0
#define FIRPFBCH_ROOTNYQUIST    1

#define LIQUID_ANALYZER         0
#define LIQUID_SYNTHESIZER      1


//
// Finite impulse response polyphase filterbank channelizer
//

#define FIRPFBCH_MANGLE_CRCF(name)  LIQUID_CONCAT(firpfbch_crcf,name)
#define FIRPFBCH_MANGLE_CCCF(name)  LIQUID_CONCAT(firpfbch_cccf,name)

// Macro:
//   FIRPFBCH   : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIRPFBCH_DEFINE_API(FIRPFBCH,TO,TC,TI)           \
typedef struct FIRPFBCH(_s) * FIRPFBCH();                       \
                                                                \
/* create finite impulse response polyphase filter-bank     */  \
/* channelizer object from external coefficients            */  \
/*  _type   : channelizer type, e.g. LIQUID_ANALYZER        */  \
/*  _M      : number of channels                            */  \
/*  _p      : number of coefficients for each channel       */  \
/*  _h      : coefficients [size: _M*_p x 1]                */  \
FIRPFBCH() FIRPFBCH(_create)(int          _type,                \
                             unsigned int _M,                   \
                             unsigned int _p,                   \
                             TC *         _h);                  \
                                                                \
/* create FIR polyphase filterbank channelizer object with  */  \
/* prototype filter based on windowed Kaiser design         */  \
/*  _type   : type (LIQUID_ANALYZER | LIQUID_SYNTHESIZER)   */  \
/*  _M      : number of channels                            */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _As     : stop-band attentuation [dB]                   */  \
FIRPFBCH() FIRPFBCH(_create_kaiser)(int          _type,         \
                                    unsigned int _M,            \
                                    unsigned int _m,            \
                                    float        _As);          \
                                                                \
/* create FIR polyphase filterbank channelizer object with  */  \
/* prototype root-Nyquist filter                            */  \
/*  _type   : type (LIQUID_ANALYZER | LIQUID_SYNTHESIZER)   */  \
/*  _M      : number of channels                            */  \
/*  _m      : filter delay (symbols)                        */  \
/*  _beta   : filter excess bandwidth factor, in [0,1]      */  \
/*  _ftype  : filter prototype (rrcos, rkaiser, etc.)       */  \
FIRPFBCH() FIRPFBCH(_create_rnyquist)(int          _type,       \
                                      unsigned int _M,          \
                                      unsigned int _m,          \
                                      float        _beta,       \
                                      int          _ftype);     \
                                                                \
/* destroy firpfbch object                                  */  \
void FIRPFBCH(_destroy)(FIRPFBCH() _q);                         \
                                                                \
/* clear/reset firpfbch internal state                      */  \
void FIRPFBCH(_reset)(FIRPFBCH() _q);                           \
                                                                \
/* print firpfbch internal parameters to stdout             */  \
void FIRPFBCH(_print)(FIRPFBCH() _q);                           \
                                                                \
/* execute filterbank as synthesizer on block of samples    */  \
/*  _q      : filterbank channelizer object                 */  \
/*  _x      : channelized input, [size: num_channels x 1]   */  \
/*  _y      : output time series, [size: num_channels x 1]  */  \
void FIRPFBCH(_synthesizer_execute)(FIRPFBCH() _q,              \
                                    TI *       _x,              \
                                    TO *       _y);             \
                                                                \
/* execute filterbank as analyzer on block of samples       */  \
/*  _q      : filterbank channelizer object                 */  \
/*  _x      : input time series, [size: num_channels x 1]   */  \
/*  _y      : channelized output, [size: num_channels x 1]  */  \
void FIRPFBCH(_analyzer_execute)(FIRPFBCH() _q,                 \
                                 TI *       _x,                 \
                                 TO *       _y);                \


LIQUID_FIRPFBCH_DEFINE_API(FIRPFBCH_MANGLE_CRCF,
                           liquid_float_complex,
                           float,
                           liquid_float_complex)

LIQUID_FIRPFBCH_DEFINE_API(FIRPFBCH_MANGLE_CCCF,
                           liquid_float_complex,
                           liquid_float_complex,
                           liquid_float_complex)


//
// Finite impulse response polyphase filterbank channelizer
// with output rate 2 Fs / M
//

#define FIRPFBCH2_MANGLE_CRCF(name) LIQUID_CONCAT(firpfbch2_crcf,name)

// Macro:
//   FIRPFBCH2  : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIRPFBCH2_DEFINE_API(FIRPFBCH2,TO,TC,TI)         \
typedef struct FIRPFBCH2(_s) * FIRPFBCH2();                     \
                                                                \
/* create firpfbch2 object                                  */  \
/*  _type   :   channelizer type (e.g. LIQUID_ANALYZER)     */  \
/*  _M      :   number of channels (must be even)           */  \
/*  _m      :   prototype filter semi-lenth, length=2*M*m   */  \
/*  _h      :   prototype filter coefficient array          */  \
FIRPFBCH2() FIRPFBCH2(_create)(int          _type,              \
                               unsigned int _M,                 \
                               unsigned int _m,                 \
                               TC *         _h);                \
                                                                \
/* create firpfbch2 object using Kaiser window prototype    */  \
/*  _type   :   channelizer type (e.g. LIQUID_ANALYZER)     */  \
/*  _M      :   number of channels (must be even)           */  \
/*  _m      :   prototype filter semi-lenth, length=2*M*m+1 */  \
/*  _As     :   filter stop-band attenuation [dB]           */  \
FIRPFBCH2() FIRPFBCH2(_create_kaiser)(int          _type,       \
                                      unsigned int _M,          \
                                      unsigned int _m,          \
                                      float        _As);        \
                                                                \
/* destroy firpfbch2 object, freeing internal memory        */  \
void FIRPFBCH2(_destroy)(FIRPFBCH2() _q);                       \
                                                                \
/* reset firpfbch2 object internals                         */  \
void FIRPFBCH2(_reset)(FIRPFBCH2() _q);                         \
                                                                \
/* print firpfbch2 object internals                         */  \
void FIRPFBCH2(_print)(FIRPFBCH2() _q);                         \
                                                                \
/* execute filterbank channelizer                           */  \
/* LIQUID_ANALYZER:     input: M/2, output: M               */  \
/* LIQUID_SYNTHESIZER:  input: M,   output: M/2             */  \
/*  _x      :   channelizer input                           */  \
/*  _y      :   channelizer output                          */  \
void FIRPFBCH2(_execute)(FIRPFBCH2() _q,                        \
                         TI *        _x,                        \
                         TO *        _y);                       \


LIQUID_FIRPFBCH2_DEFINE_API(FIRPFBCH2_MANGLE_CRCF,
                            liquid_float_complex,
                            float,
                            liquid_float_complex)



#define OFDMFRAME_SCTYPE_NULL   0
#define OFDMFRAME_SCTYPE_PILOT  1
#define OFDMFRAME_SCTYPE_DATA   2

// initialize default subcarrier allocation
//  _M      :   number of subcarriers
//  _p      :   output subcarrier allocation array, [size: _M x 1]
void ofdmframe_init_default_sctype(unsigned int _M,
                                   unsigned char * _p);

// validate subcarrier type (count number of null, pilot, and data
// subcarriers in the allocation)
//  _p          :   subcarrier allocation array, [size: _M x 1]
//  _M          :   number of subcarriers
//  _M_null     :   output number of null subcarriers
//  _M_pilot    :   output number of pilot subcarriers
//  _M_data     :   output number of data subcarriers
void ofdmframe_validate_sctype(unsigned char * _p,
                               unsigned int _M,
                               unsigned int * _M_null,
                               unsigned int * _M_pilot,
                               unsigned int * _M_data);

// print subcarrier allocation to screen
//  _p      :   output subcarrier allocation array, [size: _M x 1]
//  _M      :   number of subcarriers
void ofdmframe_print_sctype(unsigned char * _p,
                            unsigned int    _M);


// 
// OFDM frame (symbol) generator
//
typedef struct ofdmframegen_s * ofdmframegen;

// create OFDM framing generator object
//  _M          :   number of subcarriers, >10 typical
//  _cp_len     :   cyclic prefix length
//  _taper_len  :   taper length (OFDM symbol overlap)
//  _p          :   subcarrier allocation (null, pilot, data), [size: _M x 1]
ofdmframegen ofdmframegen_create(unsigned int    _M,
                                 unsigned int    _cp_len,
                                 unsigned int    _taper_len,
                                 unsigned char * _p);

void ofdmframegen_destroy(ofdmframegen _q);

void ofdmframegen_print(ofdmframegen _q);

void ofdmframegen_reset(ofdmframegen _q);

// write first S0 symbol
void ofdmframegen_write_S0a(ofdmframegen _q,
                            liquid_float_complex *_y);

// write second S0 symbol
void ofdmframegen_write_S0b(ofdmframegen _q,
                            liquid_float_complex *_y);

// write S1 symbol
void ofdmframegen_write_S1(ofdmframegen _q,
                           liquid_float_complex *_y);

// write data symbol
void ofdmframegen_writesymbol(ofdmframegen _q,
                              liquid_float_complex * _x,
                              liquid_float_complex *_y);

// write tail
void ofdmframegen_writetail(ofdmframegen _q,
                            liquid_float_complex * _x);

// 
// OFDM frame (symbol) synchronizer
//
typedef int (*ofdmframesync_callback)(liquid_float_complex * _y,
                                      unsigned char * _p,
                                      unsigned int _M,
                                      void * _userdata);
typedef struct ofdmframesync_s * ofdmframesync;

// create OFDM framing synchronizer object
//  _M          :   number of subcarriers, >10 typical
//  _cp_len     :   cyclic prefix length
//  _taper_len  :   taper length (OFDM symbol overlap)
//  _p          :   subcarrier allocation (null, pilot, data), [size: _M x 1]
//  _callback   :   user-defined callback function
//  _userdata   :   user-defined data pointer
ofdmframesync ofdmframesync_create(unsigned int           _M,
                                   unsigned int           _cp_len,
                                   unsigned int           _taper_len,
                                   unsigned char *        _p,
                                   ofdmframesync_callback _callback,
                                   void *                 _userdata);
void ofdmframesync_destroy(ofdmframesync _q);
void ofdmframesync_print(ofdmframesync _q);
void ofdmframesync_reset(ofdmframesync _q);
void ofdmframesync_execute(ofdmframesync _q,
                           liquid_float_complex * _x,
                           unsigned int _n);

// query methods
float ofdmframesync_get_rssi(ofdmframesync _q); // received signal strength indication
float ofdmframesync_get_cfo(ofdmframesync _q);  // carrier offset estimate

// debugging
void ofdmframesync_debug_enable(ofdmframesync _q);
void ofdmframesync_debug_disable(ofdmframesync _q);
void ofdmframesync_debug_print(ofdmframesync _q, const char * _filename);


// 
// MODULE : nco (numerically-controlled oscillator)
//

// oscillator type
//  LIQUID_NCO  :   numerically-controlled oscillator (fast)
//  LIQUID_VCO  :   "voltage"-controlled oscillator (precise)
typedef enum {
    LIQUID_NCO=0,
    LIQUID_VCO
} liquid_ncotype;

#define NCO_MANGLE_FLOAT(name)  LIQUID_CONCAT(nco_crcf, name)

// large macro
//   NCO    : name-mangling macro
//   T      : primitive data type
//   TC     : input/output data type
#define LIQUID_NCO_DEFINE_API(NCO,T,TC)                         \
typedef struct NCO(_s) * NCO();                                 \
                                                                \
NCO() NCO(_create)(liquid_ncotype _type);                       \
void NCO(_destroy)(NCO() _q);                                   \
void NCO(_print)(NCO() _q);                                     \
                                                                \
/* set phase/frequency to zero, reset pll filter        */      \
void NCO(_reset)(NCO() _q);                                     \
                                                                \
/* get/set/adjust internal frequency/phase              */      \
T    NCO(_get_frequency)(   NCO() _q);                          \
void NCO(_set_frequency)(   NCO() _q, T _f);                    \
void NCO(_adjust_frequency)(NCO() _q, T _df);                   \
T    NCO(_get_phase)(       NCO() _q);                          \
void NCO(_set_phase)(       NCO() _q, T _phi);                  \
void NCO(_adjust_phase)(    NCO() _q, T _dphi);                 \
                                                                \
/* increment phase by internal phase step (frequency)   */      \
void NCO(_step)(NCO() _q);                                      \
                                                                \
/* compute trigonometric functions                      */      \
T NCO(_sin)(NCO() _q);                                          \
T NCO(_cos)(NCO() _q);                                          \
void NCO(_sincos)(NCO() _q, T* _s, T* _c);                      \
void NCO(_cexpf)(NCO() _q, TC * _y);                            \
                                                                \
/* pll : phase-locked loop                              */      \
void NCO(_pll_set_bandwidth)(NCO() _q, T _b);                   \
void NCO(_pll_step)(NCO() _q, T _dphi);                         \
                                                                \
/* Rotate input sample up by NCO angle (no stepping)    */      \
void NCO(_mix_up)(NCO() _q, TC _x, TC *_y);                     \
                                                                \
/* Rotate input sample down by NCO angle (no stepping)  */      \
void NCO(_mix_down)(NCO() _q, TC _x, TC *_y);                   \
                                                                \
/* Rotate input vector up by NCO angle (stepping)       */      \
/*  _q      :   nco object                              */      \
/*  _x      :   input vector [size: _N x 1]             */      \
/*  _y      :   output vector [size: _N x 1]            */      \
/*  _N      :   vector size                             */      \
void NCO(_mix_block_up)(NCO() _q,                               \
                        TC *_x,                                 \
                        TC *_y,                                 \
                        unsigned int _N);                       \
                                                                \
/* Rotate input vector down by NCO angle (stepping)     */      \
/*  _q      :   nco object                              */      \
/*  _x      :   input vector [size: _N x 1]             */      \
/*  _y      :   output vector [size: _N x 1]            */      \
/*  _N      :   vector size                             */      \
void NCO(_mix_block_down)(NCO() _q,                             \
                          TC *_x,                               \
                          TC *_y,                               \
                          unsigned int _N);                     \

// Define nco APIs
LIQUID_NCO_DEFINE_API(NCO_MANGLE_FLOAT, float, liquid_float_complex)


// nco utilities

// unwrap phase of array (basic)
void liquid_unwrap_phase(float * _theta, unsigned int _n);

// unwrap phase of array (advanced)
void liquid_unwrap_phase2(float * _theta, unsigned int _n);



//
// MODULE : optimization
//

// utility function pointer definition
typedef float (*utility_function)(void *       _userdata,
                                  float *      _v,
                                  unsigned int _n);

// n-dimensional Rosenbrock utility function (minimum at _v = {1,1,1...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_rosenbrock(void *       _userdata,
                        float *      _v,
                        unsigned int _n);

// n-dimensional inverse Gauss utility function (minimum at _v = {0,0,0...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_invgauss(void *       _userdata,
                      float *      _v,
                      unsigned int _n);

// n-dimensional multimodal utility function (minimum at _v = {0,0,0...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_multimodal(void *       _userdata,
                        float *      _v,
                        unsigned int _n);

// n-dimensional spiral utility function (minimum at _v = {0,0,0...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_spiral(void *       _userdata,
                    float *      _v,
                    unsigned int _n);


//
// Gradient search
//

#define LIQUID_OPTIM_MINIMIZE (0)
#define LIQUID_OPTIM_MAXIMIZE (1)

typedef struct gradsearch_s * gradsearch;

// Create a gradient search object
//   _userdata          :   user data object pointer
//   _v                 :   array of parameters to optimize
//   _num_parameters    :   array length (number of parameters to optimize)
//   _u                 :   utility function pointer
//   _direction         :   search direction (e.g. LIQUID_OPTIM_MAXIMIZE)
gradsearch gradsearch_create(void *           _userdata,
                             float *          _v,
                             unsigned int     _num_parameters,
                             utility_function _utility,
                             int              _direction);

// Destroy a gradsearch object
void gradsearch_destroy(gradsearch _q);

// Prints current status of search
void gradsearch_print(gradsearch _q);

// Iterate once
float gradsearch_step(gradsearch _q);

// Execute the search
float gradsearch_execute(gradsearch   _q,
                         unsigned int _max_iterations,
                         float        _target_utility);


// quasi-Newton search
typedef struct qnsearch_s * qnsearch;

// Create a simple qnsearch object; parameters are specified internally
//   _userdata          :   userdata
//   _v                 :   array of parameters to optimize
//   _num_parameters    :   array length
//   _get_utility       :   utility function pointer
//   _direction         :   search direction (e.g. LIQUID_OPTIM_MAXIMIZE)
qnsearch qnsearch_create(void *           _userdata,
                         float *          _v,
                         unsigned int     _num_parameters,
                         utility_function _u,
                         int              _direction);

// Destroy a qnsearch object
void qnsearch_destroy(qnsearch _g);

// Prints current status of search
void qnsearch_print(qnsearch _g);

// Resets internal state
void qnsearch_reset(qnsearch _g);

// Iterate once
void qnsearch_step(qnsearch _g);

// Execute the search
float qnsearch_execute(qnsearch _g,
                       unsigned int _max_iterations,
                       float _target_utility);

// 
// chromosome (for genetic algorithm search)
//
typedef struct chromosome_s * chromosome;

// create a chromosome object, variable bits/trait
chromosome chromosome_create(unsigned int * _bits_per_trait,
                             unsigned int _num_traits);

// create a chromosome object, all traits same resolution
chromosome chromosome_create_basic(unsigned int _num_traits,
                                   unsigned int _bits_per_trait);

// create a chromosome object, cloning a parent
chromosome chromosome_create_clone(chromosome _parent);

// copy existing chromosomes' internal traits (all other internal
// parameters must be equal)
void chromosome_copy(chromosome _parent, chromosome _child);

// Destroy a chromosome object
void chromosome_destroy(chromosome _c);

// get number of traits in chromosome
unsigned int chromosome_get_num_traits(chromosome _c);

// Print chromosome values to screen (binary representation)
void chromosome_print(chromosome _c);

// Print chromosome values to screen (floating-point representation)
void chromosome_printf(chromosome _c);

// clear chromosome (set traits to zero)
void chromosome_clear(chromosome _c);

// initialize chromosome on integer values
void chromosome_init(chromosome _c,
                     unsigned int * _v);

// initialize chromosome on floating-point values
void chromosome_initf(chromosome _c,
                      float * _v);

// Mutates chromosome _c at _index
void chromosome_mutate(chromosome _c, unsigned int _index);

// Resulting chromosome _c is a crossover of parents _p1 and _p2 at _threshold
void chromosome_crossover(chromosome _p1,
                          chromosome _p2,
                          chromosome _c,
                          unsigned int _threshold);

// Initializes chromosome to random value
void chromosome_init_random(chromosome _c);

// Returns integer representation of chromosome
unsigned int chromosome_value(chromosome _c,
                              unsigned int _index);

// Returns floating-point representation of chromosome
float chromosome_valuef(chromosome _c,
                        unsigned int _index);

// 
// genetic algorithm search
//
typedef struct gasearch_s * gasearch;

typedef float (*gasearch_utility)(void * _userdata, chromosome _c);

// Create a simple gasearch object; parameters are specified internally
//  _utility            :   chromosome fitness utility function
//  _userdata           :   user data, void pointer passed to _get_utility() callback
//  _parent             :   initial population parent chromosome, governs precision, etc.
//  _minmax             :   search direction
gasearch gasearch_create(gasearch_utility _u,
                         void * _userdata,
                         chromosome _parent,
                         int _minmax);

// Create a gasearch object, specifying search parameters
//  _utility            :   chromosome fitness utility function
//  _userdata           :   user data, void pointer passed to _get_utility() callback
//  _parent             :   initial population parent chromosome, governs precision, etc.
//  _minmax             :   search direction
//  _population_size    :   number of chromosomes in population
//  _mutation_rate      :   probability of mutating chromosomes
gasearch gasearch_create_advanced(gasearch_utility _utility,
                                  void * _userdata,
                                  chromosome _parent,
                                  int _minmax,
                                  unsigned int _population_size,
                                  float _mutation_rate);


// Destroy a gasearch object
void gasearch_destroy(gasearch _q);

// print search parameter internals
void gasearch_print(gasearch _q);

// set mutation rate
void gasearch_set_mutation_rate(gasearch _q,
                                float _mutation_rate);

// set population/selection size
//  _q                  :   ga search object
//  _population_size    :   new population size (number of chromosomes)
//  _selection_size     :   selection size (number of parents for new generation)
void gasearch_set_population_size(gasearch _q,
                                  unsigned int _population_size,
                                  unsigned int _selection_size);

// Execute the search
//  _q              :   ga search object
//  _max_iterations :   maximum number of iterations to run before bailing
//  _target_utility :   target utility
float gasearch_run(gasearch _q,
                    unsigned int _max_iterations,
                    float _target_utility);

// iterate over one evolution of the search algorithm
void gasearch_evolve(gasearch _q);

// get optimal chromosome
//  _q              :   ga search object
//  _c              :   output optimal chromosome
//  _utility_opt    :   fitness of _c
void gasearch_getopt(gasearch _q,
                     chromosome _c,
                     float * _utility_opt);

//
// MODULE : quantization
//

float compress_mulaw(float _x, float _mu);
float expand_mulaw(float _x, float _mu);

void compress_cf_mulaw(liquid_float_complex _x, float _mu, liquid_float_complex * _y);
void expand_cf_mulaw(liquid_float_complex _y, float _mu, liquid_float_complex * _x);

//float compress_alaw(float _x, float _a);
//float expand_alaw(float _x, float _a);

// inline quantizer: 'analog' signal in [-1, 1]
unsigned int quantize_adc(float _x, unsigned int _num_bits);
float quantize_dac(unsigned int _s, unsigned int _num_bits);

// structured quantizer

typedef enum {
    LIQUID_COMPANDER_NONE=0,
    LIQUID_COMPANDER_LINEAR,
    LIQUID_COMPANDER_MULAW,
    LIQUID_COMPANDER_ALAW
} liquid_compander_type;

#define QUANTIZER_MANGLE_FLOAT(name)    LIQUID_CONCAT(quantizerf,  name)
#define QUANTIZER_MANGLE_CFLOAT(name)   LIQUID_CONCAT(quantizercf, name)

// large macro
//   QUANTIZER  : name-mangling macro
//   T          : data type
#define LIQUID_QUANTIZER_DEFINE_API(QUANTIZER,T)                \
typedef struct QUANTIZER(_s) * QUANTIZER();                     \
QUANTIZER() QUANTIZER(_create)(liquid_compander_type _ctype,    \
                               float _range,                    \
                               unsigned int _num_bits);         \
void QUANTIZER(_destroy)(QUANTIZER() _q);                       \
void QUANTIZER(_print)(QUANTIZER() _q);                         \
void QUANTIZER(_execute_adc)(QUANTIZER() _q,                    \
                             T _x,                              \
                             unsigned int * _sample);           \
void QUANTIZER(_execute_dac)(QUANTIZER() _q,                    \
                             unsigned int _sample,              \
                             T * _x);

LIQUID_QUANTIZER_DEFINE_API(QUANTIZER_MANGLE_FLOAT,  float)
LIQUID_QUANTIZER_DEFINE_API(QUANTIZER_MANGLE_CFLOAT, liquid_float_complex)


//
// MODULE : random (number generators)
//


// Uniform random number generator, (0,1]
float randf();
float randf_pdf(float _x);
float randf_cdf(float _x);

// Gauss random number generator, N(0,1)
//   f(x) = 1/sqrt(2*pi*sigma^2) * exp{-(x-eta)^2/(2*sigma^2)}
//
//   where
//     eta   = mean
//     sigma = standard deviation
//
float randnf();
void awgn(float *_x, float _nstd);
void crandnf(liquid_float_complex *_y);
void cawgn(liquid_float_complex *_x, float _nstd);
float randnf_pdf(float _x, float _eta, float _sig);
float randnf_cdf(float _x, float _eta, float _sig);

// Exponential
//  f(x) = lambda exp{ -lambda x }
// where
//  lambda = spread parameter, lambda > 0
//  x >= 0
float randexpf(float _lambda);
float randexpf_pdf(float _x, float _lambda);
float randexpf_cdf(float _x, float _lambda);

// Weibull
//   f(x) = (a/b) (x/b)^(a-1) exp{ -(x/b)^a }
//   where
//     a = alpha : shape parameter
//     b = beta  : scaling parameter
//     g = gamma : location (threshold) parameter
//
float randweibf(float _alpha, float _beta, float _gamma);
float randweibf_pdf(float _x, float _a, float _b, float _g);
float randweibf_cdf(float _x, float _a, float _b, float _g);

// Gamma
//          x^(a-1) exp(-x/b)
//  f(x) = -------------------
//            Gamma(a) b^a
//  where
//      a = alpha : shape parameter, a > 0
//      b = beta  : scale parameter, b > 0
//      Gamma(z) = regular gamma function
//      x >= 0
float randgammaf(float _alpha, float _beta);
float randgammaf_pdf(float _x, float _alpha, float _beta);
float randgammaf_cdf(float _x, float _alpha, float _beta);

// Nakagami-m
//  f(x) = (2/Gamma(m)) (m/omega)^m x^(2m-1) exp{-(m/omega)x^2}
// where
//      m       : shape parameter, m >= 0.5
//      omega   : spread parameter, omega > 0
//      Gamma(z): regular complete gamma function
//      x >= 0
float randnakmf(float _m, float _omega);
float randnakmf_pdf(float _x, float _m, float _omega);
float randnakmf_cdf(float _x, float _m, float _omega);

// Rice-K
//  f(x) = (x/sigma^2) exp{ -(x^2+s^2)/(2sigma^2) } I0( x s / sigma^2 )
// where
//  s     = sqrt( omega*K/(K+1) )
//  sigma = sqrt(0.5 omega/(K+1))
// and
//  K     = shape parameter
//  omega = spread parameter
//  I0    = modified Bessel function of the first kind
//  x >= 0
float randricekf(float _K, float _omega);
float randricekf_cdf(float _x, float _K, float _omega);
float randricekf_pdf(float _x, float _K, float _omega);


// Data scrambler : whiten data sequence
void scramble_data(unsigned char * _x, unsigned int _len);
void unscramble_data(unsigned char * _x, unsigned int _len);
void unscramble_data_soft(unsigned char * _x, unsigned int _len);

//
// MODULE : sequence
//

// Binary sequence (generic)

typedef struct bsequence_s * bsequence;

// Create a binary sequence of a specific length (number of bits)
bsequence bsequence_create(unsigned int num_bits);

// Free memory in a binary sequence
void bsequence_destroy(bsequence _bs);

// Clear binary sequence (set to 0's)
void bsequence_clear(bsequence _bs);

// initialize sequence on external array
void bsequence_init(bsequence _bs,
                    unsigned char * _v);

// Print sequence to the screen
void bsequence_print(bsequence _bs);

// Push bit into to back of a binary sequence
void bsequence_push(bsequence _bs,
                    unsigned int _bit);

// circular shift (left)
void bsequence_circshift(bsequence _bs);

// Correlate two binary sequences together
int bsequence_correlate(bsequence _bs1, bsequence _bs2);

// compute the binary addition of two bit sequences
void bsequence_add(bsequence _bs1, bsequence _bs2, bsequence _bs3);

// compute the binary multiplication of two bit sequences
void bsequence_mul(bsequence _bs1, bsequence _bs2, bsequence _bs3);

// accumulate the 1's in a binary sequence
unsigned int bsequence_accumulate(bsequence _bs);

// accessor functions
unsigned int bsequence_get_length(bsequence _bs);
unsigned int bsequence_index(bsequence _bs, unsigned int _i);

// Complementary codes

// intialize two sequences to complementary codes.  sequences must
// be of length at least 8 and a power of 2 (e.g. 8, 16, 32, 64,...)
//  _a      :   sequence 'a' (bsequence object)
//  _b      :   sequence 'b' (bsequence object)
void bsequence_create_ccodes(bsequence _a,
                             bsequence _b);


// M-Sequence

#define LIQUID_MAX_MSEQUENCE_LENGTH   32767

// default m-sequence generators:       g (hex)     m       n   g (oct)       g (binary)
#define LIQUID_MSEQUENCE_GENPOLY_M2     0x0007  //  2       3        7               111
#define LIQUID_MSEQUENCE_GENPOLY_M3     0x000B  //  3       7       13              1011
#define LIQUID_MSEQUENCE_GENPOLY_M4     0x0013  //  4      15       23             10011
#define LIQUID_MSEQUENCE_GENPOLY_M5     0x0025  //  5      31       45            100101
#define LIQUID_MSEQUENCE_GENPOLY_M6     0x0043  //  6      63      103           1000011
#define LIQUID_MSEQUENCE_GENPOLY_M7     0x0089  //  7     127      211          10001001
#define LIQUID_MSEQUENCE_GENPOLY_M8     0x011D  //  8     255      435         100101101
#define LIQUID_MSEQUENCE_GENPOLY_M9     0x0211  //  9     511     1021        1000010001
#define LIQUID_MSEQUENCE_GENPOLY_M10    0x0409  // 10    1023     2011       10000001001
#define LIQUID_MSEQUENCE_GENPOLY_M11    0x0805  // 11    2047     4005      100000000101
#define LIQUID_MSEQUENCE_GENPOLY_M12    0x1053  // 12    4095    10123     1000001010011
#define LIQUID_MSEQUENCE_GENPOLY_M13    0x201b  // 13    8191    20033    10000000011011
#define LIQUID_MSEQUENCE_GENPOLY_M14    0x402b  // 14   16383    40053   100000000101011
#define LIQUID_MSEQUENCE_GENPOLY_M15    0x8003  // 15   32767   100003  1000000000000011
   
typedef struct msequence_s * msequence;

// create a maximal-length sequence (m-sequence) object with
// an internal shift register length of _m bits.
//  _m      :   generator polynomial length, sequence length is (2^m)-1
//  _g      :   generator polynomial, starting with most-significant bit
//  _a      :   initial shift register state, default: 000...001
msequence msequence_create(unsigned int _m,
                           unsigned int _g,
                           unsigned int _a);

// create a maximal-length sequence (m-sequence) object from a generator polynomial
msequence msequence_create_genpoly(unsigned int _g);

// creates a default maximal-length sequence
msequence msequence_create_default(unsigned int _m);

// destroy an msequence object, freeing all internal memory
void msequence_destroy(msequence _m);

// prints the sequence's internal state to the screen
void msequence_print(msequence _m);

// advance msequence on shift register, returning output bit
unsigned int msequence_advance(msequence _ms);

// generate pseudo-random symbol from shift register by
// advancing _bps bits and returning compacted symbol
//  _ms     :   m-sequence object
//  _bps    :   bits per symbol of output
unsigned int msequence_generate_symbol(msequence _ms,
                                       unsigned int _bps);

// reset msequence shift register to original state, typically '1'
void msequence_reset(msequence _ms);

// initialize a bsequence object on an msequence object
//  _bs     :   bsequence object
//  _ms     :   msequence object
void bsequence_init_msequence(bsequence _bs,
                              msequence _ms);

// get the length of the sequence
unsigned int msequence_get_length(msequence _ms);

// get the internal state of the sequence
unsigned int msequence_get_state(msequence _ms);

// set the internal state of the sequence
void msequence_set_state(msequence    _ms,
                         unsigned int _a);


// 
// MODULE : utility
//

// pack binary array with symbol(s)
//  _src        :   source array [size: _n x 1]
//  _n          :   input source array length
//  _k          :   bit index to write in _src
//  _b          :   number of bits in input symbol
//  _sym_in     :   input symbol
void liquid_pack_array(unsigned char * _src,
                       unsigned int _n,
                       unsigned int _k,
                       unsigned int _b,
                       unsigned char _sym_in);

// unpack symbols from binary array
//  _src        :   source array [size: _n x 1]
//  _n          :   input source array length
//  _k          :   bit index to write in _src
//  _b          :   number of bits in output symbol
//  _sym_out    :   output symbol
void liquid_unpack_array(unsigned char * _src,
                         unsigned int _n,
                         unsigned int _k,
                         unsigned int _b,
                         unsigned char * _sym_out);

// pack one-bit symbols into bytes (8-bit symbols)
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_pack_bytes(unsigned char * _sym_in,
                       unsigned int _sym_in_len,
                       unsigned char * _sym_out,
                       unsigned int _sym_out_len,
                       unsigned int * _num_written);

// unpack 8-bit symbols (full bytes) into one-bit symbols
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols array
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_unpack_bytes(unsigned char * _sym_in,
                         unsigned int _sym_in_len,
                         unsigned char * _sym_out,
                         unsigned int _sym_out_len,
                         unsigned int * _num_written);

// repack bytes with arbitrary symbol sizes
//  _sym_in             :   input symbols array [size: _sym_in_len x 1]
//  _sym_in_bps         :   number of bits per input symbol
//  _sym_in_len         :   number of input symbols
//  _sym_out            :   output symbols array
//  _sym_out_bps        :   number of bits per output symbol
//  _sym_out_len        :   number of bytes allocated to output symbols array
//  _num_written        :   number of output symbols actually written
void liquid_repack_bytes(unsigned char * _sym_in,
                         unsigned int _sym_in_bps,
                         unsigned int _sym_in_len,
                         unsigned char * _sym_out,
                         unsigned int _sym_out_bps,
                         unsigned int _sym_out_len,
                         unsigned int * _num_written);
 
// shift array to the left _b bits, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_lbshift(unsigned char * _src,
                    unsigned int _n,
                    unsigned int _b);
 
// shift array to the right _b bits, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_rbshift(unsigned char * _src,
                    unsigned int _n,
                    unsigned int _b);
 
// circularly shift array to the left _b bits
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_lbcircshift(unsigned char * _src,
                        unsigned int _n,
                        unsigned int _b);
 
// circularly shift array to the right _b bits
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_rbcircshift(unsigned char * _src,
                        unsigned int _n,
                        unsigned int _b);
 



// shift array to the left _b bytes, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_lshift(unsigned char * _src,
                   unsigned int _n,
                   unsigned int _b);
 
// shift array to the right _b bytes, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_rshift(unsigned char * _src,
                   unsigned int _n,
                   unsigned int _b);
 
// circular shift array to the left _b bytes
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_lcircshift(unsigned char * _src,
                       unsigned int _n,
                       unsigned int _b);
 
// circular shift array to the right _b bytes
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_rcircshift(unsigned char * _src,
                       unsigned int _n,
                       unsigned int _b);
 
// Count the number of ones in an integer
unsigned int liquid_count_ones(unsigned int _x); 

// count number of ones in an integer, modulo 2
unsigned int liquid_count_ones_mod2(unsigned int _x);

// compute bindary dot-product between two integers
unsigned int liquid_bdotprod(unsigned int _x,
                             unsigned int _y);

// Count leading zeros in an integer
unsigned int liquid_count_leading_zeros(unsigned int _x); 

// Most-significant bit index
unsigned int liquid_msb_index(unsigned int _x);

// Print string of bits to stdout
void liquid_print_bitstring(unsigned int _x,
                            unsigned int _n);

// reverse byte, word, etc.
unsigned char liquid_reverse_byte(  unsigned char _x);
unsigned int  liquid_reverse_uint16(unsigned int  _x);
unsigned int  liquid_reverse_uint24(unsigned int  _x);
unsigned int  liquid_reverse_uint32(unsigned int  _x);

// 
// MODULE : vector
//

#define VECTOR_MANGLE_RF(name)  LIQUID_CONCAT(liquid_vectorf, name)
#define VECTOR_MANGLE_CF(name)  LIQUID_CONCAT(liquid_vectorcf,name)

// large macro
//   VECTOR     : name-mangling macro
//   T          : data type
//   TP         : data type (primitive)
#define LIQUID_VECTOR_DEFINE_API(VECTOR,T,TP)                   \
                                                                \
/* initialize vector with scalar: x[i] = c (scalar)         */  \
void VECTOR(_init)(T            _c,                             \
                   T *          _x,                             \
                   unsigned int _n);                            \
                                                                \
/* add each element: z[i] = x[i] + y[i]                     */  \
void VECTOR(_add)(T *          _x,                              \
                  T *          _y,                              \
                  unsigned int _n,                              \
                  T *          _z);                             \
/* add scalar to each element: y[i] = x[i] + c              */  \
void VECTOR(_addscalar)(T *          _x,                        \
                        unsigned int _n,                        \
                        T            _c,                        \
                        T *          _y);                       \
                                                                \
/* multiply each element: z[i] = x[i] * y[i]                */  \
void VECTOR(_mul)(T *          _x,                              \
                  T *          _y,                              \
                  unsigned int _n,                              \
                  T *          _z);                             \
/* multiply each element with scalar: y[i] = x[i] * c       */  \
void VECTOR(_mulscalar)(T *          _x,                        \
                        unsigned int _n,                        \
                        T            _c,                        \
                        T *          _y);                       \
                                                                \
/* compute complex phase rotation: x[i] = exp{j theta[i]}   */  \
void VECTOR(_cexpj)(TP *         _theta,                        \
                    unsigned int _n,                            \
                    T *          _x);                           \
/* compute angle of each element: theta[i] = arg{ x[i] }    */  \
void VECTOR(_carg)(T *          _x,                             \
                   unsigned int _n,                             \
                   TP *         _theta);                        \
/* compute absolute value of each element: y[i] = |x[i]|    */  \
void VECTOR(_abs)(T *          _x,                              \
                  unsigned int _n,                              \
                  TP *         _y);                             \
                                                                \
/* compute sum of squares: sum{ |x|^2 }                     */  \
TP VECTOR(_sumsq)(T *          _x,                              \
                  unsigned int _n);                             \
                                                                \
/* compute l-2 norm: sqrt{ sum{ |x|^2 } }                   */  \
TP VECTOR(_norm)(T *          _x,                               \
                 unsigned int _n);                              \
                                                                \
/* compute l-p norm: { sum{ |x|^p } }^(1/p)                 */  \
TP VECTOR(_pnorm)(T *          _x,                              \
                  unsigned int _n,                              \
                  TP           _p);                             \
                                                                \
/* scale vector elements by l-2 norm: y[i] = x[i]/norm(x)   */  \
void VECTOR(_normalize)(T *          _x,                        \
                        unsigned int _n,                        \
                        T *          _y);                       \

LIQUID_VECTOR_DEFINE_API(VECTOR_MANGLE_RF, float,                float);
LIQUID_VECTOR_DEFINE_API(VECTOR_MANGLE_CF, liquid_float_complex, float);

// 
// mixed types
//
#if 0
void liquid_vectorf_add(float *      _a,
                        float *      _b,
                        unsigned int _n,
                        float *      _c);
#endif

#ifdef __cplusplus
} //extern "C"
#endif // __cplusplus

#ifdef _MSC_VER
#pragma warning( pop )
#endif


#endif // __LIQUID_H__

