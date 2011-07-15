//
// iiriirqmfb_crcf_example.c
//
// perfect reconstruction infinite impulse respone
// quadrature mirror filterbank example
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "iirqmfb_crcf_example.m"

int main() {
    unsigned int order=9;           // filter order
    float beta = 0.3f;              // stop-band attenuation (ignored for now)
    int prototype=0;                // filter prototype (ignored for now)
    unsigned int num_samples=64;    // number of samples

    // derived values
    unsigned int n = num_samples*1.2;

    // ensure even number
    n += (n%2) == 1 ? 1 : 0;

    // create filterbank
    iirqmfb_crcf qmfa = iirqmfb_crcf_create(order,
                                            beta,
                                            LIQUID_QMFB_ANALYZER,
                                            prototype);
    iirqmfb_crcf qmfs = iirqmfb_crcf_create(order,
                                            beta,
                                            LIQUID_QMFB_SYNTHESIZER,
                                            prototype);

    iirqmfb_crcf_print(qmfa);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"x = zeros(1,%u);\n", n);
    fprintf(fid,"y = zeros(2,%u);\n", n/2);
    fprintf(fid,"z = zeros(1,%u);\n", n);

    unsigned int i;
    float complex x[n], y[n], z[n];

    // generate time-domain signal (windowed noise)
    for (i=0; i<n; i++) {
#if 0
        x[i] = (i<num_samples) ? kaiser(i,num_samples,10.0f,0)*randnf() : 0.0f;
        x[i] *= cexpf(_Complex_I*2*M_PI*randf());
        //x[i] = (i==4) ? 1.0f : 0.0f;
#else
        x[i]  = cexpf(_Complex_I*i*0.02f*2*M_PI);// + cexpf(_Complex_I*i*0.37*2*M_PI);
        x[i] *= (i<num_samples) ? kaiser(i,num_samples,10.0f,0) : 0.0f;
#endif
    }

    // compute QMF analysis sub-channel output
    for (i=0; i<n; i+=2) {
        iirqmfb_crcf_execute(qmfa, x[i+0], x[i+1], &y[i+0], &y[i+1]);
    }

    // compute QMF synthesis output
    for (i=0; i<n; i+=2) {
        iirqmfb_crcf_execute(qmfs, y[i+0], y[i+1], &z[i+0], &z[i+1]);
    }

    // write results to output file
    for (i=0; i<n; i+=2) {
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(x[i+0]), cimagf(x[i+0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", i+2, crealf(x[i+1]), cimagf(x[i+1]));

        fprintf(fid,"y(1,%3u) = %8.4f + j*%8.4f;\n", i/2+1, crealf(y[i+0]),  cimagf(y[i+0]));
        fprintf(fid,"y(2,%3u) = %8.4f + j*%8.4f;\n", i/2+1, crealf(y[i+1]),  cimagf(y[i+1]));

        fprintf(fid,"z(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i+0]), cimagf(z[i+0]));
        fprintf(fid,"z(%3u) = %8.4f + j*%8.4f;\n", i+2, crealf(z[i+1]), cimagf(z[i+1]));
    }

    // plot results
    fprintf(fid,"t  = 0:(n-1);\n");
    fprintf(fid,"td = 0:(n/2-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(x),'-x',t,real(z),'-o');\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    legend('original','reconstructed',1);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(t,imag(x),'-x',t,imag(z),'-o');\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    legend('original','reconstructed',1);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(td,real(y(1,:)),'-o',td,imag(y(1,:)),'-o');\n");
    fprintf(fid,"    ylabel('H_0');\n");
    fprintf(fid,"    legend('real','imag',1);\n");
    fprintf(fid,"    axis([0 n/2 -1 1]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(td,real(y(2,:)),'-o',td,imag(y(2,:)),'-o');\n");
    fprintf(fid,"    ylabel('H_1');\n");
    fprintf(fid,"    legend('real','imag',1);\n");
    fprintf(fid,"    axis([0 n/2 -1 1]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    iirqmfb_crcf_destroy(qmfa);
    iirqmfb_crcf_destroy(qmfs);
    printf("done.\n");
    return 0;
}
