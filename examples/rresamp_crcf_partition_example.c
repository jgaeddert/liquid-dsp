const char __docstr__[] = "Demonstrate partitioning rresamp work in separate blocks";

#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "rresamp_crcf_partition_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, interp,  3, 'i', "output rate (interpolation factor)", NULL);
    liquid_argparse_add(unsigned, decim,   5, 'd', "input rate (decimation factor)", NULL);
    liquid_argparse_add(unsigned, m,      20, 'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(float,    bw,     -1, 'w', "filter bandwidth (normalized); negative for default", NULL);
    liquid_argparse_add(float,    As,     60, 'a', "stop-band attenuation [dB]", NULL);
    liquid_argparse_add(unsigned, n,      10, 'n', "block size", NULL);
    liquid_argparse_parse(argc,argv);

    // create two identical resampler objects
    rresamp_crcf q0 = rresamp_crcf_create_kaiser(interp,decim,m,bw,As);
    rresamp_crcf q1 = rresamp_crcf_create_kaiser(interp,decim,m,bw,As);

    // full input, output buffers
    LIQUID_VLA(liquid_float_complex, buf_in, 2*decim*n); // input buffer
    LIQUID_VLA(liquid_float_complex, buf_out_0, 2*interp*n); // output, normal resampling operation
    LIQUID_VLA(liquid_float_complex, buf_out_1, 2*interp*n); // output, partitioned into 2 blocks

    // generate input signal (pulse)
    unsigned int i;
    for (i=0; i<2*decim*n; i++)
        buf_in[i] = liquid_hamming(i,2*decim*n) * cexpf(_Complex_I*2*M_PI*0.037f*i);

    // run resampler normally in one large block (2*decim*n inputs, 2*interp*n outputs)
    rresamp_crcf_execute_block(q0, buf_in, 2*n, buf_out_0);

    // reset and run with separate resamplers (e.g. in two threads)
    rresamp_crcf_reset(q0);
    // first block runs as normal
    rresamp_crcf_execute_block(q0, buf_in, n, buf_out_1);
    // initialize second block with decim*m samples to account for delay
    for (i=0; i<m; i++)
        rresamp_crcf_write(q1, buf_in + decim*n - (m-i)*decim);
    // run remainder of second block as normal
    rresamp_crcf_execute_block(q1, buf_in + decim*n, n, buf_out_1 + interp*n);

    // clean up allocated objects
    rresamp_crcf_destroy(q0);
    rresamp_crcf_destroy(q1);

    // compute RMS error between output buffers
    float rmse = 0.0f;
    for (i=0; i<2*interp*n; i++) {
        liquid_float_complex err = buf_out_0[i] - buf_out_1[i];
        rmse += crealf( err * conjf(err) );
    }
    rmse = sqrtf( rmse / (float)(2*interp*n) );
    printf("rmse=%.3g\n", rmse);

    // export results to file for plotting
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"interp = %u; decim = %u; m = %u; n= %u;\n", interp, decim, m, n);
    fprintf(fid,"x = zeros(1,2*decim*n);\n");
    fprintf(fid,"y = zeros(1,2*interp*n);\n");
    fprintf(fid,"z = zeros(1,2*interp*n);\n");
    fprintf(fid,"r = interp/decim;\n");
    for (i=0; i<2*decim*n; i++)
        fprintf(fid,"x(%3u) = %12.8f + 1i*%12.8f;\n", i+1, crealf(buf_in[i]), cimagf(buf_in[i]));
    for (i=0; i<2*interp*n; i++)
        fprintf(fid,"y(%3u) = %12.8f + 1i*%12.8f;\n", i+1, crealf(buf_out_0[i]), cimagf(buf_out_0[i]));
    for (i=0; i<2*interp*n; i++)
        fprintf(fid,"z(%3u) = %12.8f + 1i*%12.8f;\n", i+1, crealf(buf_out_1[i]), cimagf(buf_out_1[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"tx=0:(2*decim*n-1);\n");
    fprintf(fid,"ty=[(0:(2*interp*n-1))]/r-m;\n");
    fprintf(fid,"figure('Color','white','position',[500 500 800 600]);\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),        '-','LineWidth',2,'Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"       ty,real(y)*sqrt(r),'o','LineWidth',2,'Color',[0.5 0.5 0.5],'MarkerSize',3,...\n");
    fprintf(fid,"       ty,real(z)*sqrt(r),'o','LineWidth',2,'Color',[0.0 0.2 0.5],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled (normal)','resampled (partitions)','location','northeast');");
    fprintf(fid,"  xlabel('Input Sample Index');\n");
    fprintf(fid,"  ylabel('Real Signal');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  title('Comparison of Normal and Partitioned rresamp, RMSE=%.3g');\n", rmse);
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),        '-','LineWidth',2,'Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"       ty,imag(y)*sqrt(r),'o','LineWidth',2,'Color',[0.5 0.5 0.5],'MarkerSize',3,...\n");
    fprintf(fid,"       ty,imag(z)*sqrt(r),'o','LineWidth',2,'Color',[0.0 0.5 0.2],'MarkerSize',1);\n");
    fprintf(fid,"  legend('original','resampled (normal)','resampled (partitions)','location','northeast');");
    fprintf(fid,"  xlabel('Input Sample Index');\n");
    fprintf(fid,"  ylabel('Real Signal');\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s\n",filename);
    return 0;
}
