const char __docstr__[] = "Demonstrate symbol tracking of GMSK signal.";

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
    liquid_argparse_add(char*, filename, "gmsk_tracking_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,            4, 'k', "filter semi-length", NULL);
    liquid_argparse_add(unsigned, m,            3, 'm', "filter samples/symbol", NULL);
    liquid_argparse_add(float,    beta,       0.3, 'b', "bandwidth-time product", NULL);
    liquid_argparse_add(unsigned, num_blocks,  25, 'n', "number of symbols to simulate", NULL);
    liquid_argparse_add(unsigned, P,          203, 'P', "output rate (interpolation factor)", NULL);
    liquid_argparse_add(unsigned, Q,          200, 'Q', "input rate (decimation factor)", NULL);
    liquid_argparse_parse(argc,argv);

    // create resampler object to facilitate sample rate offset
    rresamp_crcf resamp = rresamp_crcf_create_kaiser(k*P,k*Q,12,0.5,60.0f);

    // create modulator
    gmskmod mod = gmskmod_create(k, m, beta);
    gmskmod_print(mod);

    // frequency demodulator
    freqdem dem = freqdem_create(0.01*k);

    // create matched filter
    firfilt_rrrf mf = firfilt_rrrf_create_rnyquist(LIQUID_FIRFILT_GMSKRX, k, 8, beta, 0);
    firfilt_rrrf_set_scale(mf,1./(float)k); // compensate for 'k' samples/symbol

    // create symbol synchronizer to operate after matched filter
    // NOTE: can alternatively skip the matched filter and apply with the symsync object
    symsync_rrrf sync = symsync_rrrf_create_kaiser(k, 4, 0/*ignored*/, 64);
    symsync_rrrf_set_lf_bw(sync,0.02);      // set loop filter bandwidth
    symsync_rrrf_set_output_rate(sync,2);   // set output rate as 2 samples/symbol

    // write results to output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"k = %u; v = [];\n", k);

    // run in blocks
    LIQUID_VLA(liquid_float_complex, buf_0, k*Q); // original GMSK input
    LIQUID_VLA(liquid_float_complex, buf_1, k*P); // resample output at a rate P/Q
    LIQUID_VLA(float, buf_2, k*P); // freqdem output & matched filter output
    LIQUID_VLA(float, buf_3, k*P); // symbol timing recovery output
    unsigned int i, j;
    for (i=0; i<num_blocks; i++)
    {
        // generate input GMSK signal
        for (j=0; j<Q; j++)
            gmskmod_modulate(mod, rand() & 1, &buf_0[k*j]);

        // resample k*Q samples to k*P samples
        rresamp_crcf_execute(resamp, buf_0, buf_1);

        // run through frequency demod
        freqdem_demodulate_block(dem, buf_1, k*P, buf_2);

        // run through matched filter (in place)
        firfilt_rrrf_execute_block(mf, buf_2, k*P, buf_2);

        // run result through symbol timing recovery
        unsigned int num_symbols_sync;
        symsync_rrrf_execute(sync, buf_2, k*P, buf_3, &num_symbols_sync);

        // write results to file
        for (j=0; j<num_symbols_sync; j++)
            fprintf(fid,"v(end+1) = %12.4e;\n", buf_3[j]);
    }
    fprintf(fid,"n = length(v); t = [0:(n-1)]/2; idx = 1:2:n;\n");
    fprintf(fid,"figure('color','white','position',[100 100 1200 400]);\n");
    fprintf(fid,"plot(t,v,'-','Color',[1 1 1]*0.6,...\n");
    fprintf(fid,"     t(idx),v(idx),'o','Color',[0 0.2 0.4]);\n");
    fprintf(fid,"axis([0 t(end) -2 2]); grid on;\n");
    fprintf(fid,"xlabel('Time [symbol index]'); ylabel('symsync output');\n");
    fclose(fid);
    printf("results written to '%s'\n", filename);

    // destroy objects
    gmskmod_destroy(mod);
    freqdem_destroy(dem);
    firfilt_rrrf_destroy(mf);
    symsync_rrrf_destroy(sync);

    return 0;
}
