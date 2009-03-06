#ifndef __LIQUID_CVSD_AUTOTEST_H__
#define __LIQUID_CVSD_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: check RMSE for CVSD
//
void autotest_cvsd_rmse_sine() {
    unsigned int n=256;
    unsigned int nbits=3;
    float zeta=1.5f;

    // create cvsd codecs
    cvsd cvsd_encoder = cvsd_create(nbits,zeta);
    cvsd cvsd_decoder = cvsd_create(nbits,zeta);

    float phi=0.0f;
    float dphi=0.1f;
    unsigned int i;
    unsigned char b;
    float x,y;
    float rmse=0.0f;
    for (i=0; i<n; i++) {
        x = 0.5f*sinf(phi);
        b = cvsd_encode(cvsd_encoder, x); 
        y = cvsd_decode(cvsd_decoder, b); 

        rmse += (x-y)*(x-y);
        phi += dphi;
    }   

    rmse = 10*log10f(rmse/n);
    if (_autotest_verbose)
        printf("cvsd rmse : %8.2f dB\n", rmse);
    CONTEND_LESS_THAN(rmse, -20.0f);
}


#endif // __LIQUID_CVSD_AUTOTEST_H__

