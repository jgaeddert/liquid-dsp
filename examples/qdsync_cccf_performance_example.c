const char __docstr__[] =
"This example tests the performance for detecting and decoding frames"
" with the qdsync_cccf object.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qdsync_cccf_performance_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, sequence_len,240, 'n', "number of sync symbols", NULL);
    liquid_argparse_add(unsigned, k,             2, 'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,             7, 'm', "filter delay [symbols]", NULL);
    liquid_argparse_add(float,    beta,        0.3, 'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(char*,ftype_str,"arkaiser", 'f', "filter type", liquid_argparse_firfilt);
    liquid_argparse_add(float,    threshold,  0.20, 'z', "detection threshold", NULL);
    liquid_argparse_add(unsigned, min_missed,    5, 'e', "minimum number of errors (missed detects) to simulation", NULL);
    liquid_argparse_add(unsigned, min_trials,   20, 't', "minimum number of packet trials to simulate", NULL);
    liquid_argparse_add(unsigned, max_trials,  800, 'T', "maximum number of packet trials to simulate", NULL);
    liquid_argparse_add(float,    SNRdB,       -25, 's', "noise standard deviation", NULL);
    liquid_argparse_parse(argc,argv);

    // generate synchronization sequence (QPSK symbols)
    LIQUID_VLA(liquid_float_complex, seq, sequence_len);
    unsigned int i;
    for (i=0; i<sequence_len ; i++) {
        seq[i] = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                 (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
    }

    // interpolate sequence
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,0);
    unsigned int num_symbols = sequence_len + 2*m + 3*sequence_len;
    unsigned int buf_len = num_symbols * k;
    LIQUID_VLA(liquid_float_complex, buf_0, buf_len);
    LIQUID_VLA(liquid_float_complex, buf_1, buf_len);
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        liquid_float_complex s = i < sequence_len ? seq[i] : 0;

        // interpolate symbol
        firinterp_crcf_execute(interp, s, buf_0 + i*k);
    }
    firinterp_crcf_destroy(interp);

    // create sync object and run signal through
    qdsync_cccf q = qdsync_cccf_create_linear(seq, sequence_len, ftype, k, m, beta, NULL, NULL);
    qdsync_cccf_set_threshold(q, threshold);

    // open file for storing results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all; SNR=[]; Pmd=[];\n");
    while (SNRdB <= 5.0f)
    {
        float        nstd = powf(10.0f, -SNRdB/20.0f) * M_SQRT2;
        unsigned int t;
        unsigned int num_trials = 0;
        unsigned int num_missed = 0;
        while (1) {
            for (t=0; t<min_trials; t++) {
                // copy buffer and add noise
                for (i=0; i<buf_len; i++)
                    buf_1[i] = buf_0[i] + nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

                // run through synchronizer
                qdsync_cccf_reset(q);
                qdsync_cccf_execute(q, buf_1, buf_len);
                num_missed += qdsync_cccf_is_detected(q) ? 0 : 1;
            }
            num_trials += min_trials;
            if (num_missed >= min_missed)
                break;
            if (num_trials >= max_trials)
                break;
        }
        float pmd = (float)num_missed / (float)num_trials;
        printf("SNR: %8.3f dB, missed %3u / %3u (%5.1f%%)\n", SNRdB, num_missed, num_trials, pmd*100);
        if (num_missed < min_missed)
            break;
        fprintf(fid,"SNR(end+1)=%12g; Pmd(end+1)=%12g;\n", SNRdB, pmd);
        SNRdB += 1.0f;
    }
    qdsync_cccf_destroy(q);

    fprintf(fid,"sequence_len = %u;\n", sequence_len);
    fprintf(fid,"figure('color','white','position',[100 100 640 640]);\n");
    fprintf(fid,"semilogy(SNR,Pmd,'-x');\n");
    fprintf(fid,"xlabel('SNR [dB]');\n");
    fprintf(fid,"ylabel('Probability of Missed Detect');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to '%s'\n", filename);
    return 0;
}

