//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "fbasc_example.m"

int main() {
    // options
    unsigned int num_channels=16;
    unsigned int samples_per_frame=512;
    unsigned int bytes_per_frame = samples_per_frame + num_channels + 1;

    unsigned int num_frames=2;

    // create fbasc codecs
    fbasc fbasc_encoder = fbasc_create(FBASC_ENCODER, num_channels, samples_per_frame, bytes_per_frame);
    fbasc fbasc_decoder = fbasc_create(FBASC_DECODER, num_channels, samples_per_frame, bytes_per_frame);

    fbasc_print(fbasc_encoder);

    // open debug file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"num_channels = %u;\n", num_channels);
    fprintf(fid,"samples_per_frame = %u;\n", samples_per_frame);
    fprintf(fid,"num_frames = %u;\n", num_frames);

    float phi=0.0f;
    float dphi=0.03f;
    unsigned int i, j;
    unsigned int n=0;   // output file sample counter
    float x[samples_per_frame];
    float X[samples_per_frame];
    float y[samples_per_frame];
    unsigned char framedata[bytes_per_frame];
    for (i=0; i<num_frames; i++) {
        for (j=0; j<samples_per_frame; j++) {
            x[j] = (i==0) ? 0.5f*cosf(2.0f*M_PI*phi) + 0.5f*cosf(2.0f*M_PI*phi*0.57f) : 0.0f;
            //x[j] = (i==0) ? cosf(2.0f*M_PI*phi) : 0.0f;
            phi += dphi;
            x[j] *= 0.1f*kaiser(j,samples_per_frame,10.0f,0);
            //x[j] = randnf()*0.1f;
        }

#if 0
        fbasc_encode(fbasc_encoder, x, framedata);

        //fbasc_decode(fbasc_decoder, framedata, y);
#else
        // run analyzer
        fbasc_encoder_run_analyzer(fbasc_encoder, x, X);

        // run synthesizer
        fbasc_decoder_run_synthesizer(fbasc_decoder, X, y);
#endif

        // write data to file
        for (j=0; j<samples_per_frame; j++) {
            fprintf(fid, "x(%4u) = %8.4e;\n", n+1, x[j]);
            fprintf(fid, "y(%4u) = %8.4e;\n", n+1, y[j]);
            n++;
        }
    }

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"num_samples = num_frames * samples_per_frame;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"plot(t,x,t-num_channels,y);\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"grid on\n");
    fprintf(fid,"legend('original','reconstructed',1);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"X = 20*log10(abs(fftshift(fft(x,nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"plot(f,X,f,Y);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -120 40]);\n");
    fprintf(fid,"grid on\n");
    fprintf(fid,"legend('original','reconstructed',1);\n");

    // close debug file
    fclose(fid);
    printf("results wrtten to %s\n", OUTPUT_FILENAME);

    // destroy objects
    fbasc_destroy(fbasc_encoder);
    fbasc_destroy(fbasc_decoder);

    printf("done.\n");
    return 0;
}

