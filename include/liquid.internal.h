/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
 *                                        Institute & State University
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

#define PRINTVAL_FLOAT(X,F)     printf(#F,crealf(X));
#define PRINTVAL_CFLOAT(X,F)    printf(#F "+j*" #F, crealf(X), cimagf(X));

//
// MODULE : agc
//

// 
#define LIQUID_AGC_DEFINE_INTERNAL_API(AGC,T,TC)                \
                                                                \
void AGC(_estimate_input_energy)(AGC() _q, TC _x);              \
void AGC(_estimate_gain_default)(AGC() _q, TC _x);              \
void AGC(_estimate_gain_log)(AGC() _q, TC _x);                  \
void AGC(_estimate_gain_exp)(AGC() _q, TC _x);                  \
void AGC(_limit_gain)(AGC() _q);                                \
                                                                \
/* squelch */                                                   \
void AGC(_update_auto_squelch)(AGC() _q, T _rssi);              \
void AGC(_execute_squelch)(AGC() _q);

LIQUID_AGC_DEFINE_INTERNAL_API(AGC_MANGLE_CRCF, float, liquid_float_complex)
LIQUID_AGC_DEFINE_INTERNAL_API(AGC_MANGLE_RRRF, float, float)


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

// erf (error function) activation function
float ann_af_erf(float _mu, float _x);
float ann_df_erf(float _mu, float _x);

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
    T * w;      /* weights */                                   \
    T * x;      /* input array */                               \
    T * y;      /* output array */                              \
    T v;        /* intermediate output */                       \
    T e;        /* output error */                              \
    T g;        /* activation function gradient */              \
    T delta;    /* local gradient */                            \
    T * dw;     /* weight correction */                         \
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
void   NODE(_compute_bp_error)(NODE() _n, T _error);            \
void   NODE(_train)(NODE() _n, float _eta);

// Define ann APIs
LIQUID_NODE_DEFINE_INTERNAL_API(NODE_MANGLE_FLOAT, float)


#define ANNLAYER_MANGLE_FLOAT(name)  LIQUID_CONCAT(annlayer, name)
// large macro (internal)
//   ANNLAYER   : name-mangling macro
//   T          : primitive data type
#define LIQUID_ANNLAYER_DEFINE_INTERNAL_API(ANNLAYER,T)         \
                                                                \
typedef struct ANNLAYER(_s) * ANNLAYER();                       \
                                                                \
struct ANNLAYER(_s) {                                           \
    unsigned int num_inputs;    /* number of inputs         */  \
    unsigned int num_nodes;     /* number of nodes          */  \
    node * nodes;               /* nodes in this layer      */  \
    int is_output_layer;        /* output layer flag        */  \
    int is_input_layer;         /* input layer flag         */  \
                                                                \
    /* back-propagation input error [num_inputs x 1]        */  \
    float * error;                                              \
                                                                \
};                                                              \
                                                                \
ANNLAYER() ANNLAYER(_create)(T * _w,                            \
                             T * _x,                            \
                             T * _y,                            \
                             unsigned int _num_inputs,          \
                             unsigned int _num_outputs,         \
                             int is_input_layer,                \
                             int is_output_layer,               \
                             int _activation_func,              \
                             T _mu);                            \
void ANNLAYER(_destroy)(annlayer _q);                           \
void ANNLAYER(_print)(annlayer _q);                             \
void ANNLAYER(_evaluate)(annlayer _q);                          \
void ANNLAYER(_compute_bp_error)(annlayer _q, T * _error);      \
void ANNLAYER(_train)(annlayer _q, T _eta);

// Define ann APIs
LIQUID_ANNLAYER_DEFINE_INTERNAL_API(ANNLAYER_MANGLE_FLOAT, float)

// maxnet
struct maxnet_s {
    ann * networks;
    unsigned int num_inputs;
    unsigned int num_classes;
};

//
// MODULE : audio
//

// compute normalized channel variance
void fbasc_encoder_compute_channel_variance(fbasc _q);

// computes optimal bit allocation based on channel variance
//
//  _num_channels   :   number of channels
//  _var            :   channel variance array [size: _num_channels x 1]
//  _num_bits       :   total number of bits per symbol
//  _max_bits       :   maximum number of bits per channel
//  _k              :   resulting bit allocation per channel [size: _num_channels x 1]
void fbasc_compute_bit_allocation(unsigned int _n,
                                  float * _e,
                                  unsigned int _num_bits,
                                  unsigned int _max_bits,
                                  unsigned int * _k);

// compute normalized channel energy, nominal gain, etc.
void fbasc_encoder_compute_metrics(fbasc _q);
void fbasc_decoder_compute_metrics(fbasc _q);

// run analyzer/synthesizer
void fbasc_encoder_run_analyzer(fbasc _q, float * _x, float * _X);
void fbasc_decoder_run_synthesizer(fbasc _q, float * _X, float * _x);

// quantize/de-quantize channelized data
void fbasc_encoder_quantize_samples(fbasc _q);
void fbasc_decoder_dequantize_samples(fbasc _q);

// pack/unpack header
void fbasc_encoder_pack_header(fbasc _q, unsigned char * _header);
void fbasc_decoder_unpack_header(fbasc _q, unsigned char * _header);

// pack/unpack frame
void fbasc_encoder_pack_frame(fbasc _q, unsigned char * _frame);
void fbasc_decoder_unpack_frame(fbasc _q, unsigned char * _frame);

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

LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_FLOAT,  float)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_CFLOAT, float complex)
//LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_UINT,   unsigned int)

// Windows

#define window_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW,T)             \
void WINDOW(_linearize)(WINDOW() _b);

LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_FLOAT,  float)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_CFLOAT, float complex)
//LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_UINT,   unsigned int)

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
    firfilt_cccf f;
};

struct lognorm_channel_s {
    unsigned int h_len;
    float sig;
    float fd;
    firfilt_rrrf f;
};

struct channel_s {
    float K;        // Rice-K fading
    //float omega;    // Mean power (omega=1)
    float fd;       // Maximum doppler frequency
    float theta;    // Line-of-sight angle of arrival
    float std;      // log-normal shadowing std. dev.
    float n0;       // AWGN std. dev.

    firfilt_cccf f_ricek;     // doppler filter (Rice-K fading)
    firfilt_rrrf f_lognorm;   // doppler filter (Log-normal shadowing)

    // internal
    float s, sig;
};

