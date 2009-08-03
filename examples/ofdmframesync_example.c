//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframesync_example.m"

int main() {
    // options
    unsigned int num_subcarriers=512;// 
    unsigned int cp_len=64;         // cyclic prefix length
    //unsigned int num_symbols=2;     // number of ofdm symbols

    // 
    unsigned int frame_len = num_subcarriers + cp_len;

    //unsigned int num_samples = num_subcarriers * num_frames;

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(num_subcarriers, cp_len);
    ofdmframegen_print(fg);

    ofdmframesync fs = ofdmframesync_create(num_subcarriers,cp_len,NULL,NULL);
    ofdmframesync_print(fs);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_subcarriers=%u;\n", num_subcarriers);
    fprintf(fid,"cp_len=%u;\n", cp_len);
    fprintf(fid,"frame_len=%u;\n", frame_len);

    fprintf(fid,"X = zeros(1,num_subcarriers);\n");
    fprintf(fid,"x = zeros(1,frame_len);\n");

    unsigned int i;
    float complex X[num_subcarriers];   // channelized symbols
    float complex x[frame_len];         // time-domain symbol
    float complex y[3*frame_len];       // time-domain samples (with noise)

    for (i=0; i<num_subcarriers; i++) {
        X[i] = 0.707f*(rand()&1 ? 1.0f : -1.0f) +
               0.707f*(rand()&1 ? 1.0f : -1.0f)*_Complex_I;
    }

    ofdmframegen_execute(fg,X,x);
    for (i=0; i<frame_len; i++) y[i]            = 0.0f;
    for (i=0; i<frame_len; i++) y[i+frame_len]  = x[i];
    for (i=0; i<frame_len; i++) y[i+2*frame_len]= 0.0f;

    // add noise
    for (i=0; i<3*frame_len; i++)
        cawgn(&y[i],0.1f);

    //ofdmframesync_execute(fs,z,frame_len);
    ofdmframesync_execute(fs,y,3*frame_len);

    //
    for (i=0; i<num_subcarriers; i++)
        fprintf(fid,"X(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(X[i]), cimagf(X[i]));

    //
    for (i=0; i<frame_len; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    //
    for (i=0; i<3*frame_len; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(frame_len-1);\n");
    //fprintf(fid,"plot(t,real(x),t,imag(x));\n");
    fprintf(fid,"ty=0:(3*frame_len-1);\n");
    fprintf(fid,"plot(ty,real(y),ty,imag(y));\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

