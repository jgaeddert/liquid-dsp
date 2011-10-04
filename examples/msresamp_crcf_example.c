//
// msresamp_crcf_example.c
//
// Demonstration of the multi-stage arbitrary resampler
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "msresamp_crcf_example.m"

int main() {
    // options
    float r=173.9f;         // resampling rate (output/input)
    float As=60.0f;         // resampling filter stop-band attenuation [dB]
    unsigned int n=128;     // number of input samples
    float fc=0.079f;        // complex sinusoid frequency

    // create resampler
    msresamp_crcf q = msresamp_crcf_create(r,As);
    msresamp_crcf_print(q);

    // clean up allocated objects
    msresamp_crcf_destroy(q);

    printf("done.\n");
    return 0;
}