//
// MODULE : dotprod
//

// large macro
//   DOTPROD    : name-mangling macro
//   TO         : output data type
//   TC         : coefficients data type
//   TI         : input data type
#define LIQUID_DOTPROD_DEFINE_INTERNAL_API(DOTPROD,TO,TC,TI)    \
                                                                \
/* execute dotprod 4 inputs at a time */                        \
void DOTPROD(_run4)(TC *_h, TI *_x, unsigned int _n, TO *_y);

LIQUID_DOTPROD_DEFINE_INTERNAL_API(DOTPROD_MANGLE_RRRF,
                                   float,
                                   float,
                                   float)

LIQUID_DOTPROD_DEFINE_INTERNAL_API(DOTPROD_MANGLE_CCCF,
                                   liquid_float_complex,
                                   liquid_float_complex,
                                   liquid_float_complex)

LIQUID_DOTPROD_DEFINE_INTERNAL_API(DOTPROD_MANGLE_CRCF,
                                   liquid_float_complex,
                                   float,
                                   liquid_float_complex)



//
// MODULE : fec (forward error-correction)
//

// define soft bit values
#define FEC_SOFTBIT_0       (0)
#define FEC_SOFTBIT_1       (255)
#define FEC_SOFTBIT_ERASURE (127)

// checksum / cyclic redundancy check (crc)

#define CRC8_POLY 0x07
unsigned char reverse_byte(unsigned char _x);

#define CRC24_POLY 0x5D6DCB
unsigned int reverse_uint24(unsigned int _x);

#define CRC16_POLY 0x8005
unsigned int reverse_uint16(unsigned int _x);

#define CRC32_POLY 0x04C11DB7
unsigned int reverse_uint32(unsigned int _x);

unsigned int checksum_generate_key(unsigned char * _msg, unsigned int _msg_len);
unsigned int crc8_generate_key(unsigned char * _msg, unsigned int _msg_len);
unsigned int crc16_generate_key(unsigned char * _msg, unsigned int _msg_len);
unsigned int crc24_generate_key(unsigned char * _msg, unsigned int _msg_len);
unsigned int crc32_generate_key(unsigned char * _msg, unsigned int _msg_len);


// fec : basic object
struct fec_s {
    // common
    fec_scheme scheme;
    //unsigned int dec_msg_len;
    //unsigned int enc_msg_len;
    float rate;

    // lengths: convolutional, Reed-Solomon
    unsigned int num_dec_bytes;
    unsigned int num_enc_bytes;

    // convolutional : internal memory structure
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

    // Reed-Solomon
    int symsize;    // symbol size (bits per symbol)
    int genpoly;    // generator polynomial
    int fcs;        //
    int prim;       //
    int nroots;     // number of roots in the polynomial
    //int ntrials;    //
    unsigned int rspad; // number of implicit padded symbols
    int nn;         // 2^symsize - 1
    int kk;         // nn - nroots
    void * rs;      // Reed-Solomon internal object

    // Reed-Solomon decoder
    unsigned int num_blocks;    // number of blocks: ceil(dec_msg_len / nn)
    unsigned int dec_block_len; // number of decoded bytes per block: 
    unsigned int enc_block_len; // number of encoded bytes per block: 
    unsigned int res_block_len; // residual bytes in last block
    unsigned int pad;           // padding for each block
    unsigned char * tblock;     // decoder input sequence [size: 1 x n]
    int * errlocs;              // error locations [size: 1 x n]
    int * derrlocs;             // decoded error locations [size: 1 x n]
    int erasures;               // number of erasures

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

// Repeat (5)
fec fec_rep5_create(void *_opts);
void fec_rep5_destroy(fec _q);
void fec_rep5_print(fec _q);
void fec_rep5_encode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_dec,
                     unsigned char * _msg_enc);
void fec_rep5_decode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char * _msg_enc,
                     unsigned char * _msg_dec);

// Hamming(7,4)
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

// Hamming(8,4)
fec fec_hamming84_create(void *_opts);
void fec_hamming84_destroy(fec _q);
void fec_hamming84_print(fec _q);
void fec_hamming84_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char * _msg_dec,
                          unsigned char * _msg_enc);
void fec_hamming84_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char * _msg_enc,
                          unsigned char * _msg_dec);

// Hamming(12,8)

unsigned int fec_hamming128_encode_symbol(unsigned int _sym_dec);
unsigned int fec_hamming128_decode_symbol(unsigned int _sym_enc);

fec fec_hamming128_create(void *_opts);
void fec_hamming128_destroy(fec _q);
void fec_hamming128_print(fec _q);
void fec_hamming128_encode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char * _msg_dec,
                           unsigned char * _msg_enc);
void fec_hamming128_decode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char * _msg_enc,
                           unsigned char * _msg_dec);

// Convolutional: r1/2 K=7
//                r1/2 K=9
//                r1/3 K=9
//                r1/6 K=15

// compute encoded message length for convolutional codes
//  _dec_msg_len    :   decoded message length
//  _K              :   constraint length
//  _p              :   puncturing rate, r = _p / (_p+1)
unsigned int fec_conv_get_enc_msg_len(unsigned int _dec_msg_len,
                                      unsigned int _K,
                                      unsigned int _p);

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

// Reed-Solomon

// compute encoded message length for Reed-Solomon codes
//  _dec_msg_len    :   decoded message length
//  _nroots         :   number of roots in polynomial
//  _nn             :   
//  _kk             :   
unsigned int fec_rs_get_enc_msg_len(unsigned int _dec_msg_len,
                                    unsigned int _nroots,
                                    unsigned int _nn,
                                    unsigned int _kk);


fec fec_rs_create(fec_scheme _fs);
void fec_rs_init_p8(fec _q);
void fec_rs_setlength(fec _q,
                      unsigned int _dec_msg_len);
void fec_rs_encode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char * _msg_dec,
                   unsigned char * _msg_enc);
void fec_rs_decode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char * _msg_enc,
                   unsigned char * _msg_dec);


//
// MODULE : fft (fast discrete Fourier transform)
//

// fft size below which twiddle factors
// are stored in look-up table (very fast)
#define FFT_SIZE_LUT    16

