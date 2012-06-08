//
// sandbox/ofdmframe_papr_test.c
//
// Test OFDM frame's peak-to-average power ratio (PAPR).
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframe_papr_test.m"

void usage()
{
    printf("Usage: ofdmframe_papr_test [OPTION]\n");
    printf("  h     : print help\n");
    printf("  M     : number of subcarriers (must be even), default: 64\n");
    printf("  C     : cyclic prefix length, default: 16\n");
    printf("  T     : taper length, default: 0\n");
    printf("  N     : number of data frames, default: 10\n");
}

// compute peak-to-average power ratio
//  _x  :   input time series
//  _n  :   number of samples
float ofdmframe_PAPR(float complex * _x,
                     unsigned int    _n);

int main(int argc, char*argv[])
{
    //srand(time(NULL));

    // options
    unsigned int M           = 64;  // number of subcarriers
    unsigned int cp_len      = 16;  // cyclic prefix length
    unsigned int taper_len   = 0;   // taper length
    unsigned int num_symbols = 100; // number of data symbols
    modulation_scheme ms = LIQUID_MODEM_QPSK;

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hM:C:T:N:")) != EOF){
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'M': M           = atoi(optarg);   break;
        case 'C': cp_len      = atoi(optarg);   break;
        case 'T': taper_len   = atoi(optarg);   break;
        case 'N': num_symbols = atoi(optarg);   break;
        default:
            exit(1);
        }
    }

    unsigned int i;

    // derived values
    unsigned int frame_len = M + cp_len;

    // initialize subcarrier allocation
    unsigned char p[M];
    ofdmframe_init_default_sctype(M, p);

    // create frame generator
    ofdmframegen fg = ofdmframegen_create(M, cp_len, taper_len, p);
    ofdmframegen_print(fg);

    modem mod = modem_create(ms);

    float complex X[M];             // channelized symbols
    float complex buffer[frame_len];// output time series
    float PAPR[num_symbols];        // 

    // modulate data symbols
    unsigned int j;
    unsigned int s;
    for (i=0; i<num_symbols; i++) {
        // preamble
        if      (i==0) ofdmframegen_write_S0a(fg, buffer); // S0 symbol (first)
        else if (i==1) ofdmframegen_write_S0b(fg, buffer); // S0 symbol (second)
        else if (i==2) ofdmframegen_write_S1( fg, buffer); // S1 symbol
        else {
            // data symbol
            for (j=0; j<M; j++) {
                s = modem_gen_rand_sym(mod);
                modem_modulate(mod,s,&X[j]);
            }
            // generate symbol
            ofdmframegen_writesymbol(fg, X, buffer);
        }

        // compute PAPR and print to screen
        PAPR[i] = ofdmframe_PAPR(buffer, frame_len);
        printf("%6u : PAPR=%12.8f\n", i, PAPR[i]);
    }

    // destroy objects
    ofdmframegen_destroy(fg);
    modem_destroy(mod);

    // 
    // export output file
    //

    // count subcarrier types
    unsigned int M_data  = 0;
    unsigned int M_pilot = 0;
    unsigned int M_null  = 0;
    ofdmframe_validate_sctype(p, M, &M_null, &M_pilot, &M_data);

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"M           = %u;\n", M);
    fprintf(fid,"M_data      = %u;\n", M_data);
    fprintf(fid,"M_pilot     = %u;\n", M_pilot);
    fprintf(fid,"M_null      = %u;\n", M_null);
    fprintf(fid,"cp_len      = %u;\n", cp_len);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);

    fprintf(fid,"PAPR = zeros(1,num_symbols);\n");
    for (i=0; i<num_symbols; i++)
        fprintf(fid,"  PAPR(%6u) = %12.4e;\n", i+1, PAPR[i]);

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hist(PAPR);\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"cdf = [(1:num_symbols)-0.5]/num_symbols;\n");
    fprintf(fid,"plot(sort(PAPR),cdf);\n");
    fprintf(fid,"xlabel('PAPR');\n");
    fprintf(fid,"ylabel('CDF');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

// compute peak-to-average power ratio
//  _x  :   input time series
//  _n  :   number of samples
float ofdmframe_PAPR(float complex * _x,
                     unsigned int    _n)
{
    float e;
    float e_mean = 0.0f;
    float e_max  = 0.0f;

    unsigned int i;
    for (i=0; i<_n; i++) {
        // compute |_x[i]|^2
        e = crealf( _x[i] * conjf(_x[i]) );

        e_mean += e;
        e_max   = (e > e_max) ? e : e_max;
    }

    e_mean = e_mean / (float)_n;

    return 10*log10f(e_max / e_mean);
}

