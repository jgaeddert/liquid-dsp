// waterfall example

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "spwaterfallcf_waterfall_example.gnu"

int main()
{
    // spectral periodogram options
    unsigned int nfft        = 1200;    // spectral periodogram FFT size
    unsigned int time        =  250;    // minimum time buffer
    unsigned int num_samples = 20e6;    // number of samples

    // create spectral waterfall object
    spwaterfallcf periodogram = spwaterfallcf_create_default(nfft,time);
    spwaterfallcf_print(periodogram);

    // create stream generator
    msourcecf gen = msourcecf_create();
    
    // add noise source (narrow-band)
    int id_noise = msourcecf_add_noise(gen, 0.10f);
    msourcecf_set_frequency(gen, id_noise, 0.4*2*M_PI);
    msourcecf_set_gain     (gen, id_noise, -20.0f);

    // add tone
    int id_tone = msourcecf_add_tone(gen);
    msourcecf_set_frequency(gen, id_tone, -0.4*2*M_PI);
    msourcecf_set_gain     (gen, id_tone, -10.0f);

    // add modulated data
    int id_modem = msourcecf_add_modem(gen,LIQUID_MODEM_QPSK,4,12,0.30f);
    msourcecf_set_frequency(gen, id_modem, -0.1*2*M_PI);
    msourcecf_set_gain     (gen, id_modem, 0.0f);

    // create buffers
    unsigned int  buf_len = 64;
    float complex buf[buf_len];

    // generate signals and push through spwaterfall object
    unsigned int total_samples   = 0;
    int state = 1;
    while (total_samples < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);

        // push resulting sample through periodogram
        spwaterfallcf_write(periodogram, buf, buf_len);

        // accumulated samples
        total_samples += buf_len;

        // update state for noise source
        if (state == 0 && randf() < 1e-4f) {
            state = 1;
            msourcecf_enable(gen, id_noise);
            //printf("turning noise on\n");
        } else if (state == 1 && randf() < 3e-4f) {
            state = 0;
            msourcecf_disable(gen, id_noise);
            //printf("turning noise off\n");
        }
    }
    // export output files
    spwaterfallcf_set_rate    (periodogram,100e6);
    spwaterfallcf_set_freq    (periodogram,750e6);
    spwaterfallcf_set_dims    (periodogram,1200, 800);
    spwaterfallcf_set_commands(periodogram,"set cbrange [-55:25]; set title 'waterfall'");
    spwaterfallcf_export(periodogram,"spwaterfallcf_example");

    // destroy objects
    msourcecf_destroy(gen);
    spwaterfallcf_destroy(periodogram);

    printf("done.\n");
    return 0;
}


