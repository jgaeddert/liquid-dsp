//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_VALIDATE_INPUT
#define QUANTIZER_MAX_BITS      32

// inline quantizer: 'analog' signal in [-1, 1]

unsigned int quantize_adc(float _x, unsigned int _num_bits)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (fabsf(_x) > 1.0f) {
        printf("error: quantize_adc(), input out of range\n");
        exit(1);
    } else if (_num_bits > QUANTIZER_MAX_BITS) {
        printf("error: quantize_adc(), maximum bits exceeded\n");
        exit(1);
    }
#endif
    unsigned int n = _num_bits-1;   // 
    unsigned int N = 1<<n;          // 2^n
    unsigned int r = fabsf(_x) * (N-1);

    // if negative set MSB to 1
    if (_x < 0)
        r |= N;

    return r;
}

float quantize_dac(unsigned int _s, unsigned int _num_bits)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (_num_bits > QUANTIZER_MAX_BITS) {
        printf("error: quantize_dac(), maximum bits exceeded\n");
        exit(1);
    }
#endif
    unsigned int n = _num_bits-1;   //
    unsigned int N = 1<<n;          // 2^n
    float r = (float)(_s & (N-1)) / (float) (N-1);

    // check MSB, return negative if 1
    return (_s & N) ? -r : r;
}


struct quantizer_rf_s {
    unsigned int n;
};

quantizer_rf quantizer_rf_create(int _compander_type, float _range, unsigned int _num_bits)
{
    quantizer_rf q = (quantizer_rf) malloc(sizeof(struct quantizer_rf_s));
    return q;
}

void quantizer_rf_destroy(quantizer_rf _q)
{

}

void quantizer_rf_print(quantizer_rf _q)
{

}

void quantizer_rf_adc(quantizer_rf _q, float _x, unsigned int * _sample)
{

}

void quantizer_rf_dac(quantizer_rf _q, unsigned int _sample, float * _x)
{

}


