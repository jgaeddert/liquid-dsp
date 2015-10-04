/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// Golay(24,12) product code test
//
// rate 1/4 product code:
// input    : 12 x 12 bits = 144 bits = 18 bytes
// output   : 24 x 24 bits = 576 bits = 72 bytes
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"

typedef struct g2412p_s * g2412p;
g2412p g2412p_create ();
void   g2412p_destroy(g2412p _q);
void   g2412p_print  (g2412p _q);
void   g2412p_encode (g2412p _q, unsigned char * _msg_org, unsigned char * _msg_enc);
void   g2412p_decode (g2412p _q, unsigned char * _msg_rec, unsigned char * _msg_dec);

void print_bitstring(unsigned int _x,
                     unsigned int _n)
{
    unsigned int i;
    printf("    ");
    for (i=0; i<_n; i++)
        printf("%1u ", (_x >> (_n-i-1)) & 1);
    printf("\n");
}

int main(int argc, char*argv[])
{
    unsigned int i;

    unsigned char msg_org[144];
    unsigned char msg_enc[576];
    unsigned char msg_rec[576];
    unsigned char msg_dec[144];

    // create object to handle encoding/decoding
    g2412p q = g2412p_create();

    // initialize input array
    for (i=0; i<144; i++) {
        //msg_org[i] = i % 2;
        msg_org[i] = 0;
    }

    // encode message
    g2412p_encode(q, msg_org, msg_enc);

    // corrupt message
    for (i=0; i<576; i++)
        msg_rec[i] = msg_enc[i] ? LIQUID_SOFTBIT_1 : LIQUID_SOFTBIT_0;
    msg_rec[  0] = 255 - msg_rec[  0];

    // decode message
    g2412p_decode(q, msg_rec, msg_dec);

    // count errors
    unsigned int num_errors = 0;
    for (i=0; i<144; i++)
        num_errors += (msg_dec[i] & 1) == (msg_org[i] & 1) ? 0 : 1;
    printf("bit errors: %u / %u\n", num_errors, 144);
    
    // clean up allocated objects
    g2412p_destroy(q);

    printf("done\n");
    return 0;
}

struct g2412p_s {
    unsigned char msg_buf[576];
    unsigned char b0[24];
    unsigned char b1[24];
    unsigned int  r;
    unsigned int  d;
};

g2412p g2412p_create()
{
    g2412p q = (g2412p) malloc( sizeof(struct g2412p_s) );

    return q;
}

void g2412p_destroy(g2412p _q)
{
    // free main object memory
    free(_q);
}

void g2412p_print(g2412p _q)
{
    unsigned int i;
#if 1
    unsigned int j;
    for (i=0; i<24; i++) {
        if (i==12)
            printf("------\n");
        printf("%2u:", i);
        for (j=0; j<24; j++)
            printf(" %s%3u", j==12 ? ":" : "", _q->msg_buf[24*i+j]);
        printf("\n");
    }
#else
    for (i=0; i<576; i++)
        printf(" %3u%s", _q->msg_buf[i], ((i+1)%24)==0 ? "\n" : "");
#endif
}

void g2412p_encode(g2412p          _q,
                   unsigned char * _msg_org,
                   unsigned char * _msg_enc)
{
    unsigned int i;
    for (i=0; i<576; i++)
        _msg_enc[i] = 0;
}

void g2412p_decode(g2412p          _q,
                   unsigned char * _msg_rec,
                   unsigned char * _msg_dec)
{
    // copy received message to internal buffer
    memmove(_q->msg_buf, _msg_rec, 576*sizeof(unsigned char));

    // print
    g2412p_print(_q);
    
    // copy output
    unsigned int i;
    for (i=0; i<12; i++)
        memmove(&_msg_dec[12*i], &_msg_rec[24*i], 12*sizeof(unsigned char));
}

