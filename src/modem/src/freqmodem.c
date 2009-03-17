//
// Frequency modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

struct freqmodem_s {
    float theta;    // phase angle
    float m;        // modulation index
};

freqmodem freqmodem_create()
{
    freqmodem fm = (freqmodem) malloc(sizeof(struct freqmodem_s));

    fm->theta=0.0f;
    fm->m = 1.0f;

    return fm;
}

void freqmodem_destroy(freqmodem _fm)
{
    free(_fm);
}

void freqmodem_print(freqmodem _fm)
{
    printf("freqmodem:\n");
    printf("    mod. index  :   %8.4f\n", _fm->m);
}

void freqmodem_modulate(freqmodem _fm, float _x, float complex *_y)
{

}

void freqmodem_demodulate(freqmodem _fm, float complex *_y, float *_x)
{

}


