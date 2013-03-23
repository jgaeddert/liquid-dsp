// 
// mskmodem_test.c
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "mskmodem_test.m"

// print usage/help message
void usage()
{
    printf("mskmodem_test -- minimum-shift keying modem example\n");
    printf("options (default values in <>):\n");
    printf("  h     : print help\n");
    printf("  k     : samples/symbol,         default:  8\n");
    printf("  n     : number of data symbols, default: 10\n");
    printf("  s     : SNR [dB] <30>\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int k=8;                   // filter samples/symbol
    unsigned int num_data_symbols = 20; // number of data symbols
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]

    int dopt;
    while ((dopt = getopt(argc,argv,"hk:n:s:")) != EOF) {
        switch (dopt) {
        case 'h': usage();                         return 0;
        case 'k': k = atoi(optarg);                break;
        case 'n': num_data_symbols = atoi(optarg); break;
        case 's': SNRdB = atof(optarg);            break;
        default:
            exit(1);
        }
    }

    unsigned int i;

    // derived values
    unsigned int num_symbols = num_data_symbols;
    unsigned int num_samples = k*num_symbols;
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    unsigned char sym_in[num_symbols];      // input symbols
    float phi[num_samples];                 // transmitted phase
    float complex x[num_samples];           // transmitted signal
    float complex y[num_samples];           // received signal
    float complex z[num_samples];           // output...
    unsigned char sym_out[num_symbols];     // output symbols

    // create transmit/receive interpolator/decimator
#if 1
    // regular MSK
    unsigned int ht_len = k;
    float ht[ht_len];
    for (i=0; i<ht_len; i++)
        ht[i] = M_PI / (2.0f * k);
#else
    // full-response raised-cosine pulse
    unsigned int ht_len = k;
    float ht[ht_len];
    for (i=0; i<ht_len; i++)
        ht[i] = M_PI / (2.0f*k) * (1.0f - cosf(2.0f*M_PI*i/(float)ht_len));
#endif
    for (i=0; i<ht_len; i++)
        printf("ht(%3u) = %12.8f;\n", i+1, ht[i]);
    interp_rrrf interp_tx = interp_rrrf_create(k, ht, ht_len);

    // generate symbols and interpolate
    float theta = M_PI / 4.0f;
    for (i=0; i<num_symbols; i++) {
        sym_in[i] = rand() % 2;
        interp_rrrf_execute(interp_tx, sym_in[i] ? 1.0f : -1.0f, &phi[k*i]);

        // accumulate phase
        unsigned int j;
        for (j=0; j<k; j++) {
            x[i*k+j] = cexpf(_Complex_I*theta);
            theta += phi[i*k + j];
        }
    }

    // push through channel
    for (i=0; i<num_samples; i++)
        y[i] = x[i] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
    
    // create decimator
#if 1
    unsigned int m = 3;
    float bw = 0.9f / (float)k;
    firfilt_crcf decim_rx = firfilt_crcf_create_kaiser(2*k*m+1, bw, 60.0f, 0.0f);
    printf("bw = %f\n", bw);
#else
    unsigned int hr_len = k;
    float hr[hr_len];
    for (i=0; i<hr_len; i++)
        hr[i] = 4.0f / (float)k;
    firfilt_crcf decim_rx = firfilt_crcf_create(hr, hr_len);
#endif

    // run receiver
    float complex x_prime = 0.0f;
    unsigned int n=0;
    for (i=0; i<num_samples; i++) {
        // push through filter
        firfilt_crcf_push(decim_rx, y[i]);
        firfilt_crcf_execute(decim_rx, &z[i]);

        z[i] *= 2.0f * bw;

        // decimate output
        if ( (i%k)==k-1 ) {
#if 0
            sym_out[n] = phi_prime[i] > 0.0f ? 1 : 0;
            printf("%3u : %12.8f (%1u)", n, phi_prime[i], sym_out[n]);
            if (n >= 0) printf(" (%1u)\n", sym_in[n-0]);
            else          printf("\n");
            n++;
#endif
        }
    }

    // compute number of errors
    unsigned int num_errors = 0;
    for (i=0; i<num_data_symbols; i++)
        num_errors += sym_in[i] == sym_out[i] ? 0 : 1;

    printf("errors : %3u / %3u\n", num_errors, num_data_symbols);

    // destroy objects
    interp_rrrf_destroy(interp_tx);
    firfilt_crcf_destroy(decim_rx);

    // compute power spectral density
    unsigned int num_transforms = 0;
    unsigned int nfft = 512;
    spgram periodogram = spgram_create_kaiser(nfft, nfft/2, 8.0f);
    float psd[nfft];
    float complex X[nfft];
    for (i=0; i<nfft; i++)
        psd[i] = 0.0f;
    for (i=0; i<num_samples; i++) {
        spgram_push(periodogram, &y[i], 1);
        if ( ((i+1)%(nfft/4))==0 ) {
            spgram_execute(periodogram, X);
            unsigned int k;
            for (k=0; k<nfft; k++) {
                float Xmag = cabsf(X[k]);
                if (num_transforms>0) psd[k] += Xmag;
                else                  psd[k]  = Xmag;
            }
            num_transforms++;
        }
    }
    // ensure at least one transform is taken
    if (num_transforms==0) {
        spgram_execute(periodogram, X);
        unsigned int k;
        for (k=0; k<nfft; k++)
            psd[k] = cabsf(X[k]);
        num_transforms++;
    }
    for (i=0; i<nfft; i++)
        psd[i] /= (float)(num_transforms);
    spgram_destroy(periodogram);

    // 
    // export results
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"nfft        = %u;\n", nfft);

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    fprintf(fid,"z = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }
    // save PSD with FFT shift
    fprintf(fid,"psd = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"psd(%4u) = %12.8f;\n", i+1, psd[(i+nfft/2)%nfft]);
    }

    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"i = 1:k:num_samples;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,4,1:3);\n");
    fprintf(fid,"  plot(t,real(x),'-', t,imag(x),'-');\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('x(t)');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,4,5:7);\n");
    fprintf(fid,"  plot(t,real(z),'-', t(i),real(z(i)),'ob',...\n");
    fprintf(fid,"       t,imag(z),'-', t(i),imag(z(i)),'og');\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('\"matched\" filter output');\n");
    fprintf(fid,"  grid on;\n");
    // plot I/Q constellations
    fprintf(fid,"subplot(3,4,4);\n");
    fprintf(fid,"  plot(real(y),imag(y),'-',real(y(i)),imag(y(i)),'rs','MarkerSize',4);\n");
    fprintf(fid,"  xlabel('I');\n");
    fprintf(fid,"  ylabel('Q');\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.2);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,4,8);\n");
    fprintf(fid,"  plot(real(z),imag(z),'-',real(z(i)),imag(z(i)),'rs','MarkerSize',4);\n");
    fprintf(fid,"  xlabel('I');\n");
    fprintf(fid,"  ylabel('Q');\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.2);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    // plot PSD
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"subplot(3,4,9:12);\n");
    fprintf(fid,"  plot(f,20*log10(psd),'LineWidth',1.5);\n");
    fprintf(fid,"  axis([-0.5 0.5 -40 10]);\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}
