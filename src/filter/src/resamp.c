//
// Arbitrary resampler
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;

    float r;        // rate
    float b_soft;   // filterbank index (soft value)

    FIRPFB() f;

    fir_prototype p;
};

RESAMP() RESAMP(_create)(float _r)
{
    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));
    q->r = _r;
    return q;
}

void RESAMP(_destroy)(RESAMP() _q)
{
    free(_q);
}

void RESAMP(_print)(RESAMP() _q)
{
    printf("resampler [rate: %f]\n", _q->r);
}

void RESAMP(_execute)(RESAMP() _q)
{
}

