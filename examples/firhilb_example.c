char __docstr__[] =
"Hilbert transform example.  This example demonstrates the"
" functionality of firhilbf (finite impulse response Hilbert transform)"
" which converts N complex samples at Fs to 2*N real samples at 2*Fs"
" and then back. A baseband tone at f*Fs (-0.5 < f < 0.5)"
" produces real output tones at"
" +/-(f+0.5)*Fs. The default input has two Hamming-windowed tones;"
" use -a 0 for a single windowed tone.";

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firhilb_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,             7, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    As,           60, 's', "filter stop-band suppression", NULL);
    liquid_argparse_add(float,    fc,          0.1, 'c', "signal frequency relative to the input sample rate", NULL);
    liquid_argparse_add(float,    a,           0.1, 'a', "secondary tone amplitude (0 for a single tone)", NULL);
    liquid_argparse_add(unsigned, num_samples, 240, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int h_len = 4*m+1;             // filter length
    unsigned int num_total_samples = num_samples + h_len;

    // create Hilbert transform object
    firhilbf qi = firhilbf_create(m,As);    // interpolator
    firhilbf qd = firhilbf_create(m,As);    // decimator
    firhilbf_print(qi);

    // data arrays
    float complex x[  num_total_samples];   // complex input
    float         y[2*num_total_samples];   // real output
    float complex z[  num_total_samples];   // complex output

    // initialize input array
    unsigned int i;
    for (i=0; i<num_total_samples; i++) {
        x[i]  = cexpf(_Complex_I*2*M_PI*fc*i) +
                cexpf(_Complex_I*2*M_PI*fc*i*1.3f)*a;
        x[i] *= (i < num_samples) ? 1.855f*liquid_hamming(i,num_samples) : 0.0f;
    }

    // Interpolation doubles the sample rate and translates the complex
    // baseband by Fs/2 (one quarter of the real output sample rate).
    firhilbf_interp_execute_block(qi, x, num_total_samples, y);

    // execute decimator (real to complex conversion)
    firhilbf_decim_execute_block(qd, y, num_total_samples, z);

    // destroy Hilbert transform object
    firhilbf_destroy(qi);
    firhilbf_destroy(qd);

    //
    // export results to file
    //
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"num_total_samples=%u;\n", num_total_samples);
    fprintf(fid,"%% Rates and time are relative to the complex input sample rate Fs.\n");
    fprintf(fid,"fs_x=1; fs_y=2; fs_z=1;\n");
    fprintf(fid,"tx = (0:(num_total_samples-1))/fs_x;\n");
    fprintf(fid,"ty = (0:(2*num_total_samples-1))/fs_y;\n");
    fprintf(fid,"tz = (0:(num_total_samples-1))/fs_z;\n");
    fprintf(fid,"%% The Hamming envelope and filter delays are visible in these traces.\n");

    for (i=0; i<num_total_samples; i++) {
        // print results
        fprintf(fid,"x(%3u) = %12.4e + %12.4ej;\n",   i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n",           2*i+1, y[2*i+0]);
        fprintf(fid,"y(%3u) = %12.4e;\n",           2*i+2, y[2*i+1]);
        fprintf(fid,"z(%3u) = %12.4e + %12.4ej;\n",   i+1, crealf(z[i]), cimagf(z[i]));
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(tx,imag(x),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  title('Complex input x, sample rate Fs');\n");
    fprintf(fid,"  ylabel('Amplitude');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(ty,y,'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  title('Interpolated real output y, sample rate 2 Fs');\n");
    fprintf(fid,"  ylabel('Amplitude');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(tz,real(z),'Color',[0.00 0.25 0.50],'LineWidth',1.3);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(tz,imag(z),'Color',[0.00 0.50 0.25],'LineWidth',1.3);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  legend('real','imag','location','northeast');\n");
    fprintf(fid,"  title('Recovered complex output z, sample rate Fs');\n");
    fprintf(fid,"  ylabel('Amplitude');\n");
    fprintf(fid,"  xlabel('Time (input sample periods)');\n");
    fprintf(fid,"  axis([0 num_total_samples -2 2]);\n");
    fprintf(fid,"  grid on;\n");

    // plot results
    fprintf(fid,"nfft=2^nextpow2(max(4096,length(y)));\n");
    fprintf(fid,"%% Compute magnitude spectra on one physical frequency scale.\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/num_samples,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y/num_samples,nfft))));\n");
    fprintf(fid,"Z=20*log10(abs(fftshift(fft(z/num_samples,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(f*fs_x,X,'LineWidth',1,'Color',[0.50 0.50 0.50]);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(f*fs_z,Z,'LineWidth',1,'Color',[0.00 0.25 0.50]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  title('Complex input and recovered output, sample rate Fs');\n");
    fprintf(fid,"  legend('input x','recovered z','location','northeast');\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]); grid on;\n");
    fprintf(fid,"  ylabel('Magnitude (dB)');\n");
    fprintf(fid,"  xlabel('Frequency / Fs (input sample rate)');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f*fs_y,Y,'LineWidth',1,'Color',[0.00 0.50 0.25]);\n");
    fprintf(fid,"  title('Real output: baseband shifted by Fs/2, with conjugate image');\n");
    fprintf(fid,"  axis([-1 1 -80 20]); grid on;\n");
    fprintf(fid,"  ylabel('Magnitude (dB)');\n");
    fprintf(fid,"  xlabel('Frequency / Fs (input sample rate)');\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}
