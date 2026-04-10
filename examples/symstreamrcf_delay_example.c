const char __docstr__[] = "Show delay in symstreamr object.";

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
    liquid_argparse_add(char*, filename, "symstreamrcf_delay_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*, ftype_str, "arkaiser", 'f', "filter type", liquid_argparse_firfilt);
    liquid_argparse_add(float,    bw, 0.234567,  'w', "filter bandwidth", NULL);
    liquid_argparse_add(unsigned, m,         9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,    0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,"qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, buf_len, 100,  'N', "number of samples to simulate", NULL);
    liquid_argparse_parse(argc,argv);

    // create stream generator
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    int ms    = liquid_getopt_str2mod(mod_str);
    symstreamrcf gen = symstreamrcf_create_linear(ftype,bw,m,beta,ms);
    float delay = symstreamrcf_get_delay(gen);

    // write samples to buffer
    LIQUID_VLA(liquid_float_complex, buf, 2*buf_len);
    symstreamrcf_write_samples(gen, buf, buf_len);
    symstreamrcf_set_gain(gen, 0.0f);
    symstreamrcf_write_samples(gen, buf+buf_len, buf_len);

    // destroy objects
    symstreamrcf_destroy(gen);

    // measure approximate delay; index of first sample with abs > 1
    unsigned int i;
    for (i=0; i<2*buf_len; i++) {
        if (cabsf(buf[i]) > 1.0f)
            break;
    }
    printf("expected delay: %.3f, approximate delay: %u\n", delay, i);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n = %u; delay = %.6f;\n", buf_len, delay);
    fprintf(fid,"t = [0:(2*n-1)] - delay;\n");
    for (i=0; i<2*buf_len; i++)
        fprintf(fid,"v(%6u) = %12.4e + %12.4ej;\n", i+1, crealf(buf[i]), cimagf(buf[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, real(v), t, imag(v));\n");
    fprintf(fid,"xlabel('Sample Index');\n");
    fprintf(fid,"ylabel('Signal Output');\n");
    fprintf(fid,"legend('real','imag');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}

