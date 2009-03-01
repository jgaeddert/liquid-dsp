//
// FBASC: filterbank audio synthesizer codec
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct fbasc_s {
    unsigned int num_channels;
};

fbasc fbasc_create()
{
    fbasc q = (fbasc) malloc(sizeof(struct fbasc_s));
    return q;
}

void fbasc_destroy(fbasc _q)
{
    free(_q);
}

void fbasc_print(fbasc _q)
{
    printf("filterbank audio synthesizer codec: [%u channels]\n", _q->num_channels);
}

void fbasc_encode(fbasc _q, float * _audio, unsigned char * _frame)
{

}

void fbasc_decode(fbasc _q, unsigned char * _frame, float * _audio)
{

}


