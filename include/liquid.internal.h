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

//
// liquid.internal.h
//
// Internal header file for liquid DSP for SDR
//
// This file includes function declarations which are intended
// for internal use
//

#ifndef __LIQUID_INTERNAL_H__
#define __LIQUID_INTERNAL_H__

// Configuration file
#include "config.h"

#include <complex.h>
#include "liquid.h"

//
// Debugging macros
//
#define DEBUG_PRINTF_FLOAT(F,STR,I,V)                           \
    fprintf(F,"%s(%4u) = %12.4e;\n",STR,I+1,V)
#define DEBUG_PRINTF_CFLOAT(F,STR,I,V)                          \
    fprintf(F,"%s(%4u) = %12.4e +j*%12.4e;\n",STR,I+1,crealf(V),cimagf(V))

//
// MODULE : agc
//

// initialize agc object
void agc_init(agc _agc);


//
// MODULE : ann
//

// linear activation function
float ann_af_linear(float _mu, float _x);
float ann_df_linear(float _mu, float _x);

// logistic activation function
float ann_af_logistic(float _mu, float _x);
float ann_df_logistic(float _mu, float _x);

// logistic activation function
float ann_af_tanh(float _mu, float _x);
float ann_df_tanh(float _mu, float _x);

// mu-law activation function
float ann_af_mulaw(float _mu, float _x);
float ann_df_mulaw(float _mu, float _x);

// large macro (internal)
//   ANN    : name-mangling macro
//   T      : primitive data type
#define LIQUID_ANN_DEFINE_INTERNAL_API(ANN,T)                   \
void  ANN(_train_bp)(ANN() _q,                                  \
                  T * _x,                                       \
                  T * _y);

// Define ann APIs
LIQUID_ANN_DEFINE_INTERNAL_API(ANN_MANGLE_FLOAT, float)


#define NODE_MANGLE_FLOAT(name)  LIQUID_CONCAT(node, name)
// large macro (internal)
//   NODE   : name-mangling macro
//   T      : primitive data type
#define LIQUID_NODE_DEFINE_INTERNAL_API(NODE,T)                 \
                                                                \
typedef struct NODE(_s) * NODE();                               \
struct NODE(_s) {                                               \
    T *w, *x, *y;                                               \
    T v, *dw, delta;   \
    unsigned int num_inputs;                                    \
    T(*activation_func)(float,T);                               \
    T(*d_activation_func)(float,T);                             \
    float mu;                                                   \
};                                                              \
NODE() NODE(_create)(float * _w,                                \
                     float * _x,                                \
                     float * _y,                                \
                     unsigned int _num_inputs,                  \
                     int _activation_func,                      \
                     float _mu);                                \
void   NODE(_destroy)(NODE() _n);                               \
void   NODE(_print)(NODE() _n);                                 \
void   NODE(_evaluate)(NODE() _n);                              \
void   NODE(_train)(NODE() _n, T _d, float _eta);

// Define ann APIs
LIQUID_NODE_DEFINE_INTERNAL_API(NODE_MANGLE_FLOAT, float)


//
// MODULE : audio
//

// compute optimum bit allocation for sub-band coder
// _n           :   number of channels
// _e           :   energy array [_n x 1]
// _num_bits    :   total number of bits available
// _max_bits    :   maximum number of bits per channel
// _k           :   resulting bit allocation array [_n x 1]
void fbasc_compute_bit_allocation(unsigned int _n,
                                  float * _e,
                                  unsigned int _num_bits,
                                  unsigned int _max_bits,
                                  unsigned int * _k);

void fbasc_encode_frame(fbasc _q);
void fbasc_decode_frame(fbasc _q);

//
// MODULE : buffer
//

// Buffers

#define buffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER,T)             \
void BUFFER(_linearize)(BUFFER() _b);                           \
void BUFFER(_c_read)(BUFFER() _b, T ** _v, unsigned int *_n);   \
void BUFFER(_s_read)(BUFFER() _b, T ** _v, unsigned int *_n);   \
void BUFFER(_c_write)(BUFFER() _b, T * _v, unsigned int _n);    \
void BUFFER(_s_write)(BUFFER() _b, T * _v, unsigned int _n);    \
void BUFFER(_c_release)(BUFFER() _b, unsigned int _n);          \
void BUFFER(_s_release)(BUFFER() _b, unsigned int _n);          \
void BUFFER(_c_push)(BUFFER() _b, T _v);                        \
void BUFFER(_s_push)(BUFFER() _b, T _v);

LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_UINT, unsigned int)

// Windows

#define window_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW,T)             \
void WINDOW(_linearize)(WINDOW() _b);

LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_CFLOAT, float complex)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_UINT, unsigned int)

//
// MODULE : channel
//

struct awgn_channel_s {
    float nvar;
    float nstd;
};

struct ricek_channel_s {
    unsigned int h_len;
    float K;
    float omega;
    float sig;
    float s;
    float fd;
    float theta;
    fir_filter_cccf f;
};

struct channel_s {
    float K;        // Rice-K fading
    //float omega;    // Mean power (omega=1)
    float fd;       // Maximum doppler frequency
    float theta;    // Line-of-sight angle of arrival
    float std;      // log-normal shadowing std. dev.
    float n0;       // AWGN std. dev.

    fir_filter_cccf f_ricek;     // doppler filter (Rice-K fading)
    fir_filter_rrrf f_lognorm;   // doppler filter (Log-normal shadowing)

    // internal
    float s, sig;
};


//
// MODULE : fec (forward error-correction)
//

extern unsigned char c_ones_mod2[256];

// checksum / cyclic redundancy check (crc)

#define CRC32_POLY 0x04C11DB7
extern unsigned char crc32_tab[256];
unsigned char reverse_byte(unsigned char _x);
unsigned int reverse_uint32(unsigned int _x);

// fec : basic object
struct fec_s {
    fec_scheme scheme;
    //unsigned int dec_msg_len;
    //unsigned int enc_msg_len;
    float rate;

    // convolutional : internal memory structure
    unsigned int num_dec_bytes;
    unsigned int num_enc_bytes;
    unsigned char * enc_bits;
    void * vp;      // decoder object
    int * poly;     // polynomial
    unsigned int R; // primitive rate, inverted (e.g. R=3 for 1/3)
    unsigned int K; // constraint length
    unsigned int P; // puncturing rate (e.g. p=3 for 3/4)
    int * puncturing_matrix;

    // viterbi decoder function pointers
    void*(*create_viterbi)(int);
    //void (*set_viterbi_polynomial)(int*);
    int  (*init_viterbi)(void*,int);
    int  (*update_viterbi_blk)(void*,unsigned char*,int);
    int  (*chainback_viterbi)(void*,unsigned char*,unsigned int,unsigned int);
    void (*delete_viterbi)(void*);

    // encode function pointer
    void (*encode_func)(fec _q,
                        unsigned int _dec_msg_len,
                        unsigned char * _msg_dec,
                        unsigned char * _msg_enc);

    // decode function pointer
    void (*decode_func)(fec _q,
                        unsigned int _dec_msg_len,
                        unsigned char * _msg_enc,
                        unsigned char * _msg_dec);
};

// Pass
fec fec_pass_create(void *_opts);
void fec_pass_destroy(fec _q);
void fec_pass_print(fec _q);
void fec_pass_encode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_dec,
                     unsigned char * _msg_enc);
void fec_pass_decode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_enc,
                     unsigned char * _msg_dec);

// Repeat (3)
fec fec_rep3_create(void *_opts);
void fec_rep3_destroy(fec _q);
void fec_rep3_print(fec _q);
void fec_rep3_encode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_dec,
                     unsigned char * _msg_enc);
void fec_rep3_decode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_enc,
                     unsigned char * _msg_dec);

// Hamming(8,4)
unsigned char fec_hamming84_compute_syndrome(unsigned char _r);
unsigned char fec_hamming84_decode_symbol(unsigned char _r);

// Hamming(7,4)
unsigned char fec_hamming74_compute_syndrome(unsigned char _r);
fec fec_hamming74_create(void *_opts);
void fec_hamming74_destroy(fec _q);
void fec_hamming74_print(fec _q);
void fec_hamming74_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char * _msg_dec,
                          unsigned char * _msg_enc);
void fec_hamming74_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char * _msg_enc,
                          unsigned char * _msg_dec);

// Convolutional: r1/2 K=7
//                r1/2 K=9
//                r1/3 K=9
//                r1/6 K=15

// convolutional code polynomials
extern int fec_conv27_poly[2];
extern int fec_conv29_poly[2];
extern int fec_conv39_poly[3];
extern int fec_conv615_poly[6];

