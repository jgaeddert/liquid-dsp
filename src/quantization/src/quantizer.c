//
//
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

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


