//
// gport_dma_example.c
//
// gport (generic port) direct memory access example
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.experimental.h"

int main() {
    // options
    unsigned int n=16;

    // create port
    gport p = gport_create(n,sizeof(int));

    // producer
    int * w;
    unsigned int i;
    w = (int*) gport_producer_lock(p,8);
    for (i=0; i<8; i++)
        w[i] = i;
    gport_producer_unlock(p,8);

    // consumer
    int * r;
    r = (int*) gport_consumer_lock(p,4);
    for (i=0; i<4; i++)
        printf(" r(%2u) = %d\n",i,r[i]);
    gport_consumer_unlock(p,2);
    
    // print status
    gport_print(p);

    gport_destroy(p);

    printf("done.\n");
    return 0;
}
