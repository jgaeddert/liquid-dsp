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
#include <unistd.h>     // usleep()
#include "liquid.doc.html.h"

#define HTMLGEN_NUM_TOKENS  (18)

// token table
htmlgen_token_s htmlgen_token_tab[HTMLGEN_NUM_TOKENS] = {
    {"\\begin{",            htmlgen_token_parse_begin},
    {"\\chapter{",          htmlgen_token_parse_chapter},
    {"\\section{",          htmlgen_token_parse_section},
    {"\\subsection{",       htmlgen_token_parse_subsection},
    {"\\subsubsection{",    htmlgen_token_parse_subsubsection},
    {"\\label{",            htmlgen_token_parse_label},
    {"\\input{",            htmlgen_token_parse_input},
    {"\\bibliography{",     htmlgen_token_parse_bibliography},
    {"{\\tt",               htmlgen_token_parse_tt},
    {"{\\it",               htmlgen_token_parse_it},
    {"{\\em",               htmlgen_token_parse_em},
    {"%",                   htmlgen_token_parse_comment},
    {"\\_",                 htmlgen_token_parse_underscore},
    {"\\{",                 htmlgen_token_parse_leftbrace},
    {"\\}",                 htmlgen_token_parse_rightbrace},

    // environments
    {"\\begin{equation}",   htmlgen_env_parse_equation},
    {"\\[",                 htmlgen_env_parse_eqn},
    {"$",                   htmlgen_env_parse_inline_eqn}
//    {"\n\n",                htmlgen_token_parse_fail}       // TODO : add appropriate method
};

// parse LaTeX file
void htmlgen_parse_latex_file(char * _filename_tex,
                              char * _filename_html,
                              char * _filename_eqns)
{
    htmlgen q = htmlgen_create(_filename_tex,
                               _filename_html,
                               _filename_eqns);

    // open files for reading/writing
    htmlgen_open_files(q);

    // write headers
    htmlgen_html_write_header(q);
    htmlgen_eqns_write_header(q);
    htmlgen_figs_write_header(q);

    fprintf(q->fid_html,"<h1>liquid documentation</h1>\n");

    //char filename_eqn[256];
    //sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", q->equation_id);

    // add equations
    htmlgen_add_equation_string(q, 0, "x = \\int_0^\\infty { \\gamma^2 \\cos(x) dx }");
    htmlgen_add_equation_string(q, 0, "y = \\sum_{k=0}^{N-1} { \\sin\\Bigl( \\frac{x^k}{k!} \\Bigr) }");
    htmlgen_add_equation_string(q, 0, "z = \\frac{1}{2} \\beta \\gamma^{1/t}");

    fprintf(q->fid_html,"<p>here is a pretty inline equation:\n");
    htmlgen_add_equation_string(q, 1, "\\hat{s} = r_0 \\otimes r_1 \\otimes r_2");
    fprintf(q->fid_html,"</p>\n");

    // repeat as necessary
    // strip the preamble and store in external file
    htmlgen_parse_strip_preamble(q,"html/preamble.tex");
    htmlgen_parse_seek_first_chapter(q);

    // run batch parser
    do {
        htmlgen_buffer_produce(q);
        htmlgen_parse(q);
    } while (q->buffer_size > 0);

    //htmlgen_buffer_consume(q, q->buffer_size);

    // write footer
    fprintf(q->fid_eqns,"\\end{document}\n");

    // write footers
    htmlgen_html_write_footer(q);
    htmlgen_eqns_write_footer(q);
    htmlgen_figs_write_footer(q);

    // close files
    htmlgen_close_files(q);

    // destroy object
    htmlgen_destroy(q);
}


// create htmlgen object
htmlgen htmlgen_create(char * _filename_tex,
                       char * _filename_html,
                       char * _filename_eqns)
{
    // create htmlgen object
    htmlgen q = (htmlgen) malloc(sizeof(struct htmlgen_s));

    // set counters
    q->equation_id = 1;
    q->figure_id = 1;
    q->chapter = 0;
    q->section = 0;
    q->subsection = 0;
    q->subsubsection = 0;

    q->buffer_size = 0;

    // copy file names
    strncpy(q->filename_tex,  _filename_tex,  128);
    strncpy(q->filename_html, _filename_html, 128);
    strncpy(q->filename_eqns, _filename_eqns, 128);
    strcpy(q->filename_figs, "html/fig/figures.tex\0");

    return q;
}

