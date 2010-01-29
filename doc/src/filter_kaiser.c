//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <liquid/liquid.h>

#define OUTPUT_FILENAME "figures.gen/filter_kaiser.gnu"

int main() {
    // options
    float fc=0.4f;          // filter cutoff frequency
    float ft=0.2f;          // filter transition
    float slsl=60.0f;       // sidelobe suppression level
    float mu=0.0f;          // fractional timing offset

    unsigned int h_len=(unsigned int)num_fir_filter_taps(slsl,ft);

    unsigned int i;
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,mu,h);
#if 0
    for (i=0; i<h_len; i++)
        printf("h(%3u) = %12.8f;\n", i+1, h[i]);
#endif

    unsigned int nfft=256;
    float complex x[nfft];
    float complex X[nfft];
    fftplan fft = fft_create_plan(nfft,x,X,FFT_FORWARD,0);
    for (i=0; i<nfft; i++)
        x[i] = i < h_len ? h[i] : 0.0f;
    fft_execute(fft);
    fft_destroy_plan(fft);

    //for (i=0; i<nfft; i++)
    //    printf("x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf([i]));

    // open output file and print header
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5]\n");
    fprintf(fid,"set yrange [-100:40]\n");
    //fprintf(fid,"set size square\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid lc rgb '#999999' lw 1\n");
    fprintf(fid,"plot '-' using 1:2 with lines lw 4 linecolor rgb '#004080'\n");

    // save array to output
    float f;
    unsigned int k;
    fprintf(fid,"# frequency psd\n");
    for (i=0; i<nfft; i++) {
        f = (float)(i) / (float)nfft - 0.5f;
        k = (i + nfft/2) % nfft;
        fprintf(fid,"%12.8f %12.8f\n", f, 20*log10(fabsf(X[k])));
    }   

    fclose(fid);
    return 0;
}

