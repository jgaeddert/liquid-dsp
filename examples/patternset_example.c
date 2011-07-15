//
// patternset_example.c
//
// optimization pattern set example
//
// This example demonstrates the functionality of the patternset
// structure for easily managing pattern sets for optimization.
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.experimental.h"

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
    patternset ps = patternset_create(num_inputs, num_outputs);
    patternset_print(ps);

    // add patterns to the set
    printf("\n");
    printf("appending patterns...\n");
    patternset_append_patterns(ps,x,y,num_patterns);
    patternset_print(ps);

    // retrieve pattern and print
    float *a, *b;   // read pointers
    unsigned int index = 7;
    printf("\n");
    printf("retrieving pattern %u...\n", index);
    patternset_access(ps, index, &a, &b);
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
    patternset_delete_pattern(ps, 1);
    patternset_print(ps);

    // clear pattern set
    printf("clearing pattern set...\n");
    patternset_clear(ps);
    patternset_print(ps);

    // clean up allocated memory objects
    patternset_destroy(ps);

    printf("done.\n");
    return 0;
}