typedef enum {
    LIQUID_FFT_DFT_1D   = 0,    // complex one-dimensional FFT

    // discrete cosine transforms
    LIQUID_FFT_REDFT00,         // real one-dimensional DCT-I
    LIQUID_FFT_REDFT10,         // real one-dimensional DCT-II
    LIQUID_FFT_REDFT01,         // real one-dimensional DCT-III
    LIQUID_FFT_REDFT11,         // real one-dimensional DCT-IV

    // discrete sine transforms
    LIQUID_FFT_RODFT00,         // real one-dimensional DST-I
    LIQUID_FFT_RODFT10,         // real one-dimensional DST-II
    LIQUID_FFT_RODFT01,         // real one-dimensional DST-III
    LIQUID_FFT_RODFT11,         // real one-dimensional DST-IV
} liquid_fft_kind;

// Macro    :   FFT (internal)
//  FFT     :   name-mangling macro
//  T       :   primitive data type
//  TC      :   primitive data type (complex)
#define LIQUID_FFT_DEFINE_INTERNAL_API(FFT,T,TC)                \
struct FFT(plan_s) {                                            \
    unsigned int n;             /* fft size */                  \
    TC * twiddle;               /* twiddle factors */           \
    TC * x;                     /* input array */               \
    TC * y;                     /* output array */              \
    int direction;              /* forward/reverse */           \
    int flags;                                                  \
    liquid_fft_kind kind;                                       \
                                                                \
    /* real even/odd DFTs parameters */                         \
    T * xr;                     /* input array (real) */        \
    T * yr;                     /* output array (real) */       \
    TC * xc;                    /* allocated input array */     \
    TC * yc;                    /* allocated output array */    \
                                                                \
    /* radix-2 implementation data */                           \
    int is_radix2;              /* radix-2 flag */              \
    unsigned int * index_rev;   /* input indices (reversed) */  \
    unsigned int m;             /* log2(n) */                   \
    void (*execute)(FFT(plan)); /* function pointer */          \
};                                                              \
                                                                \
/* initialization */                                            \
void FFT(_init_lut)(FFT(plan) _p);                              \
void FFT(_init_radix2)(FFT(plan) _p);                           \
                                                                \
/* execute basic dft (slow, but guarantees correct output) */   \
void FFT(_execute_dft)(FFT(plan) _p);                           \
                                                                \
/* execute basic dft using look-up table for twiddle factors */ \
/* (fast for small fft sizes) */                                \
void FFT(_execute_lut)(FFT(plan) _p);                           \
                                                                \
/* execute radix-2 fft */                                       \
void FFT(_execute_radix2)(FFT(plan) _p);                        \
                                                                \
/* discrete cosine transform (DCT) prototypes */                \
void FFT(_execute_REDFT00)(FFT(plan) _p);   /* DCT-I   */       \
void FFT(_execute_REDFT10)(FFT(plan) _p);   /* DCT-II  */       \
void FFT(_execute_REDFT01)(FFT(plan) _p);   /* DCT-III */       \
void FFT(_execute_REDFT11)(FFT(plan) _p);   /* DCT-IV  */       \
                                                                \
/* discrete sine transform (DST) prototypes */                  \
void FFT(_execute_RODFT00)(FFT(plan) _p);   /* DST-I   */       \
void FFT(_execute_RODFT10)(FFT(plan) _p);   /* DST-II  */       \
void FFT(_execute_RODFT01)(FFT(plan) _p);   /* DST-III */       \
void FFT(_execute_RODFT11)(FFT(plan) _p);   /* DST-IV  */

// miscellaneous functions
unsigned int reverse_index(unsigned int _i, unsigned int _n);

//
// fft_shift
//
//void fft_shift_odd(float complex *_x, unsigned int _n);
//void fft_shift_even(float complex *_x, unsigned int _n);

LIQUID_FFT_DEFINE_INTERNAL_API(LIQUID_FFT_MANGLE_FLOAT, float, liquid_float_complex)

// Use fftw library if installed, otherwise use internal (less
// efficient) fft library.
#if HAVE_FFTW3_H
#   include <fftw3.h>
#   define FFT_PLAN             fftwf_plan
#   define FFT_CREATE_PLAN      fftwf_plan_dft_1d
#   define FFT_DESTROY_PLAN     fftwf_destroy_plan
#   define FFT_EXECUTE          fftwf_execute
#   define FFT_DIR_FORWARD      FFTW_FORWARD
#   define FFT_DIR_BACKWARD     FFTW_BACKWARD
#   define FFT_METHOD           FFTW_ESTIMATE
#else
#   define FFT_PLAN             fftplan
#   define FFT_CREATE_PLAN      fft_create_plan
#   define FFT_DESTROY_PLAN     fft_destroy_plan
#   define FFT_EXECUTE          fft_execute
#   define FFT_DIR_FORWARD      FFT_FORWARD
#   define FFT_DIR_BACKWARD     FFT_REVERSE
#   define FFT_METHOD           0
#endif



//
// MODULE : filter
//

// esimate required filter length given transition bandwidth and
// stop-band attenuation (algorithm from [Vaidyanathan:1993])
//  _df     :   transition bandwidth (0 < _df < 0.5)
//  _As     :   stop-band attenuation [dB] (As > 0)
float estimate_req_filter_len_Kaiser(float _df,
                                     float _As);

// esimate required filter length given transition bandwidth and
// stop-band attenuation (algorithm from [Herrmann:1973])
//  _df     :   transition bandwidth (0 < _df < 0.5)
//  _As     :   stop-band attenuation [dB] (As > 0)
float estimate_req_filter_len_Herrmann(float _df,
                                       float _As);

// Direct digital [up/down] synthesizer
//#define LIQUID_DDS_DEFINE_INTERNAL_API(DDS,T)

//LIQUID_DDS_DEFINE_INTERNAL_API(DDS_MANGLE_CCCF, liquid_float_complex)


// fir_farrow
#define LIQUID_FIRFARROW_DEFINE_INTERNAL_API(FIRFARROW,TO,TC,TI)  \
void FIRFARROW(_genpoly)(FIRFARROW() _q);

LIQUID_FIRFARROW_DEFINE_INTERNAL_API(FIRFARROW_MANGLE_RRRF,
                                     float,
                                     float,
                                     float)

LIQUID_FIRFARROW_DEFINE_INTERNAL_API(FIRFARROW_MANGLE_CRCF,
                                     liquid_float_complex,
                                     float,
                                     liquid_float_complex)


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


