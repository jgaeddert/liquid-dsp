//
//
//

#ifndef __LIQUID_H__
#define __LIQUID_H__

#include <stdlib.h>
#include <complex.h>

#define LIQUID_CONCAT(prefix, name) prefix ## name

//
// agc
// 
typedef struct agc_s * agc;
agc agc_create(float _etarget, float _BT);
void agc_free(agc _agc);
void agc_init(agc _agc);
void agc_set_target(agc _agc, float _e_target);
void agc_set_bandwidth(agc _agc, float _BT);
void agc_apply_gain(agc _agc, float complex _x, float complex *_y);
float agc_get_signal_level(agc _agc);
float agc_get_gain(agc _agc);

//
// buffer
//
typedef enum {CIRCULAR=0,STATIC} buffer_type;
#define BUFFER_MANGLE_FLOAT(name)  LIQUID_CONCAT(fbuffer, name)
#define BUFFER_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfbuffer, name)
#define BUFFER_MANGLE_UINT(name)   LIQUID_CONCAT(uibuffer, name)

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
void X(_read)(X() _b, T ** _v, unsigned int *_n);       \
void X(_release)(X() _b, unsigned int _n);              \
void X(_write)(X() _b, T * _v, unsigned int _n);        \
void X(_push)(X() _b, T _v);
//void X(_force_write)(X() _b, T * _v, unsigned int _n);

// Define APIs
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)

//
// FFT
//
typedef struct fftplan_s * fftplan;

#define FFT_FORWARD 0
#define FFT_REVERSE 1
fftplan fft_create_plan(unsigned int _n, float complex * _x, float complex * _y, int _dir);
void fft_destroy_plan(fftplan _p);
void fft_execute(fftplan _p);

//
// math
//
float lngammaf(float _z);
float gammaf(float _z);
float factorialf(unsigned int _n);
float besseli_0(float _z);
float sincf(float _x);

//
// random
//
#define randf() ((float) rand() / (float) RAND_MAX)
void randnf(float * i, float * q); 
float rand_weibullf(float _alpha, float _beta, float _gamma);
void rand_gammaf();
void rand_nakagamimf(float _m, float _omega);
float rand_ricekf(float _K, float _omega);


#endif // __LIQUID_H__
