// 
// scripts/listings_check.c
//
// This program simply generates a test file for the
// listings-check make target.
//

#include <stdlib.h>
#include <stdio.h>

// print usage/help message
void usage()
{
    printf("Usage: listings_check INPUT OUTPUT\n");
    printf("  generates a test file for the listings-check\n");
    printf("  make target\n");
}


int main(int argc, char*argv[])
{
    // options
    char * input_filename = argv[1];
    char * output_filename = argv[2];

    if (argc != 3) {
        usage();
        exit(1);
    }

    // open output file
    FILE * fid = fopen(output_filename, "w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], output_filename);
        exit(1);
    }
    fprintf(fid,"// auto-generated file (do not edit)\n");
    fprintf(fid,"#include <stdio.h>\n");
    fprintf(fid,"#include <stdlib.h>\n");
    fprintf(fid,"#include <string.h>\n");
    fprintf(fid,"#include <math.h>\n");
    fprintf(fid,"#include <complex.h>\n");
    fprintf(fid,"#include \"liquid.h\"\n");
    fprintf(fid,"#include \"liquid.experimental.h\"\n");
    fprintf(fid,"\n");
    fprintf(fid,"#include \"%s\"\n", input_filename);

    // close output file
    fclose(fid);

    return 0;
}