// convolutional code puncturing matrices  [R x P]
extern int fec_conv27p23_matrix[4];     // [2 x 2]
extern int fec_conv27p34_matrix[6];     // [2 x 3]
extern int fec_conv27p45_matrix[8];     // [2 x 4]
extern int fec_conv27p56_matrix[10];    // [2 x 5]
extern int fec_conv27p67_matrix[12];    // [2 x 6]
extern int fec_conv27p78_matrix[14];    // [2 x 7]

extern int fec_conv29p23_matrix[4];     // [2 x 2]
extern int fec_conv29p34_matrix[6];     // [2 x 3]
extern int fec_conv29p45_matrix[8];     // [2 x 4]
extern int fec_conv29p56_matrix[10];    // [2 x 5]
extern int fec_conv29p67_matrix[12];    // [2 x 6]
extern int fec_conv29p78_matrix[14];    // [2 x 7]

fec fec_conv_create(fec_scheme _fs);
void fec_conv_destroy(fec _q);
void fec_conv_print(fec _q);
void fec_conv_encode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_dec,
                     unsigned char * _msg_enc);
void fec_conv_decode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_enc,
                     unsigned char * _msg_dec);
void fec_conv_setlength(fec _q,
                        unsigned int _dec_msg_len);

// internal initialization methods (sets r, K, viterbi methods)
void fec_conv_init_v27(fec _q);
void fec_conv_init_v29(fec _q);
void fec_conv_init_v39(fec _q);
void fec_conv_init_v615(fec _q);

// punctured convolutional codes
fec fec_conv_punctured_create(fec_scheme _fs);
void fec_conv_punctured_destroy(fec _q);
void fec_conv_punctured_print(fec _q);
void fec_conv_punctured_encode(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char * _msg_dec,
                               unsigned char * _msg_enc);
void fec_conv_punctured_decode(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char * _msg_enc,
                               unsigned char * _msg_dec);
void fec_conv_punctured_setlength(fec _q,
                                  unsigned int _dec_msg_len);

// internal initialization methods (sets r, K, viterbi methods,
// and puncturing matrix)
void fec_conv_init_v27p23(fec _q);
void fec_conv_init_v27p34(fec _q);
void fec_conv_init_v27p45(fec _q);
void fec_conv_init_v27p56(fec _q);
void fec_conv_init_v27p67(fec _q);
void fec_conv_init_v27p78(fec _q);

void fec_conv_init_v29p23(fec _q);
void fec_conv_init_v29p34(fec _q);
void fec_conv_init_v29p45(fec _q);
void fec_conv_init_v29p56(fec _q);
void fec_conv_init_v29p67(fec _q);
void fec_conv_init_v29p78(fec _q);

//
// MODULE : fft (fast discrete Fourier transform)
//

// fft size below which twiddle factors
// are stored in look-up table (very fast)
#define FFT_SIZE_LUT    16

struct fftplan_s {
    unsigned int n;             // fft size
    float complex * twiddle;    // twiddle factors
    float complex * x;          // input array
    float complex * y;          // output array
    int direction;              // forward/reverse

    // radix-2
    int is_radix2;              // radix-2 flag
    unsigned int * index_rev;   // input indices (reversed)
    unsigned int m;             // log2(n)
};

// initialization
void fft_init_lut(fftplan _p);
void fft_init_radix2(fftplan _p);

// execution

// execute basic dft (slow, but guarantees
// correct output)
void fft_execute_dft(fftplan _p);

// execute basic dft using look-up table for
// twiddle factors (fast for small fft sizes)
void fft_execute_lut(fftplan _p);

// execute radix-2 fft
void fft_execute_radix2(fftplan _p);

// miscellaneous functions
unsigned int reverse_index(unsigned int _i, unsigned int _n);

//
// fft_shift
//
//void fft_shift_odd(float complex *_x, unsigned int _n);
//void fft_shift_even(float complex *_x, unsigned int _n);


//
// MODULE : filter
//

// fir_farrow
#define LIQUID_FIR_FARROW_DEFINE_INTERNAL_API(FIR_FARROW,TO,TC,TI)  \
void FIR_FARROW(_genpoly)(FIR_FARROW() _q);

LIQUID_FIR_FARROW_DEFINE_INTERNAL_API(FIR_FARROW_MANGLE_RRRF, float, float, float)
LIQUID_FIR_FARROW_DEFINE_INTERNAL_API(FIR_FARROW_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)


