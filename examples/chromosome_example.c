//
// chromosome_example.c
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

int main() {
    chromosome p1 = chromosome_create(4, 4);
    chromosome p2 = chromosome_create(4, 4);
    chromosome c  = chromosome_create(4, 4);

    // 0000 1111 0000 1111
    p1->s[0] = 0x0;
    p1->s[1] = 0xF;
    p1->s[2] = 0x0;
    p1->s[3] = 0xF;

    // 0101 0101 0101 0101
    p2->s[0] = 0x5;
    p2->s[1] = 0x5;
    p2->s[2] = 0x5;
    p2->s[3] = 0x5;

    printf("parent [1]:\n");
    chromosome_print(p1);

    printf("parent [2]:\n");
    chromosome_print(p2);

    printf("\n\n");

    chromosome_crossover(p1, p2, c, 0);
    // .... .... .... ....
    // 0101 0101 0101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 4);
    // 0000 .... .... ....
    // .... 0101 0101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 6);
    // 0000 11.. .... ....
    // .... ..01 0101 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 11);
    // 0000 1111 000. ....
    // .... .... ...1 0101
    chromosome_print(c);

    chromosome_crossover(p1, p2, c, 16);
    // 0000 1111 0000 1111
    // .... .... .... ....
    chromosome_print(c);

    chromosome_destroy(p1);
    chromosome_destroy(p2);
    chromosome_destroy(c);

    return 0;
}

