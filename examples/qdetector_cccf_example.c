const char __docstr__[] =
"This example demonstrates the functionality of the qdetector object"
" to detect an arbitrary signal in time in the presence of noise,"
" carrier frequency/phase offsets, and fractional-sample timing"
" offsets.";

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qdetector_cccf_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, sequence_len,   80,   'n', "number of sync symbols", NULL);
    liquid_argparse_add(unsigned, k,               2,   'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,               7,   'm', "filter delay [symbols]", NULL);
    liquid_argparse_add(float,    beta,         0.3f,   'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    ftype_str, "arkaiser",'t', "filter type", liquid_argparse_firfilt);
    liquid_argparse_add(float,    tau,          -0.3f,  'T', "fractional sample timing offset", NULL);
    liquid_argparse_add(float,    dphi,         -0.01f, 'F', "carrier frequency offset", NULL);
    liquid_argparse_add(float,    phi,           0.5f,  'P', "carrier phase offset", NULL);
    liquid_argparse_add(float,    noise_floor,  -30.0f, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,        20.0f,  's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    threshold,     0.5f,  'z', "detection threshold", NULL);
    liquid_argparse_add(float,    range,         0.05f, 'r', "carrier offset search range [radians/sample]", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (tau < -0.5f || tau > 0.5f)
        return liquid_error(LIQUID_EICONFIG,"fractional sample offset must be in [-0.5,0.5]");

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // generate synchronization sequence (QPSK symbols)
    LIQUID_VLA(liquid_float_complex, sequence, sequence_len);
    unsigned int i;
    for (i=0; i<sequence_len; i++) {
        sequence[i] = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                      (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
    }

    //
    float rxy       = 0.0f;
    float tau_hat   = 0.0f;
    float gamma_hat = 0.0f;
    float dphi_hat  = 0.0f;
    float phi_hat   = 0.0f;
    int   frame_detected = 0;

    // create detector
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    qdetector_cccf q = qdetector_cccf_create_linear(sequence, sequence_len, ftype, k, m, beta);
    qdetector_cccf_set_threshold(q, threshold);
    qdetector_cccf_set_range    (q, range);
    qdetector_cccf_print(q);

    //
    unsigned int seq_len     = qdetector_cccf_get_seq_len(q);
    unsigned int buf_len     = qdetector_cccf_get_buf_len(q);
    unsigned int num_samples = 2*buf_len;   // double buffer length to ensure detection
    unsigned int num_symbols = buf_len;

    // arrays
    LIQUID_VLA(liquid_float_complex, y, num_samples);       // received signal
    LIQUID_VLA(liquid_float_complex, syms_rx, num_symbols); // recovered symbols

    // get pointer to sequence and generate full sequence
    liquid_float_complex * v = (liquid_float_complex*) qdetector_cccf_get_sequence(q);
    unsigned int filter_delay = 15;
    firfilt_crcf filter = firfilt_crcf_create_kaiser(2*filter_delay+1, 0.4f, 60.0f, -tau);
    for (i=0; i<num_samples; i++) {
        // add delay
        firfilt_crcf_push(filter, i < seq_len ? v[i] : 0);
        firfilt_crcf_execute(filter, &y[i]);

        // channel gain
        y[i] *= gamma;

        // carrier offset
        y[i] *= cexpf(_Complex_I*(dphi*i + phi));

        // noise
        y[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }
    firfilt_crcf_destroy(filter);

    // run detection on sequence
    for (i=0; i<num_samples; i++) {
        v = (liquid_float_complex*)qdetector_cccf_execute(q,y[i]);

        if (v != NULL) {
            printf("\nframe detected!\n");
            frame_detected = 1;

            // get statistics
            rxy       = qdetector_cccf_get_rxy(q);
            tau_hat   = qdetector_cccf_get_tau(q);
            gamma_hat = qdetector_cccf_get_gamma(q);
            dphi_hat  = qdetector_cccf_get_dphi(q);
            phi_hat   = qdetector_cccf_get_phi(q);
            break;
        }
    }

    unsigned int num_syms_rx = 0;   // output symbol counter
    unsigned int counter     = 0;   // decimation counter
    if (frame_detected) {
        // recover symbols
        firfilt_crcf mf = firfilt_crcf_create_rnyquist(ftype, k, m, beta, tau_hat);
        firfilt_crcf_set_scale(mf, 1.0f / (float)(k*gamma_hat));
        nco_crcf     nco = nco_crcf_create(LIQUID_VCO);
        nco_crcf_set_frequency(nco, dphi_hat);
        nco_crcf_set_phase    (nco,  phi_hat);

        for (i=0; i<buf_len; i++) {
            //
            liquid_float_complex sample;
            nco_crcf_mix_down(nco, v[i], &sample);
            nco_crcf_step(nco);

            // apply decimator
            firfilt_crcf_push(mf, sample);
            counter++;
            if (counter == k-1)
                firfilt_crcf_execute(mf, &syms_rx[num_syms_rx++]);
            counter %= k;
        }

        nco_crcf_destroy(nco);
        firfilt_crcf_destroy(mf);
    }

    // destroy objects
    qdetector_cccf_destroy(q);

    // print results
    printf("\n");
    printf("frame detected  :   %s\n", frame_detected ? "yes" : "no");
    if (frame_detected) {
        printf("  rxy           : %8.3f\n", rxy);
        printf("  gamma hat     : %8.3f, actual=%8.3f (error=%8.3f)\n",            gamma_hat, gamma, gamma_hat - gamma);
        printf("  tau hat       : %8.3f, actual=%8.3f (error=%8.3f) samples\n",    tau_hat,   tau,   tau_hat   - tau  );
        printf("  dphi hat      : %8.5f, actual=%8.5f (error=%8.5f) rad/sample\n", dphi_hat,  dphi,  dphi_hat  - dphi );
        printf("  phi hat       : %8.5f, actual=%8.5f (error=%8.5f) radians\n",    phi_hat,   phi,   phi_hat   - phi  );
        printf("  symbols rx    : %u\n", num_syms_rx);
    }
    printf("\n");

    // export results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"sequence_len= %u;\n", sequence_len);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"num_syms_rx = %u;\n", num_syms_rx);
    fprintf(fid,"syms_rx     = zeros(1,num_syms_rx);\n");
    for (i=0; i<num_syms_rx; i++)
        fprintf(fid,"syms_rx(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(syms_rx[i]), cimagf(syms_rx[i]));

    fprintf(fid,"t=[0:(num_samples-1)];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(4,1,1);\n");
    fprintf(fid,"  plot(t,real(y), t,imag(y));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('received signal');\n");
    fprintf(fid,"subplot(4,1,2:4);\n");
    fprintf(fid,"  plot(real(syms_rx), imag(syms_rx), 'x');\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('real');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fclose(fid);
    printf("results written to '%s'\n", filename);
    return 0;
}