// qmfb
#define LIQUID_QMFB_DEFINE_INTERNAL_API(QMFB,TO,TC,TI)          \
void    QMFB(_analysis_execute)(QMFB() _q,                      \
                                TI   _x0,                       \
                                TI   _x1,                       \
                                TO * _y0,                       \
                                TO * _y1);                      \
void    QMFB(_synthesis_execute)(QMFB() _q,                     \
                                 TI   _y0,                      \
                                 TI   _y1,                      \
                                 TO * _x0,                      \
                                 TO * _x1);

LIQUID_QMFB_DEFINE_INTERNAL_API(QMFB_MANGLE_RRRF, float, float, float)
LIQUID_QMFB_DEFINE_INTERNAL_API(QMFB_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)


// itqmfb
#define LIQUID_ITQMFB_DEFINE_INTERNAL_API(ITQMFB,TO,TC,TI)      \
void    ITQMFB(_analysis_execute)(ITQMFB() _q,                  \
                                  TO * _x,                      \
                                  TO * _y);                     \
void    ITQMFB(_synthesis_execute)(ITQMFB() _q,                 \
                                   TO * _y,                     \
                                   TO * _x);

LIQUID_ITQMFB_DEFINE_INTERNAL_API(ITQMFB_MANGLE_RRRF, float, float, float)
LIQUID_ITQMFB_DEFINE_INTERNAL_API(ITQMFB_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)

// symsync
#define LIQUID_SYMSYNC_DEFINE_INTERNAL_API(SYMSYNC,TO,TC,TI)    \
void SYMSYNC(_step)(SYMSYNC() _q,                               \
                    TI _x,                                      \
                    TO *_y,                                     \
                    unsigned int *_ny);                         \
void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q,              \
                                     TO _mf,                    \
                                     TO _dmf);

LIQUID_SYMSYNC_DEFINE_INTERNAL_API(SYMSYNC_MANGLE_RRRF, float, float, float)
LIQUID_SYMSYNC_DEFINE_INTERNAL_API(SYMSYNC_MANGLE_CRCF, liquid_float_complex, float, liquid_float_complex)


// symsynclp
#define LIQUID_SYMSYNCLP_DEFINE_INTERNAL_API(SYMSYNCLP,TO,TC,TI)\
void SYMSYNCLP(_step)(SYMSYNCLP() _q,                           \
                      TI _x,                                    \
                      TO *_y,                                   \
                      unsigned int *_ny);                       \
void SYMSYNCLP(_advance_internal_loop)(SYMSYNCLP() _q,          \
                                       TO _mf,                  \
                                       TO _dmf);

LIQUID_SYMSYNCLP_DEFINE_INTERNAL_API(SYMSYNCLP_MANGLE_RRRF,
                                     float,
                                     float,
                                     float)
LIQUID_SYMSYNCLP_DEFINE_INTERNAL_API(SYMSYNCLP_MANGLE_CRCF,
                                     liquid_float_complex,
                                     float,
                                     liquid_float_complex)


//
// MODULE : framing
//

// flexframegen
void flexframegen_compute_payload_len(flexframegen _fg);
void flexframegen_compute_frame_len(flexframegen _fg);
void flexframegen_configure_payload_buffers(flexframegen _fg);
void flexframegen_encode_header(flexframegen _fg, unsigned char * _user_header);
void flexframegen_modulate_header(flexframegen _fg);
void flexframegen_modulate_payload(flexframegen _fg);
//void flexframegen_tmp_getheaderenc(flexframegen _fg, unsigned char * _header_enc);

void flexframesync_configure_payload_buffers(flexframesync _fs);
void flexframesync_demodulate_header(flexframesync _fs);
void flexframesync_decode_header(flexframesync _fs);
void flexframesync_assemble_payload(flexframesync _fs);
void flexframesync_open_bandwidth(flexframesync _fs);
void flexframesync_close_bandwidth(flexframesync _fs);
//void flexframesync_tmp_setheaderenc(flexframesync _fs, unsigned char * _header_enc);

// packetizer

struct fecintlv_plan {
    unsigned int dec_msg_len;
    unsigned int enc_msg_len;

    // fec codec
    fec_scheme fs;
    fec f;

    // interleaver
    unsigned int intlv_scheme;
    interleaver q;
};

struct packetizer_s {
    unsigned int msg_len;
    unsigned int packet_len;

    unsigned int crc32_key;

