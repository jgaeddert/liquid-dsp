//
// asgram_example.c
//
// ASCII spectrogram example.  This example demonstrates the functionality
// of the ASCII spectrogram.  A sweeping complex sinusoid is generated and
// the resulting frequency response is printed to the screen.
//

#include <unistd.h> // usleep
#include <stdio.h>
#include <math.h>

#include "liquid.h"

int main() {
    // options
    unsigned int nfft=64;
    unsigned int num_frames = 100;
    unsigned int msdelay=100;

    // initialize objects
    float complex x[nfft];
    asgram q = asgram_create(x,nfft);
    asgram_set_scale(q,10);
    asgram_set_offset(q,20);

    unsigned int i,n;
    float theta=0.0f, dtheta=0.0f;
    float phi=0.0f, dphi = 0.001f;

    float maxval;
    float maxfreq;
    char ascii[nfft+1];
    ascii[nfft] = '\0'; // append null character to end of string
    for (n=0; n<num_frames; n++) {
        // generate data
        for (i=0; i<nfft; i++) {
            x[i] = 0.1f*cexpf(_Complex_I*theta);

            theta += dtheta;
            dtheta = 0.9f*M_PI*sinf(phi);
            phi += dphi;
        }

        // execute the spectrogram
        asgram_execute(q, ascii, &maxval, &maxfreq);

        // print the spectrogram
        printf(" > %s < pk%5.1fdB [%5.2f]\n", ascii, maxval, maxfreq);

        // sleep
        usleep(msdelay*1000);
    }

    asgram_destroy(q);
    printf("done.\n");
    return 0;
}

