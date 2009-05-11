/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

// Initialize AGC object
void agc_init(agc _agc);

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
typedef struct ann_s * ann;

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
fbasc fbasc_create(
        int _type,
        unsigned int _num_channels,
        unsigned int _samples_per_frame,
        unsigned int _bytes_per_frame);
void fbasc_destroy(fbasc _q);
void fbasc_print(fbasc _q);

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame);
void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio);

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
//   X: name-mangling macro
//   T: data type
#define LIQUID_BUFFER_DEFINE_API(X,T)                   \
                                                        \
typedef struct X(_s) * X();                             \
X() X(_create)(buffer_type _type, unsigned int _n);     \
void X(_destroy)(X() _b);                               \
void X(_print)(X() _b);                                 \
void X(_debug_print)(X() _b);                           \
void X(_clear)(X() _b);                                 \
void X(_zero)(X() _b);                                  \
void X(_read)(X() _b, T ** _v, unsigned int *_nr);      \
void X(_release)(X() _b, unsigned int _n);              \
void X(_write)(X() _b, T * _v, unsigned int _n);        \
void X(_push)(X() _b, T _v);
//void X(_force_write)(X() _b, T * _v, unsigned int _n);

// Define buffer APIs
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_CFLOAT, liquid_float_complex)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)


// Windowing functions
#define WINDOW_MANGLE_FLOAT(name)  LIQUID_CONCAT(fwindow, name)
#define WINDOW_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfwindow, name)
#define WINDOW_MANGLE_UINT(name)   LIQUID_CONCAT(uiwindow, name)

// large macro
//   X  :   name-mangling macro
//   T  :   data type
#define LIQUID_WINDOW_DEFINE_API(X,T)                   \
                                                        \
typedef struct X(_s) * X();                             \
X() X(_create)(unsigned int _n);                        \
X() X(_recreate)(X() _w, unsigned int _n);              \
void X(_destroy)(X() _w);                               \
void X(_print)(X() _w);                                 \
void X(_debug_print)(X() _w);                           \
void X(_clear)(X() _w);                                 \
void X(_read)(X() _w, T ** _v);                         \
void X(_push)(X() _b, T _v);                            \
void X(_write)(X() _b, T * _v, unsigned int _n);

// Define window APIs
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_CFLOAT, liquid_float_complex)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_UINT, unsigned int)


// Port
#define PORT_MANGLE_FLOAT(name)     LIQUID_CONCAT(fport,name)
#define PORT_MANGLE_CFLOAT(name)    LIQUID_CONCAT(cfport,name)
#define PORT_MANGLE_UINT(name)      LIQUID_CONCAT(uiport,name)

// Macro
//  X: name-mangling macro
//  T: data type
#define LIQUID_PORT_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _n); \
void X(_destroy)(X() _p); \
void X(_print)(X() _p); \
void X(_produce)(X() _p, T * _w, unsigned int _n); \
void X(_consume)(X() _p, T * _r, unsigned int _n); \

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
void gport2_produce(gport2 _p, void * _w, unsigned int _n);
void gport2_consume(gport2 _p, void * _r, unsigned int _n);
void gport2_consume_available(gport2 _p, void * _r, unsigned int _nmax, unsigned int *_nc);

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
void awgn_channel_execute(awgn_channel _q, liquid_float_complex _x, liquid_float_complex *_y);
void awgn_channel_set_noise_variance(awgn_channel _q, float _nvar);

// Rice-K channel
typedef struct ricek_channel_s * ricek_channel;
ricek_channel ricek_channel_create(unsigned int _h_len, float _K, float _fd, float _theta);
void ricek_channel_destroy(ricek_channel _q);
void ricek_channel_print(ricek_channel _q);
void ricek_channel_execute(ricek_channel _q, liquid_float_complex _x, liquid_float_complex *_y);

// Log-normal channel
typedef struct lognorm_channel_s * lognorm_channel;

// Composite channel
typedef struct channel_s * channel;
channel channel_create();
void channel_destroy(channel _c);
void channel_print(channel _c);
void channel_execute(channel _c, liquid_float_complex _x, liquid_float_complex *_y);

//
// MODULE : dotprod (vector dot product)
//

