//
// Finite impulse response filter
//

#ifndef __FIR_FILTER_H__
#define __FIR_FILTER_H__

typedef struct fir_filter_s * fir_filter;

fir_filter fir_filter_create(float * _h, unsigned int _n);

//fir_filter fir_filter_create_prototype(unsigned int _n);

void fir_filter_destroy(fir_filter _f);

void fir_filter_print(fir_filter _f);

float fir_filter_execute(fir_filter _f, float * _v);

// accessor functions
unsigned int fir_filter_get_length(fir_filter _f);

//
// internal
//
struct fir_filter_s {
    float * h;
    unsigned int h_len;
};

#endif // __FIR_FILTER_H__

