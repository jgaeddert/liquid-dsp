/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// SEC-DED(22,16) product code test
//  ________________ 
// [            |   ]
// [            |   ]
// [   16 x 16  | 6 ]
// [            |   ]
// [____________|___]
// [        6       ]
// [________________]
//
// input:   16 x 16 bits = 256 bits = 32 bytes
// output:  22 x 22 bits = 484 bits = 60 bytes + 4 bits (61 bytes)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "liquid.internal.h"

#define DEBUG_SPC2216 0

#define OUTPUT_FILENAME "spc2216_ber_test.m"

// encode message
//  _msg_org    :   original message [size: 32 bytes]
//  _msg_enc    :   encoded message  [size: 61 bytes]
void spc2216_encode(unsigned char * _msg_org,
                    unsigned char * _msg_enc);

// decode message
//  _msg_rec    :   received message [size: 61 bytes]
//  _msg_dec    :   decoded message  [size: 32 bytes]
void spc2216_decode(unsigned char * _msg_rec,
                    unsigned char * _msg_dec);

// pack message
//  _m  :   input message   [size: 32 bytes]
//  _pr :   row parities    [size: 16 bytes, @ 6 bits]
//  _pc :   col parities    [size: 22 bytes, @ 6 bits]
//  _v  :   output message  [size: 61 bytes]
void spc2216_pack(unsigned char * _m,
                  unsigned char * _pr,
                  unsigned char * _pc,
                  unsigned char * _v);

// unpack message
//  _v  :   input message   [size: 61 bytes]
//  _pr :   row parities    [size: 16 bytes, @ 6 bits]
//  _pc :   col parities    [size: 22 bytes, @ 6 bits]
//  _m  :   output message  [size: 32 bytes]
void spc2216_unpack(unsigned char * _v,
                    unsigned char * _pr,
                    unsigned char * _pc,
                    unsigned char * _m);

// transpose square message block (generic)
//  _m  :   input message block [size: _n x _n bits, _n*_n/8 bytes]
//  _n  :   matrix dimension (must be divisble by 8)
void spc2216_transpose_block(unsigned char * _m,
                             unsigned int    _n,
                             unsigned char * _mT);

// transpose message block and row parities
//  _m  :   message block       [size: 16 x 16 bits, 32 bytes]
//  _pr :   row parities        [size: 16 x  6 bits, 16 bytes]
//  _w  :   transposed array    [size: 22 x 16 bits, 44 bytes]
void spc2216_transpose_row(unsigned char * _m,
                           unsigned char * _pr,
                           unsigned char * _w);

// transpose message block and row parities (reverse)
//  _w  :   transposed array    [size: 22 x 16 bits, 44 bytes]
//  _m  :   message block       [size: 16 x 16 bits, 32 bytes]
//  _pr :   row parities        [size: 16 x  6 bits, 16 bytes]
void spc2216_transpose_col(unsigned char * _w,
                           unsigned char * _m,
                           unsigned char * _pr);

// print decoded block
void spc2216_print_decoded(unsigned char * _m);

// print encoded block
void spc2216_print_encoded(unsigned char * _v);

// print unpacked block
void spc2216_print_unpacked(unsigned char * _m,
                            unsigned char * _pr,
                            unsigned char * _pc);

void print_bitstring(unsigned char _x,
                     unsigned char _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%2s", ((_x >> (_n-i-1)) & 1) ? "1" : ".");
}

