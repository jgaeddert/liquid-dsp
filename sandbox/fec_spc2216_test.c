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

#include "liquid.internal.h"

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

// transpose raw message block
//void spc2216_transpose_row(unsigned char * _m...);

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
        printf("%2u", (_x >> (_n-i-1)) & 1);
}

int main(int argc, char*argv[])
{
    unsigned int i;
    
    // error vector
    unsigned char e[61];
    for (i=0; i<61; i++)
        e[i] = (i==0) ? 0x80 : 0x00;

    // original message [16 x 16 bits], 32 bytes
    unsigned char m[32];

    // derived values
    unsigned char v[61];    // encoded/transmitted message
    unsigned char r[61];    // received vector
    unsigned char m_hat[32];// estimated original message

    // generate random transmitted message
#if 0
    for (i=0; i<32; i++)
        m[i] = rand() & 0xff;
#else
    for (i=0; i<32; i++)
        m[i] = (i % 2) == 0 ? 0x80 >> (i/2) : 0;
#endif
    printf("m (original message):\n");
    spc2216_print_decoded(m);

    // encode
    spc2216_encode(m, v);
    printf("v (encoded message):\n");
    spc2216_print_encoded(v);

    //printf("e (error message):\n");
    //spc2216_print_encoded(e);

    // add errors
    for (i=0; i<61; i++)
        r[i] = v[i] ^ e[i];

    // decode
    spc2216_decode(v, m_hat);

    // compute errors between m, m_hat
    unsigned int num_errors_decoded = count_bit_errors_array(m, m_hat, 32);
    printf("decoding errors (original) : %2u / 256\n", num_errors_decoded);

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
        parity_row[i] = i;

    // compute column parities
    for (i=0; i<22; i++)
        parity_col[i] = i;

    // pack encoded message
    spc2216_pack(_msg_org, parity_row, parity_col, _msg_enc);

}

void spc2216_decode(unsigned char * _msg_rec,
                    unsigned char * _msg_dec)
{
    unsigned char parity_row[16];
    unsigned char parity_col[22];
    unsigned char m_hat[32];

    // unpack encoded message
    spc2216_unpack(_msg_rec, parity_row, parity_col, m_hat);

    // iterate...
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
    for (j=0; j<8; j++) {
        printf("    ");
        for (i=0; i<22; i++) {
            printf("%2u", ((_pc[i] >> j) & 0x01) ? 1 : 0);

            if (i==15) printf(" ");
        }
        printf("\n");
    }
}

