#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    printf("  header (%s)\n", _header_valid ? "valid" : "INVALID");
    printf("  payload (%s)\n", _payload_valid ? "valid" : "INVALID");

    // de-reference frame counter and increment
    unsigned int * counter = (unsigned int *) _userdata;
    *counter += 1;

    return 0;
}

int main() {
    // options
    unsigned int m=3;               // filter length (symbols)
    float beta=0.7f;                // filter excess bandwidth factor
    float phi=0.3f;                 // carrier phase offset
    float dphi=0.02f;               // carrier frequency offset
    float SNRdB = 10.0f;            // signal-to-noise ratio [dB]
    float noise_floor = -40.0f;     // noise floor [dB]
    unsigned int frame_counter = 0; // userdata passed to callback

    // allocate memory for arrays
    unsigned char header[12];       // data header
    unsigned char payload[64];      // data payload
    float complex frame_rx[1280];   // frame samples

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
    framegen64_execute(fg, header, payload, frame_rx);

    // add channel impairments (attenuation, carrier offset, noise)
    float nstd  = powf(10.0f, noise_floor*0.1f);        // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)*0.1f);// channel gain
    for (i=0; i<1280; i++) {
        frame_rx[i] *= gamma;
        frame_rx[i] *= cexpf(_Complex_I*(phi + i*dphi));
        frame_rx[i] += nstd * randnf()*cexpf(_Complex_I*M_PI*randf());
    }

    // EXECUTE synchronizer and receive the frame
#if 0
    // receive entire frame at once
    framesync64_execute(fs, frame_rx, 1280);
#else
    // receive the frame one sample at a time
    for (i=0; i<1280; i++)
        framesync64_execute(fs, &frame_rx[i], 1);
#endif

    // DESTROY objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    printf("received %u frames\n", frame_counter);
    printf("done.\n");
    return 0;
}