#define DOTPROD_MANGLE_RRRF(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define DOTPROD_MANGLE_CCCF(name)   LIQUID_CONCAT(dotprod_cccf,name)
#define DOTPROD_MANGLE_CRCF(name)   LIQUID_CONCAT(dotprod_crcf,name)

// large macro
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_DOTPROD_DEFINE_API(X,TO,TC,TI)       \
void X(_run)(TC *_h, TI *_x, unsigned int _n, TO *_y);        \
void X(_run4)(TC *_h, TI *_x, unsigned int _n, TO *_y);       \
                                                    \
typedef struct X(_s) * X();                         \
struct X(_s) {      \
    TC * h;         \
    unsigned int n; \
};                  \
X() X(_create)(TC * _v, unsigned int _n);           \
void X(_destroy)(X() _q);                           \
void X(_execute)(X() _q, TI * _v, TO * _y);

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

#define LIQUID_EQLMS_DEFINE_API(X,T)                \
typedef struct X(_s) * X();                         \
X() X(_create)(unsigned int _p);                    \
void X(_destroy)(X() _eq);                          \
void X(_print)(X() _eq);                            \
void X(_set_bw)(X() _eq, float _lambda);            \
void X(_reset)(X() _eq);                            \
void X(_execute)(X() _eq, T _x, T _d, T * _d_hat);  \
void X(_get_weights)(X() _eq, T * _w);              \
void X(_train)(X() _eq, T * _w, T * _x, T * _d, unsigned int _n);

LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_RRRF, float);
LIQUID_EQLMS_DEFINE_API(EQLMS_MANGLE_CCCF, liquid_float_complex);


// recursive least-squares (RLS)
#define EQRLS_MANGLE_RRRF(name)     LIQUID_CONCAT(eqrls_rrrf,name)
#define EQRLS_MANGLE_CCCF(name)     LIQUID_CONCAT(eqrls_cccf,name)

#define LIQUID_EQRLS_DEFINE_API(X,T)                \
typedef struct X(_s) * X();                         \
X() X(_create)(unsigned int _p);                    \
void X(_destroy)(X() _eq);                          \
void X(_print)(X() _eq);                            \
void X(_set_bw)(X() _eq, float _mu);                \
void X(_reset)(X() _eq);                            \
void X(_execute)(X() _eq, T _x, T _d, T * _d_hat);  \
void X(_get_weights)(X() _eq, T * _w);              \
void X(_train)(X() _eq, T * _w, T * _x, T * _d, unsigned int _n);

LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_RRRF, float);
LIQUID_EQRLS_DEFINE_API(EQRLS_MANGLE_CCCF, liquid_float_complex);


// 
// MODULE : estimation
//

float estimate_freqoffset(liquid_float_complex * _x, unsigned int _n);
//float estimate_phaseoffset(liquid_float_complex * _x, unsigned int _n);
//float estimate_timingoffset(liquid_float_complex * _x, unsigned int _n);


//
// MODULE : fec (forward error correction)
//



#define LIQUID_FEC_MANGLE_CRC32(name)       LIQUID_CONCAT(fec_crc32,name)
#define LIQUID_FEC_MANGLE_CHECKSUM32(name)  LIQUID_CONCAT(fec_checksum32,name)

// Macro: crc/checksum
//  CRC : name-mangling macro
#define LIQUID_CRC_DEFINE_API(CRC) \
typedef struct CRC(_s) * CRC(); \
CRC() CRC(_create)(); \
void CRC(_destroy)(CRC() _crc); \
void CRC(_print)(CRC() _crc); \
void CRC(_generate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len); \
int  CRC(_validate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len);

LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CRC32)
LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CHECKSUM32)

//
// checksum
//
unsigned char checksum_generate_key(unsigned char *_data, unsigned int _n);
int  checksum_validate_message(unsigned char *_data, unsigned int _n, unsigned char _key);

//
// crc (cyclic redundancy check)
//
//void crc_generate_key(unsigned char *_msg, unsigned int _msg_len, unsigned char *_key, unsigned int _key_len);
unsigned int crc32_generate_key(unsigned char *_data, unsigned int _n);
int  crc32_validate_message(unsigned char *_data, unsigned int _n, unsigned int _key);

// 
// FEC
//

