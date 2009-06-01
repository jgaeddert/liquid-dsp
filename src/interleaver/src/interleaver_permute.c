/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Interleaver: permutation functions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "interleaver_internal.h"

void interleaver_permute_forward(unsigned char * _x, unsigned int * _p, unsigned int _n)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++) {
        tmp[i] = _x[_p[i]];
    }
    memcpy(_x, tmp, _n);
}

void interleaver_permute_reverse(unsigned char * _x, unsigned int * _p, unsigned int _n)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        tmp[_p[i]] = _x[i];
    memcpy(_x, tmp, _n);
}


void interleaver_permute_forward_mask(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++) {
        tmp[i] = (_x[_p[i]]&_mask) | (_x[i] &(~_mask));
    }
    memcpy(_x, tmp, _n);
}

void interleaver_permute_reverse_mask(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        tmp[_p[i]] = (_x[i]&_mask) | (_x[_p[i]] &(~_mask));
    memcpy(_x, tmp, _n);
}

void interleaver_circshift_L4(unsigned char *_x, unsigned int _n)
{
    unsigned int i;
    unsigned char head, tail, mask_lo=0x0f, mask_hi=0xf0;

    // save head at _x[0]
    unsigned char tmp = (_x[0] & mask_hi) >> 4;

    for (i=0; i<_n-1; i++) {
        head = (_x[i] & mask_lo) << 4;
        tail = (_x[i+1] & mask_hi) >> 4;
        _x[i] = head | tail;
    }

    // last byte
    head = (_x[_n-1] & mask_lo) << 4;
    _x[_n-1] = head | tmp;
}

void interleaver_circshift_R4(unsigned char *_x, unsigned int _n)
{
    unsigned int i;
    unsigned char head, tail, mask_lo=0x0f, mask_hi=0xf0;

    // save tail at last byte -> head at first byte
    unsigned char tmp = (_x[_n-1] & mask_lo) << 4;

    for (i=_n-1; i>0; i--) {
        tail = (_x[i] & mask_hi) >> 4;
        head = (_x[i-1] & mask_lo) << 4;
        _x[i] = head | tail;
    }

    // first byte
    tail = (_x[0] & mask_hi) >> 4;
    _x[0] = tmp | tail;
}

#if 0
#define HEAD_TO_TAIL(x,m,mask) (((x)&(mask))>>m) // mask and shift right (head to tail)
#define TAIL_TO_HEAD(x,m,mask) (((x)&(mask))<<m) // mask and shift left (tail to head)
void interleaver_circshift_left(unsigned char *_x, unsigned int _n, unsigned int _s)
{
    unsigned int h=_s;  // head bits (number of bits by which to shift)
    unsigned int t=8-h; // tail bits
    unsigned char tail_mask = (1<<t)-1;
    unsigned char head_mask = ~tail_mask;
    unsigned char head, tail, tmp = HEAD_TO_TAIL(_x[0],h,head_mask);
    unsigned int i;
    for (i=0; i<_n-1; i++) {
        //head = (_x[i] & 0x0f) << 4;
        head = TAIL_TO_HEAD(_x[i],h,tail_mask);

        //tail = (_x[i+1] & 0xf0) >> 4;
        tail = HEAD_TO_TAIL(_x[i+1],h,head_mask);
        _x[i] = head | tail;
    }
    //head = (_x[_n-1] & 0x0f) << 4;
    head = TAIL_TO_HEAD(_x[_n-1],h,tail_mask);
    _x[_n-1] = head | tmp;
}

void interleaver_circshift_right(unsigned char *_x, unsigned int _n, unsigned int _s)
{
    unsigned int h=_s;  // head bits (number of bits by which to shift)
    //unsigned int t=8-h; // tail bits
    unsigned char tail_mask = (1<<h)-1;
    unsigned char head_mask = ~tail_mask;
    unsigned char head, tail, tmp = TAIL_TO_HEAD(_x[_n-1],h,tail_mask);
    unsigned int i;
    for (i=_n-1; i>0; i--) {
        head = TAIL_TO_HEAD(_x[i-1],h,tail_mask);
        tail = HEAD_TO_TAIL(_x[i],h,head_mask);
        _x[i] = head | tail;
    }
    tail = HEAD_TO_TAIL(_x[0],h,head_mask);
    _x[0] = tmp | tail;

}
#endif

void interleaver_compute_bit_permutation(interleaver _q, unsigned int * _p)
{
    unsigned int i, j;
    unsigned char x[_q->len], y[_q->len];

    for (i=0; i<_q->len; i++)
        x[i] = 0;

    for (i=0; i<_q->len; i++) {
        for (j=0; j<8; j++) {
            x[i] = 1<<j;
            interleaver_interleave(_q, x, y);
            // find where the bit went!
            // look for byte containing bit
            unsigned int k;
            for (k=0; k<_q->len; k++) {
                if (y[k] > 0)
                    break;
            }
            // find bit position
            unsigned char v = y[k];
            unsigned int r;
            for (r=0; r<8; r++) {
                if (v & 1)
                    break;
                v >>= 1;
            }
            _p[8*i + j] = 8*k + r;
        }
        x[i] = 0;
    }
}
