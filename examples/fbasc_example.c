//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "fbasc_example.m"

int main() {
    // options
    unsigned int num_channels=64;
    unsigned int samples_per_frame=512;
    unsigned int bytes_per_frame = 37;
    unsigned int num_frames=2;

    // derived values
    unsigned int num_samples = samples_per_frame * num_frames;

    // create fbasc codecs
    fbasc fbasc_encoder = fbasc_create(FBASC_ENCODER, num_channels, samples_per_frame, bytes_per_frame);
    fbasc fbasc_decoder = fbasc_create(FBASC_DECODER, num_channels, samples_per_frame, bytes_per_frame);

    fbasc_print(fbasc_encoder);

    unsigned int bytes_per_header = fbasc_compute_header_length(num_channels,
                                                                samples_per_frame,
                                                                bytes_per_frame);
    unsigned int i;
    float x[num_samples];   // original data sequence
    float y[num_samples];   // reconstructed sequence

#if 0
    // filtered noise signal
    unsigned int h_len = 55;
    float h[h_len];
    liquid_firdes_kaiser(h_len,0.1f,60.0f,0.0f,h);
    firfilt_rrrf q = firfilt_rrrf_create(h,h_len);
    for (i=0; i<num_samples; i++) {
        float noise = 0.1f*randnf();
        float t;
        firfilt_rrrf_push(q,noise);
        firfilt_rrrf_execute(q,&t);
        x[i] = t;
        x[i] *= hamming(i,num_samples);
    }
    firfilt_rrrf_destroy(q);
#else
    float phi=0.0f;
    float dphi=0.03f;
    for (i=0; i<num_samples; i++) {
        x[i] = 0.5f*cosf(2.0f*M_PI*phi) + 0.5f*cosf(2.0f*M_PI*phi*0.57f);
        x[i] *= 0.2f;
        x[i] *= hamming(i,num_samples);
        phi += dphi;
    }
#endif

    unsigned char header[bytes_per_header];
    unsigned char frame[bytes_per_frame];
    // encode/decode frames
    for (i=0; i<num_frames; i++) {
        // encode frame
        fbasc_encode(fbasc_encoder,
                     &x[i*samples_per_frame],
                     header,
                     frame);

        // decode frame
        fbasc_decode(fbasc_decoder,
                     header,
                     frame,
                     &y[i*samples_per_frame]);

        // optional: strip off frame id
        unsigned int frame_id = (header[0] << 8) | header[1];
        printf("frame id : %u\n", frame_id);
    }

    // compute error : signal-to-distortion ratio
    float s=0.0f;   // signal variance
    float e=0.0f;   // error variance
    for (i=0; i<num_samples-num_channels; i++) {
        // compute signal variance
        s += x[i]*x[i];

        // compute error variance
        float v = x[i] - y[i+num_channels];
        e += v*v;
    }
    float SNR = 10*log10f(s/e);
    printf("SNR = %8.2f dB\n", SNR);

    // open debug file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"num_channels = %u;\n", num_channels);
    fprintf(fid,"samples_per_frame = %u;\n", samples_per_frame);
    fprintf(fid,"num_frames = %u;\n", num_frames);
    fprintf(fid,"num_samples = samples_per_frame * num_frames;\n");

    // write data to file
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %16.8e; y(%4u) = %16.8e;\n", i+1, x[i], i+1, y[i]);

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
    fprintf(fid,"w = hamming(length(x)).';\n");
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"X = 20*log10(abs(fftshift(fft(x.*w,nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y.*w,nfft))));\n");
    fprintf(fid,"plot(f,X,f,Y);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 40]);\n");
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

