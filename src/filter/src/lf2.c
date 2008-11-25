//
// 2nd-order integrating loop filter
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "filter.h"

struct lf2_s {
    float BT;       // bandwidth-time product
    float beta;
    float alpha;

    float xi;       // damping factor
    float zeta;     // correction factor
        
    // loop filter state variables
    float tmp2;
    float q;
    float q_hat;
    float q_prime;
};

lf2 lf2_create(float _bt)
{
    // create object
    lf2 f = (lf2) malloc(sizeof(struct lf2_s));

    // reset internal state
    lf2_init(f);

    // set bandwidth
    lf2_set_bandwidth(f, _bt);

    return f;
}

void lf2_destroy(lf2 _f)
{
    free(_f);
}

void lf2_print(lf2 _f)
{
    printf("loop filter : b = %6.4e\n", _f->BT);
}

void lf2_init(lf2 _f)
{
    // set default bandwidth, compute coefficients as necessary
    _f->BT = 0.01f;
    _f->xi = 1/sqrtf(2);
    lf2_generate_filter(_f);

    // reset internal filter state variables
    _f->tmp2 = 0.0f;
    _f->q = 0.0f;
    _f->q_hat = 0.0f;
    _f->q_prime = 0.0f;
}

void lf2_set_damping_factor(lf2 _f, float _xi)
{
    _f->xi = _xi;
    
    // recompute
    lf2_set_bandwidth(_f, _f->BT);
}

void lf2_set_bandwidth(lf2 _f, float _bt)
{
    _f->BT = _bt;
    lf2_generate_filter(_f);
}

// push input and compute output
void lf2_advance(lf2 _f, float _v, float *_v_hat)
{
    _f->q_prime = _v * _f->beta + _f->tmp2;
    _f->q_hat = _f->alpha*_v + _f->q_prime;
    _f->tmp2 = _f->q_prime;
    *_v_hat = _f->q_hat;
}

// 
// internal
//
void lf2_generate_filter(lf2 _f)
{
    // beta = 2*BT/(xi+1/(4*xi))/k1;
    // alpha = 2*xi*beta;

    float k1 = 1.0f;
    _f->beta = 2*(_f->BT)*(_f->xi+1/(4*(_f->xi)))/k1;
    _f->alpha = 2*(_f->xi)*(_f->beta);
}