// iirqmfb
#define LIQUID_IIRQMFB_DEFINE_INTERNAL_API(IIRQMFB,TO,TC,TI)    \
void IIRQMFB(_analysis_execute)(IIRQMFB() _q,                   \
                                TI   _x0,                       \
                                TI   _x1,                       \
                                TO * _y0,                       \
                                TO * _y1);                      \
void IIRQMFB(_synthesis_execute)(IIRQMFB() _q,                  \
                                 TI   _y0,                      \
                                 TI   _y1,                      \
                                 TO * _x0,                      \
                                 TO * _x1);

LIQUID_IIRQMFB_DEFINE_INTERNAL_API(IIRQMFB_MANGLE_RRRF,
                                   float,
                                   float,
                                   float)
LIQUID_IIRQMFB_DEFINE_INTERNAL_API(IIRQMFB_MANGLE_CRCF,
                                   liquid_float_complex,
                                   float,
                                   liquid_float_complex)


// 
// iirfiltsos : infinite impulse respone filter (second-order sections)
//
#define IIRFILTSOS_MANGLE_RRRF(name)  LIQUID_CONCAT(iirfiltsos_rrrf,name)
#define IIRFILTSOS_MANGLE_CRCF(name)  LIQUID_CONCAT(iirfiltsos_crcf,name)
#define IIRFILTSOS_MANGLE_CCCF(name)  LIQUID_CONCAT(iirfiltsos_cccf,name)

#define LIQUID_IIRFILTSOS_DEFINE_INTERNAL_API(IIRFILTSOS,TO,TC,TI)  \
typedef struct IIRFILTSOS(_s) * IIRFILTSOS();                   \
                                                                \
/* filter structure */                                          \
struct IIRFILTSOS(_s) {                                         \
    TC b[3];    /* feed-forward coefficients                */  \
    TC a[3];    /* feed-back coefficients                   */  \
                                                                \
    /* internal buffering                                   */  \
    TI x[3];    /* Direct form I  buffer (input)            */  \
    TO y[3];    /* Direct form I  buffer (output)           */  \
    TO v[3];    /* Direct form II buffer                    */  \
};                                                              \
                                                                \
IIRFILTSOS() IIRFILTSOS(_create)(TC * _b,                       \
                                 TC * _a);                      \
void IIRFILTSOS(_set_coefficients)(IIRFILTSOS() _q,             \
                                   TC * _b,                     \
                                   TC * _a);                    \
void IIRFILTSOS(_destroy)(IIRFILTSOS() _q);                     \
void IIRFILTSOS(_print)(IIRFILTSOS() _q);                       \
void IIRFILTSOS(_clear)(IIRFILTSOS() _q);                       \
void IIRFILTSOS(_execute)(IIRFILTSOS() _q,                      \
                          TI   _x,                              \
                          TO * _y);                             \
void IIRFILTSOS(_execute_df1)(IIRFILTSOS() _q,                  \
                              TI   _x,                          \
                              TO * _y);                         \
void IIRFILTSOS(_execute_df2)(IIRFILTSOS() _q,                  \
                              TI   _x,                          \
                              TO * _y);                         \
float IIRFILTSOS(_groupdelay)(IIRFILTSOS() _q, float _fc);

LIQUID_IIRFILTSOS_DEFINE_INTERNAL_API(IIRFILTSOS_MANGLE_RRRF,
                                      float,
                                      float,
                                      float)
LIQUID_IIRFILTSOS_DEFINE_INTERNAL_API(IIRFILTSOS_MANGLE_CRCF,
                                      liquid_float_complex,
                                      float,
                                      liquid_float_complex)
LIQUID_IIRFILTSOS_DEFINE_INTERNAL_API(IIRFILTSOS_MANGLE_CCCF,
                                      liquid_float_complex,
                                      liquid_float_complex,
                                      liquid_float_complex)


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

// firdes : finite impulse response filter design

// Find approximate bandwidth adjustment factor rho based on
// filter delay and desired excess bandwdith factor.
//
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
float rkaiser_approximate_rho(unsigned int _m,
                              float _beta);

// design_rkaiser_filter_internal()
//
// Design frequency-shifted root-Nyquist filter based on
// the Kaiser-windowed sinc.
//
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _h      :   resulting filter [size: 2*_k*_m+1]
//  _rho    :   transition bandwidth adjustment, 0 < _rho < 1
void design_rkaiser_filter_internal(unsigned int _k,
                                    unsigned int _m,
                                    float _beta,
                                    float _dt,
                                    float * _h,
                                    float * _rho);

// compute filter coefficients and determine resulting ISI
//  
//  _k      :   filter over-sampling rate (samples/symbol)
//  _m      :   filter delay (symbols)
//  _beta   :   filter excess bandwidth factor (0,1)
//  _dt     :   filter fractional sample delay
//  _rho    :   transition bandwidth adjustment, 0 < _rho < 1
//  _h      :   filter buffer [size: 2*_k*_m+1]
float design_rkaiser_filter_internal_isi(unsigned int _k,
                                         unsigned int _m,
                                         float _beta,
                                         float _dt,
                                         float _rho,
                                         float * _h);

// initialize the frequency grid on the disjoint bounded set
void firdespm_init_grid(firdespm _q);

// compute interpolating polynomial
void firdespm_compute_interp(firdespm _q);

// compute error signal from actual response (interpolator
// output), desired response, and weights
void firdespm_compute_error(firdespm _q);

// search error curve for _r+1 extremal indices
void firdespm_iext_search(firdespm _q);

// evaluates result to determine if Remez exchange algorithm
// has converged
int firdespm_is_search_complete(firdespm _q);

// compute filter taps (coefficients) from result
void firdespm_compute_taps(firdespm _q, float * _h);

// iirdes : infinite impulse response filter design

// Sorts array _z of complex numbers into complex conjugate pairs to
// within a tolerance. Conjugate pairs are ordered by increasing real
// component with the negative imaginary element first. All pure-real
// elements are placed at the end of the array.
//
// Example:
//      v:              liquid_cplxpair(v):
//      10 + j*3        -3 - j*4
//       5 + j*0         3 + j*4
//      -3 + j*4        10 - j*3
//      10 - j*3        10 + j*3
//       3 + j*0         3 + j*0
//      -3 + j*4         5 + j*0
//
//  _z      :   complex array (size _n)
//  _n      :   number of elements in _z
//  _tol    :   tolerance for finding complex pairs
//  _p      :   resulting pairs, pure real values of _z at end
void liquid_cplxpair(float complex * _z,
                     unsigned int _n,
                     float _tol,
                     float complex * _p);

