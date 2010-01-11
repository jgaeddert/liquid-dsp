//
// optim_ps_example.c
//
// optimization pattern set example
//
// This example demonstrates the functionality of the optim_ps
// structure for easily managing pattern sets for optimization.
//

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

int main() {
    // options
    unsigned int num_inputs = 3;
    unsigned int num_outputs = 2;
    unsigned int num_patterns = 12;

    // initialize 
    unsigned int i, j;
    float x[num_inputs*num_patterns];
    float y[num_outputs*num_patterns];
    for (i=0; i<num_patterns; i++) {
        for (j=0; j<num_inputs; j++)
            x[i*num_inputs+j] = randnf();

        for (j=0; j<num_outputs; j++)
            y[i*num_outputs+j] = randnf();
    }

    // create pattern set (empty by default)
    optim_ps ps = optim_ps_create(num_inputs, num_outputs);
    optim_ps_print(ps);

    // add patterns to the set
    printf("\n");
    printf("appending patterns...\n");
    optim_ps_append_patterns(ps,x,y,num_patterns);
    optim_ps_print(ps);

    // retrieve pattern and print
    float *a, *b;   // read pointers
    unsigned int index = 7;
    printf("\n");
    printf("retrieving pattern %u...\n", index);
    optim_ps_access(ps, index, &a, &b);
    printf("[%3u]   : ", index);
    for (i=0; i<num_inputs; i++)
        printf("%8.5f ", a[i]);
    printf(":  ");
    for (i=0; i<num_outputs; i++)
        printf("%8.5f ", b[i]);
    printf("\n");

    // delete pattern 1
    printf("\n");
    printf("deleting pattern 1...\n");
    optim_ps_delete_pattern(ps, 1);
    optim_ps_print(ps);

    // clear pattern set
    printf("clearing pattern set...\n");
    optim_ps_clear(ps);
    optim_ps_print(ps);

    // clean up allocated memory objects
    optim_ps_destroy(ps);

    printf("done.\n");
    return 0;
}