    struct fecintlv_plan * plan;
    unsigned int plan_len;

    // buffers
    unsigned int buffer_len;
    unsigned char * buffer_0;
    unsigned char * buffer_1;
};

// reallocate memory for buffers
void packetizer_realloc_buffers(packetizer _p, unsigned int _len);


//
// MODULE : interleaver
//

struct interleaver_s {
    unsigned int * p;   // byte permutation
    unsigned int len;   // number of bytes

    unsigned char * t;  // temporary buffer
};

//void interleaver_circshift_left(unsigned char *_x, unsigned int _n, unsigned int _s);
//void interleaver_circshift_right(unsigned char *_x, unsigned int _n, unsigned int _s);
void interleaver_compute_bit_permutation(interleaver _q, unsigned int *_p);


// 
// permutation functions
//
void interleaver_permute_forward(unsigned char * _x,
                                 unsigned int * _p,
                                 unsigned int _n);
void interleaver_permute_reverse(unsigned char * _x,
                                 unsigned int * _p,
                                 unsigned int _n);

void interleaver_permute_forward_mask(unsigned char * _x,
                                      unsigned int * _p,
                                      unsigned int _n,
                                      unsigned char _mask);
void interleaver_permute_reverse_mask(unsigned char * _x,
                                      unsigned int * _p,
                                      unsigned int _n,
                                      unsigned char _mask);

void interleaver_circshift_L4(unsigned char *_x, unsigned int _n);
void interleaver_circshift_R4(unsigned char *_x, unsigned int _n);



//
// MODULE : math
//

// complex rotation vector: cexpf(_Complex_I*THETA)
#define liquid_crotf_vect(THETA) (cosf(THETA) + _Complex_I*sinf(THETA))

// complex rotation by cexpf(_Complex_I*THETA)
#define liquid_crotf(X,THETA) ((X)*liquid_crotf_vect(THETA))

//
// MODULE : matrix
//

#define MATRIX_MAX_SIZE 1024

// fast access to matrix element, read/write
#define matrix_fast_access(X,r,c,R,C) ((X)[(r)*(C)+(c)])

#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(fmatrix, name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(cfmatrix, name)

// large macro
//   MATRIX : name-mangling macro
//   T      : data type
#define LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX,T)             \
void MATRIX(_gjelim)(T * _x,                                    \
                unsigned int _rx,                               \
                unsigned int _cx);                              \
void MATRIX(_pivot)(T * _x,                                     \
               unsigned int _rx,                                \
               unsigned int _cx,                                \
               unsigned int _r,                                 \
               unsigned int _c);                                \
void MATRIX(_swaprows)(T * _x,                                  \
                  unsigned int _rx,                             \
                  unsigned int _cx,                             \
                  unsigned int _r1,                             \
                  unsigned int _r2);

LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_FLOAT, float)
LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_CFLOAT, liquid_float_complex)

//
// MODULE : modem
//

// PSK
#define PSK_ALPHA       1

// 'Square' QAM
#define QAM4_ALPHA      1/sqrt(2)
#define QAM8_ALPHA      1/sqrt(6)
#define QAM16_ALPHA     1/sqrt(10)
#define QAM32_ALPHA     1/sqrt(20)
#define QAM64_ALPHA     1/sqrt(42)
#define QAM128_ALPHA    1/sqrt(82)
#define QAM256_ALPHA    1/sqrt(170)
#define QAM1024_ALPHA   1/sqrt(682)
#define QAM4096_ALPHA   1/sqrt(2730)

// Rectangular QAM
#define RQAM4_ALPHA     QAM4_ALPHA
#define RQAM8_ALPHA     QAM8_ALPHA
#define RQAM16_ALPHA    QAM16_ALPHA
#define RQAM32_ALPHA    1/sqrt(26)
#define RQAM64_ALPHA    QAM64_ALPHA
#define RQAM128_ALPHA   1/sqrt(106)
#define RQAM256_ALPHA   QAM256_ALPHA
#define RQAM512_ALPHA   1/sqrt(426)
#define RQAM1024_ALPHA  QAM1024_ALPHA
#define RQAM2048_ALPHA  1/sqrt(1706)
#define RQAM4096_ALPHA  QAM4096_ALPHA

// ASK
#define ASK2_ALPHA      1
#define ASK4_ALPHA      1/sqrt(5)
#define ASK8_ALPHA      1/sqrt(21)
#define ASK16_ALPHA     1/sqrt(85)
#define ASK32_ALPHA     1/sqrt(341)