void htmlgen_open_files(htmlgen _q)
{
    // open files
    _q->fid_tex  = fopen(_q->filename_tex, "r");
    _q->fid_html = fopen(_q->filename_html,"w");
    _q->fid_eqns = fopen(_q->filename_eqns,"w");
    _q->fid_figs = fopen(_q->filename_figs,"w");

    // validate files opened properly
    if (!_q->fid_tex) {
        fprintf(stderr,"error, could not open '%s' for reading\n", _q->filename_tex);
        exit(1);
    } else if (!_q->fid_html) {
        fprintf(stderr,"error, could not open '%s' for writing\n", _q->filename_html);
        exit(1);
    } else  if (!_q->fid_eqns) {
        fprintf(stderr,"error, could not open '%s' for writing\n", _q->filename_eqns);
        exit(1);
    } else  if (!_q->fid_figs) {
        fprintf(stderr,"error, could not open '%s' for writing\n", _q->filename_figs);
        exit(1);
    }
}

void htmlgen_close_files(htmlgen _q)
{
    // close files
    fclose(_q->fid_tex);
    fclose(_q->fid_html);
    fclose(_q->fid_eqns);
    fclose(_q->fid_figs);
}

void htmlgen_destroy(htmlgen _q)
{

    // free memory
    free(_q);
}

