const char __docstr__[] =
"This example demonstrates interpolation and decimation of a"
" signal with a square-root Nyquist filter."
" Data symbols are generated and then interpolated according to a"
" finite impulse response square-root Nyquist filter.  The resulting"
" sequence is then decimated with the same filter, matched to the"
" interpolator.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firinterp_firdecim_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,         7, 'M', "interpolation factor", NULL);
    liquid_argparse_add(unsigned, m,         7, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    dt,      0.5, 't', "filter fractional sample delay", NULL);
    liquid_argparse_add(float,    beta,    0.2, 's', "filter excess bandwidth", NULL);
    liquid_argparse_add(unsigned, num_syms, 16, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    if (M < 2)
        return liquid_error(LIQUID_EICONFIG,"interp factor must be greater than 1");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"filter delay must be greater than 0");
    if (beta <= 0.0 || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta (excess bandwidth factor) must be in (0,1]");
    if (num_syms < 1)
        return liquid_error(LIQUID_EICONFIG,"must have at least one data symbol");

    // derived values
    unsigned int h_len          = 2*M*m + 1;        // prototype filter length
    unsigned int num_syms_total = num_syms + 2*m;   // number of total symbols (w/ delay)
    unsigned int num_samples    = M*num_syms_total; // number of samples

    // design filter and create interpolator and decimator objects
    LIQUID_VLA(float, h, h_len);     // transmit filter
    LIQUID_VLA(float, g, h_len);     // receive filter (reverse of h)
    liquid_firdes_rrcos(M,m,beta,dt,h);
    unsigned int i;
    for (i=0; i<h_len; i++)
        g[i] = h[h_len-i-1];
    firinterp_crcf interp = firinterp_crcf_create(M,h,h_len);
    firdecim_crcf  decim  = firdecim_crcf_create(M,g,h_len);
    firdecim_crcf_set_scale(decim, 1.0f/(float)M);

    // allocate memory for buffers
    LIQUID_VLA(liquid_float_complex, x, num_syms_total);   // input symbols
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // interpolated sequence
    LIQUID_VLA(liquid_float_complex, z, num_syms_total);   // decimated (received) symbols

    // generate input symbols, padded with zeros at the end
    for (i=0; i<num_syms_total; i++) {
        liquid_float_complex s = (rand() % 2 ? 1.0f : -1.0f) +
                          (rand() % 2 ? 1.0f : -1.0f) * _Complex_I;
        x[i] = i < num_syms ? s : 0;
    }

    // run interpolator
    for (i=0; i<num_syms_total; i++)
        firinterp_crcf_execute(interp, x[i], &y[M*i]);

    // run decimator
    for (i=0; i<num_syms_total; i++)
        firdecim_crcf_execute(decim, &y[M*i], &z[i]);

    // destroy objects
    firinterp_crcf_destroy(interp);
    firdecim_crcf_destroy(decim);

    // print results to screen
    printf("filter impulse response :\n");
    for (i=0; i<h_len; i++)
        printf("  [%4u] : %8.4f\n", i, h[i]);

    printf("input symbols\n");
    for (i=0; i<num_syms_total; i++) {
        printf("  [%4u] : %8.4f + j*%8.4f", i, crealf(x[i]), cimagf(x[i]));

        // highlight actual data symbols
        if (i < num_syms) printf(" *\n");
        else                      printf("\n");
    }

    printf("interpolator output samples:\n");
    for (i=0; i<num_samples; i++) {
        printf("  [%4u] : %8.4f + j*%8.4f", i, crealf(y[i]), cimagf(y[i]));

        if ( (i >= M*m) && ((i%M)==0))  printf(" **\n");
        else                            printf("\n");
    }

    printf("output symbols:\n");
    for (i=0; i<num_syms_total; i++) {
        printf("  [%4u] : %8.4f + j*%8.4f", i, crealf(z[i]), cimagf(z[i]));

        // highlight symbols (compensate for filter delay)
        if ( i < 2*m ) printf("\n");
        else           printf(" *\n");
    }

    // export results to file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"dt = %8.6f;\n", dt);
    fprintf(fid,"h_len=%u;\n",h_len);
    fprintf(fid,"num_syms_total = %u;\n", num_syms_total);
    fprintf(fid,"num_samples = M*num_syms_total;\n");
    fprintf(fid,"h = zeros(1,h_len);\n");
    fprintf(fid,"x = zeros(1,num_syms_total);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    for (i=0; i<num_syms_total; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    for (i=0; i<num_syms_total; i++)
        fprintf(fid,"z(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(z[i]), cimagf(z[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(num_syms_total-1)];\n");
    fprintf(fid,"ty = [0:(num_samples-1)]/M - m + dt/M;\n");
    fprintf(fid,"tz = [0:(num_syms_total-1)] - 2*m;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(tx,real(x),'s',ty,real(y),'-',tz,real(z),'x');\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"    legend('symbols in','interp','symbols out');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(tx,imag(x),'s',ty,imag(y),'-',tz,imag(z),'x');\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
