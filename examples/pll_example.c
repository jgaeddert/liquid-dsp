//
// pll_example.c
// 
// Demonstrates a basic phase-locked loop to track the phase of a
// complex sinusoid.
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

// output to octave-friendly format
#define OUTPUT_FILENAME "pll_example.m"

int main() {
    // parameters
    float phase_offset = 0.8f;
    float frequency_offset = 0.01f;
    float pll_bandwidth = 0.05f;
    float pll_damping_factor = 0.707f;
    unsigned int n=256;     // number of iterations
    unsigned int d=n/32;    // print every "d" lines

    //
    float theta[n];         // input phase
    float complex x[n];     // input sinusoid
    float phi[n];           // output phase
    float complex y[n];     // output sinusoid

    // generate iir loop filter(s)
    float a[2];
    float b[2];
    float wn = pll_bandwidth;
    float zeta = pll_damping_factor;
    float K = 1000; // loop gain

    // loop filter
    float t1 = K/(wn*wn);
    float t2 = 2*zeta/wn - 1/K;
    b[0] = 1 + t2/2;
    b[1] = 1 - t2/2;
    a[0] = 1 + t1/2;
    a[1] = 1 - t1/2;
    iir_filter_rrrf F = iir_filter_rrrf_create(b,2,a,2);

    // integrator
    b[0] = 2*K;
    b[1] = 2*K;
    a[0] =  1.0f;
    a[1] = -1.0f;
    iir_filter_rrrf G = iir_filter_rrrf_create(b,2,a,2);

    unsigned int i;

    // generate input
    float t=phase_offset;
    float dt = frequency_offset;
    for (i=0; i<n; i++) {
        theta[i] = t;
        x[i] = cexpf(_Complex_I*theta[i]);

        t += dt;
    }

    // run loop
    float phi_hat=0.0f;
    for (i=0; i<n; i++) {
        y[i] = cexpf(_Complex_I*phi_hat);

        // compute error
        float e = cargf(x[i]*conjf(y[i]));

        if ( (i%d)==0 )
            printf("e(%3u) = %12.8f;\n", i, e);

        // filter error
        float e_hat;
        iir_filter_rrrf_execute(F,e,&e_hat);

        // integrate
        iir_filter_rrrf_execute(G,e_hat,&phi_hat);

        phi[i] = phi_hat;
    }

    // destroy filter
    iir_filter_rrrf_destroy(F);
    iir_filter_rrrf_destroy(G);

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n=%u;\n",n);

    fprintf(fid,"a(1) = %16.8e;\n", a[0]);
    fprintf(fid,"a(2) = %16.8e;\n", a[1]);
    fprintf(fid,"a(3) = %16.8e;\n", a[2]);

    fprintf(fid,"b(1) = %16.8e;\n", b[0]);
    fprintf(fid,"b(2) = %16.8e;\n", b[1]);
    fprintf(fid,"b(3) = %16.8e;\n", b[2]);

    //fprintf(fid,"figure;\n");
    //fprintf(fid,"freqz(b,a);\n");

    for (i=0; i<n; i++) {
        fprintf(fid,"theta(%3u) = %16.8e;\n", i+1, theta[i]);
        fprintf(fid,"  phi(%3u) = %16.8e;\n", i+1, phi[i]);
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,theta,t,phi);\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('phase');\n");

    fclose(fid);

    printf("output written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
