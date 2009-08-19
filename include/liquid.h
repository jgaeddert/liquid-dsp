/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifdef __cplusplus
extern "C" {
#   define LIQUID_USE_COMPLEX_H 0
#else
#   define LIQUID_USE_COMPLEX_H 1
#endif /* __cplusplus */

#define LIQUID_CONCAT(prefix, name) prefix ## name
#define LIQUID_VALIDATE_INPUT

/* 
 * Default: use the C99 complex data type, otherwise
 * define complex type compatible with the C++ complex standard,
 * otherwise resort to defining binary compatible array.
 */
#if LIQUID_USE_COMPLEX_H==1
#   include <complex.h>
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef R _Complex C
#elif defined _GLIBCXX_COMPLEX
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef std::complex<R> C
#else
#   define LIQUID_DEFINE_COMPLEX(R,C) typedef R C[2]
#endif
//#   define LIQUID_DEFINE_COMPLEX(R,C) typedef C struct {R real; R imag;};

LIQUID_DEFINE_COMPLEX(float, liquid_float_complex);

// 
// MODULE : agc (automatic gain control)
//
typedef struct agc_s * agc;

agc agc_create(float _etarget, float _BT);
void agc_destroy(agc _agc);
void agc_print(agc _agc);

// Set target energy
void agc_set_target(agc _agc, float _e_target);

// set gain limits
void agc_set_gain_limits(agc _agc, float _gmin, float _gmax);

// Set loop filter bandwidth; attack/release time
void agc_set_bandwidth(agc _agc, float _BT);

// Apply gain to input, update tracking loop
void agc_execute(agc _agc, liquid_float_complex _x, liquid_float_complex *_y);

// Return signal level in dB relative to target
float agc_get_signal_level(agc _agc);

// Return gain in dB relative to target energy
float agc_get_gain(agc _agc);


//
// MODULE : ann (artificial neural network)
//

#define ANN_MANGLE_FLOAT(name)  LIQUID_CONCAT(ann, name)

// large macro
//   ANN    : name-mangling macro
//   T      : primitive data type
#define LIQUID_ANN_DEFINE_API(ANN,T)                            \
                                                                \
typedef struct ANN(_s) * ANN();                                 \
ANN() ANN(_create)(unsigned int * _structure,                   \
                   unsigned int _num_layers);                   \
void  ANN(_destroy)(ANN() _q);                                  \
void  ANN(_print)(ANN() _q);                                    \
void  ANN(_evaluate)(ANN() _q,                                  \
                     T * _x,                                    \
                     T * _y);                                   \
void  ANN(_train)(ANN() _q,                                     \
                  T * _x,                                       \
                  T * _y,                                       \
                  unsigned int _n,                              \
                  T _emax,                                      \
                  unsigned int _nmax);
// void ANN(_prune)(ANN() _q, ...);

// Define ann APIs
LIQUID_ANN_DEFINE_API(ANN_MANGLE_FLOAT, float)

// K-means (2-dimensional)
void kmeans(liquid_float_complex * _x,
            unsigned int _n,
            liquid_float_complex * _s,
            unsigned int _k);

//
// MODULE : audio
//

// CVSD: continuously variable slope delta
typedef struct cvsd_s * cvsd;
cvsd cvsd_create(unsigned int _num_bits, float _zeta);
void cvsd_destroy(cvsd _q);
void cvsd_print(cvsd _q);

// encode/decode single sample
unsigned char   cvsd_encode(cvsd _q, float _audio_sample);
float           cvsd_decode(cvsd _q, unsigned char _bit);

// encode/decode 8 samples
void cvsd_encode8(cvsd _q, float * _audio, unsigned char * _data);
void cvsd_decode8(cvsd _q, unsigned char _data, float * _audio);

// FBASC: filterbank audio synthesizer codec
#define FBASC_ENCODER   0
#define FBASC_DECODER   1
typedef struct fbasc_s * fbasc;
fbasc fbasc_create(int _type,
                   unsigned int _num_channels,
                   unsigned int _samples_per_frame,
                   unsigned int _bytes_per_frame);
void fbasc_destroy(fbasc _q);
void fbasc_print(fbasc _q);

void fbasc_encode(fbasc _q,
                  float * _audio,
                  unsigned char * _frame);
void fbasc_decode(fbasc _q,
                  unsigned char * _frame,
                  float * _audio);

//
// MODULE : buffer
//

// Buffer
#define BUFFER_MANGLE_FLOAT(name)  LIQUID_CONCAT(fbuffer, name)
#define BUFFER_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfbuffer, name)
#define BUFFER_MANGLE_UINT(name)   LIQUID_CONCAT(uibuffer, name)

typedef enum {
    CIRCULAR=0,
    STATIC
} buffer_type;

// large macro
//   BUFFER : name-mangling macro
//   T      : data type
#define LIQUID_BUFFER_DEFINE_API(BUFFER,T)                      \
                                                                \
typedef struct BUFFER(_s) * BUFFER();                           \
BUFFER() BUFFER(_create)(buffer_type _type, unsigned int _n);   \
void BUFFER(_destroy)(BUFFER() _b);                             \
void BUFFER(_print)(BUFFER() _b);                               \
void BUFFER(_debug_print)(BUFFER() _b);                         \
void BUFFER(_clear)(BUFFER() _b);                               \
void BUFFER(_zero)(BUFFER() _b);                                \
void BUFFER(_read)(BUFFER() _b, T ** _v, unsigned int *_nr);    \
void BUFFER(_release)(BUFFER() _b, unsigned int _n);            \
void BUFFER(_write)(BUFFER() _b, T * _v, unsigned int _n);      \
void BUFFER(_push)(BUFFER() _b, T _v);
//void BUFFER(_force_write)(BUFFER() _b, T * _v, unsigned int _n);

// Define buffer APIs
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_CFLOAT, liquid_float_complex)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)


// Windowing functions
#define WINDOW_MANGLE_FLOAT(name)  LIQUID_CONCAT(fwindow, name)
#define WINDOW_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfwindow, name)
#define WINDOW_MANGLE_UINT(name)   LIQUID_CONCAT(uiwindow, name)

// large macro
//   WINDOW  :   name-mangling macro
//   T  :   data type
#define LIQUID_WINDOW_DEFINE_API(WINDOW,T)                      \
                                                                \
typedef struct WINDOW(_s) * WINDOW();                           \
WINDOW() WINDOW(_create)(unsigned int _n);                      \
WINDOW() WINDOW(_recreate)(WINDOW() _w, unsigned int _n);       \
void WINDOW(_destroy)(WINDOW() _w);                             \
void WINDOW(_print)(WINDOW() _w);                               \
void WINDOW(_debug_print)(WINDOW() _w);                         \
void WINDOW(_clear)(WINDOW() _w);                               \
void WINDOW(_read)(WINDOW() _w, T ** _v);                       \
void WINDOW(_push)(WINDOW() _b, T _v);                          \
void WINDOW(_write)(WINDOW() _b, T * _v, unsigned int _n);

// Define window APIs
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_CFLOAT, liquid_float_complex)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_UINT, unsigned int)


// Port
#define PORT_MANGLE_FLOAT(name)     LIQUID_CONCAT(fport,name)
#define PORT_MANGLE_CFLOAT(name)    LIQUID_CONCAT(cfport,name)
#define PORT_MANGLE_UINT(name)      LIQUID_CONCAT(uiport,name)

// Macro
//  PORT    : name-mangling macro
//  T       : data type
#define LIQUID_PORT_DEFINE_API(PORT,T)                          \
typedef struct PORT(_s) * PORT();                               \
PORT() PORT(_create)(unsigned int _n);                          \
void PORT(_destroy)(PORT() _p);                                 \
void PORT(_print)(PORT() _p);                                   \
void PORT(_produce)(PORT() _p, T * _w, unsigned int _n);        \
void PORT(_consume)(PORT() _p, T * _r, unsigned int _n);

LIQUID_PORT_DEFINE_API(PORT_MANGLE_FLOAT, float)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_CFLOAT, liquid_float_complex)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_UINT, unsigned int)

