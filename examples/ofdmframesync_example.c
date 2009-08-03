//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframesync_example.m"

static int callback(float complex * _X,
                    unsigned int _n,
                    void * _userdata);

int main() {
    // options
    unsigned int num_subcarriers=512;// 
    unsigned int cp_len=64;         // cyclic prefix length
    //unsigned int num_symbols=2;     // number of ofdm symbols
    modulation_scheme ms = MOD_QAM;
    unsigned int bps     = 4;
    float nstd = 0.03f; // noise standard deviation

    // 
    unsigned int frame_len = num_subcarriers + cp_len;

    //unsigned int num_samples = num_subcarriers * num_frames;

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(num_subcarriers, cp_len);
    ofdmframegen_print(fg);

    modem mod = modem_create(ms,bps);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_subcarriers=%u;\n", num_subcarriers);
    fprintf(fid,"cp_len=%u;\n", cp_len);
    fprintf(fid,"frame_len=%u;\n", frame_len);

    fprintf(fid,"X = zeros(1,num_subcarriers);\n");
    fprintf(fid,"x = zeros(1,frame_len);\n");
    fprintf(fid,"y = zeros(1,3*frame_len);\n");
    fprintf(fid,"Y = zeros(1,num_subcarriers);\n");

    ofdmframesync fs = ofdmframesync_create(num_subcarriers,cp_len,callback,(void*)(fid));
    ofdmframesync_print(fs);

    unsigned int i;
    float complex X[num_subcarriers];   // channelized symbols
    float complex x[frame_len];         // time-domain symbol
    float complex y[3*frame_len];       // time-domain samples (with noise)

    unsigned int s;
    for (i=0; i<num_subcarriers; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod,s,&X[i]);
    }

    ofdmframegen_execute(fg,X,x);
    for (i=0; i<frame_len; i++) y[i]            = 0.0f;
    for (i=0; i<frame_len; i++) y[i+frame_len]  = x[i];
    for (i=0; i<frame_len; i++) y[i+2*frame_len]= 0.0f;

    // add noise
    for (i=0; i<3*frame_len; i++) {
        cawgn(&y[i],nstd);
        y[i] *= cexpf(_Complex_I*0.01f);
    }

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
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(Y,'x');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

static int callback(float complex * _X,
                    unsigned int _n,
                    void * _userdata)
{
    printf("**** callback invoked\n");
    FILE * fid = (FILE*)_userdata;
    unsigned int i;
    for (i=0; i<_n; i++)
        fprintf(fid,"Y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_X[i]), cimagf(_X[i]));

    return 0;
}

