//
// filter_resamp_crcf.c : arbitrary resampler plot
//

#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_TIME "figures.gen/filter_resamp_crcf_time.gnu"
#define OUTPUT_FILENAME_FREQ "figures.gen/filter_resamp_crcf_freq.gnu"

int main() {
    // options
    unsigned int h_len = 7;     // filter semi-length (filter delay)
    float r=1/sqrtf(2);         // resampling rate (output/input)
    float bw=0.25f;             // resampling filter bandwidth
    float As=60.0f;             // resampling filter stop-band attenuation [dB]
    unsigned int npfb=32;       // number of filters in bank (timing resolution)
    unsigned int n=180;         // number of input samples

    // number of input samples (adjusted for filter delay)
    unsigned int nx = n + h_len;

    // generate input sequence : windowed sum of complex sinusoids
    unsigned int i;
    float complex x[nx];
    for (i=0; i<nx; i++) {
        float complex jphi = _Complex_I*2.0f*M_PI*i;
        x[i] = cexpf(jphi*0.02f) + 1.4f*cexpf(jphi*0.07f);
        
        // window edge size
        unsigned int t = (unsigned int)(0.1*n);
        if (i < n) {
            // edge-rounded window
            if (i < t)          x[i] *= blackmanharris(i,2*t);
            else if (i >= n-t)  x[i] *= blackmanharris(n-i-1,2*t);
        } else {
            x[i] = 0.;
        }
    }

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1*r*nx) + 16;
    float complex y[y_len];

    // create resampler
    resamp_crcf f = resamp_crcf_create(r,h_len,bw,As,npfb);
    unsigned int num_written;
    unsigned int ny=0;
    for (i=0; i<nx; i++) {
        // execute resampler, storing in output buffer
        resamp_crcf_execute(f, x[i], &y[ny], &num_written);
        ny += num_written;
    }

    printf(" %u / %u\n", ny, nx);

    // clean up allocated objects
    resamp_crcf_destroy(f);

    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME_TIME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_TIME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-3:3]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Input Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'Real'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 linetype 1 linewidth 1 linecolor rgb '#999999' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '#008000' title 'resampled'\n");
    // export output
    for (i=0; i<nx; i++) {
        //fprintf(fid,"%6u %12.4e %12.4e\n", i, cos(2*M_PI*0.04*i), sin(2*M_PI*0.04*i));
        fprintf(fid,"%6u %12.4e %12.4e\n", i, crealf(x[i]), cimagf(x[i]));
    }
    fprintf(fid,"e\n");

    float t;
    for (i=0; i<ny; i++) {
        t = (float)(i) / r - (float)(h_len);
        fprintf(fid,"%12.4e %12.4e %12.4e\n", t, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:3 with linespoints pointtype 7 linetype 1 linewidth 1 linecolor rgb '#999999' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:3 with points pointtype 7 linecolor rgb '#800000' title 'resampled'\n");
    // export output
    for (i=0; i<nx; i++) {
        //fprintf(fid,"%6u %12.4e %12.4e\n", i, cos(2*M_PI*0.04*i), sin(2*M_PI*0.04*i));
        fprintf(fid,"%6u %12.4e %12.4e\n", i, crealf(x[i]), cimagf(x[i]));
    }
    fprintf(fid,"e\n");

    for (i=0; i<ny; i++) {
        t = (float)(i) / r - (float)(h_len);
        fprintf(fid,"%12.4e %12.4e %12.4e\n", t, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);


    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    unsigned int nfft = 512;
    float complex X[nfft];
    float complex Y[nfft];
    liquid_doc_compute_psdcf(x,nx,X,nfft,LIQUID_DOC_PSDWINDOW_HANN,0);
    liquid_doc_compute_psdcf(y,ny,Y,nfft,LIQUID_DOC_PSDWINDOW_HANN,0);
    fft_shift(X,nfft);
    fft_shift(Y,nfft);
    float scaling_factor = 20*log10f(nfft);

    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-0.5:0.5];\n");
    fprintf(fid,"set yrange [-120:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Normalized Input Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n",LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"# real\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#999999' title 'original',\\\n");
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '#004080' title 'resampled'\n");
    // export output
    for (i=0; i<nfft; i++) {
        float fx = (float)(i) / (float)nfft - 0.5f;
        fprintf(fid,"%12.8f %12.4e\n", fx, 20*log10f(cabsf(X[i])) - scaling_factor);
    }
    fprintf(fid,"e\n");
    for (i=0; i<nfft; i++) {
        float fy = ((float)(i) / (float)nfft - 0.5f)*r;
        fprintf(fid,"%12.8f %12.4e\n", fy, 20*log10f(cabsf(Y[i])) - scaling_factor - 20*log10(r));
    }
    fprintf(fid,"e\n");

    fclose(fid);

    printf("done.\n");
    return 0;
}
