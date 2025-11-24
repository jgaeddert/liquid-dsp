char __docstr__[] =
"This example demonstrates the interfaces to the fskframegen and"
" fskframesync objects used to completely encapsulate data for"
" over-the-air transmission.";

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
                    void *                 _userdata);

// global arrays
unsigned char header[8];
unsigned char payload[200];

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "fskframesync_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    SNRdB,        20.0f, 'S', "signal-to-noise ratio", NULL);
    liquid_argparse_add(float,    noise_floor, -20.0f, '0', "noise floor", NULL);
    liquid_argparse_add(float,    dphi,         0.01f, 'F', "carrier frequency offset", NULL);
    liquid_argparse_add(float,    theta,         0.0f, 'P', "carrier phase offset", NULL);
    liquid_argparse_add(float,    dt,           -0.2f, 'T', "fractional sample timing offset", NULL);
    liquid_argparse_add(char*,    crc,        "crc32", 'v', "FEC scheme", liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0,         "none", 'c', "FEC scheme", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,         "none", 'k', "FEC scheme", liquid_argparse_fec);
    liquid_argparse_add(unsigned, payload_len,    480, 'n', "payload length (bytes)", NULL);
    liquid_argparse_add(bool,  debug_enabled,   false, 'D', "enable debugging", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    crc_scheme check = liquid_getopt_str2crc(crc);
    fec_scheme fec0  = liquid_getopt_str2fec(fs0);
    fec_scheme fec1  = liquid_getopt_str2fec(fs1);

    printf("channel offsets: dt=%.3f, dphi=%.3f, theta=%.3f\n", dt, dphi, theta);

    // derived values
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    // create frame generator
    fskframegen fg = fskframegen_create();
    fskframegen_print(fg);

    // create frame synchronizer using default properties
    fskframesync fs = fskframesync_create(callback,NULL);
    fskframesync_print(fs);
    if (debug_enabled)
        fskframesync_debug_enable(fs);

    // data payload
    unsigned int i;
    // initialize header and payload data
    for (i=0; i<8; i++)
        header[i] = i;
    for (i=0; i<200; i++)
        payload[i] = rand() & 0xff;

    // allocate memory for the frame samples
    unsigned int  buf_len = 64;
    float complex buf_tx[buf_len];  // receive buffer
    float complex buf_rx[buf_len];  // transmit buffer
    
    // assemble the frame
    fskframegen_assemble(fg, header, payload, payload_len, check, fec0, fec1);

    // spectral periodogram
    unsigned int nfft  = 4200;
    spgramcf periodogram = spgramcf_create_default(nfft);

    // write frame in blocks
    int frame_complete = 0;
    while (!frame_complete)
    {
        frame_complete = fskframegen_write_samples(fg, buf_tx, buf_len);

        // add noise, channel gain
        for (i=0; i<buf_len; i++)
            buf_rx[i] = buf_tx[i]*gamma + nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;

        // synchronize/receive the frame
        fskframesync_execute_block(fs, buf_rx, buf_len);
        
        // estimate power spectral density
        spgramcf_write(periodogram, buf_rx, buf_len);
    }

    // compute power spectral density of received signal
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // clean up allocated objects
    spgramcf_destroy(periodogram);
    fskframegen_destroy(fg);
    fskframesync_destroy(fs);

    // 
    // export results
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"nfft        = %u;\n", nfft);

    // save power spectral density
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%4u) = %12.8f;\n", i+1, psd[i]);

    // plot PSD
    fprintf(fid,"figure('Color','white');\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"plot(f,psd,'LineWidth',1.5,'Color',[0.5 0 0]);\n");
    fprintf(fid,"axis([-0.5 0.5 -30 30]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}

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
    printf("    error vector mag.   : %12.8f dB\n", _stats.evm);
    printf("    rssi                : %12.8f dB\n", _stats.rssi);
    printf("    carrier offset      : %12.8f\n", _stats.cfo);
    printf("    mod. scheme         : %s\n", modulation_types[_stats.mod_scheme].fullname);
    printf("    mod. depth          : %u\n", _stats.mod_bps);
    printf("    payload CRC         : %s\n", crc_scheme_str[_stats.check][1]);
    printf("    payload fec (inner) : %s\n", fec_scheme_str[_stats.fec0][1]);
    printf("    payload fec (outer) : %s\n", fec_scheme_str[_stats.fec1][1]);
    printf("    header crc          : %s\n", _header_valid ? "pass" : "FAIL");
    printf("    header data         : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
            _header[0], _header[1], _header[2], _header[3],
            _header[4], _header[5], _header[6], _header[7]);
    printf("    num header errors   : %u / %u\n",
            count_bit_errors_array(_header, header, 8),
            8*8);
    printf("    payload crc         : %s\n", _payload_valid ? "pass" : "FAIL");
    printf("    num payload errors  : %u / %u\n",
            count_bit_errors_array(_payload, payload, 64),
            64*8);

    return 0;
}

