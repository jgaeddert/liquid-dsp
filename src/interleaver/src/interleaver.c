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
    //interleaver_permute_forward(_y, _q->p, _q->len, 0x0f);
    //interleaver_permute_forward(_y, _q->p, _q->len, 0x5a);

    interleaver_circshift_L4(_y, _q->len);
    //interleaver_permute_forward(_y, _q->p, _q->len);
    interleaver_permute_forward_mask(_y, _q->p, _q->len, 0x55);
}

void interleaver_deinterleave(interleaver _q, unsigned char * _y, unsigned char * _x)
{
    memcpy(_x, _y, _q->len);
    //interleaver_permute_backward(_x, _q->p, _q->len, 0x5a);
    //interleaver_permute_backward(_x, _q->p, _q->len, 0x0f);

    interleaver_permute_reverse_mask(_x, _q->p, _q->len, 0x55);
    //interleaver_permute_reverse(_x, _q->p, _q->len);
    interleaver_circshift_R4(_x, _q->len);
}