//
// Generic port
//
typedef struct gport_s * gport;
gport gport_create(unsigned int _n, unsigned int _size);
void gport_destroy(gport _p);
void gport_print(gport _p);

// producer methods
void * gport_producer_lock(gport _p, unsigned int _n);
void gport_producer_unlock(gport _p, unsigned int _n);

// consumer methods
void * gport_consumer_lock(gport _p, unsigned int _n);
void gport_consumer_unlock(gport _p, unsigned int _n);

//
// Generic port (2)
//
enum {
    GPORT2_SIGNAL_NULL=0,           // no signal
    GPORT2_SIGNAL_METADATA_UPDATE,  // metadata 
    GPORT2_SIGNAL_EOM               // end of message
};
typedef struct gport2_s * gport2;
gport2 gport2_create(unsigned int _n, unsigned int _size);
void gport2_destroy(gport2 _p);
void gport2_print(gport2 _p);

// produce/consume methods
void gport2_produce(gport2 _p,
                    void * _w,
                    unsigned int _n);
void gport2_consume(gport2 _p,
                    void * _r,
                    unsigned int _n);
void gport2_consume_available(gport2 _p,
                              void * _r,
                              unsigned int _nmax,
                              unsigned int *_nc);

// signaling methods
void gport2_signal(gport2 _p, int _message);


//
// MODULE : channel
//

// AWGN channel
typedef struct awgn_channel_s * awgn_channel;
awgn_channel awgn_channel_create(float _nvar);
void awgn_channel_destroy(awgn_channel _q);
void awgn_channel_print(awgn_channel _q);
void awgn_channel_execute(awgn_channel _q,
                          liquid_float_complex _x,
                          liquid_float_complex *_y);
void awgn_channel_set_noise_variance(awgn_channel _q, float _nvar);

// Rice-K channel
typedef struct ricek_channel_s * ricek_channel;
ricek_channel ricek_channel_create(unsigned int _h_len,
                                   float _K,
                                   float _fd,
                                   float _theta);
void ricek_channel_destroy(ricek_channel _q);
void ricek_channel_print(ricek_channel _q);
void ricek_channel_execute(ricek_channel _q,
                           liquid_float_complex _x,
                           liquid_float_complex *_y);

// Log-normal channel
typedef struct lognorm_channel_s * lognorm_channel;

// Composite channel
typedef struct channel_s * channel;
channel channel_create();
void channel_destroy(channel _c);
void channel_print(channel _c);
void channel_execute(channel _c,
                     liquid_float_complex _x,
                     liquid_float_complex *_y);

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
void DOTPROD(_run)(TC *_h, TI *_x, unsigned int _n, TO *_y);    \
void DOTPROD(_run4)(TC *_h, TI *_x, unsigned int _n, TO *_y);   \
                                                                \
typedef struct DOTPROD(_s) * DOTPROD();                         \
struct DOTPROD(_s) {                                            \
    TC * h;                                                     \
    unsigned int n;                                             \
};                                                              \
DOTPROD() DOTPROD(_create)(TC * _v, unsigned int _n);           \
void DOTPROD(_destroy)(DOTPROD() _q);                           \
void DOTPROD(_execute)(DOTPROD() _q, TI * _v, TO * _y);

// Define APIs
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_RRRF, float, float, float)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)
LIQUID_DOTPROD_DEFINE_API(DOTPROD_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)

//
// MODULE : equalization
//

// least mean-squares (LMS)
#define EQLMS_MANGLE_RRRF(name)     LIQUID_CONCAT(eqlms_rrrf,name)
#define EQLMS_MANGLE_CCCF(name)     LIQUID_CONCAT(eqlms_cccf,name)

#define LIQUID_EQLMS_DEFINE_API(EQLMS,T)                        \
typedef struct EQLMS(_s) * EQLMS();                             \
EQLMS() EQLMS(_create)(unsigned int _p);                        \
void EQLMS(_destroy)(EQLMS() _eq);                              \
void EQLMS(_print)(EQLMS() _eq);                                \
void EQLMS(_set_bw)(EQLMS() _eq, float _lambda);                \
void EQLMS(_reset)(EQLMS() _eq);                                \
void EQLMS(_execute)(EQLMS() _eq,                               \
                     T _x,                                      \
                     T _d,                                      \
                     T * _d_hat);                               \
void EQLMS(_get_weights)(EQLMS() _eq, T * _w);                  \
void EQLMS(_train)(EQLMS() _eq,                                 \
                   T * _w,                                      \
                   T * _x,                                      \
                   T * _d,                                      \
                   unsigned int _n);

LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_RRRF, float);
LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_CCCF, liquid_float_complex);


// recursive least-squares (RLS)
#define EQRLS_MANGLE_RRRF(name)     LIQUID_CONCAT(eqrls_rrrf,name)
#define EQRLS_MANGLE_CCCF(name)     LIQUID_CONCAT(eqrls_cccf,name)

#define LIQUID_EQRLS_DEFINE_API(EQRLS,T)                        \
typedef struct EQRLS(_s) * EQRLS();                             \
EQRLS() EQRLS(_create)(unsigned int _p);                        \
void EQRLS(_destroy)(EQRLS() _eq);                              \
void EQRLS(_print)(EQRLS() _eq);                                \
void EQRLS(_set_bw)(EQRLS() _eq, float _mu);                    \
void EQRLS(_reset)(EQRLS() _eq);                                \
void EQRLS(_execute)(EQRLS() _eq,                               \
                     T _x,                                      \
                     T _d,                                      \
                     T * _d_hat);                               \
void EQRLS(_get_weights)(EQRLS() _eq, T * _w);                  \
void EQRLS(_train)(EQRLS() _eq,                                 \
                   T * _w,                                      \
                   T * _x,                                      \
                   T * _d,                                      \
                   unsigned int _n);

LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_RRRF, float);
LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_CCCF, liquid_float_complex);


// 
// MODULE : estimation
//

float estimate_freqoffset(liquid_float_complex * _x, unsigned int _n);
//float estimate_phaseoffset(liquid_float_complex * _x, unsigned int _n);
//float estimate_timingoffset(liquid_float_complex * _x, unsigned int _n);


//
// MODULE : fec (forward error-correction)
//



#define LIQUID_FEC_MANGLE_CRC32(name)       LIQUID_CONCAT(fec_crc32,name)
#define LIQUID_FEC_MANGLE_CHECKSUM32(name)  LIQUID_CONCAT(fec_checksum32,name)

// Macro: crc/checksum
//  CRC : name-mangling macro
#define LIQUID_CRC_DEFINE_API(CRC)                              \
typedef struct CRC(_s) * CRC();                                 \
CRC() CRC(_create)();                                           \
void CRC(_destroy)(CRC() _crc);                                 \
void CRC(_print)(CRC() _crc);                                   \
void CRC(_generate_key)(CRC() _crc,                             \
                        unsigned char * _msg,                   \
                        unsigned int _msg_len);                 \
int  CRC(_validate_key)(CRC() _crc,                             \
                        unsigned char * _msg,                   \
                        unsigned int _msg_len);

LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CRC32)
LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CHECKSUM32)

//
// checksum
//
unsigned char checksum_generate_key(unsigned char *_data,
                                    unsigned int _n);
int checksum_validate_message(unsigned char *_data,
                              unsigned int _n,
                              unsigned char _key);

//
// crc (cyclic redundancy check)
//
unsigned int crc32_generate_key(unsigned char *_data,
                                unsigned int _n);
int crc32_validate_message(unsigned char *_data,
                           unsigned int _n,
                           unsigned int _key);

// 
// FEC
//

