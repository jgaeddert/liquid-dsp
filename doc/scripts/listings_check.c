// 
// scripts/listings_check.c
//
// This program simply generates a test file for the
// listings-check make target.
//

#include <stdlib.h>
#include <stdio.h>

#define BUFFER_LEN (256)

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
    fprintf(fid,"#include \"liquid.h\"\n");
    fprintf(fid,"#include \"liquid.experimental.h\"\n");
    fprintf(fid,"\n");
    
    fprintf(fid,"//////////////////////////////////////////\n");
    fprintf(fid,"// BEGIN INPUT FILE\n");
    fprintf(fid,"//////////////////////////////////////////\n");

    char buffer[BUFFER_LEN];    // buffer
    char * cptr = NULL;         // readline return value
    unsigned int n=0;           // input line number

    // parse input file and strip out occurrences of "#include <liquid/liquid.h>"
    FILE * fin = fopen(input_filename,"r");
    if (!fin) {
        fprintf(stderr,"error: %s, could not open '%s' for reading\n", argv[0], input_filename);
        exit(1);
    }

    // parse input file
    do {
        // read line
        cptr = fgets(buffer, BUFFER_LEN, fin);

        if (cptr != NULL) {
            // strip out certain lines
            if (strcmp(cptr,"#include <liquid/liquid.h>\n") == 0) {
                fprintf(fid,"// LINE (%3u) REMOVED: %s", n, cptr);
                continue;
            } else {
                fprintf(fid,"%s", cptr);
                n++;
            }
        } else {
            break;
        }
    } while (!feof(fin));

    fclose(fin);

    fprintf(fid,"//////////////////////////////////////////\n");
    fprintf(fid,"// END INPUT FILE\n");
    fprintf(fid,"//////////////////////////////////////////\n");

    // close output file
    fclose(fid);

    return 0;
}
