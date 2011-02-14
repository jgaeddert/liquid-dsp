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

#ifndef __LIQUID_EXPERIMENTAL_H__
#define __LIQUID_EXPERIMENTAL_H__

// Configuration file
#include "config.h"

#include <complex.h>
#include "liquid.internal.h"


// 
// MODULE : experimental
//


//
// SUBMODULE : ann (artificial neural network)
// 

typedef enum {
    LIQUID_ANN_AF_LINEAR=0,
    LIQUID_ANN_AF_LOGISTIC,
    LIQUID_ANN_AF_TANH,
    LIQUID_ANN_AF_MULAW,
    LIQUID_ANN_AF_ERF
} ann_activation_function;

#define ANN_MANGLE_FLOAT(name)  LIQUID_CONCAT(ann, name)

// large macro
//   ANN    : name-mangling macro
//   T      : primitive data type
#define LIQUID_ANN_DEFINE_API(ANN,T)                            \
                                                                \
typedef struct ANN(_s) * ANN();                                 \
ANN() ANN(_create)(unsigned int * _structure,                   \
                   unsigned int _num_layers,                    \
                   int _activation_func_hidden,                 \
                   int _activation_func_output);                \
void  ANN(_destroy)(ANN() _q);                                  \
void  ANN(_print)(ANN() _q);                                    \
ANN() ANN(_load_from_file)(char * _filename);                   \
void  ANN(_save_to_file)(ANN() _q, char * _filename);           \
void  ANN(_init_random_weights)(ANN() _q);                      \
void  ANN(_evaluate)(ANN() _q,                                  \
                     T * _x,                                    \
                     T * _y);                                   \
void  ANN(_train_patternset)(ANN() _q,                          \
                             patternset _set,                   \
                             T _emax,                           \
                             unsigned int _nmax);               \
void  ANN(_train)(ANN() _q,                                     \
                  T * _x,                                       \
                  T * _y,                                       \
                  unsigned int _num_patterns,                   \
                  T _emax,                                      \
                  unsigned int _nmax);                          \
void  ANN(_train_ga)(ANN() _q,                                  \
                     T * _x,                                    \
                     T * _y,                                    \
                     unsigned int _num_patterns,                \
                     T _emax,                                   \
                     unsigned int _nmax);                       \
float ANN(_compute_rmse)(ANN() _q,                              \
                         T * _x,                                \
                         T * _y,                                \
                         unsigned int _num_patterns);
// void ANN(_prune)(ANN() _q, ...);

// Define ann APIs
LIQUID_ANN_DEFINE_API(ANN_MANGLE_FLOAT, float)


// maxnet
typedef struct maxnet_s * maxnet;
maxnet maxnet_create(unsigned int _num_classes,
                     unsigned int * _structure,
                     unsigned int _num_layers);
void maxnet_destroy(maxnet _q);
void maxnet_print(maxnet _q);
void maxnet_evaluate(maxnet _q,
                     float * _x,
                     float * _y,
                     unsigned int * _class);
void maxnet_train(maxnet _q,
                  float * _x,
                  unsigned int _class);
void maxnet_train_group(maxnet _q,
                        float * _x,
                        unsigned int * _class,
                        unsigned int _num_patterns,
                        unsigned int _max_num_trials);
float maxnet_compute_rmse(maxnet _q,
                          float * _x,
                          unsigned int * _class,
                          unsigned int _num_patterns);

// K-means (2-dimensional)
void kmeans(liquid_float_complex * _x,
            unsigned int _n,
            liquid_float_complex * _s,
            unsigned int _k);

//
// SUBMODULE-INTERNAL : ann
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
// SUBMODULE : audio
// 


// FBASC: filterbank audio synthesizer codec
#define FBASC_ENCODER   0
#define FBASC_DECODER   1
typedef struct fbasc_s * fbasc;
unsigned int fbasc_compute_header_length(unsigned int _num_channels,
                                         unsigned int _samples_per_frame,
                                         unsigned int _bytes_per_frame);
fbasc fbasc_create(int _type,
                   unsigned int _num_channels,
                   unsigned int _samples_per_frame,
                   unsigned int _bytes_per_frame);
void fbasc_destroy(fbasc _q);
void fbasc_print(fbasc _q);
void fbasc_reset(fbasc _q);

void fbasc_encode(fbasc _q,
                  float * _audio,
                  unsigned char * _header,
                  unsigned char * _frame);
void fbasc_decode(fbasc _q,
                  unsigned char * _header,
                  unsigned char * _frame,
                  float * _audio);


//
// SUBMODULE-INTERNAL : audio
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



#endif // __LIQUID_EXPERIMENTAL_H__