typedef enum {
    FEC_UNKNOWN=0,
    FEC_NONE,           // no error-correction
    FEC_REP3,           // simple repeat code, r1/3
    FEC_HAMMING74,      // Hamming (7,4) block code, r1/2 (really 4/7)
    FEC_HAMMING84,      // Hamming (7,4) with extra parity bit, r1/2

    // codecs not defined internally (see http://www.ka9q.net/code/fec/)
    FEC_CONV_V27,       // r1/2, K=7, dfree=10
    FEC_CONV_V29,       // r1/2, K=9, dfree=12
    FEC_CONV_V39,       // r1/3, K=9, dfree=18
    FEC_CONV_V615,      // r1/6, K=15, dfree<=57 (Heller 1968)

    // punctured (perforated) codes
    FEC_CONV_V27P23,    // r2/3, K=7, dfree=6
    FEC_CONV_V27P34,    // r3/4, K=7, dfree=5
    FEC_CONV_V27P45,    // r4/5, K=7, dfree=4
    FEC_CONV_V27P56,    // r5/6, K=7, dfree=4
    FEC_CONV_V27P67,    // r6/7, K=7, dfree=3
    FEC_CONV_V27P78,    // r7/8, K=7, dfree=3

    FEC_CONV_V29P23,    // r2/3, K=9, dfree=7
    FEC_CONV_V29P34,    // r3/4, K=9, dfree=6
    FEC_CONV_V29P45,    // r4/5, K=9, dfree=5
    FEC_CONV_V29P56,    // r5/6, K=9, dfree=5
    FEC_CONV_V29P67,    // r6/7, K=9, dfree=4
    FEC_CONV_V29P78     // r7/8, K=9, dfree=4
} fec_scheme;

struct fec_conv_opts {int puncture;};

extern const char * fec_scheme_str[21];

typedef struct fec_s * fec;

// object-independent methods
unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len);
float fec_get_rate(fec_scheme _scheme);

fec fec_create(fec_scheme _scheme, void *_opts);
void fec_destroy(fec _q);
void fec_print(fec _q);

void fec_encode(fec _q,
                unsigned int _dec_msg_len,
                unsigned char * _msg_dec,
                unsigned char * _msg_enc);
void fec_decode(fec _q,
                unsigned int _dec_msg_len,
                unsigned char * _msg_enc,
                unsigned char * _msg_dec);


//
// MODULE : fft (fast Fourier transform)
//

typedef struct fftplan_s * fftplan;

#define FFT_FORWARD 0
#define FFT_REVERSE 1
fftplan fft_create_plan(unsigned int _n,
                        liquid_float_complex * _x,
                        liquid_float_complex * _y,
                        int _dir);
void fft_destroy_plan(fftplan _p);
void fft_execute(fftplan _p);

void fft_shift(liquid_float_complex *_x, unsigned int _n);

//
// ascii spectrogram
//
typedef struct asgram_s * asgram;
asgram asgram_create(liquid_float_complex *_x, unsigned int _n);
void asgram_set_scale(asgram _q, float _scale);
void asgram_set_offset(asgram _q, float _offset);
void asgram_destroy(asgram _q);
void asgram_execute(asgram _q);

//
// MODULE : filter
//

// generic prototypes
#define FIR_TEMPLATE    0   // Remez, e.g.
#define FIR_LOWPASS     1
#define FIR_HIGHPASS    2
#define FIR_BANDPASS    3
#define FIR_BANDREJECT  4

// specific prototypes
#define FIR_RCOS        5
#define FIR_RRCOS       6
#define FIR_NYQUIST     7
#define FIR_ROOTNYQUIST 8
struct fir_prototype_s {
    int type;               // bandpass, etc.
    int prototype;          // kaiser, rrcos, etc.
    float fc;               // cutoff frequency

    float bt;               // transition bandwidth
    float slsl;             // sidelobe suppression level
    unsigned int num_taps;  // filter length

    unsigned int k;         // samples/symbol
    unsigned int m;         // symbol delay
    float beta;             // excess bandwidth factor
    float dt;               // phase offset
};

typedef struct fir_prototype_s * fir_prototype;

void fir_prototype_design_length();
void fir_prototype_design_bt();
void fir_prototype_design_slsl();

void fir_prototype_design_rootnyquist(); // rrcos: k, m, ...

// returns the Kaiser window beta factor : sidelobe suppression level
float kaiser_beta_slsl(float _slsl);

// Design FIR filter using window method
//  _n      : filter length (odd)
//  _fc     : filter cutoff (0 < _fc < 1)
//  _slsl   : sidelobe suppression level (_slsl < 0)
//  _h      : output coefficient buffer
void fir_design_windowed_sinc(unsigned int _n, float _fc, float _slsl, float *_h);

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n, float _fc, float _slsl, float _mu, float *_h);

// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _theta  : LoS component angle of arrival
//  _h      : output coefficient buffer
void fir_design_doppler(unsigned int _n, float _fd, float _K, float _theta, float *_h);

// Design optimum FIR root-nyquist filter
//  _n      : filter length
//  _k      : samples/symbol (_k > 1)
//  _slsl   : sidelobe suppression level
//void fir_design_optim_root_nyquist(unsigned int _n, unsigned int _k, float _slsl, float *_h);

// Design Nyquist raised-cosine filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void design_rcos_filter(
    unsigned int _k,
    unsigned int _m,
    float _beta,
    float _dt,
    float * _h
);

// Design root-Nyquist raised-cosine filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void design_rrc_filter(
    unsigned int _k,
    unsigned int _m,
    float _beta,
    float _dt,
    float * _h
);

// Design root-Nyquist Kaiser filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void design_rkaiser_filter(
    unsigned int _k,
    unsigned int _m,
    float _beta,
    float _dt,
    float * _h
);

// Compute group delay for an FIR filter
//  _h      : filter coefficients array
//  _n      : filter length
//  _fc     : frequency at which delay is evaluated (-0.5 < _fc < 0.5)
float fir_group_delay(float * _h, unsigned int _n, float _fc);

//
// IIR filter design
//

void butterf(unsigned int _n, float * _a);

// Chebyshev type-I filter design
//  _n  :   filter order
//  _ep :   epsilon, passband ripple
//  _b  :   numerator coefficient array (length 1)
//  _a  :   denominator coefficient array (length _n+1)
void cheby1f(unsigned int _n, float _ep, float * _b, float * _a);


//
// Finite impulse response filter
//

#define FIR_FILTER_MANGLE_RRRF(name)  LIQUID_CONCAT(fir_filter_rrrf,name)
#define FIR_FILTER_MANGLE_CRCF(name)  LIQUID_CONCAT(fir_filter_crcf,name)
#define FIR_FILTER_MANGLE_CCCF(name)  LIQUID_CONCAT(fir_filter_cccf,name)

// Macro:
//   FIR_FILTER : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER,TO,TC,TI)       \
typedef struct FIR_FILTER(_s) * FIR_FILTER();                   \
FIR_FILTER() FIR_FILTER(_create)(TC * _h, unsigned int _n);     \
FIR_FILTER() FIR_FILTER(_recreate)(FIR_FILTER() _f,             \
                                   TC * _h,                     \
                                   unsigned int _n);            \
void FIR_FILTER(_destroy)(FIR_FILTER() _f);                     \
void FIR_FILTER(_clear)(FIR_FILTER() _f);                       \
void FIR_FILTER(_print)(FIR_FILTER() _f);                       \
void FIR_FILTER(_push)(FIR_FILTER() _f, TI _x);                 \
void FIR_FILTER(_execute)(FIR_FILTER() _f, TO *_y);             \
unsigned int FIR_FILTER(_get_length)(FIR_FILTER() _f);

LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER_MANGLE_RRRF, float, float, float)
LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)
LIQUID_FIR_FILTER_DEFINE_API(FIR_FILTER_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)

//
// FIR Hilbert transform
//  2:1 real-to-complex decimator
//  1:2 complex-to-real interpolator
//

#define FIRHILB_MANGLE_FLOAT(name)  LIQUID_CONCAT(firhilb, name)
//#define FIRHILB_MANGLE_DOUBLE(name) LIQUID_CONCAT(dfirhilb, name)