int main(int argc, char*argv[])
{
    srand(time(NULL));
    unsigned int i;
    
    // error vector
    unsigned char e[61];
    for (i=0; i<61; i++)
        e[i] = (rand() % 4) == 0 ? 1 << rand() % 7 : 0;

    // original message [16 x 16 bits], 32 bytes
    unsigned char m[32];

    // derived values
    unsigned char v[61];    // encoded/transmitted message
    unsigned char r[61];    // received vector
    unsigned char m_hat[32];// estimated original message

    // generate random transmitted message
    for (i=0; i<32; i++)
        m[i] = rand() & 0xff;
    printf("m (original message):\n");
    spc2216_print_decoded(m);

    // encode
    spc2216_encode(m, v);
    printf("v (encoded message):\n");
    spc2216_print_encoded(v);

    printf("e (error message):\n");
    spc2216_print_encoded(e);

    // add errors
    for (i=0; i<61; i++)
        r[i] = v[i] ^ e[i];
    printf("r (received message):\n");
    spc2216_print_encoded(r);

    // decode
    spc2216_decode(r, m_hat);

    // compute errors between m, m_hat
    unsigned int num_errors_decoded = count_bit_errors_array(m, m_hat, 32);
    printf("decoding errors (original) : %2u / 256\n", num_errors_decoded);



    // 
    // run SNR trials
    //
    float SNRdB_min = -1.0f;                // signal-to-noise ratio (minimum)
    float SNRdB_max =  7.0f;                // signal-to-noise ratio (maximum)
    unsigned int num_snr = 33;              // number of SNR steps
    unsigned int num_trials=10000;          // number of trials

    // arrays
    float complex sym_rec[8*61];            // received BPSK symbols
    unsigned int bit_errors[num_snr];

    unsigned int n_enc = 61;
    unsigned char msg_org[32];
    unsigned char msg_enc[61];
    unsigned char msg_cor[61];
    unsigned char msg_dec[32];

    // set up parameters
    float SNRdB_step = (SNRdB_max - SNRdB_min) / (num_snr-1);

    printf("  %8s %8s [%8s] %8s %12s %8s %12s %12s\n",
            "SNR [dB]", "Eb/N0", "trials", "soft", "(BER)", "hard", "(BER)", "uncoded");
    unsigned int s;
    for (s=0; s<num_snr; s++) {
        // compute SNR for this level
        float SNRdB = SNRdB_min + s*SNRdB_step; // SNR in dB for this round
        float nstd = powf(10.0f, -SNRdB/20.0f); // noise standard deviation

        // reset results
        bit_errors[s] = 0;

        unsigned int t;
        for (t=0; t<num_trials; t++) {
            // generate data
            for (i=0; i<32; i++)
                msg_org[i] = rand() & 0xff;

            // encode message
            spc2216_encode(msg_org, msg_enc);

            // modulate with BPSK
            for (i=0; i<n_enc; i++) {
                sym_rec[8*i+0] = (msg_enc[i] & 0x80) ? 1.0f : -1.0f;
                sym_rec[8*i+1] = (msg_enc[i] & 0x40) ? 1.0f : -1.0f;
                sym_rec[8*i+2] = (msg_enc[i] & 0x20) ? 1.0f : -1.0f;
                sym_rec[8*i+3] = (msg_enc[i] & 0x10) ? 1.0f : -1.0f;
                sym_rec[8*i+4] = (msg_enc[i] & 0x08) ? 1.0f : -1.0f;
                sym_rec[8*i+5] = (msg_enc[i] & 0x04) ? 1.0f : -1.0f;
                sym_rec[8*i+6] = (msg_enc[i] & 0x02) ? 1.0f : -1.0f;
                sym_rec[8*i+7] = (msg_enc[i] & 0x01) ? 1.0f : -1.0f;
            }

            // add noise
            for (i=0; i<8*n_enc; i++)
                sym_rec[i] += nstd*(randnf() + _Complex_I*randf())*M_SQRT1_2;

            // convert to hard bits (hard decoding)
            for (i=0; i<n_enc; i++) {
                msg_cor[i] = 0x00;

                msg_cor[i] |= crealf(sym_rec[8*i+0]) > 0.0f ? 0x80 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+1]) > 0.0f ? 0x40 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+2]) > 0.0f ? 0x20 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+3]) > 0.0f ? 0x10 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+4]) > 0.0f ? 0x08 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+5]) > 0.0f ? 0x04 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+6]) > 0.0f ? 0x02 : 0x00;
                msg_cor[i] |= crealf(sym_rec[8*i+7]) > 0.0f ? 0x01 : 0x00;
            }

            // decode
            spc2216_decode(msg_cor, msg_dec);
            
            // tabulate results
            bit_errors[s] += count_bit_errors_array(msg_org, msg_dec, 32);
        }

        // print results for this SNR step
        float rate = 32. / 61.; // true rate
        printf("  %8.3f %8.3f [%8u] %8u %12.4e %12.4e\n",
                SNRdB,
                SNRdB - 10*log10f(rate),
                8*32*num_trials,
                bit_errors[s], (float)(bit_errors[s]) / (float)(num_trials*32*8),
                0.5f*erfcf(1.0f/nstd));
    }
    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n = %u;    %% frame size [bytes]\n", 32);
    fprintf(fid,"k = %u;    %% encoded frame size [bytes]\n", 61);
    fprintf(fid,"r = n / k; %% true rate\n");
    fprintf(fid,"num_snr = %u;\n", num_snr);
    fprintf(fid,"num_trials = %u;\n", num_trials);
    fprintf(fid,"num_bit_trials = num_trials*n*8;\n");
    for (i=0; i<num_snr; i++) {
        fprintf(fid,"SNRdB(%4u) = %12.8f;\n",i+1, SNRdB_min + i*SNRdB_step);
        fprintf(fid,"bit_errors(%6u) = %u;\n", i+1, bit_errors[i]);
    }
    fprintf(fid,"EbN0dB = SNRdB - 10*log10(r);\n");
    fprintf(fid,"EbN0dB_bpsk = -15:0.5:40;\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(EbN0dB_bpsk, 0.5*erfc(sqrt(10.^[EbN0dB_bpsk/10]))+1e-12,'-x',\n");
    fprintf(fid,"         EbN0dB,      bit_errors / num_bit_trials + 1e-12,  '-x');\n");
    fprintf(fid,"axis([%f (%f-10*log10(r)) 1e-6 1]);\n", SNRdB_min, SNRdB_max);
    fprintf(fid,"legend('uncoded','coded',1);\n");
    fprintf(fid,"xlabel('E_b/N_0 [dB]');\n");
    fprintf(fid,"ylabel('Bit Error Rate');\n");
    fprintf(fid,"title('BER vs. E_b/N_0 for SPC(22,16)');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);


    printf("done.\n");
    return 0;
}

