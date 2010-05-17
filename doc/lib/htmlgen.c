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
void htmlgen_parse_latex_file(FILE * _fid_tex,
                              FILE * _fid_html,
                              FILE * _fid_eqmk)
{
    // html: write header
    htmlgen_html_write_header(_fid_html);
    fprintf(_fid_html,"<h1>liquid documentation</h1>\n");

    // equation makefile add header, etc.
    fprintf(_fid_eqmk,"# equations makefile : auto-generated\n");
    fprintf(_fid_eqmk,"html_eqn_texfiles := ");

    unsigned int equation_id = 0;
    char filename_eqn[256];
    sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", equation_id);

    // equation
    htmlgen_add_equation("y = \\int_0^\\infty { \\gamma^2 \\cos(x) dx }",
                         equation_id,
                         _fid_eqmk);

    // insert equation into html file
    fprintf(_fid_html,"<img src=\"eqn/eqn%.4u.png\" />\n", equation_id);

    // increment equation id
    equation_id++;

    // repeat as necessary

    // equation makefile: clear end-of-line
    fprintf(_fid_eqmk,"\n\n");

    // write html footer
    htmlgen_html_write_footer(_fid_html);
}

void htmlgen_add_equation(char * _eqn,
                          unsigned int _eqn_id,
                          FILE * _fid_eqmk)
{
    //
    char filename_eqn[64] = "";
    sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", _eqn_id);

    // open file
    FILE * fid_eqn = fopen(filename_eqn, "w");
    if (!fid_eqn) {
        fprintf(stderr,"error, could not open '%s' for writing\n", filename_eqn);
        exit(1);
    }
    fprintf(fid_eqn,"%% %s : auto-generated file\n", filename_eqn);

    // write header
    htmlgen_eqn_write_header(fid_eqn);

    // write equation
    fprintf(fid_eqn,"\\[\n");
    fprintf(fid_eqn,"%s\n", _eqn);
    fprintf(fid_eqn,"\\]\n");

    // write footer
    htmlgen_eqn_write_footer(fid_eqn);

    // close file
    fclose(fid_eqn);

    // add equation to makefile: target collection
    fprintf(_fid_eqmk,"\\\n\t%s", filename_eqn);
}

void htmlgen_eqn_write_header(FILE * _fid)
{
    fprintf(_fid,"\\documentclass{article} \n");
    fprintf(_fid,"\\usepackage{amsmath}\n");
    fprintf(_fid,"\\usepackage{amsthm}\n");
    fprintf(_fid,"\\usepackage{amssymb}\n");
    fprintf(_fid,"\\usepackage{bm}\n");
    fprintf(_fid,"\\newcommand{\\mx}[1]{\\mathbf{\\bm{#1}}} %% Matrix command\n");
    fprintf(_fid,"\\newcommand{\\vc}[1]{\\mathbf{\\bm{#1}}} %% Vector command \n");
    fprintf(_fid,"\\newcommand{\\T}{\\text{T}}              %% Transpose\n");
    fprintf(_fid,"\\pagestyle{empty} \n");
    fprintf(_fid,"\\begin{document} \n");
    fprintf(_fid,"\\newpage\n");
}

void htmlgen_eqn_write_footer(FILE * _fid)
{
    fprintf(_fid,"\\end{document}\n");
}

// Write output html header
void htmlgen_html_write_header(FILE * _fid)
{
    fprintf(_fid,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n");
    fprintf(_fid,"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    fprintf(_fid,"<!-- auto-generated file, do not edit -->\n");
    fprintf(_fid,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n");
    fprintf(_fid,"<head>\n");
    fprintf(_fid,"<!-- <style type=\"text/css\" media=\"all\">@import url(http://computing.ece.vt.edu/~jgaeddert/web.css);</style> -->\n");
    fprintf(_fid,"<title>jgaeddert</title>\n");
    fprintf(_fid,"<meta name=\"description\" content=\"Gaeddert Virginia Tech\" />\n");
    fprintf(_fid,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
    fprintf(_fid,"<!-- <link rel=\"Shortcut Icon\" type=\"image/png\" href=\"img/favicon.png\" /> -->\n");
    fprintf(_fid,"</head>\n");
    fprintf(_fid,"<body>\n");
}

// Write output html footer
void htmlgen_html_write_footer(FILE * _fid)
{
    fprintf(_fid,"    <!--\n");
    fprintf(_fid,"    <p>\n");
    fprintf(_fid,"    Validate:\n");
    fprintf(_fid,"    <a href=\"http://validator.w3.org/check?uri=https://ganymede.ece.vt.edu/\">XHTML 1.0</a>&nbsp;|\n");
    fprintf(_fid,"    <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">CSS</a>\n");
    fprintf(_fid,"    </p>\n");
    fprintf(_fid,"    -->\n");
    fprintf(_fid,"    <p>Last updated: <em> ... </em></p>\n");
    fprintf(_fid,"</body>\n");
    fprintf(_fid,"</html>\n");
}

