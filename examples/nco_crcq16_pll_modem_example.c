char __docstr__[] =
"This example demonstrates how the nco/pll object (numerically-controlled"
" oscillator with phase-locked loop) can be used for carrier frequency"
" recovery in digital modems.  The modem type, SNR, and other parameters are"
" specified via the command-line interface."
" All operations are in 16-bit fixed-point format";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"nco_crcq16_pll_modem_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    mod_str,         "qpsk", 'm', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(char*,    type_str,         "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    phase_offset,     0.200, 'p', "phase offset [radians]", NULL);
    liquid_argparse_add(float,    frequency_offset, 0.070, 'f', "frequency offset [f/Fs]", NULL);
    liquid_argparse_add(float,    pll_bandwidth,    0.010, 'w', "phase-locked loop bandwidth", NULL);
    liquid_argparse_add(float,    SNRdB,               30, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(unsigned, num_symbols,        512, 'n', "number of symbols", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (strcmp(type_str,"nco") && strcmp(type_str,"vco"))
        return liquid_error(LIQUID_EICONFIG,"invalid nco type '%s' (must be either 'nco' or 'vco')", type_str);

    // objects
    int type = strcmp(type_str,"nco")==0 ? LIQUID_NCO : LIQUID_VCO;
    nco_crcq16 nco_tx = nco_crcq16_create(LIQUID_VCO);
    nco_crcq16 nco_rx = nco_crcq16_create(LIQUID_VCO);

    // create modems
    modulation_scheme ms = liquid_getopt_str2mod(mod_str);
    modemcq16 mod   = modemcq16_create(ms);
    modemcq16 demod = modemcq16_create(ms);
    unsigned int bps = modemcq16_get_bps(mod);

    // initialize objects
    nco_crcq16_set_phase(nco_tx,         q16_angle_float_to_fixed(phase_offset) );
    nco_crcq16_set_frequency(nco_tx,     q16_angle_float_to_fixed(frequency_offset));
    nco_crcq16_pll_set_bandwidth(nco_rx, pll_bandwidth);

    float nstd = powf(10.0f, -SNRdB/20.0f);

    // print parameters
    printf("PLL example :\n");
    printf("modem : %u-%s\n", 1<<bps, modulation_types[ms].name);
    printf("frequency offset: %6.3f, phase offset: %6.3f, SNR: %6.2fdB, pll b/w: %6.3f\n",
            frequency_offset, phase_offset, SNRdB, pll_bandwidth);

    // open output file for writing
    FILE * fid = fopen(filename,"w");
    fprintf(fid, "%% %s : auto-generated file\n", filename);
    fprintf(fid, "clear all;\n");
    fprintf(fid, "phi=zeros(1,%u);\n",num_symbols);
    fprintf(fid, "r=zeros(1,%u);\n",num_symbols);

    // run loop
    unsigned int i, M=1<<bps, sym_in, sym_out, num_errors=0;
    q16_t phase_error;
    cq16_t x, r, v;
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym_in = rand() % M;

        // modulate
        modemcq16_modulate(mod, sym_in, &x);

        // channel
        //r = nco_crcq16_cexpf(nco_tx);
        nco_crcq16_mix_up(nco_tx, x, &r);

        // add complex white noise
        r.real += q16_float_to_fixed( randnf()*nstd/M_SQRT2 );
        r.imag += q16_float_to_fixed( randnf()*nstd/M_SQRT2 );

        //v = nco_crcq16_cexpf(nco_rx);
        nco_crcq16_mix_down(nco_rx, r, &v);

        // demodulate
        modemcq16_demodulate(demod, v, &sym_out);
        num_errors += count_bit_errors(sym_in, sym_out);

        // error estimation
        //phase_error = cargf(r*conjf(v));
        phase_error = modemcq16_get_demodulator_phase_error(demod);

        // perfect error estimation
        //phase_error = nco_tx->theta - nco_rx->theta;

        // print every line in a format that octave can read
        fprintf(fid, "phi(%u) = %10.6E;\n", i+1, q16_angle_fixed_to_float(phase_error));
        fprintf(fid, "r(%u) = %10.6E + j*%10.6E;\n",
                i+1,
                q16_fixed_to_float(v.real),
                q16_fixed_to_float(v.imag));

        if ((i+1)%20 == 0 || i==num_symbols-1) {
            q16_t v0 = nco_crcq16_get_phase(nco_tx) - nco_crcq16_get_phase(nco_rx);// true phase error
            q16_t v1 = nco_crcq16_get_frequency(nco_tx) - nco_crcq16_get_frequency(nco_rx);// true frequency error
            printf("  %4u: e_hat : %6.3f, phase error : %6.3f, freq error : %6.3f\n",
                    i+1,                                // iteration
                    q16_angle_fixed_to_float(phase_error),      // estimated phase error
                    q16_angle_fixed_to_float(v0),
                    q16_angle_fixed_to_float(v1)
                  );
        }

        // update tx nco object
        nco_crcq16_step(nco_tx);

        // update pll
        nco_crcq16_pll_step(nco_rx, phase_error);

        // update rx nco object
        nco_crcq16_step(nco_rx);
    }

    fprintf(fid, "figure;\n");
    fprintf(fid, "plot(1:length(phi),phi,'LineWidth',2,'Color',[0 0.25 0.5]);\n");
    fprintf(fid, "xlabel('Symbol Index');\n");
    fprintf(fid, "ylabel('Phase Error [radians]');\n");
    fprintf(fid, "grid on;\n");

    fprintf(fid, "t0 = round(0.25*length(r));\n");
    fprintf(fid, "figure;\n");
    fprintf(fid, "plot(r(1:t0),'x','Color',[0.6 0.6 0.6],r(t0:end),'x','Color',[0 0.25 0.5]);\n");
    fprintf(fid, "grid on;\n");
    fprintf(fid, "axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid, "axis('square');\n");
    fprintf(fid, "xlabel('In-Phase');\n");
    fprintf(fid, "ylabel('Quadrature');\n");
    fprintf(fid, "legend(['first 25%%'],['last 75%%'],'location','northeast');\n");
    fclose(fid);

    printf("results written to %s.\n",filename);

    nco_crcq16_destroy(nco_tx);
    nco_crcq16_destroy(nco_rx);

    modemcq16_destroy(mod);
    modemcq16_destroy(demod);

    printf("bit errors: %u / %u\n", num_errors, bps*num_symbols);
    printf("done.\n");
    return 0;
}
