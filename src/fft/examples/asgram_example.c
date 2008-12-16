//
// asgram_example.c
//

#include <unistd.h> // usleep
#include <stdio.h>
#include <math.h>

#include "../src/fft.h"

int main() {
    // options
    unsigned int nfft=64;
    unsigned int num_frames = 100;
    unsigned int msdelay=100;

    // initialize objects
    float complex x[nfft];
    asgram q = asgram_create(x,nfft);

    unsigned int i,n;
    float theta=0.0f, dtheta=0.0f;
    float phi=0.0f, dphi = 0.001f;
    for (n=0; n<num_frames; n++) {
        // generate data
        for (i=0; i<nfft; i++) {
            x[i] = 0.1f*cexpf(_Complex_I*theta);

            theta += dtheta;
            dtheta = 0.9f*M_PI*sinf(phi);
            phi += dphi;
        }

        // execute the spectrogram
        asgram_execute(q,x);

        // sleep
        usleep(msdelay*1000);
    }

    asgram_destroy(q);
    printf("done.\n");
    return 0;
}