/** \brief modem structure used for both modulation and demodulation 
 *
 * The modem structure implements a variety of common modulation schemes,
 * including (differential) phase-shift keying, and (quadrature) amplitude
 * modulation.
 *
 * While the same modem structure may be used for both modulation and
 * demodulation for most schemes, it is important to use separate objects
 * for differential-mode modems (e.g. MOD_DPSK) as the internal state
 * will change after each symbol.  It is usually good practice to keep
 * separate instances of modulators and demodulators.
 */
struct modem_s {
    modulation_scheme scheme;

    unsigned int m;     ///< bits per symbol
    unsigned int M;     ///< total symbols, \f$M=2^m\f$

    unsigned int m_i;   ///< bits per symbol, in-phase
    unsigned int M_i;   ///< total symbols, in-phase, \f$M_i=2^{m_i}\f$
    unsigned int m_q;   ///< bits per symbol, quadrature
    unsigned int M_q;   ///< total symbols, quadrature, \f$M_q=2^{m_q}\f$

    float alpha;        ///< scaling factor to ensure \f$E\{|\bar{r}|^2\}=1\f$

    /** \brief Reference vector for demodulating linear arrays
     *
     * By storing these values in an array they do not need to be
     * calculated during run-time.  This speeds up the demodulation by
     * approximately 8%.
     */
    float ref[MAX_MOD_BITS_PER_SYMBOL];

    /// Complete symbol map
    float complex * symbol_map;

    float complex state;        // received state vector
    float state_theta;          // received state vector, angle

    float complex res;          // residual error vector

    float phase_error;          // phase error after demodulation
    float evm;                  // error vector magnitude (EVM)

    float d_phi;

    // modulate function pointer
    void (*modulate_func)(modem _mod, unsigned int symbol_in, float complex *y);

    // demodulate function pointer
    void (*demodulate_func)(modem _demod, float complex x, unsigned int *symbol_out);
};


// generic modem create routines
modem modem_create_ask(unsigned int _bits_per_symbol);
modem modem_create_qam(unsigned int _bits_per_symbol);
modem modem_create_psk(unsigned int _bits_per_symbol);
modem modem_create_dpsk(unsigned int _bits_per_symbol);
modem modem_create_arb(unsigned int _bits_per_symbol);
modem modem_create_arb_mirrored(unsigned int _bits_per_symbol);
modem modem_create_arb_rotated(unsigned int _bits_per_symbol);

// specific modem create routines
modem modem_create_bpsk(void);
modem modem_create_qpsk(void);

/// Scale arbitrary modem energy to unity
void modem_arb_scale(modem _mod);

/// Balance I/Q
void modem_arb_balance_iq(modem _mod);

