// doc-html-gen.c
// 
// generate html documentation from index LaTeX file
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("doc-html-gen [options]\n");
    printf("  u/h   : print usage\n");
    printf("  i     : specify input LaTeX file\n");
    printf("  o     : specify output directory\n");
}


int main(int argc, char*argv[])
{
    char latex_filename[256] = "";
    char output_dir[256] = "html";

    int dopt;
    while ((dopt = getopt(argc,argv,"uhi:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();                            return 0;
        case 'i':   strncpy(latex_filename,optarg,256); break;
        case 'o':   strncpy(output_dir,optarg,256);     break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }

    // ensure input filename was set
    if (strcmp(latex_filename,"")==0) {
        fprintf(stderr,"error: %s, invalid or unspecified latex filename\n", argv[0]);
        return 1;
    }

    //htmlgen_parse_latex_file(latex_filename, output_dir);

    return 0;
}

