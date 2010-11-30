//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframesync_example.m"

static int callback(float complex * _X,
                    unsigned int _n,
                    void * _userdata);

int main() {
    srand(time(NULL));

    // options
    unsigned int M = 64;                // number of subcarriers
    unsigned int cp_len = 16;           // cyclic prefix length
    unsigned int num_symbols_S0 = 2;    // number of S0 symbols
    unsigned int num_symbols_S1 = 2;    // number of S0 symbols
    unsigned int num_symbols_data = 8;  // number of data symbols
    modulation_scheme ms = MOD_QAM;
    unsigned int bps = 4;
    float nstd  = 0.01f;    // noise standard deviation
    float phi   = 0.0f;     // phase offset
    float dphi  = 0.005f;   // frequency offset

    // derived values
    unsigned int frame_len = M + cp_len;
    unsigned int num_symbols = num_symbols_S0 +
                               num_symbols_S1 +
                               num_symbols_data;
    unsigned int num_samples = frame_len*num_symbols;

    // initialize subcarrier allocation
    unsigned int p[M];
    ofdmframe_init_default_sctype(M, p);

    // create frame generator
    ofdmframegen fg = ofdmframegen_create(M, cp_len, p);
    ofdmframegen_print(fg);

    // create frame synchronizer
    ofdmframesync fs = ofdmframesync_create(M, cp_len, p, callback, NULL);
    ofdmframesync_print(fs);

    modem mod = modem_create(ms,bps);

    unsigned int i;
    float complex X[M];             // channelized symbols
    float complex y[num_samples];   // output time series

    unsigned int n=0;

    // write short sequence(s)
    for (i=0; i<num_symbols_S0; i++) {
        ofdmframegen_write_S0(fg, &y[n]);
        n += frame_len;
    }

    // write long sequence(s)
    for (i=0; i<num_symbols_S1; i++) {
        ofdmframegen_write_S1(fg, &y[n]);
        n += frame_len;
    }

    // modulate data symbols
    unsigned int s;
    for (i=0; i<num_symbols_data; i++) {

        unsigned int j;
        for (j=0; j<M; j++) {
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod,s,&X[j]);
        }

        ofdmframegen_writesymbol(fg, X, &y[n]);
        n += frame_len;
    }

    // add noise, carrier offset
    for (i=0; i<num_samples; i++) {
        // add carrier offset
        y[i] *= cexpf(_Complex_I*(phi + dphi*i));

        // add noise
        y[i] += nstd*randnf()*cexp(_Complex_I*2*M_PI*randf());
    }

    // execute synchronizer
    ofdmframesync_execute(fs,y,num_samples);

    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
    modem_destroy(mod);


    // 
    // export output file
    //

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"cp_len = %u;\n", cp_len);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,num_samples);\n");

    //
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('received signal');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

static int callback(float complex * _X,
                    unsigned int _n,
                    void * _userdata)
{
    printf("**** callback invoked\n");
    return 0;
}

