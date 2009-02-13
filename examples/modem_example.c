/*
 * modem_example.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

int main() {

    modem m = modem_create(MOD_QPSK, 2);

    free_modem(m);

    printf("done.\n");
    return 0;
}