// post-process cleanup used with liquid_cplxpair
//
// once pairs have been identified and ordered, this method
// will clean up the result by ensuring the following:
//  * pairs are perfect conjugates
//  * pairs have negative imaginary component first
//  * pairs are ordered by increasing real component
//  * pure-real elements are ordered by increasing value
//
//  _p          :   pre-processed complex array [size: _n x 1]
//  _n          :   array length
//  _num_pairs  :   number of complex conjugate pairs
void liquid_cplxpair_cleanup(float complex * _p,
                             unsigned int _n,
                             unsigned int _num_pairs);

// Jacobian elliptic functions (src/filter/src/ellip.c)

// Landen transformation (_n iterations)
void landenf(float _k,
             unsigned int _n,
             float * _v);

// compute elliptic integral K(k) for _n recursions
void ellipkf(float _k,
             unsigned int _n,
             float * _K,
             float * _Kp);

// elliptic degree
float ellipdegf(float _N,
                float _k1,
                unsigned int _n);

// elliptic cd() function (_n recursions)
float complex ellip_cdf(float complex _u,
                        float _k,
                        unsigned int _n);

// elliptic inverse cd() function (_n recursions)
float complex ellip_acdf(float complex _u,
                         float _k,
                         unsigned int _n);

// elliptic sn() function (_n recursions)
float complex ellip_snf(float complex _u,
                        float _k,
                        unsigned int _n);

// elliptic inverse sn() function (_n recursions)
float complex ellip_asnf(float complex _u,
                         float _k,
                         unsigned int _n);

//
// MODULE : framing
//

// framegen64

// convert one 8-bit byte to four 2-bit symbols
//  _byte   :   input byte
//  _syms   :   output symbols [size: 4 x 1]
void framegen64_byte_to_syms(unsigned char _byte,
                             unsigned char * _syms);

// framesync64

void framesync64_open_bandwidth(framesync64 _fs);
void framesync64_close_bandwidth(framesync64 _fs);
void framesync64_decode_header(framesync64 _fs);
void framesync64_decode_payload(framesync64 _fs);

// convert four 2-bit symbols into one 8-bit byte
//  _syms   :   input symbols [size: 4 x 1]
//  _byte   :   output byte
void framesync64_syms_to_byte(unsigned char * _syms,
                              unsigned char * _byte);

// execute methods
void framesync64_execute_seekpn(    framesync64 _fs, float complex _x, unsigned int _sym);
void framesync64_execute_rxheader(  framesync64 _fs, float complex _x, unsigned int _sym);
void framesync64_execute_rxpayload( framesync64 _fs, float complex _x, unsigned int _sym);
void framesync64_execute_reset(     framesync64 _fs, float complex _x, unsigned int _sym);

// advanced mode
void framesync64_csma_lock(framesync64 _fs);
void framesync64_csma_unlock(framesync64 _fs);


// flexframegen
void flexframegen_compute_payload_len(flexframegen _fg);
void flexframegen_compute_frame_len(flexframegen _fg);
void flexframegen_configure_payload_buffers(flexframegen _fg);
void flexframegen_encode_header(flexframegen _fg);
void flexframegen_modulate_header(flexframegen _fg);
void flexframegen_modulate_payload(flexframegen _fg);
//void flexframegen_tmp_getheaderenc(flexframegen _fg, unsigned char * _header_enc);

// flexframesync
void flexframesync_configure_payload_buffers(flexframesync _fs);
void flexframesync_decode_header(flexframesync _fs);
void flexframesync_assemble_payload(flexframesync _fs);
void flexframesync_open_bandwidth(flexframesync _fs);
void flexframesync_close_bandwidth(flexframesync _fs);
void flexframesync_train_eq(flexframesync _fs);

// 
void flexframesync_execute_seekpn(   flexframesync _fs, float complex _x, unsigned int _sym);
void flexframesync_execute_rxheader( flexframesync _fs, float complex _x, unsigned int _sym);
void flexframesync_execute_rxpayload(flexframesync _fs, float complex _x, unsigned int _sym);
void flexframesync_execute_reset(    flexframesync _fs, float complex _x, unsigned int _sym);

// advanced mode
void flexframesync_csma_lock(flexframesync _fs);
void flexframesync_csma_unlock(flexframesync _fs);

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

    crc_scheme check;
    unsigned int crc_length;

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
// bpacket
//

#define BPACKET_VERSION 100

// generator
void bpacketgen_compute_packet_len(bpacketgen _q);
void bpacketgen_assemble_pnsequence(bpacketgen _q);
void bpacketgen_assemble_header(bpacketgen _q);

// synchronizer
void bpacketsync_assemble_pnsequence(bpacketsync _q);
void bpacketsync_execute_bit(bpacketsync _q, unsigned char _bit);
void bpacketsync_execute_seekpn(bpacketsync _q, unsigned char _bit);
void bpacketsync_execute_rxheader(bpacketsync _q, unsigned char _bit);
void bpacketsync_execute_rxpayload(bpacketsync _q, unsigned char _bit);
void bpacketsync_decode_header(bpacketsync _q);
void bpacketsync_decode_payload(bpacketsync _q);
void bpacketsync_reconfig(bpacketsync _q);

//
// MODULE : interleaver
//

// structured interleaver object
struct interleaver_s {
    unsigned int * p;   // byte permutation
    unsigned int len;   // number of bytes

    unsigned char * t;  // temporary buffer

    // number of iterations (permutations) beyond
    // initial block interleaving
    unsigned int num_iterations;
};

#define LIQUID_INTERLEAVER_NUM_MASKS (4)
extern const unsigned char interleaver_mask[LIQUID_INTERLEAVER_NUM_MASKS];

// initialize block interleaver
void interleaver_init_block(interleaver _q);

// initialize m-sequence interleaver
void interleaver_init_sequence(interleaver _q);

// compute bit permutation for interleaver
//  _q      :   interleaver object
//  _p      :   output permutation index array, [size: 8*_n x 1]
void interleaver_compute_bit_permutation(interleaver _q,
                                         unsigned int *_p);


// 
// permutation functions
//

// permute forward one iteration
//  _x      :   input/output data array, [size: _n x 1]
//  _p      :   permutation index array, [size: _n x 1]
//  _n      :   array size
void interleaver_permute_forward(unsigned char * _x,
                                 unsigned int * _p,
                                 unsigned int _n);