// NOTES:
//   Although firhilb is a placeholder for both the decimation and
//   interpolation, separate objects should be used for each task.
#define LIQUID_FIRHILB_DEFINE_API(FIRHILB,T,TC)                 \
typedef struct FIRHILB(_s) * FIRHILB();                         \
FIRHILB() FIRHILB(_create)(unsigned int _h_len);                \
void FIRHILB(_destroy)(FIRHILB() _f);                           \
void FIRHILB(_print)(FIRHILB() _f);                             \
void FIRHILB(_clear)(FIRHILB() _f);                             \
void FIRHILB(_decim_execute)(FIRHILB() _f, T * _x, TC * _y);    \
void FIRHILB(_interp_execute)(FIRHILB() _f, TC _x, T * _y);

LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_FLOAT, float, liquid_float_complex)
//LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_DOUBLE, double)

//
// Infinite impulse response filter
//

#define IIR_FILTER_MANGLE_RRRF(name)  LIQUID_CONCAT(iir_filter_rrrf,name)
#define IIR_FILTER_MANGLE_CRCF(name)  LIQUID_CONCAT(iir_filter_crcf,name)
#define IIR_FILTER_MANGLE_CCCF(name)  LIQUID_CONCAT(iir_filter_cccf,name)

// Macro:
//   IIR_FILTER : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER,TO,TC,TI)       \
typedef struct IIR_FILTER(_s) * IIR_FILTER();                   \
IIR_FILTER() IIR_FILTER(_create)(TC * _b,                       \
               unsigned int _nb,                                \
               TC * _a,                                         \
               unsigned int _na);                               \
void IIR_FILTER(_destroy)(IIR_FILTER() _f);                     \
void IIR_FILTER(_print)(IIR_FILTER() _f);                       \
void IIR_FILTER(_clear)(IIR_FILTER() _f);                       \
void IIR_FILTER(_execute)(IIR_FILTER() _f, TI _x, TO *_y);      \
unsigned int IIR_FILTER(_get_length)(IIR_FILTER() _f);

LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER_MANGLE_RRRF, float, float, float)
LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)
LIQUID_IIR_FILTER_DEFINE_API(IIR_FILTER_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)


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
typedef struct FIRPFB(_s) * FIRPFB();                           \
FIRPFB() FIRPFB(_create)(unsigned int _num_filters,             \
               TC * _h,                                         \
               unsigned int _h_len);                            \
void FIRPFB(_destroy)(FIRPFB() _b);                             \
void FIRPFB(_print)(FIRPFB() _b);                               \
void FIRPFB(_push)(FIRPFB() _b, TI _x);                         \
void FIRPFB(_execute)(FIRPFB() _b, unsigned int _i, TO *_y);    \
void FIRPFB(_clear)(FIRPFB() _b);

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_RRRF, float,         float,          float)
LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Interpolator
//
#define INTERP_MANGLE_RRRF(name)  LIQUID_CONCAT(interp_rrrf,name)
#define INTERP_MANGLE_CRCF(name)  LIQUID_CONCAT(interp_crcf,name)
#define INTERP_MANGLE_CCCF(name)  LIQUID_CONCAT(interp_cccf,name)

#define LIQUID_INTERP_DEFINE_API(INTERP,TO,TC,TI)               \
typedef struct INTERP(_s) * INTERP();                           \
INTERP() INTERP(_create)(unsigned int _M,                       \
              TC *_h,                                           \
              unsigned int _h_len);                             \
INTERP() INTERP(_create_rrc)(unsigned int _k,                   \
                   unsigned int _m,                             \
                   float _beta,                                 \
                   float _dt);                                  \
void INTERP(_destroy)(INTERP() _q);                             \
void INTERP(_print)(INTERP() _q);                               \
void INTERP(_execute)(INTERP() _q, TI _x, TO *_y);

LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_RRRF, float,         float,          float)
LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Decimator
//
#define DECIM_MANGLE_RRRF(name) LIQUID_CONCAT(decim_rrrf,name)
#define DECIM_MANGLE_CRCF(name) LIQUID_CONCAT(decim_crcf,name)
#define DECIM_MANGLE_CCCF(name) LIQUID_CONCAT(decim_cccf,name)

#define LIQUID_DECIM_DEFINE_API(DECIM,TO,TC,TI)                 \
typedef struct DECIM(_s) * DECIM();                             \
DECIM() DECIM(_create)(unsigned int _D,                         \
                       TC *_h,                                  \
                       unsigned int _h_len);                    \
void    DECIM(_destroy)(DECIM() _q);                            \
void    DECIM(_print)(DECIM() _q);                              \
void    DECIM(_execute)(DECIM() _q,                             \
                        TI *_x,                                 \
                        TO *_y,                                 \
                        unsigned int _index);

LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_RRRF, float,           float,          float)
LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)

// 
// quadrature mirror filter banks
//
#define QMFB_MANGLE_RRRF(name)      LIQUID_CONCAT(qmfb_rrrf,name)
#define QMFB_MANGLE_CRCF(name)      LIQUID_CONCAT(qmfb_crcf,name)
//#define QMFB_MANGLE_CCCF(name)      LIQUID_CONCAT(qmfb_cccf,name)

#define LIQUID_QMFB_ANALYZER    0
#define LIQUID_QMFB_SYNTHESIZER 1

#define LIQUID_QMFB_DEFINE_API(QMFB,TO,TC,TI)                   \
typedef struct QMFB(_s) * QMFB();                               \
QMFB()  QMFB(_create)(unsigned int _h_len,                      \
                      float _beta,                              \
                      int _type);                               \
QMFB()  QMFB(_recreate)(QMFB() _q, unsigned int _m);            \
void    QMFB(_destroy)(QMFB() _q);                              \
void    QMFB(_print)(QMFB() _q);                                \
void    QMFB(_clear)(QMFB() _q);                                \
void    QMFB(_execute)(QMFB() _q,                               \
                       TI   _x0,                                \
                       TI   _x1,                                \
                       TO * _y0,                                \
                       TO * _y1);

LIQUID_QMFB_DEFINE_API(QMFB_MANGLE_RRRF, float, float, float)
LIQUID_QMFB_DEFINE_API(QMFB_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)
//LIQUID_QMFB_DEFINE_API(QMFB_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)

// 
// iterative tree quadrature mirror filter banks
//
#define ITQMFB_MANGLE_RRRF(name)      LIQUID_CONCAT(itqmfb_rrrf,name)
#define ITQMFB_MANGLE_CRCF(name)      LIQUID_CONCAT(itqmfb_crcf,name)
//#define ITQMFB_MANGLE_CCCF(name)      LIQUID_CONCAT(itqmfb_cccf,name)

#define LIQUID_ITQMFB_ANALYZER      0
#define LIQUID_ITQMFB_SYNTHESIZER   1

#define LIQUID_ITQMFB_DEFINE_API(ITQMFB,TO,TC,TI)               \
typedef struct ITQMFB(_s) * ITQMFB();                           \
ITQMFB()    ITQMFB(_create)(unsigned int _n,                    \
                            unsigned int _m,                    \
                            float _slsl,                        \
                            int _type);                         \
ITQMFB()    ITQMFB(_recreate)(ITQMFB() _q, unsigned int _m);    \
void        ITQMFB(_destroy)(ITQMFB() _q);                      \
void        ITQMFB(_print)(ITQMFB() _q);                        \
void        ITQMFB(_execute)(ITQMFB() _q,                       \
                             TO * _x,                           \
                             TO * _y);

LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_RRRF, float, float, float)
LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)
//LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)

// 
// Half-band resampler
//
#define RESAMP2_MANGLE_RRRF(name)   LIQUID_CONCAT(resamp2_rrrf,name)
#define RESAMP2_MANGLE_CRCF(name)   LIQUID_CONCAT(resamp2_crcf,name)
#define RESAMP2_MANGLE_CCCF(name)   LIQUID_CONCAT(resamp2_cccf,name)

#define LIQUID_RESAMP2_DEFINE_API(RESAMP2,TO,TC,TI)             \
typedef     struct RESAMP2(_s) * RESAMP2();                     \
RESAMP2()   RESAMP2(_create)(unsigned int _h_len);              \
RESAMP2()   RESAMP2(_recreate)(RESAMP2() _q,                    \
                               unsigned int _h_len);            \