void spc2216_encode(unsigned char * _msg_org,
                    unsigned char * _msg_enc)
{
    unsigned int i;

    unsigned char parity_row[16];
    unsigned char parity_col[22];

    // compute row parities
    for (i=0; i<16; i++)
        parity_row[i] = fec_secded2216_compute_parity(&_msg_org[2*i]);

    // compute column parities
    unsigned char w[44];
    spc2216_transpose_row(_msg_org, parity_row, w);
    for (i=0; i<22; i++)
        parity_col[i] = fec_secded2216_compute_parity(&w[2*i]);

    // pack encoded message
    spc2216_pack(_msg_org, parity_row, parity_col, _msg_enc);
}

void spc2216_decode(unsigned char * _msg_rec,
                    unsigned char * _msg_dec)
{
    unsigned char parity_row[16];
    unsigned char parity_col[22];
    unsigned char m_hat[32];
    unsigned char w[44];

    // unpack encoded message
    spc2216_unpack(_msg_rec, parity_row, parity_col, m_hat);

    // decode
    unsigned int i;
    unsigned char sym_enc[3];   // encoded 22-bit message
    unsigned char e_hat[3];     // estimated error vector
    
    // transpose
    spc2216_transpose_row(m_hat, parity_row, w);

    // compute syndromes on columns and decode
    for (i=0; i<22; i++) {
        sym_enc[0] = parity_col[i];
        sym_enc[1] = w[2*i+0];
        sym_enc[2] = w[2*i+1];

#if DEBUG_SPC2216
        int syndrome_flag = fec_secded2216_estimate_ehat(sym_enc, e_hat);

        if (syndrome_flag == 0) {
            printf("%3u : no errors detected\n", i);
        } else if (syndrome_flag == 1) {
            printf("%3u : one error detected and corrected!\n", i);
        } else {
            printf("%3u : multiple errors detected\n", i);
        }
#endif

        // apply error vector estimate to appropriate arrays
        parity_col[i] ^= e_hat[0];
        w[2*i+0]      ^= e_hat[1];
        w[2*i+1]      ^= e_hat[2];
    }
#if DEBUG_SPC2216
    printf("******** transposed: **********\n");
    for (i=0; i<22; i++) {
        printf("    ");
        print_bitstring(w[2*i+0], 8);
        print_bitstring(w[2*i+1], 8);
        printf("\n");
        if (i==15)
            printf("    ---------------------------------\n");
    }
#endif

    // transpose back
    spc2216_transpose_col(w, m_hat, parity_row);

    // compute syndromes on rows and decode
    unsigned int num_uncorrected_errors = 0;
    for (i=0; i<16; i++) {
        sym_enc[0] = parity_row[i];
        sym_enc[1] = m_hat[2*i+0];
        sym_enc[2] = m_hat[2*i+1];
        int syndrome_flag = fec_secded2216_estimate_ehat(sym_enc, e_hat);

#if DEBUG_SPC2216
        if (syndrome_flag == 0) {
            printf("%3u : no errors detected\n", i);
        } else if (syndrome_flag == 1) {
            printf("%3u : one error detected and corrected!\n", i);
        } else {
            printf("%3u : multiple errors detected\n", i);
        }
#endif

        if (syndrome_flag == 2)
            num_uncorrected_errors++;

        // apply error vector estimate to appropriate arrays
        parity_col[i] ^= e_hat[0];
        m_hat[2*i+0]  ^= e_hat[1];
        m_hat[2*i+1]  ^= e_hat[2];
    }

    //printf("number of uncorrected errors: %u\n", num_uncorrected_errors);
    
    // copy decoded message to output
    memmove(_msg_dec, m_hat, 32*sizeof(unsigned char));
}

