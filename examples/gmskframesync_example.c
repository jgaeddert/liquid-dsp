char __docstr__[] = "Example demonstrating the GMSK flexible frame synchronizer.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "liquid.h"
#include "liquid.argparse.h"

// callback function
int callback(const unsigned char *  _header,
             int                    _header_valid,
             const unsigned char *  _payload,
             unsigned int           _payload_len,
             int                    _payload_valid,
             framesyncstats_s       _stats,
             void *                 _userdata)
{
    printf("***** gmskframesync callback invoked *****\n");
    return 0;
}

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "gmskframesync_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    crc,     "crc32",   'v', "CRC scheme",         liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0,      "none",   'c', "FEC scheme (inner)", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,      "none",   'k', "FEC scheme (outer)", liquid_argparse_fec);
    liquid_argparse_add(unsigned, k,            2,    'M', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,            3,    'm', "filter delay (symbols)", NULL);
    liquid_argparse_add(float,    BT,           0.5f, 'b', "filter bandwidth-time product", NULL);
    liquid_argparse_add(unsigned, payload_len,  40,   'n', "length of payload (bytes)", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    crc_scheme        check = liquid_getopt_str2crc(crc);
    fec_scheme        fec0  = liquid_getopt_str2fec(fs0);
    fec_scheme        fec1  = liquid_getopt_str2fec(fs1);

    unsigned int i;

    // allocate memory for payload and initialize
    unsigned char header[8] = {0,1,2,3,4,5,6,7};
    unsigned char payload[payload_len];
    memset(payload, 0x00, payload_len);

    // create frame generator and assemble
    gmskframegen fg = gmskframegen_create_set(k, m, BT);
    gmskframegen_assemble(fg, header, payload, payload_len, check, fec0, fec1);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create_set(k, m, BT, callback, NULL);

    // allocate buffer for storing entire frame
    unsigned int num_samples = gmskframegen_getframelen(fg) + 800;
    float complex buf[num_samples];
    memset(buf, 0x00, num_samples*sizeof(float complex));

    // generate frame in one shot with sample offset
    gmskframegen_write(fg, buf+250, num_samples-250);

    // add channel gain and noise
    for (i=0; i<num_samples; i++)
        buf[i] = 0.3*buf[i] + 0.01f*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;

    // push samples through synchronizer
    gmskframesync_execute(fs, buf, num_samples);
    gmskframesync_print(fs);

    // destroy objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);

    // export output
    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,num_samples);\n");
    fprintf(fid,"\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%6u) = %12.4e + j*%12.4e;\n", i+1, crealf(buf[i]), cimagf(buf[i]));
    fprintf(fid,"\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, real(y), t,imag(y));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('received signal');\n");
    fprintf(fid,"legend('real','imag');\n");
    fclose(fid);
    printf("results written to '%s'\n", filename);
    return 0;
}