void        RESAMP2(_destroy)(RESAMP2() _q);                    \
void        RESAMP2(_print)(RESAMP2() _q);                      \
void        RESAMP2(_decim_execute)(RESAMP2() _f,               \
                                    TI * _x,                    \
                                    TO * _y);                   \
void        RESAMP2(_interp_execute)(RESAMP2() _f,              \
                                     TI _x,                     \
                                     TO * _y);

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_RRRF, float,           float,          float)
LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)


// 
// Arbitrary resampler
//
#define RESAMP_MANGLE_RRRF(name)    LIQUID_CONCAT(resamp_rrrf,name)
#define RESAMP_MANGLE_CRCF(name)    LIQUID_CONCAT(resamp_crcf,name)
#define RESAMP_MANGLE_CCCF(name)    LIQUID_CONCAT(resamp_cccf,name)

#define LIQUID_RESAMP_DEFINE_API(RESAMP,TO,TC,TI)               \
typedef     struct RESAMP(_s) * RESAMP();                       \
RESAMP()    RESAMP(_create)(float _r,                           \
                            unsigned int _h_len,                \
                            float _fc,                          \
                            float _slsl,                        \
                            unsigned int _npfb);                \
void        RESAMP(_destroy)(RESAMP() _q);                      \
void        RESAMP(_print)(RESAMP() _q);                        \
void        RESAMP(_reset)(RESAMP() _q);                        \
void        RESAMP(_setrate)(RESAMP() _q, float _rate);         \
void        RESAMP(_execute)(RESAMP() _q,                       \
                             TI _x,                             \
                             TO * _y,                           \
                             unsigned int *_num_written);

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_RRRF, float,         float,          float)
LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Symbol timing recovery (symbol synchronizer)
//
#define SYMSYNC_MANGLE_RRRF(name)   LIQUID_CONCAT(symsync_rrrf,name)
#define SYMSYNC_MANGLE_CRCF(name)   LIQUID_CONCAT(symsync_crcf,name)
#define SYMSYNC_MANGLE_CCCF(name)   LIQUID_CONCAT(symsync_cccf,name)

#define LIQUID_SYMSYNC_DEFINE_API(SYMSYNC,TO,TC,TI)             \
typedef struct SYMSYNC(_s) * SYMSYNC();                         \
SYMSYNC()   SYMSYNC(_create)(unsigned int _k,                   \
                             unsigned int _num_filters,         \
                             TC * _h,                           \
                             unsigned int _h_len);              \
void        SYMSYNC(_destroy)(SYMSYNC() _q);                    \
void        SYMSYNC(_print)(SYMSYNC() _q);                      \
void        SYMSYNC(_execute)(SYMSYNC() _q,                     \
                              TI * _x,                          \
                              unsigned int _nx,                 \
                              TO * _y,                          \
                              unsigned int *_ny);               \
void        SYMSYNC(_set_lf_bw)(SYMSYNC() _q, float _bt);       \
void        SYMSYNC(_clear)(SYMSYNC() _q);                      \
void        SYMSYNC(_estimate_timing)(SYMSYNC() _q,             \
                                      TI * _x,                  \
                                      unsigned int _n);

LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_RRRF, float,           float,          float)
LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)

// 
// Symbol timing recovery (symbol synchronizer, 2 samples/symbol in/out)
//
#define SYMSYNC2_MANGLE_RRRF(name)  LIQUID_CONCAT(symsync2_rrrf,name)
#define SYMSYNC2_MANGLE_CRCF(name)  LIQUID_CONCAT(symsync2_crcf,name)
#define SYMSYNC2_MANGLE_CCCF(name)  LIQUID_CONCAT(symsync2_cccf,name)

#define LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2,TO,TC,TI)           \
typedef struct SYMSYNC2(_s) * SYMSYNC2();                       \
SYMSYNC2()  SYMSYNC2(_create)(unsigned int _num_filters,        \
                              TC * _h,                          \
                              unsigned int _h_len);             \
void        SYMSYNC2(_destroy)(SYMSYNC2() _q);                  \
void        SYMSYNC2(_print)(SYMSYNC2() _q);                    \
void        SYMSYNC2(_execute)(SYMSYNC2() _q,                   \
                               TI * _x,                         \
                               unsigned int _nx,                \
                               TO * _y,                         \
                               unsigned int *_ny);              \
void        SYMSYNC2(_set_lf_bw)(SYMSYNC2() _q, float _bt);     \
void        SYMSYNC2(_clear)(SYMSYNC2() _q);                    \
void        SYMSYNC2(_estimate_timing)(SYMSYNC2() _q,           \
                                       TI * _x,                 \
                                       unsigned int _n);

LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_RRRF, float,         float,          float)
LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

//
// Finite impulse response Farrow filter
//

#define FIR_FARROW_MANGLE_RRRF(name)  LIQUID_CONCAT(fir_farrow_rrrf,name)
#define FIR_FARROW_MANGLE_CRCF(name)  LIQUID_CONCAT(fir_farrow_crcf,name)
//#define FIR_FARROW_MANGLE_CCCF(name)  LIQUID_CONCAT(fir_farrow_cccf,name)

// Macro:
//   FIR_FARROW : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_FIR_FARROW_DEFINE_API(FIR_FARROW,TO,TC,TI)       \
typedef struct FIR_FARROW(_s) * FIR_FARROW();                   \
FIR_FARROW() FIR_FARROW(_create)(unsigned int _n,               \
                                 unsigned int _p,               \
                                 float _fc,                     \
                                 float _beta);                  \
void FIR_FARROW(_destroy)(FIR_FARROW() _f);                     \
void FIR_FARROW(_clear)(FIR_FARROW() _f);                       \
void FIR_FARROW(_print)(FIR_FARROW() _f);                       \
void FIR_FARROW(_push)(FIR_FARROW() _f, TI _x);                 \
void FIR_FARROW(_set_delay)(FIR_FARROW() _f, float _mu);        \
void FIR_FARROW(_execute)(FIR_FARROW() _f, TO *_y);             \
unsigned int FIR_FARROW(_get_length)(FIR_FARROW() _f);          \
void FIR_FARROW(_get_coefficients)(FIR_FARROW() _f, float * _h);

LIQUID_FIR_FARROW_DEFINE_API(FIR_FARROW_MANGLE_RRRF, float, float, float)
LIQUID_FIR_FARROW_DEFINE_API(FIR_FARROW_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)
//LIQUID_FIR_FARROW_DEFINE_API(FIR_FARROW_MANGLE_CCCF, liquid_float_complex, liquid_float_complex, liquid_float_complex)


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



//
// MODULE : framing
//

//
// Basic frame generator (64 bytes data payload)
//
typedef struct framegen64_s * framegen64;
framegen64 framegen64_create(unsigned int _m,
                             float _beta);
void framegen64_destroy(framegen64 _fg);
void framegen64_print(framegen64 _fg);
void framegen64_execute(framegen64 _fg,
                        unsigned char * _header,
                        unsigned char * _payload,
                        liquid_float_complex * _y);
void framegen64_flush(framegen64 _fg,
                      unsigned int _n,
                      liquid_float_complex * _y);

// Basic frame synchronizer (64 bytes data payload)
typedef int (*framesync64_callback)(unsigned char * _header,
                                    int _header_valid,
                                    unsigned char * _payload,
                                    int _payload_valid,
                                    void * _userdata);
typedef struct framesync64_s * framesync64;
framesync64 framesync64_create(unsigned int _m,
                               float _beta,
                               framesync64_callback _callback,
                               void * _userdata);
void framesync64_destroy(framesync64 _fs);
void framesync64_print(framesync64 _fs);
void framesync64_reset(framesync64 _fs);
void framesync64_execute(framesync64 _fs,
                         liquid_float_complex * _x,
                         unsigned int _n);

void framesync64_set_agc_bw0(framesync64 _fs, float _agc_bw0);
void framesync64_set_agc_bw1(framesync64 _fs, float _agc_bw1);
void framesync64_set_pll_bw0(framesync64 _fs, float _pll_bw0);
void framesync64_set_pll_bw1(framesync64 _fs, float _pll_bw1);
void framesync64_set_sym_bw0(framesync64 _fs, float _sym_bw0);
void framesync64_set_sym_bw1(framesync64 _fs, float _sym_bw1);
void framesync64_set_squelch_threshold(framesync64 _fs, float _squelch_threshold);