typedef enum {
    FEC_UNKNOWN=0,
    FEC_NONE,
    FEC_REP3,
    FEC_HAMMING74,
    FEC_HAMMING84,

    // codecs not defined internally
    FEC_CONV_V27,
    FEC_CONV_V29,
    FEC_CONV_V39,
    FEC_CONV_V615
} fec_scheme;

struct fec_conv_opts {int puncture;};

extern const char * fec_scheme_str[5];

typedef struct fec_s * fec;

// object-independent methods
unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len);
float fec_get_rate(fec_scheme _scheme);

fec fec_create(fec_scheme _scheme, void *_opts);
void fec_destroy(fec _q);
void fec_print(fec _q);

void fec_encode(fec _q, unsigned int _dec_msg_len, unsigned char * _msg_dec, unsigned char * _msg_enc);
void fec_decode(fec _q, unsigned int _dec_msg_len, unsigned char * _msg_enc, unsigned char * _msg_dec);


//
// MODULE : fft (fast Fourier transform)
//

typedef struct fftplan_s * fftplan;

#define FFT_FORWARD 0
#define FFT_REVERSE 1
fftplan fft_create_plan(unsigned int _n, liquid_float_complex * _x, liquid_float_complex * _y, int _dir);
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

// Design FIR filter using window method
//  _n      : filter length (odd)
//  _fc     : filter cutoff (0 < _fc < 1)
//  _slsl   : sidelobe suppression level (_slsl < 0)
//  _h      : output coefficient buffer
void fir_design_windowed_sinc(unsigned int _n, float _fc, float _slsl, float *_h);

//
void fir_design_halfband_windowed_sinc(float * _h, unsigned int _n);

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n, float _fc, float _slsl, float *_h);

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
void fir_design_optim_root_nyquist(unsigned int _n, unsigned int _k, float _slsl, float *_h);

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
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_FIR_FILTER_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X();   \
X() X(_create)(TC * _h, unsigned int _n); \
X() X(_recreate)(X() _f, TC * _h, unsigned int _n); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_push)(X() _f, TI _x); \
void X(_execute)(X() _f, TO *_y); \
unsigned int X(_get_length)(X() _f);

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
#define LIQUID_FIRHILB_DEFINE_API(X,T,TC) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _h_len); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_clear)(X() _f); \
void X(_decim_execute)(X() _f, T * _x, TC * _y); \
void X(_interp_execute)(X() _f, TC _x, T * _y);

LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_FLOAT, float, liquid_float_complex)
//LIQUID_FIRHILB_DEFINE_API(FIRHILB_MANGLE_DOUBLE, double)

//
// Infinite impulse response filter
//

#define IIR_FILTER_MANGLE_RRRF(name)  LIQUID_CONCAT(iir_filter_rrrf,name)
#define IIR_FILTER_MANGLE_CRCF(name)  LIQUID_CONCAT(iir_filter_crcf,name)
#define IIR_FILTER_MANGLE_CCCF(name)  LIQUID_CONCAT(iir_filter_cccf,name)

// Macro:
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_IIR_FILTER_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X();   \
X() X(_create)(TC * _b, unsigned int _nb, TC * _a,  unsigned int _na); \
void X(_destroy)(X() _f); \
void X(_print)(X() _f); \
void X(_clear)(X() _f); \
void X(_execute)(X() _f, TI _x, TO *_y); \
unsigned int X(_get_length)(X() _f);

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
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_FIRPFB_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _num_filters, TC * _h, unsigned int _h_len); \
void X(_destroy)(X() _b); \
void X(_print)(X() _b); \
void X(_push)(X() _b, TI _x); \
void X(_execute)(X() _b, unsigned int _i, TO *_y); \
void X(_clear)(X() _b);

LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_RRRF, float,         float,          float)
LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_FIRPFB_DEFINE_API(FIRPFB_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Interpolator
//
#define INTERP_MANGLE_RRRF(name)  LIQUID_CONCAT(interp_rrrf,name)
#define INTERP_MANGLE_CRCF(name)  LIQUID_CONCAT(interp_crcf,name)
#define INTERP_MANGLE_CCCF(name)  LIQUID_CONCAT(interp_cccf,name)

#define LIQUID_INTERP_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _M, TC *_h, unsigned int _h_len); \
X() X(_create_rrc)(unsigned int _k, unsigned int _m, float _beta, float _dt); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, TI _x, TO *_y);

LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_RRRF, float,         float,          float)
LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_INTERP_DEFINE_API(INTERP_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Decimator
//
#define DECIM_MANGLE_RRRF(name) LIQUID_CONCAT(decim_rrrf,name)
#define DECIM_MANGLE_CRCF(name) LIQUID_CONCAT(decim_crcf,name)
#define DECIM_MANGLE_CCCF(name) LIQUID_CONCAT(decim_cccf,name)

#define LIQUID_DECIM_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _D, TC *_h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, TI *_x, TO *_y, unsigned int _index);

LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_RRRF, float,           float,          float)
LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_DECIM_DEFINE_API(DECIM_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)

// 
// Half-band resampler
//
#define RESAMP2_MANGLE_RRRF(name)   LIQUID_CONCAT(resamp2_rrrf,name)
#define RESAMP2_MANGLE_CRCF(name)   LIQUID_CONCAT(resamp2_crcf,name)
#define RESAMP2_MANGLE_CCCF(name)   LIQUID_CONCAT(resamp2_cccf,name)

#define LIQUID_RESAMP2_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _h_len); \
X() X(_recreate)(X() _q, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_decim_execute)(X() _f, TI * _x, TO * _y); \
void X(_interp_execute)(X() _f, TI _x, TO * _y);

LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_RRRF, float,           float,          float)
LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_RESAMP2_DEFINE_API(RESAMP2_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)


// 
// Arbitrary resampler
//
#define RESAMP_MANGLE_RRRF(name)    LIQUID_CONCAT(resamp_rrrf,name)
#define RESAMP_MANGLE_CRCF(name)    LIQUID_CONCAT(resamp_crcf,name)
#define RESAMP_MANGLE_CCCF(name)    LIQUID_CONCAT(resamp_cccf,name)

#define LIQUID_RESAMP_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(float _r); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q);

LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_RRRF, float,         float,          float)
LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_RESAMP_DEFINE_API(RESAMP_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)

// 
// Symbol timing recovery (symbol synchronizer)
//
#define SYMSYNC_MANGLE_RRRF(name)   LIQUID_CONCAT(symsync_rrrf,name)
#define SYMSYNC_MANGLE_CRCF(name)   LIQUID_CONCAT(symsync_crcf,name)
#define SYMSYNC_MANGLE_CCCF(name)   LIQUID_CONCAT(symsync_cccf,name)

#define LIQUID_SYMSYNC_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _k, unsigned int _num_filters, TC * _h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, TI * _x, unsigned int _nx, TO * _y, unsigned int *_ny); \
void X(_set_lf_bw)(X() _q, float _bt); \
void X(_clear)(X() _q); \
void X(_estimate_timing)(X() _q, TI * _x, unsigned int _n);

LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_RRRF, float,           float,          float)
LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CRCF, liquid_float_complex,   float,          liquid_float_complex)
LIQUID_SYMSYNC_DEFINE_API(SYMSYNC_MANGLE_CCCF, liquid_float_complex,   liquid_float_complex,  liquid_float_complex)

// 
// Symbol timing recovery (symbol synchronizer, 2 samples/symbol in/out)
//
#define SYMSYNC2_MANGLE_RRRF(name)  LIQUID_CONCAT(symsync2_rrrf,name)
#define SYMSYNC2_MANGLE_CRCF(name)  LIQUID_CONCAT(symsync2_crcf,name)
#define SYMSYNC2_MANGLE_CCCF(name)  LIQUID_CONCAT(symsync2_cccf,name)

#define LIQUID_SYMSYNC2_DEFINE_API(X,TO,TC,TI) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _num_filters, TC * _h, unsigned int _h_len); \
void X(_destroy)(X() _q); \
void X(_print)(X() _q); \
void X(_execute)(X() _q, TI * _x, unsigned int _nx, TO * _y, unsigned int *_ny); \
void X(_set_lf_bw)(X() _q, float _bt); \
void X(_clear)(X() _q); \
void X(_estimate_timing)(X() _q, TI * _x, unsigned int _n);

LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_RRRF, float,         float,          float)
LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_CRCF, liquid_float_complex, float,          liquid_float_complex)
LIQUID_SYMSYNC2_DEFINE_API(SYMSYNC2_MANGLE_CCCF, liquid_float_complex, liquid_float_complex,  liquid_float_complex)


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

