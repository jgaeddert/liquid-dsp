//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "interleaver_internal.h"
#include "../../sequence/src/sequence.h"
#include "../../utility/src/utility.h"

void interleaver_print(interleaver _q)
{
    unsigned int i;
    printf("interleaver [%u] :\n", _q->len);
    for (i=0; i<_q->len; i++)
        printf("  p[%u] : %u\n", i, _q->p[i]);
}

void interleaver_debug_print(interleaver _q)
{
    unsigned int n = (_q->len)*sizeof(unsigned char)*8;
    unsigned int t[n];
    interleaver_compute_bit_permutation(_q, t);

    unsigned int i,j;

    printf("\t|");
    j=0;
    for (i=0; i<n; i++) {
        if ((i%10)==0)  printf("%u", j++);
        else            printf(" ");
    }
    printf("|\n");

    for (i=0; i<n; i++) {
        printf("%u\t|", i);
        for (j=0; j<n; j++) {
            if (j==t[i])
                printf("*");
            else if (j==i)
                printf("\\");
            else if ((j%10)==0)
                printf("|");
            else if ((i%10)==0)
                printf("-");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void interleaver_interleave(interleaver _q, unsigned char * _x, unsigned char * _y)
{
    memcpy(_y, _x, _q->len);
    interleaver_permute_forward(_y, _q->p, _q->len, 0x0f);
    interleaver_permute_forward(_y, _q->p, _q->len, 0x5a);
}

void interleaver_deinterleave(interleaver _q, unsigned char * _y, unsigned char * _x)
{
    memcpy(_x, _y, _q->len);
    interleaver_permute_backward(_x, _q->p, _q->len, 0x5a);
    interleaver_permute_backward(_x, _q->p, _q->len, 0x0f);
}

// protected
void interleaver_permute_forward(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++) {
        tmp[i] = (_x[_p[i]]&_mask) | (_x[i] &(~_mask));
    }
    memcpy(_x, tmp, _n);
}

void interleaver_permute_backward(unsigned char * _x, unsigned int * _p, unsigned int _n, unsigned char _mask)
{
    unsigned char tmp[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        tmp[_p[i]] = (_x[i]&_mask) | (_x[_p[i]] &(~_mask));
    memcpy(_x, tmp, _n);
}


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

