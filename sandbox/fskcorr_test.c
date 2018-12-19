// 
// fskcorr_test.c
//
// This example tests detection of FSK sequences.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fskcorr_test.m"

int main(int argc, char*argv[])
{
    unsigned int    M       = 1000; // samples per symbol
    unsigned int    n       =  256; // sequence length (number of symbols)
    unsigned int    p       = 4;    // over-sampling rate for output
    //float           SNRdB   =   20; //
    unsigned int    timer   = 0;
    float           fc      = 0.2f; // tone frequency

    unsigned int i;
    unsigned int j;

    // derived values
    float nstd = 1e-3f;
    float gain = 0.3125f;
    unsigned int i1  = (unsigned int)round(fc*M);
    unsigned int i0  = M - (i1 - 1);
    printf("M = %u, fc = %.3f {%u,%u}\n", M, fc, i0, i1);

    // generate sequence
    uint8_t seq[n];
    for (i=0; i<n; i++)
        seq[i] = rand() & 1;

    // generate 'matched filter' receiver
    float triangle[2*p];
    for (i=0; i<p; i++) triangle[  i] = (float)(  i) / (float)p;
    for (i=0; i<p; i++) triangle[p+i] = (float)(p-i) / (float)p;
    //for (i=0; i<2*p; i++) printf("%8.3f\n", triangle[i]);
    firinterp_rrrf interp = firinterp_rrrf_create(p, triangle, 2*p);
    float buf_mf[n*p];
    for (i=0; i<n; i++)
        firinterp_rrrf_execute(interp, 2.0f*(float)seq[i] - 1.0f, buf_mf + i*p);
    firinterp_rrrf_destroy(interp);
    //for (i=0; i<n*p; i++) printf("mf(%3u) = %8.3f;\n", i+1, buf_mf[i]);
    // generate buffer and dot product objects for cross-correlation
    dotprod_rrrf xcorr     = dotprod_rrrf_create(buf_mf, n*p);
    windowf      buf_xcorr = windowf_create(p*n);

    // allocate memory arrays
    float complex * buf_0 = (float complex*) malloc(M*sizeof(float complex));
    float complex * buf_1 = (float complex*) malloc(M*sizeof(float complex));
    float complex * buf_2 = (float complex*) malloc(M*sizeof(float complex));
    float complex * buf_3 = (float complex*) malloc(M*sizeof(float complex));
    windowcf buf_rx = windowcf_create(M);

    // create fft plans
    fftplan ifft = fft_create_plan(M, buf_0, buf_1, LIQUID_FFT_BACKWARD, 0);
    fftplan  fft = fft_create_plan(M, buf_2, buf_3, LIQUID_FFT_FORWARD,  0);

    // fill input with noise
    for (i=0; i<M; i++)
        windowcf_push(buf_rx, nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2);

    //
    FILE * fid = fopen("fskcorr_test.m","w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"p = %u;\n", p);
    for (i=0; i<n; i++)
        fprintf(fid,"seq(%3u) = %1u;\n", i+1, seq[i]);
    fprintf(fid,"p = %u;\n", p);
    fprintf(fid,"%%s = reshape([(2*seq(:)-1) zeros(n,p-1)]',1,[]);\n");
    fprintf(fid,"s = []; for i=1:p, s=[s seq(:)]; end; s=reshape(s',1,[]);\n");
    fprintf(fid,"llr = [];\n");
    for (i=0; i<n+10; i++) {
        // clear input, generate symbol
        memset(buf_0, 0x0, M*sizeof(float complex));
        if (i < n)
            buf_0[ seq[i] ? i1 : i0 ] = gain;
        fft_execute(ifft); // buf_0 -> buf_1

        // add noise
        for (j=0; j<M; j++)
            buf_1[j] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // execute...
        for (j=0; j<M; j++) {
            if (!timer) {
                timer = M/p;
                float complex * r;
                windowcf_read(buf_rx, &r);
                memmove(buf_2, r, M*sizeof(float complex));
                fft_execute(fft);
                float r2 = liquid_sumsqcf(buf_3, M);
                float g  = 1.0f / sqrtf(r2);
                float v0 = cabsf(buf_3[i0]) * g;
                float v1 = cabsf(buf_3[i1]) * g;
                float llr = log10f( v1 / v0 );
                fprintf(fid,"llr(end+1) = %12.4e;\n", llr);

                // run through cross-correlator
                float rxy = 0;
                windowf_push(buf_xcorr, llr);
                float * rf;
                windowf_read(buf_xcorr, &rf);
                dotprod_rrrf_execute(xcorr, rf, &rxy);
                fprintf(fid,"rxy(end+1) = %12.4e;\n", rxy);
                
                printf(" %12.4e { %12.6f, %12.6f } : llr:%12.6f, rxy:%10.1f\n", r2, v0, v1, llr, rxy);
            }
            windowcf_push(buf_rx,buf_1[j]);
            timer--;
        }
    }
    fprintf(fid,"num_samples = length(llr);\n");
    fprintf(fid,"txy         = [0:(num_samples-1)] - num_samples/2;\n");
    fprintf(fid,"S = fft(s,  num_samples);\n");
    fprintf(fid,"L = fft(llr,num_samples);\n");
    fprintf(fid,"%%rxy = fftshift(ifft(L.*conj(S)));\n");
    fprintf(fid,"plot(rxy);\n");
    fclose(fid);
    printf("results written to fskcorr_test.m\n");

    free(buf_0);
    free(buf_1);
    free(buf_2);
    free(buf_3);
    fft_destroy_plan(fft);
    fft_destroy_plan(ifft);
    windowcf_destroy(buf_rx);
    dotprod_rrrf_destroy(xcorr);
    windowf_destroy(buf_xcorr);

    printf("done.\n");
    return 0;
}
