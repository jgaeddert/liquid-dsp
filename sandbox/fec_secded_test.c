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
// (72,64) code test (SEC-DED)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FEC_SECDED 1

// P matrix [8 x 64]
//  11111111 00001111 00001111 00001100 01101000 10001000 10001000 10000000 : 
//  11110000 11111111 00000000 11110011 01100100 01000100 01000100 01000000 : 
//  00110000 11110000 11111111 00001111 00000010 00100010 00100010 00100110 : 
//  11001111 00000000 11110000 11111111 00000001 00010001 00010001 00010110 : 
//  01101000 10001000 10001000 10000000 11111111 00001111 00000000 11110011 : 
//  01100100 01000100 01000100 01000000 11110000 11111111 00001111 00001100 : 
//  00000010 00100010 00100010 00100110 11001111 00000000 11111111 00001111 : 
//  00000001 00010001 00010001 00010110 00110000 11110000 11110000 11111111 : 
unsigned int P[32] = {
    0xFF0F0F0C, 0x68888880,
    0xF0FF00F3, 0x64444440,
    0x30F0FF0F, 0x02222226,
    0xCF00F0FF, 0x01111116,
    0x68888880, 0xFF0F00F3,
    0x64444440, 0xF0FF0F0C,
    0x02222226, 0xCF00FF0F,
    0x01111116, 0x30F0F0FF};

// parity check matrix: H = [I(8) P]
unsigned int H[48] = {
    0x80, 0xFF0F0F0C, 0x68888880,
    0x40, 0xF0FF00F3, 0x64444440,
    0x20, 0x30F0FF0F, 0x02222226,
    0x10, 0xCF00F0FF, 0x01111116,
    0x08, 0x68888880, 0xFF0F00F3,
    0x04, 0x64444440, 0xF0FF0F0C,
    0x02, 0x02222226, 0xCF00FF0F,
    0x01, 0x01111116, 0x30F0F0FF};

void print_bitstring_short(unsigned int _x,
                           unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}

void print_bitstring(unsigned int * _x,
                     unsigned int   _n)
{
    unsigned int i;
    // compute number of elements in _x
    div_t d = div(_n, 32);
    unsigned int N = d.quot + (d.rem ? 1 : 0);

    // print leader
    printf("    ");
    if (d.rem == 0) printf(" ");
    for (i=0; i<32-d.rem-1; i++)
        printf(" ");

    // print bitstring
    for (i=0; i<N; i++) {
        if (i==0 && d.rem)
            print_bitstring_short(_x[i], d.rem);
        else
            print_bitstring_short(_x[i], 32);

        printf(" ");

    }
    printf("\n");
}

