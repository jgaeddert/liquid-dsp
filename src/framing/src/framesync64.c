//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"

struct framesync64_s {
    modem demod;

    // synchronizer objects
    // agc
    // symsync_crcf
    // pll
    // pnsync_crcf
};

framesync64 framesync64_create()
{
    framesync64 fg = (framesync64) malloc(sizeof(struct framesync64_s));

    return fg;

}

void framesync64_destroy(framesync64 _fg)
{
    free(_fg);
}

void framesync64_print(framesync64 _fg)
{
    printf("framesync:\n");
}



