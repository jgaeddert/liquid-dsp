//
// filter_rnyquist.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_FREQ "figures.gen/filter_rnyquist.gnu"

int main() {
    // options
    unsigned int k = 2;     // samples/symbol
    unsigned int m = 9;     // symbol delay
    float beta = 0.3f;      // excess bandwidth factor
    unsigned int nfft = 2048;

    // derived values
    unsigned int h_len = 2*k*m+1;

    // arrays
    float h_arkaiser[h_len];
    float h_rkaiser[h_len];
    float h_rrc[h_len];
    float h_hM3[h_len];

    // design filters
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_ARKAISER,k, m, beta, 0, h_arkaiser);
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_RKAISER, k, m, beta, 0, h_rkaiser);
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_RRC,     k, m, beta, 0, h_rrc);
    liquid_firdes_rnyquist(LIQUID_RNYQUIST_hM3,     k, m, beta, 0, h_hM3);

    // compute relative stop-band energy
    float As_arkaiser = liquid_filter_energy(h_arkaiser, h_len, (1.0f+beta)/(2.0f*k), nfft);
    float As_rkaiser  = liquid_filter_energy(h_rkaiser,  h_len, (1.0f+beta)/(2.0f*k), nfft);
    float As_rrc      = liquid_filter_energy(h_rrc,      h_len, (1.0f+beta)/(2.0f*k), nfft);
    float As_hM3      = liquid_filter_energy(h_hM3,      h_len, (1.0f+beta)/(2.0f*k), nfft);
    printf("    arkaiser    :   %12.6f dB\n", 20*log10f(As_arkaiser));
    printf("    rkaiser     :   %12.6f dB\n", 20*log10f(As_rkaiser));
    printf("    rrc         :   %12.6f dB\n", 20*log10f(As_rrc));
    printf("    hM3         :   %12.6f dB\n", 20*log10f(As_hM3));

    // compute filter power spectral density
    float complex H_arkaiser[nfft];
    float complex H_rkaiser[nfft];
    float complex H_rrc[nfft];
    float complex H_hM3[nfft];
    int normalize = 1;
    liquid_doc_compute_psdf(h_arkaiser, h_len, H_arkaiser,  nfft, LIQUID_DOC_PSDWINDOW_NONE, normalize);
    liquid_doc_compute_psdf(h_rkaiser,  h_len, H_rkaiser,   nfft, LIQUID_DOC_PSDWINDOW_NONE, normalize);
    liquid_doc_compute_psdf(h_rrc,      h_len, H_rrc,       nfft, LIQUID_DOC_PSDWINDOW_NONE, normalize);
    liquid_doc_compute_psdf(h_hM3,      h_len, H_hM3,       nfft, LIQUID_DOC_PSDWINDOW_NONE, normalize);

    // generate frequency array
    float f[nfft];
    unsigned int i;
    for (i=0; i<nfft; i++)
        f[i] = (float)(i) / (float)nfft - 0.5f;

    // 
    // export data to file(s)
    //
    FILE * fid = NULL;

    // generate frequency-domain plot
    fid = fopen(OUTPUT_FILENAME_FREQ,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_FREQ);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0.0:0.5];\n");
    fprintf(fid,"set yrange [-120:20]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'ARKAISER',\\\n", LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'RKAISER',\\\n",  LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'RRC',\\\n",      LIQUID_DOC_COLOR_PURPLE);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title 'hM3',\\\n",      LIQUID_DOC_COLOR_RED);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '#000000' notitle\n");

    // save array to output
    unsigned int index;
    fprintf(fid,"# %12s %12s (ARKAISER)\n", "freq", "PSD [dB]");
    for (i=0; i<nfft; i++) {
        index = (i + nfft/2) % nfft;
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(H_arkaiser[index])) );
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# %12s %12s (RKAISER)\n", "freq", "PSD [dB]");
    for (i=0; i<nfft; i++) {
        index = (i + nfft/2) % nfft;
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(H_rkaiser[index])) );
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# %12s %12s (RRC)\n", "freq", "PSD [dB]");
    for (i=0; i<nfft; i++) {
        index = (i + nfft/2) % nfft;
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(H_rrc[index])) );
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# %12s %12s (hM3)\n", "freq", "PSD [dB]");
    for (i=0; i<nfft; i++) {
        index = (i + nfft/2) % nfft;
        fprintf(fid,"  %12.8f %12.4e\n", f[i], 20.0f*log10f(cabsf(H_hM3[index])) );
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# %12s %12s bounds\n", "freq", "PSD [dB]");
    for (i=0; i<20; i++)
        fprintf(fid,"  %12.8f %12.4e\n", (1.0f+beta)/(2.0f*k), -160.0f + 20.0f*i);
    fprintf(fid,"e\n");

    fclose(fid);
    return 0;
}