// pack message
//  _m  :   input message   [size: 32 bytes]
//  _pr :   row parities    [size: 16 bytes, @ 6 bits]
//  _pc :   col parities    [size: 22 bytes, @ 6 bits]
//  _v  :   output message  [size: 61 bytes]
void spc2216_pack(unsigned char * _m,
                  unsigned char * _pr,
                  unsigned char * _pc,
                  unsigned char * _v)
{
    // copy input message to begining of encoded vector
    memmove(_v, _m, 32*sizeof(unsigned char));

    // pack row parities
    unsigned int i;
    unsigned int k=0;
    for (i=0; i<16; i++) {
        liquid_pack_array(&_v[32], 29, k, 6, _pr[i]);
        k += 6;
    }

    // pack column parities
    for (i=0; i<22; i++) {
        liquid_pack_array(&_v[32], 29, k, 6, _pc[i]);
        k += 6;
    }
}

// unpack message
//  _v  :   input message   [size: 61 bytes]
//  _pr :   row parities    [size: 16 bytes, @ 6 bits]
//  _pc :   col parities    [size: 22 bytes, @ 6 bits]
//  _m  :   output message  [size: 32 bytes]
void spc2216_unpack(unsigned char * _v,
                    unsigned char * _pr,
                    unsigned char * _pc,
                    unsigned char * _m)
{
    // copy input message to begining of encoded vector
    memmove(_m, _v, 32*sizeof(unsigned char));

    // unpack row parities
    unsigned int i;
    unsigned int k=0;
    for (i=0; i<16; i++) {
        //liquid_unpack_array(&_pr[k], &_v[32*8+k], 
        liquid_unpack_array(&_v[32], 29, k, 6, &_pr[i]);
        k += 6;
    }

    // unpack column parities
    for (i=0; i<22; i++) {
        //liquid_unpack_array(&_pr[k], &_v[32*8+k], 
        liquid_unpack_array(&_v[32], 29, k, 6, &_pc[i]);
        k += 6;
    }
}

// transpose message block and row parities
//  _m  :   message block       [size: 16 x 16 bits, 32 bytes]
//  _pr :   row parities        [size: 16 x  6 bits, 16 bytes]
//  _w  :   transposed array    [size: 22 x 16 bits, 44 bytes]
void spc2216_transpose_row(unsigned char * _m,
                           unsigned char * _pr,
                           unsigned char * _w)
{
    // transpose main input message, store in first 32
    // bytes of _w array
    spc2216_transpose_block(_m, 16, _w);

    // transpose row parities
    unsigned int i;
    unsigned int j;
    for (i=0; i<6; i++) {
        unsigned char mask = 1 << (6-i-1);
        unsigned char w0 = 0;
        unsigned char w1 = 0;
        for (j=0; j<8; j++) {
            w0 |= (_pr[j  ] & mask) ? 1 << (8-j-1) : 0;
            w1 |= (_pr[j+8] & mask) ? 1 << (8-j-1) : 0;
        }

        _w[32 + 2*i + 0] = w0;
        _w[32 + 2*i + 1] = w1;
    }

#if 0
    printf("transposed:\n");
    for (i=0; i<22; i++) {
        printf("    ");
        print_bitstring(_w[2*i+0], 8);
        print_bitstring(_w[2*i+1], 8);
        printf("\n");
        if (i==15)
            printf("    ---------------------------------\n");
    }
#endif
}

