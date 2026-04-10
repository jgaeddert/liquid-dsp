const char __docstr__[] = "Show delay in symstream object.";

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
    liquid_argparse_add(char*, filename, "symstreamcf_delay_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*, ftype_str, "arkaiser", 'f', "filter type", liquid_argparse_firfilt);
    liquid_argparse_add(unsigned, k,         4,  'k', "interpolation factor", NULL);
    liquid_argparse_add(unsigned, m,         9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,    0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,"qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, buf_len, 100,  'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // create stream generator
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    int ms    = liquid_getopt_str2mod(mod_str);
    symstreamcf gen = symstreamcf_create_linear(ftype,k,m,beta,ms);
    unsigned int delay = symstreamcf_get_delay(gen);

    // write samples to buffer
    LIQUID_VLA(liquid_float_complex, buf, 2*buf_len);
    symstreamcf_write_samples(gen, buf, buf_len);
    symstreamcf_set_gain(gen, 0.0f);
    symstreamcf_write_samples(gen, buf+buf_len, buf_len);

    // destroy objects
    symstreamcf_destroy(gen);

    // measure approximate delay; index of first sample with abs > 1
    unsigned int i;
    for (i=0; i<2*buf_len; i++) {
        if (cabsf(buf[i]) > 1.0f)
            break;
    }
    printf("expected delay: %u, approximate delay: %u\n", delay, i);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n = %u; delay = %u;\n", buf_len, delay);
    fprintf(fid,"t = [0:(2*n-1)] - delay;\n");
    for (i=0; i<2*buf_len; i++)
        fprintf(fid,"v(%6u) = %12.4e + %12.4ej;\n", i+1, crealf(buf[i]), cimagf(buf[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, real(v), t, imag(v));\n");
    fprintf(fid,"xlabel('Sample Index');\n");
    fprintf(fid,"ylabel('Signal Output');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}

