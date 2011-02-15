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

typedef struct patternset_s * patternset;

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


//
// SUBMODULE : buffer
//

//
// Generic port
//

typedef struct gport_s * gport;

// create gport object
//
//  _n      :   size of the internal buffer
//  _size   :   size of each element in bytes (e.g. sizeof(int))
gport gport_create(unsigned int _n, unsigned int _size);

// destroy gport object and free all internal memory
void gport_destroy(gport _p);

// print gport object internals
void gport_print(gport _p);

// producer methods

// lock producer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to lock
//
// returns pointer to internal memory buffer for writing
void * gport_producer_lock(gport _p, unsigned int _n);

// unlock producer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to unlock
void gport_producer_unlock(gport _p, unsigned int _n);

// produce data (indirect memory access)
//  _p      :   gport object
//  _w      :   external data buffer [size: 1 x _n]
//  _n      :   number of elements to produce (size of _w)
// returns status
int gport_produce(gport _p,
                  void * _w,
                  unsigned int _n);

// produce available data (indirect memory access)
//  _p      :   gport object
//  _w      :   external data buffer [size: 1 x _nmax]
//  _nmax   :   number of elements in _w
//  _np     :   returned number of elements in _w produced by _p
// returns status
int gport_produce_available(gport _p,
                            void * _w,
                            unsigned int _nmax,
                            unsigned int *_np);


// consumer methods

// lock consumer (direct memory access)
//
//  _p      :   gport object
//  _n      :   requested number of elements to lock
//
// returns pointer to internal memory buffer for reading
void * gport_consumer_lock(gport _p, unsigned int _n);

// unlock consumer (direct memory access)
//  _p      :   gport object
//  _n      :   requested number of elements to unlock
void gport_consumer_unlock(gport _p, unsigned int _n);

// consume data (indirect memory access)
//  _p      :   gport object
//  _r      :   external data buffer [size: 1 x _n]
//  _n      :   number of elements to consume (size of _r)
// returns status
int gport_consume(gport _p,
                  void * _r,
                  unsigned int _n);

// produce available data (indirect memory access)
//  _p      :   gport object
//  _r      :   external data buffer [size: 1 x _nmax]
//  _nmax   :   number of elements in _r
//  _nc     :   returned number of elements in _r consumed by _p
// returns status
int gport_consume_available(gport _p,
                            void * _r,
                            unsigned int _nmax,
                            unsigned int *_nc);

// broadcast eom signal
void gport_signal_eom(gport _p);

// clear eom signal
void gport_clear_eom(gport _p);

#if 0
enum {
    GPORT_SIGNAL_NULL=0,            // no signal
    GPORT_SIGNAL_METADATA_UPDATE,   // metadata 
    GPORT_SIGNAL_EOM                // end of message
};
// signaling methods
void gport_signal(gport _p, int _message);
#endif

//
// SUBMODULE-INTERNAL : buffer
//



//
// SUBMODULE : filter
//

// Direct digital [up/down] synthesizer

#define DDS_MANGLE_CCCF(name)  LIQUID_CONCAT(dds_cccf,name)

// Macro:
//   DDS        : name-mangling macro
//   T          : output data type
#define LIQUID_DDS_DEFINE_API(DDS,T)                            \
                                                                \
typedef struct DDS(_s) * DDS();                                 \
DDS() DDS(_create)(unsigned int _num_stages,                    \
                   float _fc,                                   \
                   float _bw,                                   \
                   float _As);                                  \
void DDS(_destroy)(DDS() _q);                                   \
void DDS(_print)(DDS() _q);                                     \
void DDS(_reset)(DDS() _q);                                     \
void DDS(_decim_execute)(DDS() _q,                              \
                         T * _x,                                \
                         T * _y);                               \
void DDS(_interp_execute)(DDS() _q,                             \
                          T _x,                                 \
                          T * _y);

LIQUID_DDS_DEFINE_API(DDS_MANGLE_CCCF, liquid_float_complex)


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

LIQUID_QMFB_DEFINE_API(QMFB_MANGLE_RRRF,
                       float,
                       float,
                       float)

LIQUID_QMFB_DEFINE_API(QMFB_MANGLE_CRCF,
                       liquid_float_complex,
                       float,
                       liquid_float_complex)

// 
// iir quadrature mirror filter banks
//
#define IIRQMFB_MANGLE_RRRF(name)   LIQUID_CONCAT(iirqmfb_rrrf,name)
#define IIRQMFB_MANGLE_CRCF(name)   LIQUID_CONCAT(iirqmfb_crcf,name)

