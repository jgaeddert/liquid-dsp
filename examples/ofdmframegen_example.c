const char __docstr__[] = "Example demonstrating the OFDM frame generator.";

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "ofdmframegen_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_subcarriers,64, 'M', "number of subcarriers", NULL);
    liquid_argparse_add(unsigned, cp_len,         16, 'C', "cyclic prefix length", NULL);
    liquid_argparse_add(unsigned, taper_len,       4, 'T', "taper length", NULL);
    liquid_argparse_parse(argc,argv);

    // 
    unsigned int frame_len = num_subcarriers + cp_len;

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(num_subcarriers, cp_len, taper_len, NULL);
    ofdmframegen_print(fg);

    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_subcarriers=%u;\n", num_subcarriers);
    fprintf(fid,"cp_len=%u;\n", cp_len);
    fprintf(fid,"frame_len=%u;\n", frame_len);

    fprintf(fid,"X = zeros(1,num_subcarriers);\n");
    fprintf(fid,"x = zeros(1,frame_len);\n");

    unsigned int i;
    LIQUID_VLA(liquid_float_complex, X, num_subcarriers);   // channelized symbols
    LIQUID_VLA(liquid_float_complex, x, frame_len);         // time-domain samples

    for (i=0; i<num_subcarriers; i++) {
        X[i] = i==4 ? 0.707f + _Complex_I*0.707f : 0.0f;
    }

    ofdmframegen_writesymbol(fg,X,x);

    //
    for (i=0; i<num_subcarriers; i++)
        fprintf(fid,"X(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(X[i]), cimagf(X[i]));

    //
    for (i=0; i<frame_len; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(frame_len-1);\n");
    fprintf(fid,"plot(t,real(x),t,imag(x));\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    // destroy objects
    ofdmframegen_destroy(fg);

    printf("done.\n");
    return 0;
}

