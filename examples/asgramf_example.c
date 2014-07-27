//
// asgramf_example.c
//
// ASCII spectrogram example.  This example demonstrates the functionality
// of the ASCII spectrogram.  A sweeping real-valued sinusoid is generated and
// the resulting frequency response is printed to the screen.
//

#include <unistd.h> // usleep
#include <stdio.h>
#include <math.h>

#include "liquid.h"

int main() {
    // options
    unsigned int nfft        =   64;    // transform size
    unsigned int num_frames  =  200;    // total number of frames
    unsigned int msdelay     =   50;    // delay between transforms [ms]
    float        noise_floor = -40.0f;  // noise floor

    // initialize objects
    asgramf q = asgramf_create(nfft);
    asgramf_set_scale(q, noise_floor+15.0f, 5.0f);

    unsigned int i;
    unsigned int n;
    float theta  = 0.0f;    // current instantaneous phase
    float dtheta = 0.0f;    // current instantaneous frequency
    float phi    = 0.0f;    // phase of sinusoidal frequency drift
    float dphi   = 0.003f;  // frequency of sinusoidal frequency drift

    float x[nfft];
    float nstd = powf(10.0f,noise_floor/20.0f);  // noise standard deviation
    for (n=0; n<num_frames; n++) {
        // generate a frame of data samples
        for (i=0; i<nfft; i++) {
            // cosine wave of time-varying frequency
            x[i] = cosf(theta);

            // add noise to signal
            x[i] += nstd * randnf();

            // adjust frequency and phase
            theta  += dtheta;
            dtheta =  0.5f*M_PI + 0.4f*M_PI*sinf(phi) * hamming(n, num_frames);
            phi    += dphi;
        }

        // push samples into the spectrogram object
        asgramf_push(q, x, nfft);

        // print the spectrogram to stdout
        asgramf_print(q);

        // sleep for some time before generating the next frame
        usleep(msdelay*1000);
    }

    asgramf_destroy(q);
    printf("done.\n");
    return 0;
}

