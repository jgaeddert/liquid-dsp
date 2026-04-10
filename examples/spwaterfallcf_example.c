const char __docstr__[] = "Spectrum waterfall example.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filebase, "spwaterfallcf_waterfall_example", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, nfft,           1024, 'n', "spectral periodogram FFT size", NULL);
    liquid_argparse_add(unsigned, time,           1000, 't', "minimum time buffer", NULL);
    liquid_argparse_add(unsigned, num_samples, 2000000, 'N', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // create spectral waterfall object
    spwaterfallcf periodogram = spwaterfallcf_create_default(nfft,time);
    spwaterfallcf_print(periodogram);

    // create stream generator
    msourcecf gen = msourcecf_create_default();
    
    // add noise source (wide band)
    msourcecf_add_noise(gen, 0.0f, 1.00f, -40);

    // add noise source (narrow-band, pulsed)
    int id_noise = msourcecf_add_noise(gen, 0.4f, 0.10f, -20);

    // add tone
    msourcecf_add_tone(gen, -0.4f, 0.0f, 0);

    // add fsk modem
    msourcecf_add_fsk(gen, -0.35f, 0.03f, -30.0f, 2, 500);

    // add chirp signal
    msourcecf_add_chirp(gen, 0.17f, 0.10f, -50, 250e3, 0, 0);

    // add modulated data
    msourcecf_add_modem(gen, -0.1f, 0.15f, -10, LIQUID_MODEM_QPSK, 12, 0.25);

    // create buffers
    unsigned int  buf_len = 64;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);

    // generate signals and push through spwaterfall object
    unsigned int total_samples   = 0;
    int state = 1;
    while (total_samples < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);

        // push resulting samples through waterfall object
        spwaterfallcf_write(periodogram, buf, buf_len);

        // accumulated samples
        total_samples += buf_len;

        // update state for noise source
        if (state == 0 && randf() < 1e-3f) {
            state = 1;
            msourcecf_enable(gen, id_noise);
            //printf("turning noise on\n");
        } else if (state == 1 && randf() < 3e-3f) {
            state = 0;
            msourcecf_disable(gen, id_noise);
            //printf("turning noise off\n");
        }
    }
    // export output files
    spwaterfallcf_set_rate    (periodogram,100e6);
    spwaterfallcf_set_freq    (periodogram,750e6);
    spwaterfallcf_set_dims    (periodogram,1200, 800);
    spwaterfallcf_set_commands(periodogram,"set cbrange [-45:25]; set title 'waterfall'");
    spwaterfallcf_export(periodogram,filebase);

    // destroy objects
    msourcecf_destroy(gen);
    spwaterfallcf_destroy(periodogram);

    printf("done.\n");
    return 0;
}


