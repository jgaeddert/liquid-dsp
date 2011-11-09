// file: doc/tutorials/framing_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

// user-defined static callback function
static int mycallback(unsigned char * _header,
                      int _header_valid,
                      unsigned char * _payload,
                      int _payload_valid,
                      framesyncstats_s _stats,
                      void * _userdata)
{
    printf("***** callback invoked!\n");
    printf("  header (%s)\n",  _header_valid  ? "valid" : "INVALID");
    printf("  payload (%s)\n", _payload_valid ? "valid" : "INVALID");

    // type-cast, de-reference, and increment frame counter
    unsigned int * counter = (unsigned int *) _userdata;
    (*counter)++;

    return 0;
}

int main() {
    // options
    unsigned int m=3;               // filter length (symbols)
    float beta=0.7f;                // filter excess bandwidth factor
    unsigned int frame_counter = 0; // userdata passed to callback
    float phase_offset=0.3f;        // carrier phase offset
    float frequency_offset=0.02f;   // carrier frequency offset
    float SNRdB = 10.0f;            // signal-to-noise ratio [dB]
    float noise_floor = -40.0f;     // noise floor [dB]

    // allocate memory for arrays
    unsigned char header[12];       // data header
    unsigned char payload[64];      // data payload
    float complex y[1280];          // frame samples

    // create frame generator
    framegen64 fg = framegen64_create(m,beta);
    framegen64_print(fg);

    // create frame synchronizer using default properties
    framesync64 fs = framesync64_create(NULL,
                                        mycallback,
                                        (void*)&frame_counter);
    framesync64_print(fs);

    // initialize header, payload
    unsigned int i;
    for (i=0; i<12; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // EXECUTE generator and assemble the frame
    framegen64_execute(fg, header, payload, y);

    // add channel impairments (attenuation, carrier offset, noise)
    float nstd  = powf(10.0f, noise_floor/20.0f);        // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f);// channel gain
    for (i=0; i<1280; i++) {
        y[i] *= gamma;
        y[i] *= cexpf(_Complex_I*(phase_offset + i*frequency_offset));
        y[i] += nstd * (randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

    // EXECUTE synchronizer and receive the frame one sample at a time
    for (i=0; i<1280; i++)
        framesync64_execute(fs, &y[i], 1);

    // DESTROY objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    printf("received %u frames\n", frame_counter);
    printf("done.\n");
    return 0;
}
