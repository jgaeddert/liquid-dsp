const char __docstr__[] =
"Demonstrate partial-band reconstruction using firpfbch2_crcf object.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firpfbch2_crcf_reconstruct_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,            64, 'M', "number of channels in analysis filterbank", NULL);
    liquid_argparse_add(unsigned, P,            22, 'P', "number of channels in synthesis filterbank", NULL);
    liquid_argparse_add(unsigned, m,             5, 'm', "filter length [symbols]", NULL);
    liquid_argparse_add(float,    As,           60, 'a', "filter stop-band attenuation", NULL);
    liquid_argparse_add(float,    fc,         0.23, 'f', "frequency in band to center synthesis bank", NULL);
    liquid_argparse_add(unsigned, num_blocks,1<<14, 'n', "number of symbols", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int M2=M/2, P2=P/2;    // channelizer half sizes, for convenience
    unsigned int channel_id = (unsigned int)(roundf(fc*M)) % M; // index corresponding to fc
    unsigned int i;

    // create filterbank objects from prototype
    firpfbch2_crcf qa = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER,    M, m, As);
    firpfbch2_crcf qs = firpfbch2_crcf_create_kaiser(LIQUID_SYNTHESIZER, P, m, As);

    // create multi-signal source generator
    msourcecf gen = msourcecf_create_default();

    // add signals     (gen,  fc,    bw,    gain, {options})
    msourcecf_add_noise(gen,  0.00f, 1.0f, -60);   // wide-band noise
    msourcecf_add_noise(gen, -0.30f, 0.1f, -20);   // narrow-band noise
    msourcecf_add_tone (gen,  0.08f, 0.0f,   0);   // tone
    // modulated data
    msourcecf_add_modem(gen,
       0.18f,                      // center frequency
       0.080f,                     // bandwidth (symbol rate)
       -20,                        // gain
       LIQUID_MODEM_QPSK,          // modulation scheme
       12,                         // filter semi-length
       0.3f);                      // modem parameters
    msourcecf_add_tone (gen,  0.24f, 0.0f, -40);   // another tone

    // create spectral periodogoram
    unsigned int nfft = 2400;
    spgramcf     p0   = spgramcf_create_default(nfft); // original spectrum
    spgramcf     p1   = spgramcf_create_default(nfft); // reconstructed spectrum

    // run channelizer
    LIQUID_VLA(liquid_float_complex, buf_a_time, M2);   // analysis, time
    LIQUID_VLA(liquid_float_complex, buf_a_freq, M);    // analysis, frequency
    LIQUID_VLA(liquid_float_complex, buf_s_freq, P);    // synthesis, frequency
    LIQUID_VLA(liquid_float_complex, buf_s_time, P2);   // synthesis, time
    for (i=0; i<num_blocks; i++) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf_a_time, M2);

        // run analysis filterbank
        firpfbch2_crcf_execute(qa, buf_a_time, buf_a_freq);

        // pull relevant samples, applying FFT shift on the input
        for (unsigned int k=0; k<P; k++)
            buf_s_freq[(k+P2)%P] = buf_a_freq[ (M+channel_id+k-P2) % M ];
        //buf_s_freq[P2] = 0.0f; // optionally disable high-frequency crossover point

        // run synthesis filterbank
        firpfbch2_crcf_execute(qs, buf_s_freq, buf_s_time);

        // push results through periodograms
        spgramcf_write(p0, buf_a_time, M2);
        spgramcf_write(p1, buf_s_time, P2);
    }
    spgramcf_print(p0);
    spgramcf_print(p1);

    // compute power spectral density output
    LIQUID_VLA(float, psd_0, nfft);
    LIQUID_VLA(float, psd_1, nfft);
    spgramcf_get_psd(p0, psd_0);
    spgramcf_get_psd(p1, psd_1);

    // destroy objects
    firpfbch2_crcf_destroy(qa);
    firpfbch2_crcf_destroy(qs);
    msourcecf_destroy(gen);
    spgramcf_destroy(p0);
    spgramcf_destroy(p1);

    //
    // EXPORT DATA TO FILE
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"P = %u;\n", P);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"ch = %u;\n", channel_id);
    fprintf(fid,"f  = [0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"f0 = f;\n");
    fprintf(fid,"f1 = f*P/M + %d/M;\n", channel_id < M/2 ? channel_id : (int)channel_id - (int)M);
    fprintf(fid,"X  = zeros(1,nfft);\n");
    fprintf(fid,"Y  = zeros(1,nfft);\n");

    // save input and output arrays
    for (i=0; i<nfft; i++) {
        fprintf(fid,"X(%4u) = %12.4e;\n", i+1, psd_0[i]);
        fprintf(fid,"Y(%4u) = %12.4e;\n", i+1, psd_1[i]);
    }
    fprintf(fid,"Y = Y + 10*log10(M/P); %% scale by channelizer gain\n");

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"title('composite');\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(f0, X, '-', 'LineWidth',1.5,...\n");
    fprintf(fid,"       f1, Y, '-', 'LineWidth',2.5);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f0, Y, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  ylabel('Power Spectral Density [dB]');\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    printf("done.\n");
    return 0;
}
