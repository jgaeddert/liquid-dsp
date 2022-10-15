// This example demonstrates the functionality of the qdsync object to
// detect and synchronize an arbitrary signal in time in the presence of noise,
// carrier frequency/phase offsets, and fractional-sample timing offsets.
// offsets.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "liquid.h"

#define OUTPUT_FILENAME "qdsync_cccf_example.m"

// synchronization callback, return 0:continue, 1:reset
int callback(float complex * _buf,
             unsigned int    _buf_len,
             void *          _context)
{
    printf("callback got %u samples\n", _buf_len);
    unsigned int i;
    for (i=0; i<_buf_len; i++)
        fprintf((FILE*)_context, "y(end+1) = %12.8f + %12.8fj;\n", crealf(_buf[i]), cimagf(_buf[i]));
    return 0;
}

int main(int argc, char*argv[])
{
    // options
    unsigned int sequence_len =   80;   // number of sync symbols
    unsigned int k            =    2;   // samples/symbol
    unsigned int m            =    7;   // filter delay [symbols]
    float        beta         = 0.3f;   // excess bandwidth factor
    int          ftype        = LIQUID_FIRFILT_ARKAISER;
    float        nstd         = 0.01f;

    unsigned int i;

    // generate synchronization sequence (QPSK symbols)
    unsigned int  seq_len = k*(sequence_len + 2*m);
    float complex seq[seq_len];
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,0);
    for (i=0; i<sequence_len + 2*m; i++) {
        float complex s = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                          (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
        firinterp_crcf_execute(interp, i<sequence_len ? s : 0, seq + k*i);
    }
    firinterp_crcf_destroy(interp);

    // open file for storing results
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all; close all; y=[];\n");

    // create sync object
    qdsync_cccf q = qdsync_cccf_create(seq, seq_len, callback, fid);
    qdsync_cccf_print(q);

    //
    float complex buf[seq_len];
    for (i=0; i<20; i++) {
        if (i==10) memmove(buf, seq, seq_len*sizeof(float complex));
        else       memset (buf, 0x00, seq_len*sizeof(float complex));

        // add noise
        unsigned int j;
        for (j=0; j<seq_len; j++)
            buf[j] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // run through synchronizer
        qdsync_cccf_execute(q, buf, seq_len);
    }
    qdsync_cccf_destroy(q);

    // export results
    fprintf(fid,"s = [];\n");
    for (i=0; i<seq_len; i++)
        fprintf(fid,"s(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(seq[i]), cimagf(seq[i]));

    fprintf(fid,"t=[0:(length(s)-1)];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"  plot(t,real(s),t,imag(s));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);
    return 0;
}
