const char __docstr__[] =
"Tests recursive least-squares (RLS) equalizer (EQ) on a QPSK"
" signal at the symbol level.";

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "eqrls_cccf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_symbols, 512,    'n', "number of symbols to observe", NULL);
    liquid_argparse_add(unsigned, num_train,   256,    't', "number of symbols to train over", NULL);
    liquid_argparse_add(unsigned, hc_len,      6,      'c', "channel filter length", NULL);
    liquid_argparse_add(unsigned, p,           12,     'p', "equalizer length (symbols, hp_len = 2*k*p+1)", NULL);
    liquid_argparse_parse(argc,argv);

    // bookkeeping variables
    LIQUID_VLA(liquid_float_complex, d, num_symbols);  // data sequence
    LIQUID_VLA(liquid_float_complex, y, num_symbols);  // received data sequence (filtered by channel)
    LIQUID_VLA(liquid_float_complex, d_hat, num_symbols);  // recovered data sequence
    LIQUID_VLA(liquid_float_complex, h, hc_len);       // channel filter coefficients
    LIQUID_VLA(liquid_float_complex, w, p);            // equalizer filter coefficients
    unsigned int i;

    // create equalizer (default initial coefficients)
    eqrls_cccf eq = eqrls_cccf_create(NULL,p);

    // create channel filter (random delay taps)
    h[0] = 1.0f;
    for (i=1; i<hc_len; i++)
        h[i] = (randnf() + randnf()*_Complex_I) * 0.1f;
    firfilt_cccf f = firfilt_cccf_create(h,hc_len);

    // generate random data signal
    for (i=0; i<num_symbols; i++)
        d[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;

    // filter data signal through channel
    for (i=0; i<num_symbols; i++) {
        firfilt_cccf_push(f,d[i]);
        firfilt_cccf_execute(f,&y[i]);
    }

    // run equalizer
    for (i=0; i<p; i++)
        w[i] = 0;
    eqrls_cccf_train(eq, w, y, d, num_train);

    // create filter from equalizer output
    firfilt_cccf feq = firfilt_cccf_create(w,p);

    // run equalizer filter
    for (i=0; i<num_symbols; i++) {
        firfilt_cccf_push(feq,y[i]);
        firfilt_cccf_execute(feq,&d_hat[i]);
    }

    //
    // print results
    //
    printf("channel:\n");
    for (i=0; i<hc_len; i++)
        printf("  h(%3u) = %12.8f + j*%12.8f\n", i, crealf(h[i]), cimagf(h[i]));

    printf("equalizer:\n");
    for (i=0; i<p; i++)
        printf("  w(%3u) = %12.8f + j*%12.8f\n", i, crealf(w[i]), cimagf(w[i]));

    // compute MSE
    liquid_float_complex e;
    float mse=0.0f;
    for (i=0; i<num_symbols; i++) {
        // compute mse
        e = d[i] - d_hat[i];
        mse += crealf(e*conj(e));
    }
    mse /= num_symbols;
    printf("mse: %12.8f\n", mse);

    // clean up objects
    firfilt_cccf_destroy(f);
    eqrls_cccf_destroy(eq);
    firfilt_cccf_destroy(feq);


    // 
    // export data to file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);

    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n=%u;\n",num_symbols);
    fprintf(fid,"num_train=%u;\n",num_train);
    fprintf(fid,"p=%u;\n",p);
    fprintf(fid,"hc_len=%u;\n",hc_len);

    for (i=0; i<hc_len; i++)
        fprintf(fid,"  h(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(h[i]), cimagf(h[i]));

    for (i=0; i<p; i++)
        fprintf(fid,"  w(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(w[i]), cimagf(w[i]));

    for (i=0; i<num_symbols; i++) {
        fprintf(fid,"  d(%3u)     = %12.4e + j*%12.4e;\n", i+1, crealf(d[i]),     cimagf(d[i]));
        fprintf(fid,"  y(%3u)     = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]),     cimagf(y[i]));
        fprintf(fid,"  d_hat(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(d_hat[i]), cimagf(d_hat[i]));
    }

    // plot results
    fprintf(fid,"\n\n");

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"W=20*log10(abs(fftshift(fft(w,nfft))));\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-r',f,W,'-b', f,H+W,'-k','LineWidth',2);\n");
    fprintf(fid,"xlabel('Normalied Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -10 10]);\n");
    fprintf(fid,"legend('channel','equalizer','composite');\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"stem(0:(hc_len-1),real(h),'-r');\n");
    fprintf(fid,"stem(0:(p-1),    real(w),'-b');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"ylabel('Real Coefficients');\n");
    fprintf(fid,"legend('channel','equalizer');\n");
    fprintf(fid,"axis([-0.25 max(hc_len,p)-0.75 -0.5 1.5]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"stem(0:(hc_len-1),imag(h),'-r');\n");
    fprintf(fid,"stem(0:(p-1),    imag(w),'-b');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"ylabel('Imag Coefficients');\n");
    fprintf(fid,"legend('channel','equalizer');\n");
    fprintf(fid,"axis([-0.25 max(hc_len,p)-0.75 -0.5 1.5]);\n");


    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(y,'xr',d_hat,'xb');\n");
    fprintf(fid,"axis('square');\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature');\n");
    fprintf(fid,"legend('received','equalized');\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    return 0;
}
