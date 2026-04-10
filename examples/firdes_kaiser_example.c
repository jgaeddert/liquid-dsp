const char __docstr__[] =
"This example demonstrates finite impulse response filter design"
" using a Kaiser window.";

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firdes_kaiser_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    fc,    0.20f, 'f', "filter cutoff frequency (relative)", NULL);
    liquid_argparse_add(float,    As,    60.0f, 's', "filter stop-band attenuation [dB]",  NULL);
    liquid_argparse_add(float,    mu,    0.00f, 'm', "filter fractional timing offset",    NULL);
    liquid_argparse_add(unsigned, h_len, 55,    'n', "filter length (number of coefficients)", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if ( fc <= 0.0f )
        return liquid_error(LIQUID_EICONFIG,"filter cutoff frequency must be greater than zero");
    if ( h_len == 0 )
        return liquid_error(LIQUID_EICONFIG,"filter length must be greater than zero");

    printf("filter design parameters\n");
    printf("    cutoff frequency            :   %8.4f\n", fc);
    printf("    stop-band attenuation [dB]  :   %8.4f\n", As);
    printf("    fractional sample offset    :   %8.4f\n", mu);
    printf("    filter length               :   %u\n", h_len);

    // generate the filter
    unsigned int i;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_kaiser(h_len,fc,As,mu,h);

    // print coefficients
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // output to file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n",h_len);
    fprintf(fid,"fc=%12.4e;\n",fc);
    fprintf(fid,"As=%12.4e;\n",As);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h*2*fc,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Filter design/Kaiser window f_c: %f, S_L: %f, h: %u']);\n",
            fc, -As, h_len);
    fprintf(fid,"axis([-0.5 0.5 -As-40 10]);\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

