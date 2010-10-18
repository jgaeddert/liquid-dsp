// 
// carrier_estimation_example.c
//
// This example demonstrates carrier frequency and
// phase estimation.
//

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

int main() {
    srand(time(NULL));

    // options
    unsigned int n=64;      // number of samples
    float dphi = 0.50;      // carrier frequency [-pi,pi]
    float phi = 0.8f;       // carrier phase [-pi,pi]
    float SNRdB = 20.0f;    // signal-to-noise ratio [dB]
    int method = LIQUID_ESTIMATE_CARRIER_ITERATIVE;

    //
    float nstd = powf(10.0f, -SNRdB/20.0f);
    float complex y[n];

    unsigned int i;
    for (i=0; i<n; i++) {
        y[i] = cexpf(_Complex_I*(phi + i*dphi));
        y[i] += nstd*randnf()*cexpf(_Complex_I*2*M_PI*randf());
    }

    // estimate carrier frequency, phase
    float dphi_hat;
    float phi_hat;
    liquid_estimate_carrier(y,n,method,&dphi_hat,&phi_hat);

    // print results
    printf(" dphi_hat   :   %12.8f (true : %12.8f)\n", dphi_hat, dphi);
    printf("  phi_hat   :   %12.8f (true : %12.8f)\n",  phi_hat,  phi);

    printf("done.\n");
    return 0;
}
