//
// artificial neural network (ann) example
//

#include <stdio.h>
#include "liquid.h"

#define DEBUG_FILENAME "ann_example.m"

int main() {
    // options
    unsigned int structure[3] = {2, 4, 3};

    float x[2] = {0,0};
    float y[3];

    ann q = ann_create(structure, 3);
    ann_print(q);

    ann_evaluate(q,x,y);

    unsigned int i;
    for (i=0; i<3; i++)
        printf("y[%3u] = %12.8f\n", i, y[i]);

#if 0
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);
#endif
    
    ann_destroy(q);

    printf("done.\n");
    return 0;
}