void htmlgen_add_equation_string(htmlgen _q,
                                 int _inline,
                                 char * _eqn)
{
    printf("************ adding equation %u\n", _q->equation_id);
    fprintf(_q->fid_eqns,"\\newpage\n");
    fprintf(_q->fid_eqns,"\\[");

    fprintf(_q->fid_eqns,"%s\n", _eqn);

    fprintf(_q->fid_eqns,"\\]\n");

    // insert equation into html file
    if (!_inline) fprintf(_q->fid_html,"<p align=\"center\">\n");
    fprintf(_q->fid_html,"<img src=\"eqn/eqn%u.png\" />\n", _q->equation_id);
    if (!_inline) fprintf(_q->fid_html,"</p>\n");

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
    fprintf(_q->fid_html,"    <!-- <style type=\"text/css\" media=\"all\">@import url(http://computing.ece.vt.edu/~jgaeddert/web.css);</style> -->\n");
    fprintf(_q->fid_html,"    <title>liquid documentation</title>\n");
    fprintf(_q->fid_html,"    <meta name=\"description\" content=\"Gaeddert Virginia Tech\" />\n");
    fprintf(_q->fid_html,"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
    fprintf(_q->fid_html,"    <!-- <link rel=\"Shortcut Icon\" type=\"image/png\" href=\"img/favicon.png\" /> -->\n");
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

// write output equations header
void htmlgen_eqns_write_header(htmlgen _q)
{
    // equations file
    fprintf(_q->fid_eqns,"%%\n");
    fprintf(_q->fid_eqns,"%% %s : auto-generated file (do not edit)\n", _q->filename_eqns);
    fprintf(_q->fid_eqns,"%%\n");
    fprintf(_q->fid_eqns,"\\documentclass{article} \n");
    fprintf(_q->fid_eqns,"\\usepackage{amsmath}\n");
    fprintf(_q->fid_eqns,"\\usepackage{amsthm}\n");
    fprintf(_q->fid_eqns,"\\usepackage{amssymb}\n");
    fprintf(_q->fid_eqns,"\\usepackage{bm}\n");
    fprintf(_q->fid_eqns,"\\newcommand{\\sinc}{\\textup{sinc}}\n");
    fprintf(_q->fid_eqns,"\\renewcommand{\\vec}[1]{\\boldsymbol{#1}}\n");
    fprintf(_q->fid_eqns,"\\newcommand{\\ord}{\\mathcal{O}}\n");
    fprintf(_q->fid_eqns,"\\newcommand{\\liquid}{{\\it liquid}}\n");
    fprintf(_q->fid_eqns,"\\newcommand{\\liquidfpm}{{\\it liquid-fpm}}\n");

    fprintf(_q->fid_eqns,"\\pagestyle{empty} \n");
    fprintf(_q->fid_eqns,"\\begin{document} \n");
}

// write output equations footer
void htmlgen_eqns_write_footer(htmlgen _q)
{
    // add dummy equation to ensure document contains some content
    fprintf(_q->fid_eqns,"\\newpage\n");
    fprintf(_q->fid_eqns,"\\[ dummy equation\\]\n");

    fprintf(_q->fid_eqns,"\\end{document}\n");
}

// write output figures header
void htmlgen_figs_write_header(htmlgen _q)
{
    // figures file
    fprintf(_q->fid_figs,"%%\n");
    fprintf(_q->fid_figs,"%% %s : auto-generated file (do not edit)\n", _q->filename_figs);
    fprintf(_q->fid_figs,"%%\n");
    fprintf(_q->fid_figs,"\\documentclass{article} \n");
    fprintf(_q->fid_figs,"\\usepackage{amsmath}\n");
    fprintf(_q->fid_figs,"\\usepackage{amsthm}\n");
    fprintf(_q->fid_figs,"\\usepackage{amssymb}\n");
    fprintf(_q->fid_figs,"\\usepackage{bm}\n");
    fprintf(_q->fid_figs,"\\newcommand{\\sinc}{\\textup{sinc}}\n");
    fprintf(_q->fid_figs,"\\renewcommand{\\vec}[1]{\\boldsymbol{#1}}\n");
    fprintf(_q->fid_figs,"\\newcommand{\\ord}{\\mathcal{O}}\n");
    fprintf(_q->fid_figs,"\\newcommand{\\liquid}{{\\it liquid}}\n");
    fprintf(_q->fid_figs,"\\newcommand{\\liquidfpm}{{\\it liquid-fpm}}\n");

    // figures packages
    //fprintf(_q->fid_figs,"\\ifx\\pdfoutput\\undefined\n");
    fprintf(_q->fid_figs,"\\usepackage{graphicx}\n");
    //fprintf(_q->fid_figs,"\\else\n");
    //fprintf(_q->fid_figs,"\\usepackage[pdftex]{graphicx}\n");
    //fprintf(_q->fid_figs,"\\fi\n");
    //fprintf(_q->fid_figs,"\\usepackage{epsfig}\n");
    //fprintf(_q->fid_figs,"\\usepackage{epstopdf}\n");
    fprintf(_q->fid_figs,"\\usepackage{colortbl}\n");
    fprintf(_q->fid_figs,"\\usepackage{color}\n");
    fprintf(_q->fid_figs,"\\usepackage{subfigure}\n");
    fprintf(_q->fid_figs,"\\usepackage{amsmath}\n");
    fprintf(_q->fid_figs,"\\usepackage[small,bf]{caption}\n");
    fprintf(_q->fid_figs,"\\setcaptionwidth{15cm}\n");
    fprintf(_q->fid_figs,"\\setlength{\\belowcaptionskip}{0.5cm}\n");

    fprintf(_q->fid_figs,"\\pagestyle{empty} \n");
    fprintf(_q->fid_figs,"\\begin{document} \n");
}

// write output figures header
void htmlgen_figs_write_footer(htmlgen _q)
{
    // add dummy figure to ensure document contains some content
    fprintf(_q->fid_figs,"\\newpage\n");
    fprintf(_q->fid_figs,"\\begin{figure}n");
    fprintf(_q->fid_figs,"dummy figure\n");
    fprintf(_q->fid_figs,"\\end{figure}\n");

    fprintf(_q->fid_figs,"\\end{document}\n");
}

// read from file into buffer
void htmlgen_buffer_produce(htmlgen _q)
{
    // number of empty elements in buffer
    unsigned int num_remaining = HTMLGEN_BUFFER_LENGTH - _q->buffer_size;

    // read values from file into buffer
    unsigned int k = fread((void*) &_q->buffer[_q->buffer_size],
                           sizeof(char),
                           num_remaining,
                           _q->fid_tex);

    if (k != num_remaining && !feof(_q->fid_tex)) {
        fprintf(stderr,"warning: htmlgen_buffer_produce(), expected %u but only read %u\n", num_remaining, k);
        usleep(10000);
    }

    _q->buffer_size += k;

    // append '\0' to end of buffer
    _q->buffer[_q->buffer_size] = '\0';
    
}

// consume _n elements from buffer
void htmlgen_buffer_consume(htmlgen _q,
                            unsigned int _n)
{
    if (_n > _q->buffer_size) {
        fprintf(stderr,"error: htmlgen_html_buffer_consume(), cannot consume more elments than size of buffer\n");
        exit(1);
    }
    
    // compute remaining values in buffer
    unsigned int num_remaining = _q->buffer_size - _n;

    memmove(_q->buffer, &_q->buffer[_n], num_remaining*sizeof(char));

    _q->buffer_size -= _n;

    // set null character at end of buffer
    _q->buffer[_q->buffer_size] = '\0';
}

// consume through end-of-line character
void htmlgen_buffer_consume_eol(htmlgen _q)
{
    // eat up rest of line using strcspn method
    unsigned int n = strcspn(_q->buffer, "\n\r\v\f");

    // consume buffer up until end-of-line
    htmlgen_buffer_consume(_q, n);
}

// consume entire buffer
void htmlgen_buffer_consume_all(htmlgen _q)
{
    htmlgen_buffer_consume(_q, _q->buffer_size);
}

// write _n characters to output to html file
void htmlgen_buffer_dump(htmlgen _q,
                         FILE * _fid,
                         unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        fprintf(_fid, "%c", _q->buffer[i]);
}


// write entire buffer to output to html file
void htmlgen_buffer_dump_all(htmlgen _q,
                             FILE * _fid)
{
    htmlgen_buffer_dump(_q, _fid, _q->buffer_size);
}


//
// parsing methods
//


// strip preamble and store in external file; stop when
// '\begin{document}' is found
void htmlgen_parse_strip_preamble(htmlgen _q,
                                  char * _filename)
{
    htmlgen_token_s begindoc = {"\\begin{document}", NULL};

    // open output preamble file for writing
    FILE * fid = fopen(_filename, "w");
    if (!fid) {
        fprintf(stderr,"error: htmlgen_strip_preamble(), could not open '%s' for writing\n", _filename);
        exit(1);
    }

    int token_found=0;
    unsigned int token_index, n;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!token_found && _q->buffer_size > 0) {
        // search for token
        token_found = htmlgen_get_token(_q, &begindoc, 1, &token_index, &n);

        // write buffer to file
        htmlgen_buffer_dump(_q, fid, n);

        // consume buffer
        htmlgen_buffer_consume(_q, n);
        
        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (token_found) {
        // consume buffer through token
        htmlgen_buffer_consume(_q, strlen(begindoc.token) );
    } else if (feof(_q->fid_tex)) {
        fprintf(stderr,"error: htmlgen_parse_strip_preamble(), found EOF prematurely\n");
        exit(1);
    }

    fclose(fid);
}

void htmlgen_parse_seek_first_chapter(htmlgen _q)
{
    // define chapter token
    htmlgen_token_s chtok = {"\\chapter{", NULL};

    int token_found=0;
    unsigned int token_index, n;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!token_found && _q->buffer_size > 0) {
        // search for token
        token_found = htmlgen_get_token(_q, &chtok, 1, &token_index, &n);

        // TODO : dump to file?

        // consume buffer
        htmlgen_buffer_consume(_q, n);

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (token_found) {

    } else if (feof(_q->fid_tex)) {
        fprintf(stderr,"error: htmlgen_parse_seek_first_chapter(), found EOF prematurely\n");
        exit(1);
    }

    // buffer should now contain: \chapter{...
}

// base parsing method, writing html output
void htmlgen_parse(htmlgen _q)
{
    // parse file
    
    htmlgen_buffer_produce(_q); // fill buffer

    unsigned int token_index, n;

    // look for next token
    int token_found = htmlgen_get_token(_q,
                                        htmlgen_token_tab,
                                        HTMLGEN_NUM_TOKENS,
                                        &token_index,
                                        &n);

    // TODO : check for EOF
    if (token_found) {
        //printf("next token in buffer at %d is '%s'\n", n, htmlgen_token_tab[token_index].token);

        // write output to html file up to token
        htmlgen_buffer_dump(_q, _q->fid_html, n);

        // consume buffer up through this point
#if 1
        // clear token
        unsigned int token_len = strlen(htmlgen_token_tab[token_index].token);
        htmlgen_buffer_consume(_q, n + token_len);
#else
        htmlgen_buffer_consume(_q, n);
#endif

        // execute token-specific function
        htmlgen_token_tab[token_index].func(_q);
    } else {
        //printf("no token found\n");

        // write all of buffer to html file
        htmlgen_buffer_dump_all(_q, _q->fid_html);

        // clear buffer
        htmlgen_buffer_consume_all(_q);
    }
}

