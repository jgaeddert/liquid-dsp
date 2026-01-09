char __docstr__[] =
"This example demonstrates the interfaces to the framegen64 and"
" framesync64 objects used to completely encapsulate data for"
" over-the-air transmission.  A 64-byte payload is generated, and then"
" encoded, modulated, and interpolated using the framegen64 object."
" The resulting complex baseband samples are corrupted with noise and"
" moderate carrier frequency and phase offsets before the framesync64"
" object attempts to decode the frame.  The resulting data are compared"
" to the original to validate correctness.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "liquid.h"
#include "liquid.argparse.h"

// static callback function
static int callback(const unsigned char *  _header,
                    int                    _header_valid,
                    const unsigned char *  _payload,
                    unsigned int           _payload_len,
                    int                    _payload_valid,
                    framesyncstats_s       _stats,
                    void *                 _userdata)
{
    printf("*** callback invoked ***\n");
    framesyncstats_print(&_stats);
    return 0;
}

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "framesync64_example.m", 'o', "output filename", NULL);
    liquid_argparse_parse(argc,argv);

    // create frame generator, synchronizer objects
    framegen64  fg = framegen64_create();
    framesync64 fs = framesync64_create(callback,NULL);

    // create buffer for the frame samples
    unsigned int frame_len = LIQUID_FRAME64_LEN; // fixed frame length
    float complex frame[frame_len];
    
    // generate the frame with random header and payload
    framegen64_execute(fg, NULL, NULL, frame);

    // add minor channel effects
    unsigned int i;
    for (i=0; i<frame_len; i++) {
        frame[i] *= cexpf(_Complex_I*(0.01f*i + 1.23456f));
        frame[i] *= 0.1f;
        frame[i] += 0.02f*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

    // synchronize/receive the frame
    framesync64_execute(fs, frame, frame_len);
    framesync64_print(fs);

    // clean up allocated objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
    
    // export results
    FILE* fid = fopen(filename, "w");
    fprintf(fid,"%% %s: auto-generated file\n", filename);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"frame_len   = %u;\n", frame_len);
    for (i=0; i<frame_len; i++)
        fprintf(fid,"y(%4u)=%12.4e+j*%12.4e;\n", i+1, crealf(frame[i]), cimagf(frame[i]));
    fprintf(fid,"t=0:(length(y)-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    printf("done.\n");
    return 0;
}
