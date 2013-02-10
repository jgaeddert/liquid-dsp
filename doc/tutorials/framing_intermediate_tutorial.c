// file: doc/tutorials/framing_intermediate_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

// user-defined static callback function
static int mycallback(unsigned char *  _header,
                      int              _header_valid,
                      unsigned char *  _payload,
                      unsigned int     _payload_len,
                      int              _payload_valid,
                      framesyncstats_s _stats,
                      void *           _userdata)
{
    printf("***** callback invoked!\n");
    return 0;
}

int main() {
    // allocate memory for arrays
    unsigned char header[8];        // data header
    unsigned char payload[64];      // data payload
    float complex y[1340];          // frame samples

    // create frame generator
    framegen64 fg = framegen64_create();
    framegen64_print(fg);

    // create frame synchronizer using default properties
    framesync64 fs = framesync64_create(mycallback, NULL);
    framesync64_print(fs);

    // initialize header, payload
    unsigned int i;
    for (i=0; i<8; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // EXECUTE generator and assemble the frame
    framegen64_execute(fg, header, payload, y);

    // EXECUTE synchronizer and receive the entire frame at once
    framesync64_execute(fs, y, 1340);

    // DESTROY objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    printf("done.\n");
    return 0;
}
