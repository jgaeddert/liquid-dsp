//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_example.m"

int main() {
    // options
    unsigned int num_channels=6;    // number of channels
    unsigned int m=5;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_symbols=64;    // number of baseband symbols
    int type = FIRPFBCH_NYQUIST;

    unsigned int i, j, k, n;

    // create objects
    firpfbch cs = firpfbch_create(num_channels, m, slsl, 0, type, FIRPFBCH_SYNTHESIZER);
    firpfbch ca = firpfbch_create(num_channels, m, slsl, 0, type, FIRPFBCH_ANALYZER);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);
    fprintf(fid,"num_samples = num_channels*num_symbols;\n");

    fprintf(fid,"x  = zeros(num_channels,num_symbols);\n");
    fprintf(fid,"y0 = zeros(1,num_samples);\n");
    fprintf(fid,"y1 = zeros(1,num_samples);\n");
    fprintf(fid,"z0 = zeros(num_channels,num_symbols);\n");
    fprintf(fid,"z1 = zeros(num_channels,num_symbols);\n");

    // generate data buffers
    float complex  x[num_channels]; // input
    float complex y0[num_channels]; // conventional synthesizer output
    float complex y1[num_channels]; // filterbank synthesizer output
    float complex z0[num_channels]; // conventional analyzer output
    float complex z1[num_channels]; // filterbank analyzer output

    // objects to run conventional channelizer
    interp_crcf interp[num_channels];
    nco nco_cs[num_channels];
    nco nco_ca[num_channels];
    decim_crcf decim[num_channels];

    // retrieve filter taps from channelizer objects
    unsigned int h_len = 2*m*num_channels;
    float hcs[h_len];
    float hca[h_len];
    firpfbch_get_filter_taps(cs,hcs);
    firpfbch_get_filter_taps(ca,hca);

    float f;
    for (i=0; i<num_channels; i++) {
        f = 2.0f * M_PI * (float)(i) / (float)(num_channels);
        // synthesis
        interp[i] = interp_crcf_create(num_channels, hcs, h_len);
        nco_cs[i] = nco_create(LIQUID_VCO);
        nco_set_frequency(nco_cs[i], f);

        // analysis
        decim[i] = decim_crcf_create(num_channels, hca, h_len);
        nco_ca[i] = nco_create(LIQUID_VCO);
        nco_set_frequency(nco_ca[i], f);
    }

    //for (i=0; i<h_len; i++)
    //    printf("hcs(%3u) = %12.8f; hca(%3u) = %12.8f;\n", i+1, hcs[i], i+1, hca[i]);

    // synthesize time series
    // intermediate conventional channelizer buffers
    float complex y0a[num_channels];
    float complex y0b[num_channels];
    float complex z0a[num_channels];
    float rmse_synthesis=0.0f;
    float rmse_analysis=0.0f;
    float complex err;
    n=0;
    for (i=0; i<num_symbols; i++) {
        // generate random samples
        for (j=0; j<num_channels; j++) {
            x[j] = randnf() + randnf()*_Complex_I;
        }

        // 
        // execute conventional synthesizer
        //
        for (j=0; j<num_channels; j++)
            y0[j] = 0.0f;

        for (j=0; j<num_channels; j++) {
            // run interpolator
            interp_crcf_execute(interp[j],x[j],y0a);

            // up-convert
            nco_mix_block_up(nco_cs[j],y0a,y0b,num_channels);

            // append to output buffer
            for (k=0; k<num_channels; k++) {
                y0[k] += y0b[k] / (float)(num_channels);
            }
        }

        // 
        // execute synthesis filter bank
        //
        firpfbch_execute(cs, x, y1);



        //
        // execute conventional analyzer
        //
        for (j=0; j<num_channels; j++)
            z0[j] = 0.0f;
        for (j=0; j<num_channels; j++) {
            // down-convert
            nco_mix_block_down(nco_ca[j],y0,z0a,num_channels);

            // run decimator
            //decim_crcf_execute(decim[j],z0a,&z0[j],num_channels-1);
            decim_crcf_execute(decim[j],z0a,&z0[j],0);
        }

        // 
        // execute analysis filter bank
        //
        firpfbch_execute(ca, y1, z1);

        // compute error
        for (j=0; j<num_channels; j++) {
            // synthesis
            err = y0[j] - y1[j];
            rmse_synthesis += crealf(err*conjf(err));

            // analysis
            err = z0[j] - z1[j];
            rmse_analysis += crealf(err*conjf(err));
        }

        // save output
        for (j=0; j<num_channels; j++) {
            // input
            fprintf(fid,"x(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(x[j]), cimag(x[j]));

            // synthesis
            fprintf(fid,"y0(%4u) = %12.4e + j*%12.4e;\n", n+j+1, crealf(y0[j]), cimag(y0[j]));
            fprintf(fid,"y1(%4u) = %12.4e + j*%12.4e;\n", n+j+1, crealf(y1[j]), cimag(y1[j]));

            // analysis
            fprintf(fid,"z0(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(z0[j]), cimag(z0[j]));
            fprintf(fid,"z1(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(z1[j]), cimag(z1[j]));
        }
        n += num_channels;
    }
    rmse_synthesis = sqrtf(rmse_synthesis / (float)(num_channels*num_symbols) );
    rmse_analysis  = sqrtf(rmse_analysis  / (float)(num_channels*num_symbols) );
    printf("rmse (synthesis)    : %12.4e\n", rmse_synthesis);
    printf("rmse (analysis)     : %12.4e\n", rmse_analysis);

    fprintf(fid,"\n\n");
    fprintf(fid,"t  = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(y0), t,real(y1));\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(t,imag(y0), t,imag(y1));\n");

    fprintf(fid,"ts = 0:(num_symbols-1);\n");
    fprintf(fid,"for i=1:num_channels,\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(ts,real(z0(i,:)),'-x', ts,real(z1(i,:)),'-x');\n");
    fprintf(fid,"    axis([0 (num_symbols-1) -2 2]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(ts,imag(z0(i,:)),'-x', ts,imag(z1(i,:)),'-x');\n");
    fprintf(fid,"    axis([0 (num_symbols-1) -2 2]);\n");
    fprintf(fid,"end;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // clean up allocated objects
    for (i=0; i<num_channels; i++) {
        interp_crcf_destroy(interp[i]);
        nco_destroy(nco_cs[i]);
    }
    firpfbch_destroy(cs);

    printf("done.\n");
    return 0;
}