//
// Flexible frame : adjustable payload, mod scheme, etc., but bring
//                  your own error correction, redundancy check
//

// frame generator
typedef struct {
    unsigned int rampup_len;
    unsigned int phasing_len;
    unsigned int payload_len;
    unsigned int mod_scheme;
    unsigned int mod_bps;
    unsigned int rampdn_len;
} flexframegenprops_s;
typedef struct flexframegen_s * flexframegen;
flexframegen flexframegen_create(flexframegenprops_s * _props);
void flexframegen_destroy(flexframegen _fg);
void flexframegen_getprops(flexframegen _fg, flexframegenprops_s * _props);
void flexframegen_setprops(flexframegen _fg, flexframegenprops_s * _props);
void flexframegen_print(flexframegen _fg);
unsigned int flexframegen_getframelen(flexframegen _fg);
void flexframegen_execute(flexframegen _fg,
                          unsigned char * _header,
                          unsigned char * _payload,
                          liquid_float_complex * _y);
void flexframegen_flush(flexframegen _fg,
                        unsigned int _n,
                        liquid_float_complex * _y);

// frame synchronizer
typedef int (*flexframesync_callback)(unsigned char * _header,
                                      int _header_valid,
                                      unsigned char * _payload,
                                      unsigned int _payload_len,
                                      void * _userdata);
typedef struct {
    float agc_bw0, agc_bw1;     // automatic gain control bandwidth
    float sym_bw0, sym_bw1;     // symbol synchronizer bandwidth
    float pll_bw0, pll_bw1;     // phase-locked loop bandwidth
    unsigned int k;             // decimation rate
    unsigned int npfb;          // number of filters in symbol sync.
    unsigned int m;             // filter length
    float beta;                 // excess bandwidth
    float squelch_threshold;    // squelch enable/disable threshold
    //flexframesync_callback callback;
    //void * userdata;
} flexframesyncprops_s;
typedef struct flexframesync_s * flexframesync;
flexframesync flexframesync_create(flexframesyncprops_s * _props,
                                   flexframesync_callback _callback,
                                   void * _userdata);
void flexframesync_destroy(flexframesync _fs);
void flexframesync_getprops(flexframesync _fs, flexframesyncprops_s * _props);
void flexframesync_setprops(flexframesync _fs, flexframesyncprops_s * _props);
void flexframesync_print(flexframesync _fs);
void flexframesync_reset(flexframesync _fs);
void flexframesync_execute(flexframesync _fs,
                           liquid_float_complex * _x,
                           unsigned int _n);

//
// P/N synchronizer
//
#define PNSYNC_MANGLE_RRRF(name)    LIQUID_CONCAT(pnsync_rrrf,name)
#define PNSYNC_MANGLE_CRCF(name)    LIQUID_CONCAT(pnsync_crcf,name)
#define PNSYNC_MANGLE_CCCF(name)    LIQUID_CONCAT(pnsync_cccf,name)

// Macro:
//   PNSYNC : name-mangling macro
//   TO     : output data type
//   TC     : coefficients data type
//   TI     : input data type
#define LIQUID_PNSYNC_DEFINE_API(PNSYNC,TO,TC,TI)               \
typedef struct PNSYNC(_s) * PNSYNC();                           \
                                                                \
PNSYNC() PNSYNC(_create)(unsigned int _n, TC * _v);             \
PNSYNC() PNSYNC(_create_msequence)(unsigned int _g);            \
void PNSYNC(_destroy)(PNSYNC() _fs);                            \
void PNSYNC(_print)(PNSYNC() _fs);                              \
void PNSYNC(_correlate)(PNSYNC() _fs, TI _sym, TO * _y);

LIQUID_PNSYNC_DEFINE_API(PNSYNC_MANGLE_RRRF, float,           float,          float)
LIQUID_PNSYNC_DEFINE_API(PNSYNC_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_PNSYNC_DEFINE_API(PNSYNC_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)


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
BSYNC() BSYNC(_create_msequence)(unsigned int _g);              \
void BSYNC(_destroy)(BSYNC() _fs);                              \
void BSYNC(_print)(BSYNC() _fs);                                \
void BSYNC(_correlate)(BSYNC() _fs, TI _sym, TO * _y);

LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_RRRF, float,                   float,                  float)
LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_CRCF, liquid_float_complex,    float,                  liquid_float_complex)
LIQUID_BSYNC_DEFINE_API(BSYNC_MANGLE_CCCF, liquid_float_complex,    liquid_float_complex,   liquid_float_complex)


// 
// Packetizer
//

typedef struct packetizer_s * packetizer;

packetizer packetizer_create(unsigned int _dec_msg_len, int _fec0, int _fec1);
void packetizer_destroy(packetizer _p);
void packetizer_print(packetizer _p);

unsigned int packetizer_get_packet_length(unsigned int _n, int _fec0, int _fec1);

void packetizer_encode(packetizer _p, unsigned char * _msg, unsigned char * _pkt);
int  packetizer_decode(packetizer _p, unsigned char * _pkt, unsigned char * _msg);


//
// MODULE : interleaver
//

typedef struct interleaver_s * interleaver;
typedef enum {
    INT_BLOCK=0,
    INT_SEQUENCE
} interleaver_type;

// create interleaver
//   _n     : number of bytes
//   _type  : type of re-ordering
interleaver interleaver_create(unsigned int _n, interleaver_type _type);

void interleaver_destroy(interleaver _i);

void interleaver_print(interleaver _i);

void interleaver_init_block(interleaver _i);

void interleaver_init_sequence(interleaver _i);

void interleaver_interleave(interleaver _i, unsigned char * _x, unsigned char * _y);

void interleaver_deinterleave(interleaver _i, unsigned char * _y, unsigned char * _x);

void interleaver_debug_print(interleaver _i);


//
// MODULE : math
//

// ln( gamma(z) )
float lngammaf(float _z);

// gamma(z)
float gammaf(float _z);

// n!
float factorialf(unsigned int _n);

// Bessel function of the first kind
float besselj_0(float _z);

// Modified Bessel function of the first kind
float besseli_0(float _z);

// sin(pi x) / (pi x)
float sincf(float _x);

// 
// Windowing functions
//

// Kaiser window
float kaiser(unsigned int _n, unsigned int _N, float _beta, float _dt);

// Hamming window
float hamming(unsigned int _n, unsigned int _N);

// Hann window
float hann(unsigned int _n, unsigned int _N);

// Blackman-harris window
float blackmanharris(unsigned int _n, unsigned int _N);


// polynomials

// evaluate polynomial _p (order _k-1) at value _x
float polyval(float * _p, unsigned int _k, float _x);

// least-squares polynomial fit (order _k-1)
void polyfit(float * _x,
             float * _y,
             unsigned int _n,
             float * _p,
             unsigned int _k);



//
// MODULE : matrix
//

#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(fmatrix, name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(cfmatrix, name)

// large macro
//   MATRIX : name-mangling macro
//   T      : data type
#define LIQUID_MATRIX_DEFINE_API(MATRIX,T)                      \
void MATRIX(_print)(T * _x,                                     \
                    unsigned int _rx,                           \
                    unsigned int _cx);                          \
void MATRIX(_add)(unsigned int _r, unsigned int _c,             \
                  T * _x,                                       \
                  T * _y,                                       \
                  T * _z);                                      \
void MATRIX(_sub)(unsigned int _r, unsigned int _c,             \
                  T * _x,                                       \
                  T * _y,                                       \
                  T * _z);                                      \
void MATRIX(_mul)(T * _x, unsigned int _rx, unsigned int _cx,   \
                  T * _y, unsigned int _ry, unsigned int _cy,   \
                  T * _z, unsigned int _rz, unsigned int _cz);  \
void MATRIX(_div)(T * _x, unsigned int _rx, unsigned int _cx,   \
                  T * _y, unsigned int _ry, unsigned int _cy,   \
                  T * _z, unsigned int _rz, unsigned int _cz);  \