#define LIQUID_IIRQMFB_DEFINE_API(IIRQMFB,TO,TC,TI)             \
typedef struct IIRQMFB(_s) * IIRQMFB();                         \
IIRQMFB() IIRQMFB(_create)(unsigned int _order,                 \
                           float _beta,                         \
                           int _type,                           \
                           int _prototype);                     \
IIRQMFB() IIRQMFB(_recreate)(IIRQMFB() _q, unsigned int _m);    \
void IIRQMFB(_destroy)(IIRQMFB() _q);                           \
void IIRQMFB(_print)(IIRQMFB() _q);                             \
void IIRQMFB(_clear)(IIRQMFB() _q);                             \
void IIRQMFB(_execute)(IIRQMFB() _q,                            \
                       TI   _x0,                                \
                       TI   _x1,                                \
                       TO * _y0,                                \
                       TO * _y1);

LIQUID_IIRQMFB_DEFINE_API(IIRQMFB_MANGLE_RRRF,
                          float,
                          float,
                          float)

LIQUID_IIRQMFB_DEFINE_API(IIRQMFB_MANGLE_CRCF,
                          liquid_float_complex,
                          float,
                          liquid_float_complex)

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
ITQMFB() ITQMFB(_create)(unsigned int _n,                       \
                         unsigned int _m,                       \
                         float _As,                             \
                         int _type);                            \
ITQMFB() ITQMFB(_recreate)(ITQMFB() _q, unsigned int _m);       \
void ITQMFB(_destroy)(ITQMFB() _q);                             \
void ITQMFB(_print)(ITQMFB() _q);                               \
void ITQMFB(_execute)(ITQMFB() _q,                              \
                      TO * _x,                                  \
                      TO * _y);

LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_RRRF,
                         float,
                         float,
                         float)

LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_CRCF,
                         liquid_float_complex,
                         float,
                         liquid_float_complex)

//LIQUID_ITQMFB_DEFINE_API(ITQMFB_MANGLE_CCCF,
//                         liquid_float_complex,
//                         liquid_float_complex,
//                         liquid_float_complex)

//
// SUBMODULE-INTERNAL : filter
//

// Direct digital [up/down] synthesizer
//#define LIQUID_DDS_DEFINE_INTERNAL_API(DDS,T)

//LIQUID_DDS_DEFINE_INTERNAL_API(DDS_MANGLE_CCCF, liquid_float_complex)


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

//
// SUBMODULE : optimization
//

//
// optimization pattern set
//

// forward declaration of patternset in artificial neural network module
//typedef struct patternset_s * patternset;

// create pattern set
//  _num_inputs     :   number of inputs in the set
//  _num_outputs    :   number of output in the set
patternset patternset_create(unsigned int _num_inputs,
                             unsigned int _num_outputs);

// destroy pattern set object
void patternset_destroy(patternset _q);

// print pattern set
void patternset_print(patternset _q);

// get number of patterns currently in the set
unsigned int patternset_get_num_patterns(patternset _q);

// append single pattern to set
//  _q      :   pattern set object
//  _x      :   input [size: _num_inputs x 1]
//  _y      :   output [size: _num_outputs x 1]
void patternset_append_pattern(patternset _q,
                               float *_x,
                               float *_y);

// append multiple patterns to the set
//  _q      :   pattern set object
//  _x      :   inputs [size: _num_inputs x _n]
//  _y      :   outputs [size: _num_outputs x _n]
//  _n      :   number of patterns to append
void patternset_append_patterns(patternset _q,
                                float * _x,
                                float * _y,
                                unsigned int _n);

// remove pattern from set at index _i
void patternset_delete_pattern(patternset _q,
                               unsigned int _i);

// remove all patterns from the set
void patternset_clear(patternset _q);

// access a single pattern in the set
//  _q      :   pattern set object
//  _i      :   index of pattern
//  _x      :   input pointer
//  _y      :   output pointer
void patternset_access(patternset _q,
                       unsigned int _i,
                       float ** _x,
                       float ** _y);

// access all patterns in the set
//  _q      :   pattern set object
//  _x      :   input pointer
//  _y      :   output pointer
void patternset_access_all(patternset _q,
                           float ** _x,
                           float ** _y);

typedef void(*optim_target_function)(float *_x, float *_y, void *_p);
typedef float(*optim_obj_function)(patternset _q, void *_p, optim_target_function _f);

//
// SUBMODULE-INTERNAL : optimization
//


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


#endif // __LIQUID_EXPERIMENTAL_H__

