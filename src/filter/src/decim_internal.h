//
// Decimator
//

#ifndef __LIQUID_DECIM_INTERNAL_H__
#define __LIQUID_DECIM_INTERNAL_H__

#include "filter_common.h"

typedef struct decim_s * decim;

// Create decimator object
//   D    : decimation factor (0 < D < DECIM_FACTOR_MAX)
//   fc   : carrier frequency (-1 < fc < 1)
//   t    : transition bandwidth (0 < b < 1/D < 0.5)
//   slsl : sidelobe suppression level [dB]
decim decim_create(unsigned int _D, float _fc, float _t, float _slsl);

// Destroy decimator object
void decim_destroy(decim _d);

// Print
void decim_print(decim _d);

// Execute
void decim_execute(
    decim _d,
    float * _x,
    unsigned int _x_len,
    unsigned int * _nr,
    float * _y,
    unsigned int _y_len,
    unsigned int * _nw);

// 
// internal
//

#include "../../buffer/src/buffer_internal.h"
#define DECIM_FACTOR_MAX 2048
struct decim_s {
    decim_type type;
    float * h;
    unsigned int h_len;

    unsigned int D; // decimation factor (D > 0)
    float fc;       // center frequency (-0.5 < fc < 0.5)
    float b;        // filter bandwidth (0 < b < 0.5)
    float t;        // transition bandwidth (0 < t < 0.5), t = 1/D
    float slsl;     // sidelobe suppression level [dB] (slsl < 0)

    unsigned int branch;

    fbuffer * buffers;
    unsigned int num_buffers;
};

void decim_debug_print(decim _d);

decim decim_create_generic(unsigned int _D, float _fc, float _b, float _slsl);
decim decim_create_halfband(float _fc, float _b, float _slsl);

void decim_dotprod(float * _h, unsigned int _h_len, float *_x, float *_y);
float dotprod(float * _x, float * _y, unsigned int _n);

// Execute generic decimator
void decim_execute_generic(
    decim _d,
    float * _x,
    unsigned int _x_len,
    unsigned int * _nr,
    float * _y,
    unsigned int _y_len,
    unsigned int * _nw);


#endif // __LIQUID_DECIM_INTERNAL_H__
