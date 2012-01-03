//
// src/modem_phase_error.c
//
// generates the following output files:
//  * gnuplot: figures.gen/modem_phase_error_[modscheme][M].gnu
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

// print usage/help message
void usage()
{
    printf("Usage: modem_phase_error [OPTION]\n");
    printf("\n");
    printf("  u/h   : print usage\n");
    printf("  v/q   : verbose/quiet, default: verbose\n");
    printf("  n     : number of phase steps, default: 21\n");
    printf("  t     : number of trials, default: 1000\n");
    printf("  s     : SNR [dB], default: 12\n");
    printf("  P     : absolute phase offset [radians], default: pi/4\n");
    printf("  m     : modulation scheme, default: psk\n");
    liquid_print_modulation_schemes();
    printf("  o     : output filename, default: 'figures.gen/modem_phase_error[modscheme].dat'\n");
}

int main(int argc, char*argv[]) {
    srand( time(NULL) );

    // options
    int verbose = 1;                // verbose output flag
    float phi_max_abs = M_PI/4.0f;  // absolute maximum phase offset
    unsigned int num_phi = 21;      // number of phase steps
    unsigned int num_trials = 1000; // number of trials
    float SNRdB = 12.0f;            // signal-to-noise ratio [dB]
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    unsigned int bps = 2;
    char filename[256] = "";    // output filename

    int dopt;
    while ((dopt = getopt(argc,argv,"uhvqn:t:s:P:m:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();    return 0;
        case 'v':   verbose = 1;                    break;
        case 'q':   verbose = 0;                    break;
        case 'n':   num_phi = atoi(optarg);         break;
        case 't':   num_trials = atoi(optarg);      break;
        case 's':   SNRdB = atof(optarg);           break;
        case 'P':   phi_max_abs = atof(optarg);     break;
        case 'm':
            liquid_getopt_str2modbps(optarg, &ms, &bps);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme \"%s\"\n", argv[0], optarg);
                return 1;
            }
            break;
        case 'o':   strncpy(filename,optarg,255);   break;
        default:
            exit(1);
        }
    }

    // validate input
    if (phi_max_abs <= 0.0f) {
        fprintf(stderr,"error: %s, maximum absolute phase offset must be greater than 0\n", argv[0]);
        exit(1);
    } else if (num_phi < 3) {
        fprintf(stderr,"error: %s, number of phase steps must be at least 3\n", argv[0]);
        exit(1);
        fprintf(stderr,"error: %s, number of trials must be greater than 0\n", argv[0]);
        exit(1);
    } else if (bps == 0 || bps > 8) {
        fprintf(stderr,"error: %s, bits per symbol must be in [1,8]\n", argv[0]);
        exit(1);
    } else if (ms == LIQUID_MODEM_DPSK) {
        fprintf(stderr,"error: %s, differential PSK not allowed\n", argv[0]);
        exit(1);
    }

    // generate filenames
    if ( strcmp(filename,"")==0 )
        sprintf(filename,"figures.gen/modem_phase_error_%s%u.dat", modulation_types[ms].name, 1<<bps);

    // derived values
    float phi_min = 0.0f; //phi_max_abs;
    float phi_max = phi_max_abs;
    float phi_step = (phi_max - phi_min) / (float)(num_phi-1);
    float nstd = powf(10.0f, -SNRdB/20.0f);

    // arrays
    float phi_hat_mean[num_phi];        // phase error estimate
    float phi_hat_mean_smooth[num_phi]; // phase error estimate (smoothed)

    // create modulator/demodulator
    modem mod = modem_create(ms,bps);
    modem demod = modem_create(ms,bps);
    bps = modem_get_bps(mod);
    unsigned int M = 1 << bps;

    //
    unsigned int i;
    unsigned int sym_in;
    unsigned int sym_out;
    unsigned int n=0;       // trials counter
    float complex x;
    float phi_hat;
    float phi=0.0f;
    for (i=0; i<num_phi; i++) {
        phi = phi_min + i*phi_step;

        phi_hat_mean[i] = 0.0f;

        // reset number of trials
        n = 0;

        do {
            for (sym_in=0; sym_in<M; sym_in++) {
                // modulate
                modem_modulate(mod, sym_in, &x);

                // channel (phase offset)
                x *= cexpf(_Complex_I*phi);
                x += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;

                // demodulate
                modem_demodulate(demod, x, &sym_out);

                // get error
                phi_hat = modem_get_demodulator_phase_error(demod);

                // accumulate average
                phi_hat_mean[i] += phi_hat;
            }

            n += M;
        } while (n < num_trials);

        // scale by bps^2
        //phi_hat_mean[i] *= bps*bps;

        // normalize mean by number of trials
        phi_hat_mean[i] /= (float) (n);

        // print results
        if (verbose)
            printf("%6u / %6u : phi=%12.8f, phi-hat=%12.8f\n",
                    i+1, num_phi, phi, phi_hat_mean[i]);
    }

    // compute smoothed curve
    float phi_hat_tmp[num_phi];
    memmove(phi_hat_mean_smooth, phi_hat_mean, num_phi*sizeof(float));
    unsigned int j;
    for (j=0; j<5; j++) {
        memmove(phi_hat_tmp, phi_hat_mean_smooth, num_phi*sizeof(float));

        for (i=0; i<num_phi; i++) {
            if (i==0 || i == num_phi-1) {
                phi_hat_mean_smooth[i] = phi_hat_tmp[i];
            } else {
                phi_hat_mean_smooth[i] = 0.20f*phi_hat_tmp[i-1] +
                                         0.60f*phi_hat_tmp[i  ] +
                                         0.20f*phi_hat_tmp[i+1];
            }
        }
    }

    // destroy objects
    modem_destroy(mod);
    modem_destroy(demod);

    //
    // export output file
    //

    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, cannot open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid, "# %s : auto-generated file\n", filename);
    fprintf(fid, "# \n");

    // low SNR
    fprintf(fid, "# %12s %12s %12s\n", "phi", "phi-hat", "phi-hat-smooth");
    for (i=0; i<num_phi; i++) {
        phi = phi_min + i*phi_step;
        
        fprintf(fid,"  %12.8f %12.8f %12.8f\n", phi, phi_hat_mean[i], phi_hat_mean_smooth[i]);
    }

    fclose(fid);

    if (verbose)
        printf("results written to '%s'\n", filename);

    return 0;
}
