const char __docstr__[] =
"Generate continuous signals, decompose with analysis channelizer, show spectra.";

#include <stdio.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firpfbch_crcf_msource_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_channels,     5, 'M', "number of channels", NULL);
    liquid_argparse_add(unsigned, m,               12, 'm', "filter length [symbols]", NULL);
    liquid_argparse_add(unsigned, num_samples, 512000, 'n', "number of samples", NULL);
    liquid_argparse_add(unsigned, nfft,          1200, 'N', "FFT size", NULL);
    liquid_argparse_parse(argc,argv);

    // data arrays
    unsigned int i;
    LIQUID_VLA(liquid_float_complex, buf_0, num_channels); // time-domain input
    LIQUID_VLA(liquid_float_complex, buf_1, num_channels); // channelized output

    // create filterbank channelizer object using external filter coefficients
    firpfbch_crcf q = firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, num_channels, m, 0.5f,
            LIQUID_FIRFILT_ARKAISER);

    // create multi-signal source generator
    msourcecf gen = msourcecf_create_default();
    // add signals     (gen,  fc,   bw,    gain, {options})
    msourcecf_add_noise(gen,  0.00f,1.00f, -40);               // noise floor
    msourcecf_add_noise(gen,  0.02f,0.05f,   0);               // narrow-band noise
    msourcecf_add_tone (gen, -0.40f,0.00f,  20);               // tone
    msourcecf_add_modem(gen,  0.20f,0.10f,   0, LIQUID_MODEM_QPSK, 12, 0.2f);  // modulated data (linear)
    msourcecf_add_gmsk (gen, -0.20f,0.05f,   0, 4, 0.3f);        // modulated data (GMSK)

    // create objects for computing spectra
    spgramcf psd_0 = spgramcf_create_default(nfft);
    LIQUID_VLA(spgramcf, psd_1, num_channels);
    for (i=0; i<num_channels; i++)
        psd_1[i] = spgramcf_create_default(nfft);

    // generate signals
    while (msourcecf_get_num_samples(gen) < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf_0, num_channels);

        // run through channelizer
        firpfbch_crcf_analyzer_execute(q, buf_0, buf_1);

        // push resulting sample through periodogram objects
        spgramcf_write(psd_0, buf_0, num_channels);
        for (i=0; i<num_channels; i++)
            spgramcf_push(psd_1[i], buf_1[i]);
    }

    // export results to file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_channels = %u;\n", num_channels);
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"%% input signal spectrum\n");
    fprintf(fid,"X = zeros(1,nfft);\n");
    LIQUID_VLA(float, psd, nfft);
    spgramcf_get_psd(psd_0, psd);
    for (i=0; i<nfft; i++)
        fprintf(fid,"  X(%6u) = %12.4e;\n", i+1, psd[i]);
    fprintf(fid,"%% channelized output signal spectra\n");
    fprintf(fid,"Y = zeros(num_channels, nfft);\n");
    // save channelized output signals
    for (i=0; i<num_channels; i++) {
        spgramcf_get_psd(psd_1[i], psd);
        fprintf(fid,"Y(%u,:) = [", i+1);
        unsigned int k;
        for (k=0; k<nfft; k++) {
            fprintf(fid,"%6g,", psd[k]);
        }
        fprintf(fid,"];\n");
    }

    // plot results
    fprintf(fid,"\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure('color','white','position',[100 100 1200 600]);\n");
    fprintf(fid,"subplot(2,num_channels,1:num_channels);\n");
    fprintf(fid,"  plot(f, X, 'Color', [0 0.5 0.25], 'LineWidth', 2);\n");
    fprintf(fid,"  axis([-0.5 0.5 -50 30]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Input PSD [dB]');\n");

    fprintf(fid,"for i=1:num_channels\n");
    fprintf(fid,"  k = mod((i-1) + ceil(num_channels/2),num_channels) + 1;\n");
    fprintf(fid,"  subplot(2,num_channels,num_channels+i);\n");
    fprintf(fid,"    plot(f, Y(k,:)-10*log10(num_channels),'Color',[0.25 0 0.25],'LineWidth', 1.5);\n");
    fprintf(fid,"    axis([-0.5 0.5 -50 30]);\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"    title(['Channel ' num2str(k-1)]);\n");
    fprintf(fid,"end;\n");
    fclose(fid);
    printf("results written to %s\n", filename);

    // destroy objects
    msourcecf_destroy(gen);
    firpfbch_crcf_destroy(q);
    spgramcf_destroy(psd_0);
    for (i=0; i<num_channels; i++)
        spgramcf_destroy(psd_1[i]);

    printf("done.\n");
    return 0;
}