#define LIQUID_CONCAT(prefix,name) prefix ## name

//
// Frame descriptor
//
typedef enum {
    FRAME_UNKNOWN=0,

    FRAME_SRC0,
    FRAME_SRC1,
    FRAME_DST0,
    FRAME_DST1,
    FRAME_MOD_SCHEME,
    FRAME_MOD_BPS,
    FRAME_FEC_INNER_SCHEME,
    FRAME_FEC_OUTER_SCHEME,
    FRAME_INTLV_INNER_SCHEME,
    FRAME_INTLV_OUTER_SCHEME,
    FRAME_PROTOCOL,
    FRAME_MSG_LENGTH,
    FRAME_NUM_SYMBOLS
} frame_keyid;

typedef struct frame_s * frame;

frame frame_create();
void frame_destroy(frame _f);
void frame_print(frame _f);

void frame_setkey(frame _f, frame_keyid _id, unsigned int _value);

void frame_encode(frame _f, unsigned char * _header, unsigned char *_out);
int  frame_decode(frame _f, unsigned char * _in, unsigned char * _header);

//
// Basic frame generator (64 bytes data payload)
//
typedef struct framegen64_s * framegen64;
framegen64 framegen64_create(
    unsigned int _m,
    float _beta);
void framegen64_destroy(framegen64 _fg);
void framegen64_print(framegen64 _fg);
void framegen64_execute(framegen64 _fg, unsigned char * _header, unsigned char * _payload, liquid_float_complex * _y);
void framegen64_flush(framegen64 _fg, unsigned int _n, liquid_float_complex * _y);

// Basic frame synchronizer (64 bytes data payload)
typedef int (*framesync64_callback)(
    unsigned char * _header,
    int _header_valid,
    unsigned char * _payload,
    int _payload_valid,
    void * _userdata);
typedef struct framesync64_s * framesync64;
framesync64 framesync64_create(
    unsigned int _m,
    float _beta,
    framesync64_callback _callback,
    void * _userdata
    );
void framesync64_destroy(framesync64 _fg);
void framesync64_print(framesync64 _fg);
void framesync64_reset(framesync64 _fg);
void framesync64_execute(framesync64 _fg, liquid_float_complex * _x, unsigned int _n);


//
// P/N synchronizer
//
#define PNSYNC_MANGLE_RRRF(name)    LIQUID_CONCAT(pnsync_rrrf,name)
#define PNSYNC_MANGLE_CRCF(name)    LIQUID_CONCAT(pnsync_crcf,name)
#define PNSYNC_MANGLE_CCCF(name)    LIQUID_CONCAT(pnsync_cccf,name)

// Macro:
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_PNSYNC_DEFINE_API(PNSYNC,TO,TC,TI)       \
typedef struct PNSYNC(_s) * PNSYNC();                   \
                                                        \
PNSYNC() PNSYNC(_create)(unsigned int _n, TC * _v);     \
PNSYNC() PNSYNC(_create_msequence)(unsigned int _g);    \
void PNSYNC(_destroy)(PNSYNC() _fs);                    \
void PNSYNC(_print)(PNSYNC() _fs);                      \
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
//   X  : name-mangling macro
//   TO : output data type
//   TC : coefficients data type
//   TI : input data type
#define LIQUID_BSYNC_DEFINE_API(BSYNC,TO,TC,TI)         \
typedef struct BSYNC(_s) * BSYNC();                     \
                                                        \
BSYNC() BSYNC(_create)(unsigned int _n, TC * _v);       \
BSYNC() BSYNC(_create_msequence)(unsigned int _g);      \
void BSYNC(_destroy)(BSYNC() _fs);                      \
void BSYNC(_print)(BSYNC() _fs);                        \
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
float kaiser(unsigned int _n, unsigned int _N, float _beta);

// Hamming window
float hamming(unsigned int _n, unsigned int _N);

// Hann window
float hann(unsigned int _n, unsigned int _N);

// Blackman-harris window
float blackmanharris(unsigned int _n, unsigned int _N);


//
// MODULE : matrix
//

