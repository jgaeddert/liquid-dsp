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
char default_filename_eqmk[256]  = "html/equations.mk";
char default_dirname_eq[256]     = "html/eqn/";

// global file identifiers
FILE * fid_tex  = NULL;
FILE * fid_eqmk = NULL;
FILE * fid_eq   = NULL;
FILE * fid_html = NULL;

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
    // open files
    //

    // latex
    fid_tex = fopen(filename_tex,"r");
    if (!fid_tex) {
        fprintf(stderr,"error, could not open '%s' for reading\n", filename_tex);
        exit(1);
    }

    // html
    fid_html = fopen(filename_html,"w");
    if (!fid_html) {
        fprintf(stderr,"error, could not open '%s' for writing\n", filename_html);
        exit(1);
    }

    // equations makefile
    fid_eqmk = fopen("html/equations.mk","w");
    if (!fid_eqmk) {
        fprintf(stderr,"error, could not open html/equations.mk for writing\n");
        exit(1);
    }

    // 
    // run parser
    //
    printf("parsing latex file '%s'...\n", filename_tex);
    htmlgen_parse_latex_file(fid_tex,
                             fid_html,
                             fid_eqmk);

    // 
    // close files
    //
    fclose(fid_tex);
    fclose(fid_html);
    fclose(fid_eqmk);

    return 0;
}

