// This example tests the performance for detecting and decoding frames
// with the framegen64 and framesync64 objects.
// SEE ALSO: framesync64_example.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#define OUTPUT_FILENAME  "framesync64_performance_example.m"

// add noise to channel
void frame64_add_noise(float complex * _buf, float _SNRdB)
{
    float nstd = powf(10.0f, -_SNRdB/20.0f) * M_SQRT1_2;
    unsigned int i;
    for (i=0; i<LIQUID_FRAME64_LEN; i++)
        _buf[i] += nstd*( randnf() + _Complex_I*randnf() );
}

int main(int argc, char*argv[])
{
    // create frame generator, synchronizer objects
    framegen64  fg = framegen64_create();
    framesync64 fs = framesync64_create(NULL,NULL);
    unsigned int num_trials = 100;

    // create buffer for the frame samples
    float complex frame[LIQUID_FRAME64_LEN];
    float SNRdB = -6.0f;
    FILE* fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"SNR=[]; pdetect=[]; pvalid=[];\n");
    printf("# %8s %6s %6s %6s\n", "SNR", "detect", "valid", "trials");
    while (SNRdB < 6.0f) {
        framesync64_reset_framedatastats(fs);
        unsigned int i;
        for (i=0; i<num_trials; i++) {
            // generate the frame with random header and payload
            framegen64_execute(fg, NULL, NULL, frame);

            // add channel effects
            frame64_add_noise(frame, SNRdB);

            // synchronize/receive the frame
            framesync64_execute(fs, frame, LIQUID_FRAME64_LEN);
        }
        // print statistics
        framedatastats_s stats = framesync64_get_framedatastats(fs);
        printf("  %8.3f %6u %6u %6u\n",
            SNRdB,stats.num_frames_detected,stats.num_payloads_valid,num_trials);
        fprintf(fid,"SNR(end+1)=%g; pdetect(end+1)=%g; pvalid(end+1)=%g;\n",
                SNRdB,
                (float)stats.num_frames_detected / (float)num_trials,
                (float)stats.num_payloads_valid  / (float)num_trials);
        SNRdB += 0.5f;
    }
    fprintf(fid,"semilogy(SNR, 1-pdetect+eps, 'LineWidth', 2, SNR, 1-pvalid+eps, 'LineWidth', 2);\n");
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Prob. of Error');\n");
    fprintf(fid,"legend('detect','decoding','location','northeast');\n");
    fprintf(fid,"axis([SNR(1) SNR(end) 1e-3 1]);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // clean up allocated objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
    return 0;
}
