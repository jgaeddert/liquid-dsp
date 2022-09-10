// test half-band filter generation
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "firdespm_halfband_test.m"

// ideally all even index values up to center is zero
float firdespm_halfband_utility(unsigned int _m,
                                float *      _h)
{
    float u = 0.0f;
    unsigned int i;
    for (i=0; i<_m; i++)
        u += _h[2*i] * _h[2*i];
    return 10*log10f( u / (float)_m );
}

float firdespm_halfband(unsigned int _m,
                        float *      _h,
                        float        _ft,
                        float        _gamma)
{
    unsigned int h_len = 4*_m + 1;
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int num_bands = 2;
    float f0 = 0.25f - 0.5f*_ft*_gamma;
    float f1 = 0.25f + 0.5f*_ft;
    float bands[4]   = {0.00f, f0, f1, 0.50f};
    float des[2]     = {1.0f, 0.0f};
    float weights[2] = {1.0f, 1.0f}; // best with {1, 1}
    liquid_firdespm_wtype wtype[2] = { // best with {flat, flat}
        LIQUID_FIRDESPM_FLATWEIGHT, LIQUID_FIRDESPM_FLATWEIGHT,};
    firdespm_run(h_len,num_bands,bands,des,weights,wtype,btype,_h);
    return firdespm_halfband_utility(_m, _h);
}

int main(int argc, char*argv[])
{
    unsigned int m = 25;        // filter semi-length
    float        ft = 0.05f;    // transition bandwidth
    unsigned int h_len = 4*m+1;

    // adjust lower edge in design until utility is minimized
    float h[h_len];
    float g_opt = 0.0f;
    float u_opt = 0.0f;
    float gamma = 1.1f;
    while (gamma > 0.8f) {
        float u = firdespm_halfband(m, h, ft, gamma);
        printf("gamma=%12.8f, u = %12.6f", gamma, u);
        if (u < u_opt) {
            u_opt = u;
            g_opt = gamma;
            printf(" *\n");
        } else {
            printf("\n");
            break;
        }
        gamma *= 0.9999f;
    }
    //g_opt = 0.92329967f - 3.2e-3f;
    float u = firdespm_halfband(m, h, ft, g_opt);
    printf("g_opt : %12.8f, u = %12.8f\n", g_opt, u);

    // print coefficients
    unsigned int i;
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u; m=%u;\n", h_len, m);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %20.8e;\n", i+1, h[i]);
        
    fprintf(fid,"g = h; g([0:(m-1)]*2+1) = 0;\n");
    fprintf(fid,"g(4*m - [0:(m-1)]*2+1) = 0;\n");

    fprintf(fid,"nfft=9600;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"G=20*log10(abs(fftshift(fft(g,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"  plot(f,H,'LineWidth',2,f,G,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  legend('firdespm','zeroed even values');\n");
    fprintf(fid,"  title('Filter design (firdespm)');\n");
    fprintf(fid,"  axis([-0.5 0.5 -120 5]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

