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
}

// global defaults
char default_filename_tex[256]   = "liquid.tex";
char default_filename_html[256]  = "html/index.html";
char default_filename_eqmk[256]  = "html-equations.mk";
char default_dirname_eq[256]     = "html/eqn/";

int main(int argc, char*argv[])
{
    char filename_tex[256] = "";
    char filename_html[256] = "";
    char filename_eqmk[256] = "";

    // set defaults
    strcpy(filename_tex,  default_filename_tex);
    strcpy(filename_html, default_filename_html);
    strcpy(filename_eqmk, default_filename_eqmk);

    int dopt;
    while ((dopt = getopt(argc,argv,"uhi:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();                            return 0;
        case 'i':   strncpy(filename_tex,optarg,256); break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }

    // print options to screen
    printf("filenames:\n");
    printf("  tex   :   %s\n", filename_tex);
    printf("  html  :   %s\n", filename_html);
    printf("  eqmk  :   %s\n", filename_eqmk);

    // 
    // run parser
    //
    printf("parsing latex file '%s'...\n", filename_tex);
    htmlgen_parse_latex_file(filename_tex,
                             filename_html,
                             filename_eqmk);
    return 0;
}

