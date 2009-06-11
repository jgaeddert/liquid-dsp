//
// perfect reconstruction quadrature mirror filterbank example
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "prqmfb_crcf_example.m"

int main() {
    unsigned int m=5;                   // filter delay
    float beta = 0.3f;                  // sidelobe suppression level
    unsigned int num_samples=64;        // number of samples

    // derived values
    unsigned int n = num_samples + 2*m;   // extend length of synthesis to
                                        // incorporate filter delay

    // ensure even number
    n += (n%2) == 1 ? 1 : 0;

    // create filterbank
    qmfb_crcf qmfa = qmfb_crcf_create(m, beta);
    qmfb_crcf qmfs = qmfb_crcf_create(m, beta);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"m=%u;\n", m);
    fprintf(fid,"x = zeros(1,%u);\n", n);
    fprintf(fid,"y = zeros(2,%u);\n", n/2);
    fprintf(fid,"z = zeros(1,%u);\n", n);

    unsigned int i;
    float complex x[n], y[n], z[n];

    // generate time-domain signal (windowed noise)
    for (i=0; i<n; i++) {
        //x[i] = (i<num_samples) ? kaiser(i,num_samples,10.0f,0)*randnf() : 0.0f;
        x[i] = (i==4) ? 1.0f : 0.0f;
    }

    // compute QMF analysis sub-channel output
    for (i=0; i<n; i+=2) {
        qmfb_crcf_analysis_execute(qmfa, x[i+0], x[i+1], &y[i+0], &y[i+1]);
    }

    // compute QMF synthesis output
    for (i=0; i<n; i+=2) {
        qmfb_crcf_synthesis_execute(qmfs, y[i+0], y[i+1], &z[i+0], &z[i+1]);
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

    // plot time-domain results
    fprintf(fid,"return;\n");
    fprintf(fid,"t0=0:(2*n-1);\n");
    fprintf(fid,"t1=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1); plot(t1,real(y(1,:)),t1,imag(y(1,:))); ylabel('y_0(t)');\n");
    fprintf(fid,"subplot(3,1,2); plot(t1,real(y(2,:)),t1,imag(y(2,:))); ylabel('y_1(t)');\n");
    fprintf(fid,"subplot(3,1,3); plot(t0,real(x),t0,imag(x));           ylabel('x(t),composite');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    qmfb_crcf_destroy(qmfa);
    qmfb_crcf_destroy(qmfs);
    printf("done.\n");
    return 0;
}