// generic modem modulate routines
void modem_modulate_ask(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_qam(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_psk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_dpsk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_arb(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_mirrored(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_rotated(modem _mod, unsigned int symbol_in, float complex *y);

// specific modem modulate routines
void modem_modulate_bpsk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_qpsk(modem _mod, unsigned int symbol_in, float complex *y);

// generic modem demodulate routines
void modem_demodulate_ask(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qam(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_psk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_dpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_arb(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_mirrored(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_rotated(modem _demod, float complex x, unsigned int *symbol_out);

// specific modem demodulate routines
void modem_demodulate_bpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qpsk(modem _demod, float complex x, unsigned int *symbol_out);

// get demodulator phase error
//void get_demodulator_phase_error(modem _demod, float* _phi);

// get error vector magnitude
//void get_demodulator_evm(modem _demod, float* _evm);

// demodulator helper functions

/** \brief Demodulates a linear symbol constellation using dynamic threshold calculation
 *
 * \param[in]   _v      value
 * \param[in]   _m      bits per symbol
 * \param[in]   _alpha  scaling factor
 * \param[out]  _s      demodulated symbol
 * \param[out]  _res    residual
 */
void modem_demodulate_linear_array(
    float _v,
    unsigned int _m,
    float _alpha,
    unsigned int *_s,
    float *_res);

/** \brief Demodulates a linear symbol constellation using refereneced lookup table
 *
 * \param[in]   _v      value
 * \param[in]   _m      bits per symbol
 * \param[in]   _ref    array of thresholds
 * \param[out]  _s      demodulated symbol
 * \param[out]  _res    residual
 */
void modem_demodulate_linear_array_ref(
    float _v,
    unsigned int _m,
    float *_ref,
    unsigned int *_s,
    float *_res);


//
// MODULE : multicarrier
//

void ofdmframesync_cpcorrelate(ofdmframesync _q);
void ofdmframesync_findrxypeak(ofdmframesync _q);
void ofdmframesync_rxpayload(ofdmframesync _q);

#define OFDMFRAME64_SCTYPE_NULL     0
#define OFDMFRAME64_SCTYPE_PILOT    1
#define OFDMFRAME64_SCTYPE_DATA     2
int ofdmframe64_getsctype(unsigned int _id);

// ofdmframe64sync
void ofdmframe64sync_debug_print(ofdmframe64sync _q);
void ofdmframe64sync_execute_plcpshort(ofdmframe64sync _q, float complex _x);
void ofdmframe64sync_execute_plcplong0(ofdmframe64sync _q, float complex _x);
void ofdmframe64sync_execute_plcplong1(ofdmframe64sync _q, float complex _x);
void ofdmframe64sync_estimate_cfo_plcplong(ofdmframe64sync _q);
void ofdmframe64sync_correct_cfo_plcplong(ofdmframe64sync _q);
void ofdmframe64sync_compute_plcplong0(ofdmframe64sync _q);
void ofdmframe64sync_compute_plcplong1(ofdmframe64sync _q);
void ofdmframe64sync_estimate_gain_plcplong(ofdmframe64sync _q);
void ofdmframe64sync_estimate_gain_plcplong_flat(ofdmframe64sync _q);
void ofdmframe64sync_execute_rxpayload(ofdmframe64sync _q, float complex _x);
extern const float complex ofdmframe64_plcp_Sf[64];
extern const float complex ofdmframe64_plcp_St[64];
extern const float complex ofdmframe64_plcp_Lf[64];
extern const float complex ofdmframe64_plcp_Lt[64];

// 
// MODULE : nco (numerically-controlled oscillator)
//


// Numerically-controlled oscillator, floating point phase precision
struct nco_s {
    liquid_ncotype type;
    float theta;        // NCO phase
    float d_theta;      // NCO frequency
    float sintab[256];  // sine table
    unsigned int index; // table index
    float sine;
    float cosine;
    void (*compute_sincos)(nco _q);
};

void nco_constrain_phase(nco _nco);

void nco_compute_sincos(nco _nco);
void vco_compute_sincos(nco _nco);

//
// MODULE : random
//
float complex icrandnf();

//
// MODULE : sequence
//

struct bsequence_s {
    unsigned char * s;          // sequence array, memory pointer
    unsigned int num_bits;      // number of bits in sequence
    unsigned int num_bits_msb;  // number of bits in most-significant block
    unsigned char bit_mask_msb; // bit mask for most-significant block
    unsigned int s_len;         // length of array, number of allocated blocks
};


struct msequence_s {
    unsigned int m;     // length generator polynomial, shift register
    unsigned int g;     // generator polynomial
    unsigned int a;     // initial shift register state, default: 1

    unsigned int n;     // length of sequence, \f$ n=2^m-1 \f$
    unsigned int v;     // shift register
    unsigned int b;     // return bit
};

// Default msequence generator objects
extern struct msequence_s msequence_default[13];

//
// Miscellaneous utilities
//

// number of 1's in byte
extern unsigned int c_ones[256];

// Count the number of ones in an integer
unsigned int count_ones_static(unsigned int _x); 

// Count the number of ones in an integer, inline insertion
#define count_ones_inline_uint2(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>>8)  & 0xFF ])

#define count_ones_inline_uint4(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>> 8) & 0xFF ] +        \
    c_ones[ ((x)>>16) & 0xFF ] +        \
    c_ones[ ((x)>>24) & 0xFF ])

#if SIZEOF_INT == 2
#  define count_ones(x) count_ones_inline_uint2(x)
#elif SIZEOF_INT == 4
#  define count_ones(x) count_ones_inline_uint4(x)
#endif

//#define count_ones(x) count_ones_static(x)


// number of leading zeros in byte
extern unsigned int leading_zeros[256];

// Count leading zeros in an integer
unsigned int count_leading_zeros(unsigned int _x); 

// Most-significant bit index
unsigned int msb_index(unsigned int _x);

#endif // __LIQUID_INTERNAL_H__
