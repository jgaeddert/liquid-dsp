//
// symsync_crcf_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "liquid.h"

#define OUTPUT_FILENAME "symsync_crcf_example.m"

int main() {
    // options
    unsigned int k=2;   // samples/symbol
    unsigned int m=3;   // filter delay (symbols)
    float beta=0.3f;    // filter excess bandwidth factor
    unsigned int num_filters=32;
    unsigned int num_symbols=1024;

    float bt=0.02f;     // loop filter bandwidth
    float dt=0.2f;      // fractional sample offset
    unsigned int ds=1;  // additional symbol delay
    
    // use random data or 101010 phasing pattern
    bool random_data=true;


    unsigned int i, n=0;

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,dt,h);

    // create interpolator
    interp_crcf q = interp_crcf_create(k,h,h_len);

    // design polyphase filter
    unsigned int H_len = 2*num_filters*k*m + 1;
    float H[H_len];
    design_rrc_filter(k*num_filters,m,beta,0,H);
    // create symbol synchronizer
    symsync_crcf d = symsync_crcf_create(k, num_filters, H, H_len);
    symsync_crcf_set_lf_bw(d,bt);

    unsigned int num_samples = k*num_symbols;
    float complex x[num_symbols];
    float complex y[num_samples];
    float complex z[num_samples];

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<num_symbols; i++) {
        if (random_data) {
            // random signal (QPSK)
            x[i]  = rand() % 2 ? 1.0f : -1.0f;
            x[i] += rand() % 2 ? _Complex_I * 1.0f : -_Complex_I*1.0f;
        } else {
            x[i] = (i%2) ? 1.0f : -1.0f;  // 101010 phasing pattern
        }
    }

    // run interpolator
    for (i=0; i<num_symbols; i++) {
        interp_crcf_execute(q, x[i], &y[n]);
        n+=k;
    }

    // run symbol synchronizer
    unsigned int num_symbols_sync=0;
    unsigned int nn;
    float tau[num_samples];
    for (i=ds; i<num_samples; i++) {
        tau[num_symbols_sync] = symsync_crcf_get_tau(d);
        symsync_crcf_execute(d, &y[i], 1, &z[num_symbols_sync], &nn);
        num_symbols_sync += nn;
    }

    printf("h(t) :\n");
    for (i=0; i<h_len; i++) {
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);
    }

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++) {
        //printf("  x(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"x(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(x[i]), cimagf(x[i]));
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        //printf("  y(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"y(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    printf("z(t) :\n");
    for (i=num_symbols_sync-10; i<num_symbols_sync; i++)
        printf("  z(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i]), cimagf(z[i]));
    for (i=0; i<num_symbols_sync; i++) {
        //printf("  z(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i]), cimagf(z[i]));
        fprintf(fid,"z(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }

    // save timing offset
    for (i=0; i<num_symbols_sync; i++) {
        //printf("  z(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i]), cimagf(z[i]));
        fprintf(fid,"tau(%3u) = %12.8f;\n", i+1, tau[i]);
    }


    fprintf(fid,"\n\n");
    fprintf(fid,"zp = filter(h,1,y);\n");
    fprintf(fid,"figure;\nhold on;\n");
    fprintf(fid,"plot([0:length(x)-1],          real(x),    'ob');\n");
    fprintf(fid,"plot([0:length(y)-1]/2  -m,    real(y),    '-','Color',[0.8 0.8 0.8]);\n");
    fprintf(fid,"plot([0:length(zp)-1]/2 -2*m,  real(zp/k), '-b');\n");
    fprintf(fid,"plot([0:length(z)-1]    -2*m+1,real(z),    'xr');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('Symbol Index');\n");
    fprintf(fid,"ylabel('Output Signal');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('sym in','interp','mf','sym out',0);\n");

    fprintf(fid,"t0=1:floor(0.25*length(z));\n");
    fprintf(fid,"t1=ceil(0.25*length(z)):length(z);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(real(z(t0)/k),imag(z(t0)/k),'x','Color',[0.6 0.6 0.6]);\n");
    fprintf(fid,"plot(real(z(t1)/k),imag(z(t1)/k),'x','Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square; grid on;\n");
    fprintf(fid,"xlabel('In-phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"legend(['first 25%%'],['last 75%%'],1);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"tt = 0:(length(tau)-1);\n");
    fprintf(fid,"b = floor(num_filters*tau + 0.5);\n");
    fprintf(fid,"plot(tt,tau*num_filters,tt,b);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('filterbank index');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([0 length(tau) -1 num_filters]);\n");
    fclose(fid);

    printf("results written to %s.\n", OUTPUT_FILENAME);

    // clean it up
    interp_crcf_destroy(q);
    symsync_crcf_destroy(d);
    printf("done.\n");
    return 0;
}
