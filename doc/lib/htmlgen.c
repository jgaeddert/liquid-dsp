/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// htmlgen.c : html documentation generator
//
// rules:
//  * comments begin with '%'
//  * tilda character '~' is a space
//  * environment tokens begin with "\begin" or "\end"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "liquid.doc.h"

// token table
htmlgen_token_s htmlgen_token_tab[] = {
    {"\\begin",         htmlgen_token_parse_begin},
    {"\\end",           htmlgen_token_parse_end},
    {"document",        htmlgen_token_parse_document},
    {"section",         htmlgen_token_parse_section},
    {"subsection",      htmlgen_token_parse_subsection},
    {"subsubsection",   htmlgen_token_parse_subsubsection},
    {"figure",          htmlgen_token_parse_figure},
    {"tabular",         htmlgen_token_parse_tabular},
    {"enumerate",       htmlgen_token_parse_enumerate},
    {"itemize",         htmlgen_token_parse_itemize},
};

// parse LaTeX file
void htmlgen_parse_latex_file(char * _filename_tex,
                              char * _filename_html,
                              char * _filename_eqmk)
{
    htmlgen q = htmlgen_create(_filename_tex,
                               _filename_html,
                               _filename_eqmk);

    htmlgen_open_files(q);

    // html: write header
    htmlgen_html_write_header(q);
    fprintf(q->fid_html,"<h1>liquid documentation</h1>\n");

    // equation makefile add header, etc.
    fprintf(q->fid_eqmk,"# equations makefile : auto-generated\n");
    fprintf(q->fid_eqmk,"html_eqn_texfiles := ");

    char filename_eqn[256];
    sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", q->equation_id);

    // add equations
    htmlgen_add_equation(q, "y = \\int_0^\\infty { \\gamma^2 \\cos(x) dx }", 0);
    htmlgen_add_equation(q, "y = \\sum_{k=0}^{N-1} { \\sin\\Bigl( \\frac{x^k}{k!} \\Bigr) }", 0);
    htmlgen_add_equation(q, "z = \\frac{1}{2} \\beta \\gamma^{1/t}", 0);

    fprintf(q->fid_html,"<p>here is a pretty inline equation:\n");
    htmlgen_add_equation(q, "\\hat{s} = r_0 \\otimes r_1 \\otimes r_2", 1);
    fprintf(q->fid_html,"</p>\n");

    // repeat as necessary

    // equation makefile: clear end-of-line
    fprintf(q->fid_eqmk,"\n\n");

    // write html footer
    htmlgen_html_write_footer(q);

    // close files
    htmlgen_close_files(q);
}


// create htmlgen object
htmlgen htmlgen_create(char * _filename_tex,
                       char * _filename_html,
                       char * _filename_eqmk)
{
    // create htmlgen object
    htmlgen q = (htmlgen) malloc(sizeof(struct htmlgen_s));
    q->equation_id = 0;

    // copy file names
    strncpy(q->filename_tex,  _filename_tex,  256);
    strncpy(q->filename_html, _filename_html, 256);
    strncpy(q->filename_eqmk, _filename_eqmk, 256);

    return q;
}

void htmlgen_open_files(htmlgen _q)
{
    // open files
    _q->fid_tex  = fopen(_q->filename_tex, "r");
    _q->fid_html = fopen(_q->filename_html,"w");
    _q->fid_eqmk = fopen(_q->filename_eqmk,"w");

    // validate files opened properly
    if (!_q->fid_tex) {
        fprintf(stderr,"error, could not open '%s' for reading\n", _q->filename_tex);
        exit(1);
    } else if (!_q->fid_html) {
        fprintf(stderr,"error, could not open '%s' for writing\n", _q->filename_html);
        exit(1);
    } else  if (!_q->fid_eqmk) {
        fprintf(stderr,"error, could not open '%s' for writing\n", _q->filename_eqmk);
        exit(1);
    }
}

void htmlgen_close_files(htmlgen _q)
{
    // close files
    fclose(_q->fid_tex);
    fclose(_q->fid_html);
    fclose(_q->fid_eqmk);
}

void htmlgen_destroy(htmlgen _q)
{

    // free memory
    free(_q);
}