#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(fmatrix, name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(cfmatrix, name)

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_MATRIX_DEFINE_API(X,T)                       \
void X(_print)(T * _X, unsigned int _R, unsigned int _C);   \
void X(_add)(unsigned int _R, unsigned int _C,              \
        T * _x, T * _y, T * _z);                            \
void X(_sub)(unsigned int _R, unsigned int _C,              \
        T * _x, T * _y, T * _z);                            \
void X(_mul)(T * _X, unsigned int _XR, unsigned int _XC,    \
             T * _Y, unsigned int _YR, unsigned int _YC,    \
             T * _Z, unsigned int _ZR, unsigned int _ZC);   \
void X(_div)(T * _X, unsigned int _XR, unsigned int _XC,    \
             T * _Y, unsigned int _YR, unsigned int _YC,    \
             T * _Z);                                       \
void X(_trans)(T * _X, unsigned int _XR, unsigned int _XC); \
void X(_inv)(T * _X, unsigned int _XR, unsigned int _XC);

#define matrix_access(X,R,C,r,c) ((X)[(r)*(C)+(c)])

LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_FLOAT, float)
LIQUID_MATRIX_DEFINE_API(MATRIX_MANGLE_CFLOAT, liquid_float_complex)

#if 0
typedef struct X(_s) * X();                                 \
X() X(_create)(unsigned int _M, unsigned int _N);           \
X() X(_copy)(X() _x);                                       \
void X(_destroy)(X() _x);                                   \
void X(_print)(X() _x);                                     \
void X(_clear)(X() _x);                                     \
void X(_dim)(X() _x, unsigned int *_M, unsigned int *_N);   \
void X(_assign)(X() _x, unsigned int _m, unsigned int _n,   \
    T _value);                                              \
void X(_access)(X() _x, unsigned int _m, unsigned int _n,   \
    T * _value);                                            \
void X(_add)(X() _x, X() _y, X() _z);                       \
void X(_lu_decompose)(X() _x, X() _lower, X() _upper);
#endif

//
// Polynomials
//

typedef struct polynomial_s * polynomial;

polynomial polynomial_create(unsigned int _order);

void polynomial_destroy(polynomial _p);


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
// Analog modems
//

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
                         int _nyquist,
                         int _type);
void firpfbch_destroy(firpfbch _c);
void firpfbch_print(firpfbch _c);

void firpfbch_execute(firpfbch _c, liquid_float_complex * _x, liquid_float_complex * _y);
//void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _X);
//void firpfbch_analyzer_execute(firpfbch _c, float complex * _X, float complex * _x);

// FIR OFDM/OQAM
typedef struct ofdmoqam_s * ofdmoqam;
#define OFDMOQAM_ANALYZER       FIRPFBCH_ANALYZER
#define OFDMOQAM_SYNTHESIZER    FIRPFBCH_SYNTHESIZER
ofdmoqam ofdmoqam_create(unsigned int _num_channels, unsigned int _m, int _type);
void ofdmoqam_destroy(ofdmoqam _c);
void ofdmoqam_print(ofdmoqam _c);
void ofdmoqam_execute(ofdmoqam _c, liquid_float_complex * _x, liquid_float_complex * _y);

// Discrete multi-tone (OFDM)
typedef struct ofdm_s * ofdm;
#define OFDM_ANALYZER           FIRPFBCH_ANALYZER
#define OFDM_SYNTHESIZER        FIRPFBCH_SYNTHESIZER
ofdm ofdm_create(unsigned int   _num_channels,
                 unsigned int   _cp_len,
                 unsigned int   _taper_len,
                 int            _type);
void ofdm_destroy(ofdm _c);
void ofdm_print(ofdm _c);
void ofdm_execute(ofdm _c, liquid_float_complex * _x, liquid_float_complex *_y);


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
void nco_mix_block_up(
    nco _nco,
    liquid_float_complex *_x,
    liquid_float_complex *_y,
    unsigned int _N);

void nco_mix_block_down(
    nco _nco,
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

// Gaussian random number generator, N(0,1)
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


//
// Data scrambler
//

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
void pack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int *num_written);

// unpacks bytes with 8 bits of information
void unpack_bytes(
    unsigned char * input,
    unsigned int input_length,
    unsigned char * output,
    unsigned int output_length,
    unsigned int *num_written);

// repacks bytes with arbitrary symbol sizes
void repack_bytes(
    unsigned char * input,
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
