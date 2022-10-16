// This example demonstrates the basic interface to the dsssframe64gen and
// dsssframe64sync objects.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>

#include "liquid.h"

// static callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    printf("*** callback invoked (%s)***\n", _payload_valid ? "pass" : "FAIL");
    framesyncstats_print(&_stats);
    return 0;
}

int main(int argc, char *argv[])
{
    // options
    unsigned int nfft=2400;

    // create dsssframe64gen object
    dsssframe64gen fg = dsssframe64gen_create();

    // generate the frame in blocks
    unsigned int  buf_len = dsssframe64gen_get_frame_len(fg);
    float complex buf_tx[buf_len];  // transmit buffer
    float complex buf_rx[buf_len];  // receive buffer (channel output)

    // create spectral periodogram for displaying spectrum
    spgramcf periodogram = spgramcf_create_default(nfft);

    // generate in one step (for now)
    dsssframe64gen_assemble(fg, NULL, NULL);
    dsssframe64gen_write(fg, buf_tx, buf_len);

    // TODO: apply channel
    memmove(buf_rx, buf_tx, buf_len*sizeof(float complex));

    // run through sync
    dsssframe64sync fs = dsssframe64sync_create(callback, NULL);
    dsssframe64sync_execute(fs, buf_rx, buf_len);
    dsssframe64sync_destroy(fs);

    // push resulting sample through periodogram
    spgramcf_write(periodogram, buf_tx, buf_len);
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // export results
    const char * filename = "dsssframe64sync_example.m";
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all;\n");
    unsigned int i;
#if 0
    fprintf(fid,"n=%u; y=zeros(1,n);\n", buf_len);
    for (i=0; i<buf_len; i++)
        fprintf(fid,"y(%7u) = %12.8f + %12.8fj;\n", i+1, crealf(buf_tx[i]), cimagf(buf_tx[i]));
#endif
    fprintf(fid,"nfft=%u; Y=zeros(1,nfft);\n", nfft);
    for (i=0; i<nfft; i++)
        fprintf(fid,"Y(%4u) = %12.8f;\n", i+1, psd[i]);
    fprintf(fid,"figure; f=[0:(nfft-1)]/nfft-0.5; plot(f,Y); xlim([-0.5 0.5]); grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/Fs]'); ylabel('PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s\n", filename);

    // destroy allocated objects
    dsssframe64gen_destroy(fg);
    spgramcf_destroy(periodogram);
    return 0;
}

