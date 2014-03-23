/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// (22,16) code test (SEC-DED)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FEC_SECDED 1

// P matrix [6 x 16 bits], [6 x 2 bytes]
//  1001 1001 0011 1100 :
//  0011 1110 1000 1010 :
//  1110 1110 0110 0000 :
//  1110 0001 1101 0001 :
//  0001 0011 1100 0111 :
//  0100 0100 0011 1111 :
unsigned char P[12] = {
    0x99, 0x3c,
    0x3e, 0x8a,
    0xee, 0x60,
    0xe1, 0xd1,
    0x13, 0xc7,
    0x44, 0x3f};

void print_bitstring_short(unsigned char _x,
                           unsigned char _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}

void print_bitstring(unsigned char * _x,
                     unsigned char   _n)
{
    unsigned int i;
    // compute number of elements in _x
    div_t d = div(_n, 8);
    unsigned int N = d.quot + (d.rem ? 1 : 0);

    // print leader
    printf("    ");
    if (d.rem == 0) printf(" ");
    for (i=0; i<8-d.rem-1; i++)
        printf(" ");

    // print bitstring
    for (i=0; i<N; i++) {
        if (i==0 && d.rem)
            print_bitstring_short(_x[i], d.rem);
        else
            print_bitstring_short(_x[i], 8);

        printf(" ");

    }
    printf("\n");
}

int main(int argc, char*argv[])
{
    unsigned int i;
    
    // error vector [22 x 1]
    unsigned char err[3] = {0x00, 0x0000, 0x0001};

    // original message [16 x 1]
    unsigned char m[2] = {0x0000, 0x0001};
    m[0] = rand() & 0xffff;
    m[1] = rand() & 0xffff;

    // derived values
    unsigned char v[3];     // encoded/transmitted message
    unsigned char e[3];     // error vector
    unsigned char r[3];     // received vector
    unsigned char s;        // syndrome vector
    unsigned char e_hat[3] = {0,0,0};  // estimated error vector
    unsigned char v_hat[3]; // estimated transmitted message
    unsigned char m_hat[2]; // estimated original message

#if 0
    // print P matrix
    printf("P : \n");
    print_bitstring(&P[ 0],16);
    print_bitstring(&P[ 2],16);
    print_bitstring(&P[ 4],16);
    print_bitstring(&P[ 6],16);
    print_bitstring(&P[ 8],16);
    print_bitstring(&P[10],16);
#endif

    // original message
    printf("m (original message):   ");
    print_bitstring(m,16);

    // compute encoded/transmitted message: v = m*G
    v[0] = 0;
    for (i=0; i<6; i++) {
        v[0] <<= 1;

        unsigned int p = liquid_c_ones[P[2*i+0] & m[0]] +
                         liquid_c_ones[P[2*i+1] & m[1]];
        printf("p = %u\n", p);
        v[0] |= p & 0x01;
    }
    v[1] = m[0];
    v[2] = m[1];
    printf("v (encoded message):    ");
    print_bitstring(v,22);

    // use pre-determined error vector
    e[0] = err[0];
    e[1] = err[1];
    e[2] = err[2];
    printf("e (error vector):       ");
    print_bitstring(e,22);

    // compute received vector: r = v + e
    r[0] = v[0] ^ e[0];
    r[1] = v[1] ^ e[1];
    r[2] = v[2] ^ e[2];
    printf("r (received vector):    ");
    print_bitstring(r,22);

    // compute syndrome vector, s = r*H^T = ( H*r^T )^T
    s = 0;
    for (i=0; i<6; i++) {
        s <<= 1;

        unsigned int p =
            ( (r[0] & (1<<(6-i-1))) ? 1 : 0 )+
            liquid_count_ones(P[2*i+0] & r[1]) +
            liquid_count_ones(P[2*i+1] & r[2]);
        //printf("p = %u\n", p);

        s |= p & 0x01;
    }
    printf("s (syndrome vector):    ");
    print_bitstring(&s,6);

    // compute weight of s
    unsigned int ws = liquid_count_ones(s);
    printf("weight(s) = %u\n", ws);

    if (ws == 0) {
        printf("no errors detected\n");
    } else {
        // estimate error location
        unsigned char e_test[3]  = {0x00, 0x0000, 0x0001};
        int syndrome_match = 0;

        // TODO : these can be pre-computed
        unsigned int n;
        for (n=0; n<22; n++) {
            // compute syndrome
            unsigned int s_test = 0;

            for (i=0; i<6; i++) {
                s_test <<= 1;
                unsigned int p =
                    ( (e_test[0] & (1<<(6-i-1))) ? 1 : 0 )+
                    liquid_count_ones(P[2*i+0] & e_test[1]) +
                    liquid_count_ones(P[2*i+1] & e_test[2]);

                s_test |= p & 0x01;
            }

#if 1
            // print results
            //printf("e_test:"); print_bitstring(e_test, 72);
            printf("%3u : e = ", n);
            print_bitstring_short(e_test[0],6); printf(" ");
            print_bitstring_short(e_test[1],8); printf(" ");
            print_bitstring_short(e_test[2],8); printf(" ");
            printf(", s = ");
            print_bitstring_short(s_test,6);
            if (s == s_test) printf(" *");
            printf("\n");
#else
            // print output array (secded2216_syndrome_w1[])
            printf("0x%.2x\n", s_test);
#endif

            if (s == s_test) {
                memmove(e_hat, e_test, sizeof(e_test));
                syndrome_match = 1;
            }

            // shift e_test
            e_test[0] = (e_test[0] << 1) | ((e_test[1] & 0x80) ? 1 : 0);
            e_test[1] = (e_test[1] << 1) | ((e_test[2] & 0x80) ? 1 : 0);
            e_test[2] <<= 1;

        }

        if (syndrome_match) {
            printf("syndrome match!\n");
        } else {
            printf("no syndrome match; expected multiple errors\n");
        }
    }

    // compute estimated transmitted message: v_hat = r + e_hat
    printf("e-hat (estimated error):");
    print_bitstring(e_hat,22);

    printf("v-hat (estimated tx):   ");
    v_hat[0] = r[0] ^ e_hat[0];
    v_hat[1] = r[1] ^ e_hat[1];
    v_hat[2] = r[2] ^ e_hat[2];
    print_bitstring(v_hat,22);

    // compute estimated original message: (last 16 bits of encoded message)
    m_hat[0] = v_hat[1];
    m_hat[1] = v_hat[2];
    printf("m-hat (estimated orig.):");
    print_bitstring(m_hat,16);

    // compute errors between v, v_hat
    unsigned int num_errors_encoded = count_bit_errors(v[0], v_hat[0]) +
                                      count_bit_errors(v[1], v_hat[1]) +
                                      count_bit_errors(v[2], v_hat[2]);
    printf("decoding errors (encoded)  : %2u / 22\n", num_errors_encoded);

    // compute errors between m, m_hat
    unsigned int num_errors_decoded = count_bit_errors(m[0], m_hat[0]) +
                                      count_bit_errors(m[1], m_hat[1]);
    printf("decoding errors (original) : %2u / 16\n", num_errors_decoded);

    return 0;
}