void MATRIX(_trans)(T * _x, unsigned int _rx, unsigned int _cx);\
void MATRIX(_inv)(T * _x,                                       \
                  unsigned int _rx,                             \
                  unsigned int _cx);

#define matrix_access(X,R,C,r,c) ((X)[(r)*(C)+(c)])

LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_FLOAT, float)
LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_CFLOAT, liquid_float_complex)


//
// MODULE : modem (modulator/demodulator)
//

// Maximum number of allowed bits per symbol
#define MAX_MOD_BITS_PER_SYMBOL 8

// Modulation schemes available
typedef enum {
    MOD_UNKNOWN=0,      // Unknown modulation scheme
    MOD_PSK,            // Phase-shift keying (PSK)
    MOD_BPSK,           // Specific: binary PSK
    MOD_QPSK,           // specific: quaternary PSK
    MOD_DPSK,           // differential PSK
    MOD_ASK,            // amplitude-shift keying
    MOD_QAM,            // quadrature amplitude-shift keying (QAM)
    MOD_ARB,            // arbitrary QAM

    /* \brief Arbitrary QAM, only the constellation points in the first
     * quadrant are defined, the rest are mirrored about the abscissa and
     * ordinate
     *
     * \image html Constellation64ARBM.png "64-QAM (arb, mirrored) constellation"
     */
    MOD_ARB_MIRRORED,

    /* \brief Arbitrary QAM, only the constellation points in the first
     * quadrant are defined, the rest are rotated about the origin to the
     * other three quadrants
     *
     * \image html Constellation64ARBR.png "64-QAM (arb, rotated) constellation"
     */
    MOD_ARB_ROTATED

} modulation_scheme;

// Modulation scheme string for printing purposes
extern const char* modulation_scheme_str[10];

// useful functions

// counts the number of different bits between two symbols
unsigned int count_bit_errors(unsigned int _s1, unsigned int _s2);

// converts binary-coded decimal (BCD) to gray, ensuring successive values
// differ by exactly one bit
unsigned int gray_encode(unsigned int symbol_in);

// converts a gray-encoded symbol to binary-coded decimal (BCD)
unsigned int gray_decode(unsigned int symbol_in);


//
// Linear modem
//

// define struct pointer
typedef struct modem_s * modem;

// create modulation scheme, allocating memory as necessary
modem modem_create(modulation_scheme, unsigned int _bits_per_symbol);

void modem_destroy(modem _mod);
void modem_print(modem _mod);

// Initialize arbitrary modem constellation
void modem_arb_init(modem _mod, liquid_float_complex *_symbol_map, unsigned int _len);

// Initialize arbitrary modem constellation on data from external file
void modem_arb_init_file(modem _mod, char* filename);

// Generate random symbol
unsigned int modem_gen_rand_sym(modem _mod);

// Accessor functions
unsigned int modem_get_bps(modem _mod);

// generic modulate function; simply queries modem scheme and calls
// appropriate subroutine
void modem_modulate(modem _mod, unsigned int _s, liquid_float_complex *_y);

void modem_demodulate(modem _demod, liquid_float_complex _x, unsigned int *_s);
void get_demodulator_phase_error(modem _demod, float* _phi);
void get_demodulator_evm(modem _demod, float* _evm);

//
// Continuous phase modems
//
typedef enum {
    CPMOD_FSK,
    CPMOD_MSK
} cpmodem_scheme;

typedef struct cpmodem_s * cpmodem;
cpmodem cpmodem_create(cpmodem_scheme _ms,
                       unsigned int _bps,
                       unsigned int _k);
void cpmodem_destroy(cpmodem _mod);
void cpmodem_print(cpmodem _mod);
void cpmodem_modulate(cpmodem _mod,
                      unsigned int _s,
                      liquid_float_complex *_y);
void cpmodem_demodulate(cpmodem _mod,
                        liquid_float_complex _x,
                        unsigned int * _s);

// 
// Analog modems
//

// TODO : complete analog modems
typedef struct freqmodem_s * freqmodem;
freqmodem freqmodem_create();
void freqmodem_destroy(freqmodem _fm);
void freqmodem_print(freqmodem _fm);
void freqmodem_modulate(freqmodem _fm, float _x, liquid_float_complex *_y);
void freqmodem_demodulate(freqmodem _fm, liquid_float_complex *_y, float *_x);

//
// MODULE : multicarrier
//

// FIR polyphase filterbank channelizer
typedef struct firpfbch_s * firpfbch;

#define FIRPFBCH_NYQUIST        0
#define FIRPFBCH_ROOTNYQUIST    1

#define FIRPFBCH_ANALYZER       0
#define FIRPFBCH_SYNTHESIZER    1
// TODO: use filter prototype object
// _num_channels:   number of channels
// _m           :   filter delay (length = 2*k*m)
// _beta        :   sidelobe suppression level (nyquist), excess bandwidth (root nyquist)
// _nyquist     :   0: nyquist, 1: root nyquist
// _type        :   0: analyzer, 1: synthesizer
firpfbch firpfbch_create(unsigned int _num_channels,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         int _nyquist,
                         int _type);
void firpfbch_destroy(firpfbch _c);
void firpfbch_clear(firpfbch _c);
void firpfbch_print(firpfbch _c);
void firpfbch_execute(firpfbch _c, liquid_float_complex * _x, liquid_float_complex * _y);
//void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _X);
//void firpfbch_analyzer_execute(firpfbch _c, float complex * _X, float complex * _x);

// FIR OFDM/OQAM
typedef struct ofdmoqam_s * ofdmoqam;
#define OFDMOQAM_ANALYZER       FIRPFBCH_ANALYZER
#define OFDMOQAM_SYNTHESIZER    FIRPFBCH_SYNTHESIZER
ofdmoqam ofdmoqam_create(unsigned int _num_channels,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         int _type);
void ofdmoqam_destroy(ofdmoqam _c);
void ofdmoqam_print(ofdmoqam _c);
void ofdmoqam_clear(ofdmoqam _c);
void ofdmoqam_execute(ofdmoqam _c,
                      liquid_float_complex * _x,
                      liquid_float_complex * _y);

// Discrete multi-tone (OFDM)
#if 0
typedef struct ofdm_s * ofdm;
#define OFDM_ANALYZER           FIRPFBCH_ANALYZER
#define OFDM_SYNTHESIZER        FIRPFBCH_SYNTHESIZER
ofdm ofdm_create(unsigned int   _num_channels,
                 unsigned int   _cp_len,
                 unsigned int   _taper_len,
                 int            _type);
void ofdm_destroy(ofdm _c);
void ofdm_print(ofdm _c);
void ofdm_execute(ofdm _c,
                  liquid_float_complex * _x,
                  liquid_float_complex *_y);
#endif

// OFDM frame (symbol) generator
typedef struct ofdmframegen_s * ofdmframegen;
ofdmframegen ofdmframegen_create(unsigned int _num_subcarriers,
                                 unsigned int  _cp_len);
                                 //unsigned int  _taper_len);
void ofdmframegen_destroy(ofdmframegen _q);
void ofdmframegen_print(ofdmframegen _q);
void ofdmframegen_execute(ofdmframegen _q,
                          liquid_float_complex * _x,
                          liquid_float_complex *_y);

// OFDM frame (symbol) synchronizer
typedef int (*ofdmframesync_callback)(liquid_float_complex * _y,
                                      unsigned int _n,
                                      void * _userdata);
typedef struct ofdmframesync_s * ofdmframesync;
ofdmframesync ofdmframesync_create(unsigned int _num_subcarriers,
                                   unsigned int  _cp_len,
                                   //unsigned int  _taper_len,
                                   ofdmframesync_callback _callback,
                                   void * _userdata);
void ofdmframesync_destroy(ofdmframesync _q);
void ofdmframesync_print(ofdmframesync _q);
void ofdmframesync_reset(ofdmframesync _q);
void ofdmframesync_execute(ofdmframesync _q,
                           liquid_float_complex * _x,
                           unsigned int _n);

