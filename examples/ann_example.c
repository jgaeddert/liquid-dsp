//
// artificial neural network (ann) example
//

#include <stdio.h>
#include "liquid.h"

#define DEBUG_FILENAME "ann_example.m"

int main() {
    // options
    unsigned int structure[3] = {2, 4, 2};

    ann q = ann_create(structure, 3);
    ann_print(q);

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

