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

#include "liquid.internal.h"

typedef struct g2412p_s * g2412p;
g2412p g2412p_create  ();
void   g2412p_destroy (g2412p _q);
void   g2412p_print   (g2412p _q);
void   g2412p_encode  (g2412p _q, unsigned char * _msg_org, unsigned char * _msg_enc);
void   g2412p_decode  (g2412p _q, unsigned char * _msg_rec, unsigned char * _msg_dec);

void   g2412p_iterate (g2412p _q);
void   g2412p_step    (g2412p _q);

void   g2412p_load_row(g2412p _q, unsigned int _row);
void   g2412p_save_row(g2412p _q, unsigned int _row);
void   g2412p_load_col(g2412p _q, unsigned int _col);
void   g2412p_save_col(g2412p _q, unsigned int _col);

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
    unsigned char reg0[24];
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
    printf("msg_buf:\n");
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
    unsigned int i;

    // copy received message to internal buffer
    memmove(_q->msg_buf, _msg_rec, 576*sizeof(unsigned char));

    // print
    g2412p_print(_q);

    // iterate
    // while...
    for (i=0; i<4; i++)
        g2412p_iterate(_q);
    
    // print
    g2412p_print(_q);

    // copy resulting output
    for (i=0; i<12; i++)
        memmove(&_msg_rec[24*i], &_q->msg_buf[12*i], 12*sizeof(unsigned char));
}

// run single iteration
void g2412p_iterate(g2412p _q)
{
    unsigned int i;

    // decode columns
    for (i=0; i<24; i++) {
        g2412p_load_col(_q, i);
        g2412p_step    (_q   );
        g2412p_save_col(_q, i);
    }

#if 0
    // decode rows
    for (i=0; i<12; i++) {
        g2412p_load_row(_q, i);
        g2412p_step    (_q   );
        g2412p_save_row(_q, i);
    }
#endif
}

// 
void g2412p_step(g2412p _q)
{
    // TODO: determine if parity check passed

    // run hard-decision decoding
    unsigned int sym_rec = 0;
    unsigned int i;
    for (i=0; i<24; i++) {
        sym_rec <<= 1;
        sym_rec |= _q->reg0[i] > 127 ? 1 : 0;
    }

    // decode to 12-bit symbol
    unsigned int sym_dec = fec_golay2412_decode_symbol(sym_rec);

    // re-encode 24-bit symbol
    unsigned int sym_enc = fec_golay2412_encode_symbol(sym_dec);

    // update register
    // TODO: this is the crux of the algorithm and should be adjusted to
    //       produce maximum performance.
    for (i=0; i<24; i++) {
#if 0
        int v = (int)(_q->reg0[i]) + ((sym_enc >> i) & 1 ? 16 : -16);
#else
        int p = (sym_enc >> i) & 1 ? LIQUID_SOFTBIT_1 : LIQUID_SOFTBIT_0;
        int v = ((int)(_q->reg0[i]) + p) / 2;
#endif

        if      (v <   0) _q->reg0[i] = 0;
        else if (v > 255) _q->reg0[i] = 255;
        else              _q->reg0[i] = (unsigned char)v;
    }
}

void g2412p_load_row(g2412p       _q,
                     unsigned int _row)
{
    memmove(_q->reg0, &_q->msg_buf[_row*24], 24*sizeof(unsigned char));
}

void g2412p_save_row(g2412p       _q,
                     unsigned int _row)
{
    memmove(&_q->msg_buf[_row*24], _q->reg0, 24*sizeof(unsigned char));
}

void g2412p_load_col(g2412p       _q,
                     unsigned int _col)
{
    unsigned int i;
    for (i=0; i<24; i++)
        _q->reg0[i] = _q->msg_buf[24*i + _col];
}

void g2412p_save_col(g2412p       _q,
                     unsigned int _col)
{
    unsigned int i;
    for (i=0; i<24; i++)
        _q->msg_buf[24*i + _col] = _q->reg0[i];
}

