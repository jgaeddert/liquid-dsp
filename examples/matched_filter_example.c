const char __docstr__[] = "Demonstrate how to run a matched filter.";

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
    liquid_argparse_add(char*, filename, "matched_filter_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, k,           2,      'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,           8,      'm', "symbol delay", NULL);
    liquid_argparse_add(float,    beta,        0.2f,   'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(unsigned, num_symbols, 32,     'n', "number of symbols", NULL);
    liquid_argparse_add(char*,    ftype_str,   "rrcos",'t', "filter type", liquid_argparse_firfilt);
    liquid_argparse_parse(argc,argv);

    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k must be at least 2");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m must be at least 1");
    if (beta <= 0.0f || beta >= 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta must be in (0,1)");

    // set filter types
    liquid_firfilt_type ftype_tx = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
    liquid_firfilt_type ftype_rx = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);
    // ensure appropriate GMSK filter pairs are used if either tx or rx is specified
    if (strcmp(ftype_str,"gmsktx")==0)
        ftype_rx = LIQUID_FIRFILT_GMSKRX;
    if (strcmp(ftype_str,"gmskrx")==0)
        ftype_tx = LIQUID_FIRFILT_GMSKTX;

    unsigned int i;

    // derived values
    unsigned int num_samples = num_symbols*k;
    unsigned int h_len  = 2*k*m+1;               // transmit/receive filter length
    unsigned int hc_len = 4*k*m+1;               // composite filter length

    // arrays
    LIQUID_VLA(float, ht, h_len);    // transmit filter
    LIQUID_VLA(float, hr, h_len);    // receive filter
    LIQUID_VLA(float, hc, hc_len);   // composite filter

    // design the filter(s)
    liquid_firdes_prototype(ftype_tx, k, m, beta, 0, ht);
    liquid_firdes_prototype(ftype_rx, k, m, beta, 0, hr);

    for (i=0; i<h_len; i++) printf("ht(%3u) = %12.8f;\n", i+1, ht[i]);
    for (i=0; i<h_len; i++) printf("hr(%3u) = %12.8f;\n", i+1, hr[i]);

    // compute composite filter response
    for (i=0; i<4*k*m+1; i++) {
        int lag = (int)i - (int)(2*k*m);
        hc[i] = liquid_filter_crosscorr(ht,h_len, hr,h_len, lag);
    }

    // compute filter inter-symbol interference
    float rxy0 = liquid_filter_crosscorr(ht,h_len, hr,h_len, 0);
    float isi_rms=0;
    for (i=1; i<2*m; i++) {
        float e = liquid_filter_crosscorr(ht,h_len, hr,h_len, i*k) / rxy0;
        isi_rms += e*e;
    }
    isi_rms = sqrtf( isi_rms / (float)(2*m-1) );
    printf("  isi (rms) : %12.8f dB\n", 20*log10f(isi_rms));

    // compute relative stop-band energy
    unsigned int nfft = 2048;
    float As = liquid_filter_energy(ht, h_len, 0.5f*(1.0f + beta)/(float)k, nfft);
    printf("  As        : %12.8f dB\n", 20*log10f(As));

    // generate signal
    LIQUID_VLA(float, sym_in, num_symbols);
    LIQUID_VLA(float, y, num_samples);
    LIQUID_VLA(float, sym_out, num_symbols);

    // create interpolator and decimator
    firinterp_rrrf interp = firinterp_rrrf_create(k, ht, h_len);
    firdecim_rrrf  decim  = firdecim_rrrf_create( k, hr, h_len);

    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym_in[i] = (rand() % 2) ? 1.0f : -1.0f;

        // interpolate
        firinterp_rrrf_execute(interp, sym_in[i], &y[i*k]);

        // decimate
        firdecim_rrrf_execute(decim, &y[i*k], &sym_out[i]);

        // normalize output
        sym_out[i] /= k;

        printf("  %3u : %8.5f", i, sym_out[i]);
        if (i>=2*m) printf(" *\n");
        else        printf("\n");
    }

    // clean up objects
    firinterp_rrrf_destroy(interp);
    firdecim_rrrf_destroy(decim);

    //
    // export results
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %12.8f;\n", beta);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = k*num_symbols;\n");

    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid," y(%3u) = %12.8f;\n", i+1, y[i]);

    for (i=0; i<h_len;  i++) fprintf(fid,"ht(%3u) = %20.8e;\n", i+1, ht[i]);
    for (i=0; i<h_len;  i++) fprintf(fid,"hr(%3u) = %20.8e;\n", i+1, hr[i]);
    for (i=0; i<hc_len; i++) fprintf(fid,"hc(%3u) = %20.8e;\n", i+1, hc[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Ht = 20*log10(abs(fftshift(fft(ht/k,   nfft))));\n");
    fprintf(fid,"Hr = 20*log10(abs(fftshift(fft(hr/k,   nfft))));\n");
    fprintf(fid,"Hc = 20*log10(abs(fftshift(fft(hc/k^2, nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Hc,'-','LineWidth',2,...\n");
    fprintf(fid,"     [0.5/k],[-6],'or',...\n");
    fprintf(fid,"     [0.5/k*(1-beta) 0.5/k*(1-beta)],[-100 10],'-r',...\n");
    fprintf(fid,"     [0.5/k*(1+beta) 0.5/k*(1+beta)],[-100 10],'-r');\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");
    fprintf(fid,"grid on;\n");

    // compute composite filter
    fprintf(fid,"figure;\n");
    fprintf(fid,"hc = conv(ht,hr)/k;\n");
    fprintf(fid,"t = [(-2*k*m):(2*k*m)]/k;\n");
    fprintf(fid,"i0 = [0:k:4*k*m]+1;\n");
    fprintf(fid,"plot(t,    hc,    '-s',...\n");
    fprintf(fid,"     t(i0),hc(i0),'or');\n");
    fprintf(fid,"xlabel('symbol index');\n");
    fprintf(fid,"ylabel('matched filter response');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", filename);
    
    printf("done.\n");
    return 0;
}

