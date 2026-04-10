const char __docstr__[] = "Demonstrate Nyquist filter operation.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "nyquist_filter_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,           2,      'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,           8,      'm', "symbol delay", NULL);
    liquid_argparse_add(float,    beta,        0.2f,   'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(unsigned, num_symbols, 32,     'n', "number of symbols", NULL);
    liquid_argparse_add(char*,    ftype_str,   "kaiser",'t', "filter type", liquid_argparse_firfilt);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k must be at least 2");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m must be at least 1");
    if (beta <= 0.0f || beta >= 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta must be in (0,1)");

    // design the filter
    liquid_firfilt_type ftype = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
    unsigned int h_len = 2*k*m+1;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_prototype(ftype,k,m,beta,0,h);

    // print the coefficients to the screen
    unsigned int i;
    for (i=0; i<h_len; i++)
        printf("h(%3u) = %12.8f\n", i+1, h[i]);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %12.8f;\n", beta);
    fprintf(fid,"h_len = 2*k*m+1;\n");

    fprintf(fid,"h = zeros(1,h_len);\n");
    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %20.8e;\n", i+1, h[i]);
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H = 20*log10(abs(fftshift(fft(h/k,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-','LineWidth',2,...\n");
    fprintf(fid,"     [0.5/k],[-20*log10(2)],'or',...\n");
    fprintf(fid,"     [0.5/k*(1-beta) 0.5/k*(1-beta)],[-100 10],'-r',...\n");
    fprintf(fid,"     [0.5/k*(1+beta) 0.5/k*(1+beta)],[-100 10],'-r');\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", filename);
    
    printf("done.\n");
    return 0;
}