void htmlgen_add_equation(htmlgen _q,
                          char * _eqn,
                          int _inline)
{
    //
    char filename_eqn[64] = "";
    sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", _q->equation_id);

    // open file
    FILE * fid_eqn = fopen(filename_eqn, "w");
    if (!fid_eqn) {
        fprintf(stderr,"error, could not open '%s' for writing\n", filename_eqn);
        exit(1);
    }
    fprintf(fid_eqn,"%% %s : auto-generated file\n", filename_eqn);

    // write header
    fprintf(fid_eqn,"\\documentclass{article} \n");
    fprintf(fid_eqn,"\\usepackage{amsmath}\n");
    fprintf(fid_eqn,"\\usepackage{amsthm}\n");
    fprintf(fid_eqn,"\\usepackage{amssymb}\n");
    fprintf(fid_eqn,"\\usepackage{bm}\n");
    fprintf(fid_eqn,"\\newcommand{\\mx}[1]{\\mathbf{\\bm{#1}}} %% Matrix command\n");
    fprintf(fid_eqn,"\\newcommand{\\vc}[1]{\\mathbf{\\bm{#1}}} %% Vector command \n");
    fprintf(fid_eqn,"\\newcommand{\\T}{\\text{T}}              %% Transpose\n");
    fprintf(fid_eqn,"\\pagestyle{empty} \n");
    fprintf(fid_eqn,"\\begin{document} \n");
    fprintf(fid_eqn,"\\newpage\n");

    // write equation
    fprintf(fid_eqn,"\\[\n");
    fprintf(fid_eqn,"%s\n", _eqn);
    fprintf(fid_eqn,"\\]\n");

    // write footer
    fprintf(fid_eqn,"\\end{document}\n");

    // close file
    fclose(fid_eqn);

    // insert equation into html file
    if (!_inline) fprintf(_q->fid_html,"<p>\n");
    fprintf(_q->fid_html,"<img src=\"eqn/eqn%.4u.png\" />\n", _q->equation_id);
    if (!_inline) fprintf(_q->fid_html,"</p>\n");

    // add equation to makefile: target collection
    fprintf(_q->fid_eqmk,"\\\n\t%s", filename_eqn);

    // increment equation id
    _q->equation_id++;
}

// Write output html header
void htmlgen_html_write_header(htmlgen _q)
{
    fprintf(_q->fid_html,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n");
    fprintf(_q->fid_html,"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    fprintf(_q->fid_html,"<!-- auto-generated file, do not edit -->\n");
    fprintf(_q->fid_html,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n");
    fprintf(_q->fid_html,"<head>\n");
    fprintf(_q->fid_html,"<!-- <style type=\"text/css\" media=\"all\">@import url(http://computing.ece.vt.edu/~jgaeddert/web.css);</style> -->\n");
    fprintf(_q->fid_html,"<title>jgaeddert</title>\n");
    fprintf(_q->fid_html,"<meta name=\"description\" content=\"Gaeddert Virginia Tech\" />\n");
    fprintf(_q->fid_html,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
    fprintf(_q->fid_html,"<!-- <link rel=\"Shortcut Icon\" type=\"image/png\" href=\"img/favicon.png\" /> -->\n");
    fprintf(_q->fid_html,"</head>\n");
    fprintf(_q->fid_html,"<body>\n");
}

// Write output html footer
void htmlgen_html_write_footer(htmlgen _q)
{
    fprintf(_q->fid_html,"    <!--\n");
    fprintf(_q->fid_html,"    <p>\n");
    fprintf(_q->fid_html,"    Validate:\n");
    fprintf(_q->fid_html,"    <a href=\"http://validator.w3.org/check?uri=https://ganymede.ece.vt.edu/\">XHTML 1.0</a>&nbsp;|\n");
    fprintf(_q->fid_html,"    <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a>\n");
    fprintf(_q->fid_html,"    </p>\n");
    fprintf(_q->fid_html,"    -->\n");
    fprintf(_q->fid_html,"    <p>Last updated: <em> TODO : add date/time here </em></p>\n");
    fprintf(_q->fid_html,"</body>\n");
    fprintf(_q->fid_html,"</html>\n");
}

