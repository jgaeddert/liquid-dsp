//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"

struct framegen64_s {
    modem mod_preamble;
    modem mod;

    // preamble
    float pn_sequence[63];

    //
};

framegen64 framegen64_create()
{
    framegen64 fg = (framegen64) malloc(sizeof(struct framegen64_s));

    return fg;

}

void framegen64_destroy(framegen64 _fg)
{
    free(_fg);
}

void framegen64_print(framegen64 _fg)
{
    printf("framegen:\n");
}



