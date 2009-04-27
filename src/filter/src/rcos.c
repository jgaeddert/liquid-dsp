//
// Design raised-cosine filter
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

void design_rcos_filter(
  unsigned int _k,
  unsigned int _m,
  float _beta,
  float _dt,
  float * _h
)
{
    unsigned int h_len;

    if ( _k < 1 ) {
        printf("error: design_rcos_filter(): k must be greater than 0\n");
        exit(0);
    } else if ( _m < 1 ) {
        printf("error: design_rcos_filter(): m must be greater than 0\n");
        exit(0);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        printf("error: design_rcos_filter(): beta must be in [0,1]\n");
        exit(0);
    } else;

    unsigned int n;
    float z, t1, t2, t3;

    float nf, kf, mf;

    h_len = 2*_k*_m + 1;

    // Calculate filter coefficients
    for (n=0; n<h_len; n++) {
        nf = (float) n;
        kf = (float) _k;
        mf = (float) _m;

        z = (nf+_dt)/kf-mf;
        t1 = cosf(_beta*M_PI*z);
        t2 = sincf(z);
        t3 = 1 - 4.0f*_beta*_beta*z*z;

        _h[n] = t1*t2/t3;
    }
}