// 
// MODULE : nco (numerically-controlled oscillator)
//


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

static inline void nco_reset(nco _nco) {
    _nco->theta   = 0.0f;
    _nco->d_theta = 0.0f;
}

void nco_set_phase(nco _nco, float _phi);
void nco_adjust_phase(nco _nco, float _dphi);

void nco_step(nco _nco);
void nco_constrain_phase(nco _nco);

float nco_sin(nco _nco);
float nco_cos(nco _nco);

void nco_sincos(nco _nco, float* _s, float* _c);

void nco_cexpf(nco _nco, liquid_float_complex * _y);

// mixing functions

// Rotate input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_up(nco _nco, liquid_float_complex _x, liquid_float_complex *_y);

// Rotate input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
void nco_mix_down(nco _nco, liquid_float_complex _x, liquid_float_complex *_y);

// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_up(nco _nco,
                      liquid_float_complex *_x,
                      liquid_float_complex *_y,
                      unsigned int _N);

void nco_mix_block_down(nco _nco,
                        liquid_float_complex *_x,
                        liquid_float_complex *_y,
                        unsigned int _N);

//
// Phase-locked loop
//

typedef struct pll_s * pll;
pll pll_create(void);
void pll_destroy(pll _p);
void pll_print(pll _p);
void pll_reset(pll _p);

void pll_set_bandwidth(pll _p, float _bt);
void pll_set_damping_factor(pll _p, float _xi);
//void pll_execute(pll _p, liquid_float_complex _x, liquid_float_complex *_y, float _e);
void pll_step(pll _p, nco _nco, float _e);



//
// MODULE : optimization
//

// optim pattern set (struct)
struct optim_ps_s {
    float *x, *y;
    unsigned int nx, ny, np;
    unsigned int na; // num allocated
};

typedef struct optim_ps_s * optim_ps;

optim_ps optim_ps_create(unsigned int _nx, unsigned int _ny);
void optim_ps_destroy(optim_ps _ps);
void optim_ps_print(optim_ps _ps);
void optim_ps_append_pattern(optim_ps _ps, float *_x, float *_y);
void optim_ps_append_patterns(optim_ps _ps, float *_x, float *_y, unsigned int _np);
void optim_ps_delete_pattern(optim_ps _ps, unsigned int _i);
void optim_ps_clear(optim_ps _ps);
void optim_ps_access(optim_ps _ps, unsigned int _i, float **_x, float **_y);

typedef void(*optim_target_function)(float *_x, float *_y, void *_p);
typedef float(*optim_obj_function)(optim_ps _ps, void *_p, optim_target_function _f);

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

#define COMPANDER_LINEAR    0
#define COMPANDER_MULAW     1
#define COMPANDER_ALAW      2

typedef struct quantizer_rf_s * quantizer_rf;
quantizer_rf quantizer_rf_create(int _compander_type, float _range, unsigned int _num_bits);
void quantizer_rf_destroy(quantizer_rf _q);
void quantizer_rf_print(quantizer_rf _q);

void quantizer_rf_adc(quantizer_rf _q, float _x, unsigned int * _sample);
void quantizer_rf_dac(quantizer_rf _q, unsigned int _sample, float * _x);



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
float randn_pdf(float _x, float _eta, float _sig);
float randn_cdf(float _x, float _eta, float _sig);

// Weibull
//   f(x) = a*(x-g)^(b-1)*exp{-(a/b)*(x-g)^b}  x >= g
//          0                                  else
//   where
//     a = alpha : scaling parameter
//     b = beta  : shape parameter
//     g = gamma : location (threshold) parameter
//
float rand_weibullf(float _alpha, float _beta, float _gamma);
float rand_pdf_weibullf(float _x, float _a, float _b, float _g);
float rand_cdf_weibullf(float _x, float _a, float _b, float _g);

// Gamma
//void rand_gammaf();

// Nakagami-m
//void rand_nakagamimf(float _m, float _omega);
//float rand_pdf_nakagamimf(float _x, float _m, float _omega);
//float rand_cdf_nakagamimf(float _x, float _m, float _omega);

// Rice-K
float rand_ricekf(float _K, float _omega);
float rand_pdf_ricekf(float _x, float _K, float _omega);
float rand_cdf_ricekf(float _x, float _K, float _omega);


// Data scrambler : whiten data sequence
void scramble_data(unsigned char * _x, unsigned int _len);
void unscramble_data(unsigned char * _x, unsigned int _len);

//
// MODULE : sequence
//

// Binary sequence (generic)

typedef struct bsequence_s * bsequence;

// Create a binary sequence of a specific length
bsequence bsequence_create(unsigned int num_bits);

// Free memory in a binary sequence
void bsequence_destroy(bsequence _bs);

// Clear binary sequence (set to 0's)
void bsequence_clear(bsequence _bs);

// Print sequence to the screen
void bsequence_print(bsequence _bs);

// Push bit into to back of a binary sequence
void bsequence_push(bsequence _bs, unsigned int b);

// circular shift (left)
void bsequence_circshift(bsequence _bs);

// Correlates two binary sequences together
int bsequence_correlate(bsequence _bs1, bsequence _bs2);

// Binary addition of two bit sequences
void bsequence_add(bsequence _bs1, bsequence _bs2, bsequence _bs3);

// Accumulates the 1's in a binary sequence
unsigned int bsequence_accumulate(bsequence _bs);

// accessor functions
unsigned int bsequence_get_length(bsequence _bs);

// M-Sequence

#define LIQUID_MAX_MSEQUENCE_LENGTH   4095

// default m-sequence generators:   g (hex)    m    n       g (octal)
#define LIQUID_MSEQUENCE_N3         0x0007  // 2    3       7
#define LIQUID_MSEQUENCE_N7         0x000B  // 3    7       13
#define LIQUID_MSEQUENCE_N15        0x0013  // 4    15      23
#define LIQUID_MSEQUENCE_N31        0x0025  // 5    31      45
#define LIQUID_MSEQUENCE_N63        0x0043  // 6    63      103
#define LIQUID_MSEQUENCE_N127       0x0089  // 7    127     211
#define LIQUID_MSEQUENCE_N255       0x011D  // 8    255     435
#define LIQUID_MSEQUENCE_N511       0x0211  // 9    511     1021
#define LIQUID_MSEQUENCE_N1023      0x0409  // 10   1023    2011
#define LIQUID_MSEQUENCE_N2047      0x0805  // 11   2047    4005
#define LIQUID_MSEQUENCE_N4095      0x1053  // 12   4095    10123

typedef struct msequence_s * msequence;

msequence msequence_create(unsigned int _m);
void msequence_destroy(msequence _m);

// Initialize msequence generator
void msequence_init(msequence _ms, unsigned int _m, unsigned int _g, unsigned int _a);

// Advance msequence on shift register
unsigned int msequence_advance(msequence _ms);

// Generate symbol
unsigned int msequence_generate_symbol(msequence _ms, unsigned int _bps);

// Reset msequence shift register to original state
void msequence_reset(msequence _ms);

// Initializes a bsequence object on a maximum length P/N sequence
void bsequence_init_msequence(bsequence _bs, msequence _ms);

// accessor methods
unsigned int msequence_get_length(msequence _ms);


// Complementary codes

//typedef struct csequence_s * csequence;

void bsequence_create_ccodes(bsequence _a, bsequence _b);

// 
// MODULE : utility
//

// packs bytes with one bit of information into a byte
void pack_bytes(unsigned char * input,
                unsigned int input_length,
                unsigned char * output,
                unsigned int output_length,
                unsigned int *num_written);

// unpacks bytes with 8 bits of information
void unpack_bytes(unsigned char * input,
                  unsigned int input_length,
                  unsigned char * output,
                  unsigned int output_length,
                  unsigned int *num_written);

// repacks bytes with arbitrary symbol sizes
void repack_bytes(unsigned char * input,
                  unsigned int input_sym_size,
                  unsigned int input_length,
                  unsigned char * output,
                  unsigned int output_sym_size,
                  unsigned int output_length,
                  unsigned int *num_written);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __LIQUID_H__

