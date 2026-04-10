const char __docstr__[] =
"Demonstrates the reconfigurability of the flexframegen and"
" flexframesync objects.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    //liquid_argparse_add(char*,    filename, "flexframesync_reconfig_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    noise_floor,-30, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,       20, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(unsigned, num_frames,   3, 'n', "number of frames", NULL);
    liquid_argparse_parse(argc,argv);

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    flexframegen fg = flexframegen_create(NULL);

    // frame data
    LIQUID_VLA(unsigned char, header, 14);
    unsigned char * payload = NULL;

    // create flexframesync object with default properties
    flexframesync fs = flexframesync_create(NULL,NULL);

    // channel
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    unsigned int i;
    // initialize header, payload
    for (i=0; i<14; i++)
        header[i] = i;

    // frame buffers, properties
    unsigned int  buf_len = 256;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);

    unsigned int j;
    for (j=0; j<num_frames; j++) {
        // configure frame generator properties
        unsigned int payload_len = (rand() % 256) + 1;   // random payload length
        fgprops.check            = LIQUID_CRC_NONE;      // data validity check
        fgprops.fec0             = LIQUID_FEC_NONE;      // inner FEC scheme
        fgprops.fec1             = LIQUID_FEC_NONE;      // outer FEC scheme
        fgprops.mod_scheme       = (rand() % 2) ? LIQUID_MODEM_QPSK : LIQUID_MODEM_QAM16;

        // reallocate memory for payload
        payload = (unsigned char*)realloc(payload, payload_len*sizeof(unsigned char));

        // initialize payload
        for (i=0; i<payload_len; i++)
            payload[i] = rand() & 0xff;

        // set properties and assemble the frame
        flexframegen_setprops(fg, &fgprops);
        flexframegen_assemble(fg, header, payload, payload_len);
        printf("frame %u, ", j);
        flexframegen_print(fg);

        // write the frame in blocks
        int frame_complete = 0;
        while (!frame_complete) {
            // write samples to buffer
            frame_complete = flexframegen_write_samples(fg, buf, buf_len);

            // add channel impairments (gain and noise)
            for (i=0; i<buf_len; i++)
                buf[i] = buf[i]*gamma + nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;

            // push through sync
            flexframesync_execute(fs, buf, buf_len);
        }

    } // num frames

    // print frame data statistics
    flexframesync_print(fs);

    // clean up allocated memory
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    free(payload);

    printf("done.\n");
    return 0;
}