// permute reverse one iteration
//  _x      :   input/output data array, [size: _n x 1]
//  _p      :   permutation index array, [size: _n x 1]
//  _n      :   array size
void interleaver_permute_reverse(unsigned char * _x,
                                 unsigned int * _p,
                                 unsigned int _n);

// permute forward one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _p      :   permutation index array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_forward_mask(unsigned char * _x,
                                      unsigned int * _p,
                                      unsigned int _n,
                                      unsigned char _mask);

// permute reverse one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _p      :   permutation index array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_reverse_mask(unsigned char * _x,
                                      unsigned int * _p,
                                      unsigned int _n,
                                      unsigned char _mask);



//
// MODULE : math
//

// internal trig helper functions

// complex rotation vector: cexpf(_Complex_I*THETA)
#define liquid_cexpjf(THETA) (cosf(THETA) + _Complex_I*sinf(THETA))

// polynomials
#define LIQUID_POLY_DEFINE_INTERNAL_API(POLY,T,TC)          \
void POLY(_findroots_durandkerner)(T * _p,                  \
                                   unsigned int _k,         \
                                   TC * _roots);            \
void POLY(_findroots_bairstow)(T * _p,                      \
                               unsigned int _k,             \
                               TC * _roots);                \
void POLY(_findroots_bairstow_recursion)(T * _p,            \
                                         unsigned int _k,   \
                                         T * _p1,           \
                                         T * _u,            \
                                         T * _v);

LIQUID_POLY_DEFINE_INTERNAL_API(POLY_MANGLE_DOUBLE,
                                double,
                                double complex)

LIQUID_POLY_DEFINE_INTERNAL_API(POLY_MANGLE_FLOAT,
                                float,
                                float complex)

LIQUID_POLY_DEFINE_INTERNAL_API(POLY_MANGLE_CDOUBLE,
                                double complex,
                                double complex)

LIQUID_POLY_DEFINE_INTERNAL_API(POLY_MANGLE_CFLOAT,
                                float complex,
                                float complex)



//
// MODULE : matrix
//

// large macro
//   MATRIX : name-mangling macro
//   T      : data type
#define LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX,T)             \
T    MATRIX(_det2x2)(T * _x,                                    \
                     unsigned int _rx,                          \
                     unsigned int _cx);


LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_FLOAT,   float)
LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_DOUBLE,  double)

LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_CFLOAT,  liquid_float_complex)
LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_CDOUBLE, liquid_double_complex)

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

    // APSK modem
    unsigned int apsk_num_levels;   // number of levels
    unsigned int * apsk_p;          // number of symbols per level
    float * apsk_r;                 // radii of levels
    float * apsk_r_slicer;          // slicer radii of levels
    float * apsk_phi;               // phase offset of levels
    unsigned int * apsk_symbol_map; // symbol mapping

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
modem modem_create_apsk(unsigned int _bits_per_symbol);
modem modem_create_arb(unsigned int _bits_per_symbol);
modem modem_create_arb_mirrored(unsigned int _bits_per_symbol);
modem modem_create_arb_rotated(unsigned int _bits_per_symbol);

