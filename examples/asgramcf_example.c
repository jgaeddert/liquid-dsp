const char __docstr__[] =
"ASCII spectrogram example for complex inputs. This example demonstrates"
" the functionality of the ASCII spectrogram. A sweeping complex sinusoid"
" is generated and the resulting spectral periodogram is printed to the"
" screen.";

#include <unistd.h> // usleep
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(unsigned, nfft,         64, 'n', "transform size", NULL);
    liquid_argparse_add(unsigned, num_frames,  200, 'N', "total number of frames", NULL);
    liquid_argparse_add(unsigned, msdelay,      25, 'd', "delay between transforms [ms]", NULL);
    liquid_argparse_add(float,    noise_floor, -80, 'f', "noise floor", NULL);
    liquid_argparse_add(bool,     autoscale,     0, 'a', "enable autoscale", NULL);
    liquid_argparse_parse(argc,argv);

    // initialize object
    asgramcf q = asgramcf_create(nfft);
    if (!autoscale) // enabled by default
        asgramcf_autoscale_disable(q);
    asgramcf_set_scale(q, noise_floor+50, 5.0f);

    unsigned int i;
    unsigned int n;
    float theta  = 0.0f;    // current instantaneous phase
    float dtheta = 0.0f;    // current instantaneous frequency
    float phi    = 0.0f;    // phase of sinusoidal frequency drift
    float dphi   = 0.003f;  // frequency of sinusoidal frequency drift

    LIQUID_VLA(liquid_float_complex, x, nfft);
    float nstd = powf(10.0f,noise_floor/20.0f);  // noise standard deviation
    for (n=0; n<num_frames; n++) {
        // generate a frame of data samples
        for (i=0; i<nfft; i++) {
            // complex exponential
            x[i] = cexpf(_Complex_I*theta);

            // add noise to signal
            x[i] += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;

            // adjust frequency and phase
            theta  += dtheta;
            dtheta =  0.9f*M_PI*sinf(phi) * liquid_hamming(n, num_frames);
            phi    += dphi;
        }

        // write block of samples to the spectrogram object
        asgramcf_write(q, x, nfft);

        // print result to screen
        asgramcf_print(q);

        // sleep for some time before generating the next frame
        usleep(msdelay*1000);
    }

    asgramcf_destroy(q);
    printf("done.\n");
    return 0;
}

