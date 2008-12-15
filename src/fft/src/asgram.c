//
// asgram (ASCII spectrogram)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "fft.h"

struct asgram_s {
    float complex * x;
    float complex * y;
    float * psd;
    unsigned int n;
    fftplan p;
};

asgram asgram_create(unsigned int _n)
{
    asgram q = (asgram) malloc(sizeof(struct asgram_s));
    q->n = 64;

    q->x = (float complex*) malloc((q->n)*sizeof(float complex));
    q->y = (float complex*) malloc((q->n)*sizeof(float complex));
    q->psd = (float*) malloc((q->n)*sizeof(float));

    q->p = fft_create_plan(q->n, q->x, q->y, FFT_FORWARD);

    return q;
}

void asgram_destroy(asgram _q)
{
    free(_q->x);
    free(_q->y);
    free(_q->psd);
    fft_destroy_plan(_q->p);
    free(_q);
}

void asgram_execute(asgram _q, float complex *_x)
{
    // copy x and execute fft plan
    memcpy(_q->x, _x, (_q->n)*sizeof(float complex));
    fft_execute(_q->p);

    // fftshift
    fft_shift(_q->y, _q->n);

    // compute PSD magnitude
    unsigned int i;
    for (i=0; i<_q->n; i++)
        _q->psd[i] = 20*log10f(cabsf(_q->y[i]));

    unsigned int num_levels=6;
    char levelchar[] = {'M', '#', '+', '-', '.', ' '};
    // >    ....++++MMMM++++....    <
    unsigned int j;
    float offset, scale=20.0f;
    printf(" > ");
    for (i=0; i<_q->n; i++) {
        offset = 0;
        for (j=0; j<num_levels-1; j++) {
            if ( _q->psd[i] > (scale - offset) )
                break;
            else
                offset += 10.0f;
        }
        printf("%c", levelchar[j]);
    }   
    printf(" < ");

    // print scale
    printf("\n");
}