// transpose message block and row parities (reverse)
//  _w  :   transposed array    [size: 22 x 16 bits, 44 bytes]
//  _m  :   message block       [size: 16 x 16 bits, 32 bytes]
//  _pr :   row parities        [size: 16 x  6 bits, 16 bytes]
void spc2216_transpose_col(unsigned char * _w,
                           unsigned char * _m,
                           unsigned char * _pr)
{
    // transpose main message block from first 32
    // bytes of _w array
    spc2216_transpose_block(_w, 16, _m);

    // transpose row parities
    unsigned int i; // input row
    unsigned int j; // input column (rem 8)

    // initialize to zero
    for (i=0; i<16; i++)
        _pr[i] = 0;

    for (i=0; i<6; i++) {
        unsigned char mask = 1 << (6-i-1);
        for (j=0; j<8; j++) {
            _pr[j  ] |= (_w[32 + 2*i + 0] & (1 << (8-j-1)) ) ? mask : 0;
            _pr[j+8] |= (_w[32 + 2*i + 1] & (1 << (8-j-1)) ) ? mask : 0;
        }
    }

#if 0
    printf("transposed (reverse):\n");
    for (i=0; i<16; i++) {
        printf("    ");
        print_bitstring(_m[2*i+0], 8);
        print_bitstring(_m[2*i+1], 8);
        printf(" ");
        print_bitstring(_pr[i], 6);
        printf("\n");
    }
#endif
}


// transpose square message block (generic)
//  _m  :   input message block [size: _n x _n bits, _n*_n/8 bytes]
//  _n  :   matrix dimension (must be divisble by 8)
void spc2216_transpose_block(unsigned char * _m,
                             unsigned int    _n,
                             unsigned char * _mT)
{
    unsigned int i;
    unsigned int j;
    unsigned int k;

    // ensure that _n is divisible by 8
    if (_n % 8) {
        fprintf(stderr,"error: spc2216_transpose_block(), number of rows must be divisible by 8\n");
        exit(1);
    }
    unsigned int c = _n / 8;    // number of byte columns
    
    unsigned char w0;
    unsigned char w1;
    for (k=0; k<c; k++) {
        for (i=0; i<8; i++) {
            unsigned char mask = 1 << (8-i-1);
            w0 = 0;
            w1 = 0;
            for (j=0; j<8; j++) {
                w0 |= (_m[2*j + c*8*k + 0] & mask) ? 1 << (8-j-1) : 0;
                w1 |= (_m[2*j + c*8*k + 1] & mask) ? 1 << (8-j-1) : 0;
            }

            _mT[2*i       + k] = w0;
            _mT[2*i + c*8 + k] = w1;
        }
    }
}

// print decoded block
void spc2216_print_decoded(unsigned char * _m)
{
    unsigned int i;
    
    for (i=0; i<16; i++) {
        printf("    ");
        print_bitstring(_m[2*i+0],8);
        print_bitstring(_m[2*i+1],8);
        printf("\n");
    }
}

// print encoded block
void spc2216_print_encoded(unsigned char * _v)
{
    unsigned char parity_row[16];
    unsigned char parity_col[22];
    unsigned char m[32];

    // unpack encoded message
    spc2216_unpack(_v, parity_row, parity_col, m);

    // print unpacked version
    spc2216_print_unpacked(m, parity_row, parity_col);
}

// print unpacked block
void spc2216_print_unpacked(unsigned char * _m,
                            unsigned char * _pr,
                            unsigned char * _pc)
{
    unsigned int i;
    
    for (i=0; i<16; i++) {
        printf("    ");
        print_bitstring(_m[2*i+0],8);
        print_bitstring(_m[2*i+1],8);
        printf(" ");
        print_bitstring(_pr[i], 6);
        printf("\n");
    }
    printf("\n");

    // print column parities
    unsigned int j;
    for (j=0; j<6; j++) {
        printf("    ");
        for (i=0; i<22; i++) {
            printf("%2s", ((_pc[i] >> j) & 0x01) ? "1" : ".");

            if (i==15) printf(" ");
        }
        printf("\n");
    }
}