// specific modem create routines
modem modem_create_bpsk(void);
modem modem_create_qpsk(void);
modem modem_create_apsk4(void);
modem modem_create_apsk8(void);
modem modem_create_apsk16(void);
modem modem_create_apsk32(void);
modem modem_create_apsk64(void);
modem modem_create_apsk128(void);
modem modem_create_arb16opt(void);
modem modem_create_arb64vt(void);

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
void modem_modulate_apsk(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_mirrored(modem _mod, unsigned int symbol_in, float complex *y);
//void modem_modulate_arb_rotated(modem _mod, unsigned int symbol_in, float complex *y);

// specific modem modulate routines
void modem_modulate_bpsk(modem _mod, unsigned int symbol_in, float complex *y);
void modem_modulate_qpsk(modem _mod, unsigned int symbol_in, float complex *y);

// modem demodualte phase error approximation
float cargf_demod_approx(float complex _x);

// generic modem demodulate routines
void modem_demodulate_ask(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qam(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_psk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_dpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_arb(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_apsk(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_mirrored(modem _demod, float complex x, unsigned int *symbol_out);
//void modem_demodulate_arb_rotated(modem _demod, float complex x, unsigned int *symbol_out);

// specific modem demodulate routines
void modem_demodulate_bpsk(modem _demod, float complex x, unsigned int *symbol_out);
void modem_demodulate_qpsk(modem _demod, float complex x, unsigned int *symbol_out);

// specific modem definitions
extern const unsigned int apsk4_num_levels;
extern const unsigned int apsk4_p[2];
extern const float apsk4_r[2];
extern const float apsk4_phi[2];
extern const float apsk4_r_slicer[1];
extern const unsigned int apsk4_symbol_map[4];

extern const unsigned int apsk8_num_levels;
extern const unsigned int apsk8_p[2];
extern const float apsk8_r[2];
extern const float apsk8_phi[2];
extern const float apsk8_r_slicer[1];
extern const unsigned int apsk8_symbol_map[8];

extern const unsigned int apsk16_num_levels;
extern const unsigned int apsk16_p[2];
extern const float apsk16_r[2];
extern const float apsk16_phi[2];
extern const float apsk16_r_slicer[1];
extern const unsigned int apsk16_symbol_map[16];

extern const unsigned int apsk32_num_levels;
extern const unsigned int apsk32_p[3];
extern const float apsk32_r[3];
extern const float apsk32_phi[3];
extern const float apsk32_r_slicer[2];
extern const unsigned int apsk32_symbol_map[32];

extern const unsigned int apsk64_num_levels;
extern const unsigned int apsk64_p[4];
extern const float apsk64_r[4];
extern const float apsk64_phi[4];
extern const float apsk64_r_slicer[3];
extern const unsigned int apsk64_symbol_map[64];

extern const unsigned int apsk128_num_levels;
extern const unsigned int apsk128_p[5];
extern const float apsk128_r[5];
extern const float apsk128_phi[5];
extern const float apsk128_r_slicer[4];
extern const unsigned int apsk128_symbol_map[128];

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
// analog modems
//

// freqmodem
struct freqmodem_s {
    liquid_fmtype type; // demodulator type (PLL, DELAY_CONJ)
    nco_crcf oscillator;// nco
    float fc;           // carrier frequency
    float m;            // modulation index
    float m_inv;        // 1/m

    // phase difference
    float complex q;
};

//
// MODULE : multicarrier
//

// ofdm frame (common)

// generate short sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S0                 :   output symbol
//  _M_S0               :   total number of enabled subcarriers in S0
void ofdmframe_init_S0(unsigned int * _p,
                       unsigned int _num_subcarriers,
                       float complex * _S0,
                       unsigned int * _M_S0);

// generate long sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S1                 :   output symbol
//  _M_S1               :   total number of enabled subcarriers in S1
void ofdmframe_init_S1(unsigned int * _p,
                       unsigned int _num_subcarriers,
                       float complex * _S1,
                       unsigned int * _M_S1);


void ofdmframesync_cpcorrelate(ofdmframesync _q);
void ofdmframesync_findrxypeak(ofdmframesync _q);
void ofdmframesync_rxpayload(ofdmframesync _q);

void ofdmframesync_execute_plcpshort(ofdmframesync _q, float complex _x);
void ofdmframesync_execute_plcplong0(ofdmframesync _q, float complex _x);
void ofdmframesync_execute_plcplong1(ofdmframesync _q, float complex _x);
void ofdmframesync_execute_rxsymbols(ofdmframesync _q, float complex _x);

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
void ofdmframe64sync_smooth_gain(ofdmframe64sync _q,
                                 float _alpha,
                                 unsigned int _range);
void ofdmframe64sync_execute_rxpayload(ofdmframe64sync _q, float complex _x);
extern const float complex ofdmframe64_plcp_Sf[64];
extern const float complex ofdmframe64_plcp_St[64];
extern const float complex ofdmframe64_plcp_Lf[64];
extern const float complex ofdmframe64_plcp_Lt[64];


// 
// ofdm/oqam framing
//

// generate short sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S0                 :   output symbol
//  _M_S0               :   total number of enabled subcarriers in S0
void ofdmoqamframe_init_S0(unsigned int * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S0,
                           unsigned int * _M_S0);

// generate long sequence symbols
//  _p                  :   subcarrier allocation array
//  _num_subcarriers    :   total number of subcarriers
//  _S1                 :   output symbol
//  _M_S1               :   total number of enabled subcarriers in S1
void ofdmoqamframe_init_S1(unsigned int * _p,
                           unsigned int _num_subcarriers,
                           float complex * _S1,
                           unsigned int * _M_S1);

void ofdmoqamframesync_execute_plcpshort(ofdmoqamframesync _q, float complex _x);
void ofdmoqamframesync_execute_plcplong0(ofdmoqamframesync _q, float complex _x);
void ofdmoqamframesync_execute_plcplong1(ofdmoqamframesync _q, float complex _x);
void ofdmoqamframesync_execute_rxsymbols(ofdmoqamframesync _q, float complex _x);

void ofdmoqamframesync_S0_metrics(ofdmoqamframesync _q,
                                  float complex * _g_hat,
                                  float complex * _s_hat);

void ofdmoqamframesync_S1_metrics(ofdmoqamframesync _q,
                                  float complex * _t0_hat,
                                  float complex * _t1_hat);

void ofdmoqamframesync_correct_buffer(ofdmoqamframesync _q);

void ofdmoqamframesync_init_gain_window(ofdmoqamframesync _q,
                                        float _sigma);

void ofdmoqamframesync_estimate_gain(ofdmoqamframesync _q,
                                     float complex * _G_hat,
                                     float complex * _G);

void ofdmoqamframesync_rxpayload(ofdmoqamframesync _q,
                                 float complex * _Y0,
                                 float complex * _Y1);

// 
// MODULE : nco (numerically-controlled oscillator)
//


// Numerically-controlled oscillator, floating point phase precision
#define LIQUID_NCO_DEFINE_INTERNAL_API(NCO,T,TC)                \
struct NCO(_s) {                                                \
    liquid_ncotype type;                                        \
    T theta;            /* NCO phase                    */      \
    T d_theta;          /* NCO frequency                */      \
    T sintab[256];      /* sine table                   */      \
    unsigned int index; /* table index                  */      \
    T sine;                                                     \
    T cosine;                                                   \
    void (*compute_sincos)(NCO() _q);                           \
                                                                \
    /* phase-locked loop */                                     \
    T bandwidth;        /* loop filter bandwidth        */      \
    T zeta;             /* loop filter damping factor   */      \
    T a[3];             /* feed-back coefficients       */      \
    T b[3];             /* feed-forward coefficients    */      \
    iirfiltsos_rrrf pll_filter; /* iir filter object    */      \
    T pll_phi_prime;    /* pll phase state              */      \
    T pll_phi_hat;      /* pll output phase             */      \
    T pll_dtheta_base;  /* NCO base frequency           */      \
};                                                              \
                                                                \
void NCO(_constrain_phase)(NCO() _q);                           \
                                                                \
void NCO(_compute_sincos_nco)(NCO() _q);                        \
void NCO(_compute_sincos_vco)(NCO() _q);                        \
                                                                \
/* phase-locked loop methods */                                 \
void NCO(_pll_reset)(NCO() _q);

// Define nco internal APIs
LIQUID_NCO_DEFINE_INTERNAL_API(NCO_MANGLE_FLOAT,
                               float,
                               float complex)

// 
// MODULE : optim (non-linear optimization)
//

// gradient search algorithm (steepest descent) object
// \f[ \bar{x}_{n+1} = \bar{x}_n - \gamma \nabla f(\bar{x}_n) \f]
struct gradient_search_s {
    float* v;           // vector to optimize (externally allocated)
    unsigned int num_parameters;

    float gamma;        // nominal stepsize
    float delta;        // differential used to compute (estimate) derivative
    float mu;           // decremental gamma parameter
    float gamma_hat;    // step size (decreases each epoch)
    float* v_prime;     // temporary vector array
    float* dv;          // vector step
    float* dv_hat;      // vector step (previous iteration)
    float alpha;        // filter (feed-forward parameter)
    float beta;         // filter (feed-back parameter)

    float* gradient;    // gradient approximation
    float utility;      // current utility

    // External utility function.
    utility_function get_utility;
    void * userdata;    // object to optimize (user data)
    int minimize;       // minimize/maximimze utility (search direction)
};

// compute the gradient vector (estimate)
void gradient_search_compute_gradient(gradient_search _g);

// normalize the gradient vector
void gradient_search_normalize_gradient(gradient_search _g);


// quasi-Newton search object
struct quasinewton_search_s {
    float* v;           // vector to optimize (externally allocated)
    unsigned int num_parameters;    // number of parameters to optimize [n]

    float gamma;        // nominal stepsize
    float delta;        // differential used to compute (estimate) derivative
    float dgamma;       // decremental gamma parameter
    float gamma_hat;    // step size (decreases each epoch)
    float* v_prime;     // temporary vector array
    float* dv;          // parameter step vector

    float * B;          // approximate Hessian matrix inverse [n x n]
    float * H;          // Hessian matrix

    float* p;           // search direction
    float* gradient;    // gradient approximation
    float* gradient0;   // gradient approximation (previous step)

    // External utility function.
    utility_function get_utility;
    float utility;      // current utility
    void* obj;          // object to optimize (user data)
    int minimize;       // minimize/maximimze utility (search direction)
};

// compute gradient(x_k)
void quasinewton_search_compute_gradient(quasinewton_search _q);

// compute the norm of the gradient(x_k)
void quasinewton_search_normalize_gradient(quasinewton_search _q);

// compute gradient(x_k)
void quasinewton_search_compute_Hessian(quasinewton_search _q);

// compute the updated inverse hessian matrix using the Broyden, Fletcher,
// Goldfarb & Shanno method (BFGS)
void quasinewton_search_update_hessian_bfgs(quasinewton_search _q);

// optim pattern set (struct)
struct patternset_s {
    float * x;      // input
    float * y;      // output
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int num_patterns;
    unsigned int num_allocated;
};

void patternset_increase_mem(patternset _q, unsigned int _n);


// Chromosome structure used in genetic algorithm searches
struct chromosome_s {
    unsigned int num_traits;            // number of represented traits
    unsigned int * bits_per_trait;      // bits to represent each trait
    unsigned long * max_value;          // maximum representable integer value
    unsigned long * traits;             // chromosome data

    unsigned int num_bits;              // total number of bits
};

struct ga_search_s {
    chromosome * population;            // population of chromosomes
    unsigned int population_size;       // size of the population
    unsigned int selection_size;        // number of 
    float mutation_rate;                // rate of mutation

    unsigned int num_parameters;        // number of parameters to optimize
    unsigned int bits_per_chromosome;   // total number of bits in each chromosome

    float *utility;                     // utility array
    unsigned int *rank;                 // rank indices of chromosomes (best to worst)

    chromosome c;                       // copy of best chromosome, optimal solution
    float utility_opt;                  // optimum utility (fitness of best solution)

    // External utility function.
    //
    // The success of a GA search algorithm is contingent upon the
    // design of a good utility function.  It should meet the following
    // criteria:
    //   - monotonically increasing (never flat)
    //   - efficient to compute
    //   - maps the [0,1] bounded output vector to desired range
    //   - for multiple objectives, utility should be high \em only when
    //         all objectives are met (multiplicative, not additive)
    float (*get_utility)(void*, chromosome);
    void * userdata;                    // object to optimize
    int minimize;                       // minimize/maximize utility (search direction)
};

//
// ga_search internal methods
//

// evaluate fitness of entire population
void ga_search_evaluate(ga_search _g);

// crossover population
void ga_search_crossover(ga_search);

// mutate population
void ga_search_mutate(ga_search);

// rank population by fitness
void ga_search_rank(ga_search);

// sort values by index
//  _v          :   input values [size: _len x 1]
//  _rank       :   output rank array (indices) [size: _len x 1]
//  _len        :   length of input array
//  _descending :   descending/ascending
void optim_sort(float *_v,
                unsigned int * _rank,
                unsigned int _len,
                int _descending);


//
// MODULE : random
//
float complex icrandnf();

// data scrambler masks
#define LIQUID_SCRAMBLE_MASK0   (0xb4)
#define LIQUID_SCRAMBLE_MASK1   (0x6a)
#define LIQUID_SCRAMBLE_MASK2   (0x8b)
#define LIQUID_SCRAMBLE_MASK3   (0xc5)

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

// maximal-length sequence
struct msequence_s {
    unsigned int m;     // length generator polynomial, shift register
    unsigned int g;     // generator polynomial
    unsigned int a;     // initial shift register state, default: 1

    unsigned int n;     // length of sequence, n = (2^m)-1
    unsigned int v;     // shift register
    unsigned int b;     // return bit
};

// Default msequence generator objects
extern struct msequence_s msequence_default[16];


//
// Miscellaneous utilities
//


// number of 1's in byte
extern unsigned int liquid_c_ones[256];

// Count the number of ones in an integer, inline insertion
#define liquid_count_ones_uint16(x) (           \
    liquid_c_ones[  (x)      & 0xff ] +         \
    liquid_c_ones[ ((x)>>8)  & 0xff ])

#define liquid_count_ones_uint32(x) (           \
    liquid_c_ones[  (x)      & 0xff ] +         \
    liquid_c_ones[ ((x)>> 8) & 0xff ] +         \
    liquid_c_ones[ ((x)>>16) & 0xff ] +         \
    liquid_c_ones[ ((x)>>24) & 0xff ])


// number of 1's in a byte modulo 2
extern unsigned char liquid_c_ones_mod2[256];

// Count the number of ones in an integer modulo 2, inline insertion
#define liquid_count_ones_mod2_uint16(x) ((         \
    liquid_c_ones_mod2[  (x)      & 0xff ] +        \
    liquid_c_ones_mod2[ ((x)>>8)  & 0xff ]) % 2)

#define liquid_count_ones_mod2_uint32(x) ((         \
    liquid_c_ones_mod2[  (x)       & 0xff ] +       \
    liquid_c_ones_mod2[ ((x)>> 8)  & 0xff ] +       \
    liquid_c_ones_mod2[ ((x)>>16)  & 0xff ] +       \
    liquid_c_ones_mod2[ ((x)>>24)  & 0xff ]) % 2)

// compute binary dot-products (inline pre-processor macros)
#define liquid_bdotprod_uint8(x,y)  liquid_c_ones_mod2[(x)&(y)]
#define liquid_bdotprod_uint16(x,y) liquid_count_ones_mod2_uint16((x)&(y))
#define liquid_bdotprod_uint32(x,y) liquid_count_ones_mod2_uint32((x)&(y))

// number of leading zeros in byte
extern unsigned int liquid_c_leading_zeros[256];

#endif // __LIQUID_INTERNAL_H__

