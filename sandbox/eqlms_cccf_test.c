// 
// eqlms_cccf_test.c
//
// Tests least mean-squares (LMS) equalizer (EQ).
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include "liquid.h"

#define OUTPUT_FILENAME "eqlms_cccf_test.m"

int main() {
    // options
    unsigned int k=2;       // samples/symbol
    unsigned int m=3;
    float beta = 0.2f;
    float dt = 0.4f;        // timing offset
    unsigned int num_symbols = 256;
    unsigned int hc_len=4;  // channel impulse response length
    unsigned int p=15;      // equalizer filter length
    float mu = 0.005f;

    // derived values
    unsigned int num_samples = k*num_symbols;

    // data arrays
    float complex s[num_symbols];   // original QPSK symbols
    float complex x[num_samples];   // interpolated samples
    float complex y[num_samples];   // received samples
    float complex z[num_samples];   // recovered samples

    // generate data sequence
    unsigned int i;
    for (i=0; i<num_symbols; i++) {
        s[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f) * _Complex_I;
    }

    // interpolate 
    interp_crcf interp = interp_crcf_create_rrc(k,m,beta,dt);
    for (i=0; i<num_symbols; i++)
        interp_crcf_execute(interp, s[i], &x[i*k]);
    interp_crcf_destroy(interp);

    // generate channel filter
    float complex hc[hc_len];
    for (i=0; i<hc_len; i++)
        hc[i] = (i==0) ? 1.0f : 0.1f*randnf()*cexpf(_Complex_I*2*M_PI*randf());

    // push signal through channel...
    firfilt_cccf fchannel = firfilt_cccf_create(hc,hc_len);
    for (i=0; i<num_samples; i++) {
        firfilt_cccf_push(fchannel, x[i]);
        firfilt_cccf_execute(fchannel, &y[i]);
    }
    firfilt_cccf_destroy(fchannel);
    
    // initialize equalizer
#if 0
    float complex h[p]; // coefficients
    for (i=0; i<p; i++)
        h[i] = (i==0) ? 1.0f : 0.0f;
#else
    p = 2*k*m+1;
    float complex h[p];
    float h_tmp[p];
    design_rrc_filter(k,m,beta,0.0f,h_tmp);
    for (i=0; i<p; i++)
        h[i] = h_tmp[i] / k;
#endif
    
    // run equalizer
    float complex w0[p];
    float complex w1[p];
    memmove(w0, h, p*sizeof(float complex));
    windowcf buffer = windowcf_create(p);
    for (i=0; i<num_samples; i++) {
        // push value into buffer
        windowcf_push(buffer, y[i]);

        // compute d_hat
        float complex d_hat = 0.0f;
        float complex * r;
        windowcf_read(buffer, &r);
        unsigned int j;
        for (j=0; j<p; j++)
            d_hat += conjf(w0[j]) * r[j];

        // store in output
        z[i] = d_hat;

        // check to see if buffer is full, return if not
        if (i <= p) continue;

        // decimate by k
        if ( (i%k) != 0 ) continue;

        // estimate transmitted QPSK symbol
        float complex d_prime = (crealf(d_hat) > 0.0f ? 1.0f : -1.0f) +
                                (cimagf(d_hat) > 0.0f ? 1.0f : -1.0f) * _Complex_I;

        // compute error
        float complex alpha = d_prime - d_hat;

        // compute signal energy
        float ex2 = 1.414f;

        // update weighting vector
        for (j=0; j<p; j++)
            w1[j] = w0[j] + mu*conjf(alpha)*r[j]/ex2;

        // copy new filter values
        memmove(w0, w1, p*sizeof(complex float));
    }

    // destroy additional objects
    windowcf_destroy(buffer);

    // print results to file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = num_symbols*k;\n");

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }
    fprintf(fid,"t = 1:num_samples;\n");
    fprintf(fid,"tsym = 1:k:num_samples;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1), plot(t, real(z), tsym, real(z(tsym)),'x');\n");
    fprintf(fid,"subplot(2,1,2), plot(t, imag(z), tsym, imag(z(tsym)),'x');\n");

    fclose(fid);

    printf("results written to '%s'\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
