const char __docstr__[] = "Demonstrate using qpilotsync for carrier recovery.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qpilotsync_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    mod_str,  "qpsk", 'm', "FEC scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, payload_len, 480, 'n', "data length [bytes]", NULL);
    liquid_argparse_add(unsigned, pilot_spacing,20, 'p', "pilot spacing [symbols]", NULL);
    liquid_argparse_add(float,    SNRdB,        20, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    dphi,     -0.007, 'F', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    phi,       2.180, 'P', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    gain,      0.500, 'g', "channel gain", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // create pilot generator and synchronizer objects
    qpilotgen  pg = qpilotgen_create( payload_len, pilot_spacing);
    qpilotsync ps = qpilotsync_create(payload_len, pilot_spacing);
    qpilotgen_print(pg);

    // get frame length
    unsigned int frame_len = qpilotgen_get_frame_len(pg);

    // allocate arrays
    LIQUID_VLA(unsigned char, payload_sym_tx, payload_len);  // transmitted payload symbols
    LIQUID_VLA(liquid_float_complex, payload_tx, payload_len);  // transmitted payload samples
    LIQUID_VLA(liquid_float_complex, frame_tx, frame_len);    // transmitted frame samples
    LIQUID_VLA(liquid_float_complex, frame_rx, frame_len);    // received frame samples
    LIQUID_VLA(liquid_float_complex, payload_rx, payload_len);  // received payload samples
    LIQUID_VLA(unsigned char, payload_sym_rx, payload_len);  // received payload symbols

    // create modem objects for payload
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_str);
    modemcf mod = modemcf_create(ms);
    modemcf dem = modemcf_create(ms);

    // assemble payload symbols
    unsigned int i;
    for (i=0; i<payload_len; i++) {
        // generate random symbol
        payload_sym_tx[i] = modemcf_gen_rand_sym(mod);

        // modulate
        modemcf_modulate(mod, payload_sym_tx[i], &payload_tx[i]);
    }

    // assemble frame
    qpilotgen_execute(pg, payload_tx, frame_tx);

    // add channel impairments
    for (i=0; i<frame_len; i++) {
        frame_rx[i]  = frame_tx[i] * cexpf(_Complex_I*dphi*i + _Complex_I*phi);
        frame_rx[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
        frame_rx[i] *= gain;
    }

    // receive frame
    qpilotsync_execute(ps, frame_rx, payload_rx);

    // demodulate
    for (i=0; i<payload_len; i++) {
        unsigned int sym_demod;
        modemcf_demodulate(dem, payload_rx[i], &sym_demod);
        payload_sym_rx[i] = (unsigned char)sym_demod;
    }

    // count errors
    unsigned int bit_errors = 0;
    for (i=0; i<payload_len; i++)
        bit_errors += count_bit_errors(payload_sym_rx[i], payload_sym_tx[i]);
    printf("received bit errors : %u / %u\n", bit_errors, payload_len * modemcf_get_bps(mod));

    // destroy allocated objects
    qpilotgen_destroy(pg);
    qpilotsync_destroy(ps);
    modemcf_destroy(mod);
    modemcf_destroy(dem);

    // write symbols to output file for plotting
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: could not open '%s' for writing\n", filename);
        return -1;
    }
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"payload_len = %u;\n", payload_len);
    fprintf(fid,"frame_len   = %u;\n", frame_len);
    fprintf(fid,"frame_tx   = zeros(1,frame_len);\n");
    fprintf(fid,"payload_rx = zeros(1,payload_len);\n");
    for (i=0; i<frame_len; i++)
        fprintf(fid,"frame_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));
    for (i=0; i<payload_len; i++)
        fprintf(fid,"payload_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(payload_rx[i]), cimagf(payload_rx[i]));
    fprintf(fid,"figure('Color','white','position',[100 100 950 400]);\n");
    fprintf(fid,"subplot(1,2,1);\n");
    fprintf(fid,"  plot(real(frame_rx),  imag(frame_rx),  'x','MarkerSize',3);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  title('received');\n");
    fprintf(fid,"subplot(1,2,2);\n");
    fprintf(fid,"  plot(real(payload_rx),imag(payload_rx),'x','MarkerSize',3);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  title('recovered');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}

