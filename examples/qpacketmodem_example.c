const char __docstr__[] =
"This example demonstrates the basic packet modem encoder/decoder"
" operation. A packet of data is encoded and modulated into symbols,"
" channel noise is added, and the resulting packet is demodulated"
" and decoded.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qpacketmodem_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    mod,      "qpsk", 'm', "FEC scheme", liquid_argparse_modem);
    liquid_argparse_add(char*,    crc,     "crc32", 'v', "CRC scheme", liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0,      "none", 'c', "FEC scheme (inner)", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,      "none", 'k', "FEC scheme (outer)", liquid_argparse_fec);
    liquid_argparse_add(unsigned, payload_len, 480, 'n', "data length (bytes)", NULL);
    liquid_argparse_add(float,    SNRdB,        20, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    modulation_scheme ms    = (modulation_scheme)liquid_getopt_str2mod(mod);
    crc_scheme        check = (crc_scheme)liquid_getopt_str2crc(crc);
    fec_scheme        fec0  = (fec_scheme)liquid_getopt_str2fec(fs0);
    fec_scheme        fec1  = (fec_scheme)liquid_getopt_str2fec(fs1);

    unsigned int i;

    // derived values
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // create and configure packet encoder/decoder object
    qpacketmodem q = qpacketmodem_create();
    qpacketmodem_configure(q, payload_len, check, fec0, fec1, ms);
    qpacketmodem_print(q);

    // initialize payload
    LIQUID_VLA(unsigned char, payload_tx, payload_len);
    LIQUID_VLA(unsigned char, payload_rx, payload_len);

    // initialize payload
    for (i=0; i<payload_len; i++) {
        payload_tx[i] = rand() & 0xff;
        payload_rx[i] = 0x00;
    }

    // get frame length
    unsigned int frame_len = qpacketmodem_get_frame_len(q);

    // allocate memory for frame samples
    LIQUID_VLA(liquid_float_complex, frame_tx, frame_len);
    LIQUID_VLA(liquid_float_complex, frame_rx, frame_len);

    // encode frame
    qpacketmodem_encode(q, payload_tx, frame_tx);

    // add noise
    for (i=0; i<frame_len; i++)
        frame_rx[i] = frame_tx[i] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

    // decode frame
    int crc_pass = qpacketmodem_decode(q, frame_rx, payload_rx);

    // count errors
    unsigned int num_bit_errors = count_bit_errors_array(payload_tx, payload_rx, payload_len);

    // print results
    printf("payload pass ? %s, errors: %u / %u\n",
            crc_pass ? "pass" : "FAIL",
            num_bit_errors,
            8*payload_len);

    // destroy allocated objects
    qpacketmodem_destroy(q);

    // write symbols to output file for plotting
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: could not open '%s' for writing\n", filename);
        return -1;
    }
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"frame_len = %u;\n", frame_len);
    fprintf(fid,"y = zeros(1,frame_len);\n");
    for (i=0; i<frame_len; i++)
        fprintf(fid,"y(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));
    fprintf(fid,"figure('Color','white');\n");
    fprintf(fid,"plot(real(y),imag(y),'x','MarkerSize',3);\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('real');\n");
    fprintf(fid,"ylabel('imag');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}