int main(int argc, char*argv[])
{
    unsigned int i;
    
    // error vector [72 x 1]
    unsigned int err[3] = {0x00, 0x00000000, 0x00000001};

    // original message [64 x 1]
    unsigned int m[2] = {0x00000000, 0x00000001};
    m[0] = rand() & 0xffffffff;
    m[1] = rand() & 0xffffffff;

    // derived values
    unsigned int v[3];      // encoded/transmitted message
    unsigned int e[3];      // error vector
    unsigned int r[3];      // received vector
    unsigned int s;         // syndrome vector
    unsigned int e_hat[3] = {0,0,0};  // estimated error vector
    unsigned int v_hat[3];  // estimated transmitted message
    unsigned int m_hat[2];  // estimated original message

    // original message
    printf("m (original message):\n");
    print_bitstring(m,64);

    // compute encoded/transmitted message: v = m*G
    v[0] = 0;
    for (i=0; i<8; i++) {
        v[0] <<= 1;

        unsigned int p = liquid_count_ones(P[2*i+0] & m[0]) +
                         liquid_count_ones(P[2*i+1] & m[1]);
        printf("p = %u\n", p);
        v[0] |= p & 0x01;
    }
    v[1] = m[0];
    v[2] = m[1];
    printf("v (encoded/transmitted message):\n");
    print_bitstring(v,72);

    // use pre-determined error vector
    e[0] = err[0];
    e[1] = err[1];
    e[2] = err[2];
    printf("e (error vector):\n");
    print_bitstring(e,72);

    // compute received vector: r = v + e
    r[0] = v[0] ^ e[0];
    r[1] = v[1] ^ e[1];
    r[2] = v[2] ^ e[2];
    printf("r (received vector):\n");
    print_bitstring(r,72);

    // compute syndrome vector, s = r*H^T = ( H*r^T )^T
    s = 0;
    for (i=0; i<8; i++) {
        s <<= 1;
#if 0
        unsigned int p =
            ( (r[0] & (1<<(8-i-1))) ? 1 : 0 )+
            liquid_count_ones(P[2*i+0] & r[1]) +
            liquid_count_ones(P[2*i+1] & r[2]);
#else
        unsigned int p =
            liquid_count_ones(H[3*i+0] & r[0]) +
            liquid_count_ones(H[3*i+1] & r[1]) +
            liquid_count_ones(H[3*i+2] & r[2]);
#endif
        printf("p = %u\n", p);

        s |= p & 0x01;
    }
    printf("s (syndrome vector):\n");
    print_bitstring(&s,8);

    // compute weight of s
    unsigned int ws = liquid_count_ones(s);
    printf("w(s) = %u\n", ws);

    if (ws == 0) {
        printf("no errors detected\n");
    } else {
        // estimate error location
        unsigned int e_test[3]  = {0x00, 0x00000000, 0x00000001};
        int syndrome_match = 0;

        // TODO : these can be pre-computed
        unsigned int n;
        for (n=0; n<72; n++) {
            // compute syndrome
            unsigned int s_hat = 0;

            for (i=0; i<8; i++) {
                s_hat <<= 1;
                unsigned int p =
                    liquid_count_ones(H[3*i+0] & e_test[0]) +
                    liquid_count_ones(H[3*i+1] & e_test[1]) +
                    liquid_count_ones(H[3*i+2] & e_test[2]);

                s_hat |= p & 0x01;
            }

            // print results
            //printf("e_test:"); print_bitstring(e_test, 72);
            printf("%3u : s = ", n);
            print_bitstring_short(s_hat,8);
            if (s == s_hat) printf(" *");
            printf("\n");

            if (s == s_hat) {
                memmove(e_hat, e_test, sizeof(e_test));
                syndrome_match = 1;
            }

            // shift e_test
            e_test[0] = (e_test[0] << 1) | ((e_test[1] & 0x80000000) ? 1 : 0);
            e_test[1] = (e_test[1] << 1) | ((e_test[2] & 0x80000000) ? 1 : 0);
            e_test[2] <<= 1;

        }

        if (syndrome_match) {
            printf("syndrome match!\n");
        } else {
            printf("no syndrome match; expected multiple errors\n");
        }
    }

    // step 8: compute estimated transmitted message: v_hat = r + e_hat
    printf("e-hat (estimated error vector):\n");
    print_bitstring(e_hat,72);

    printf("v-hat (estimated transmitted vector):\n");
    v_hat[0] = r[0] ^ e_hat[0];
    v_hat[1] = r[1] ^ e_hat[1];
    v_hat[2] = r[2] ^ e_hat[2];
    print_bitstring(v_hat,72);
    print_bitstring(v,    72);

    // compute errors between v, v_hat
    unsigned int num_errors_encoded = count_bit_errors(v[0], v_hat[0]) +
                                      count_bit_errors(v[1], v_hat[1]) +
                                      count_bit_errors(v[2], v_hat[2]);
    printf("decoding errors (encoded)  : %2u / 72\n", num_errors_encoded);

    // compute estimated original message: (last 64 bits of encoded message)
    m_hat[0] = v_hat[1];
    m_hat[1] = v_hat[2];
    print_bitstring(m_hat,64);
    print_bitstring(m,    64);

    // compute errors between m, m_hat
    unsigned int num_errors_decoded = count_bit_errors(m[0], m_hat[0]) +
                                      count_bit_errors(m[1], m_hat[1]);
    printf("decoding errors (original) : %2u / 64\n", num_errors_decoded);

    return 0;
}

