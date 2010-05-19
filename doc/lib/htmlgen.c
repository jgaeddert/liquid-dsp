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
#include "liquid.doc.html.h"

#define HTMLGEN_NUM_TOKENS  (19)

// token table
htmlgen_token_s htmlgen_token_tab[HTMLGEN_NUM_TOKENS] = {
    {"\\begin{",            htmlgen_token_parse_begin},
    {"\\chapter{",          htmlgen_token_parse_chapter},
    {"\\section{",          htmlgen_token_parse_section},
    {"\\subsection{",       htmlgen_token_parse_subsection},
    {"\\subsubsection{",    htmlgen_token_parse_subsubsection},
    {"\\label{",            htmlgen_token_parse_label},
    {"\\input{",            htmlgen_token_parse_input},
    {"\\bibliography",      htmlgen_token_parse_fail},      // TODO : add appropriate method
    {"\\input{",            htmlgen_token_parse_fail},      // TODO : add appropriate method
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
                              char * _filename_eqmk)
{
    htmlgen q = htmlgen_create(_filename_tex,
                               _filename_html,
                               _filename_eqmk);

    // open files for reading/writing
    htmlgen_open_files(q);

    // html: write header
    htmlgen_html_write_header(q);
    fprintf(q->fid_html,"<h1>liquid documentation</h1>\n");

    // equation makefile add header, etc.
    fprintf(q->fid_eqmk,"# equations makefile : auto-generated\n");
    fprintf(q->fid_eqmk,"html_eqn_texfiles := ");

    //char filename_eqn[256];
    //sprintf(filename_eqn,"html/eqn/eqn%.4u.tex", q->equation_id);

    // add equations
    htmlgen_add_equation(q, "y = \\int_0^\\infty { \\gamma^2 \\cos(x) dx }", 0);
    htmlgen_add_equation(q, "y = \\sum_{k=0}^{N-1} { \\sin\\Bigl( \\frac{x^k}{k!} \\Bigr) }", 0);
    htmlgen_add_equation(q, "z = \\frac{1}{2} \\beta \\gamma^{1/t}", 0);

    fprintf(q->fid_html,"<p>here is a pretty inline equation:\n");
    htmlgen_add_equation(q, "\\hat{s} = r_0 \\otimes r_1 \\otimes r_2", 1);
    fprintf(q->fid_html,"</p>\n");

    // repeat as necessary
    // strip the preamble and store in external file
    htmlgen_parse_strip_preamble(q,"html/preamble.tex");
    htmlgen_parse_seek_first_chapter(q);

    // run batch parser
#if 1
    while (!feof(q->fid_tex))
        htmlgen_parse(q);
#else
    unsigned int i;
    for (i=0; i<40; i++)
        htmlgen_parse(q);
#endif

    //htmlgen_buffer_consume(q, q->buffer_size);

    // equation makefile: clear end-of-line
    fprintf(q->fid_eqmk,"\n\n");

    // write html footer
    htmlgen_html_write_footer(q);

    // close files
    htmlgen_close_files(q);

    // destroy object
    htmlgen_destroy(q);
}


// create htmlgen object
htmlgen htmlgen_create(char * _filename_tex,
                       char * _filename_html,
                       char * _filename_eqmk)
{
    // create htmlgen object
    htmlgen q = (htmlgen) malloc(sizeof(struct htmlgen_s));

    // set counters
    q->equation_id = 0;
    q->chapter = 0;
    q->section = 0;
    q->subsection = 0;
    q->subsubsection = 0;

    // set environment mode to NONE
    q->environment_mode = HTMLGEN_ENV_NONE;

    q->buffer_size = 0;

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

    if (feof(_q->fid_tex)) {
        printf("htmlgen_buffer_produce() : eof!\n");
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
    while (!token_found && !feof(_q->fid_tex)) {
        // fill buffer
        htmlgen_buffer_produce(_q);

        // search for token
        token_found = htmlgen_get_token(_q, &begindoc, 1, &token_index, &n);

        if (token_found) {
            // write partial buffer to file (up until '\begin{document}')
            htmlgen_buffer_dump(_q, fid, n);

            // consume buffer through token and EOL
            htmlgen_buffer_consume(_q, n);
            htmlgen_buffer_consume_eol(_q); // strip '\begin{document}...\n'
        } else {
            // write full buffer to file
            htmlgen_buffer_dump_all(_q, fid);

            // empty buffer (consume all elements)
            htmlgen_buffer_consume_all(_q);
        }
    }

    if (feof(_q->fid_tex)) {
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
    while (!token_found && !feof(_q->fid_tex)) {
        // fill buffer
        htmlgen_buffer_produce(_q);

        // search for token
        token_found = htmlgen_get_token(_q, &chtok, 1, &token_index, &n);

        if (token_found) {
            // consume buffer through token
            htmlgen_buffer_consume(_q, n);
        } else {
            // empty buffer (consume all elements)
            htmlgen_buffer_consume(_q, _q->buffer_size);
        }
    }

    if (feof(_q->fid_tex)) {
        fprintf(stderr,"error: htmlgen_parse_seek_first_chapter(), found EOF prematurely\n");
        exit(1);
    }

    // buffer should now contain: \chapter{...
}

// base parsing method, writing html output
void htmlgen_parse(htmlgen _q)
{
    // parse file
    
    printf("parse: buffer produce\n");
    htmlgen_buffer_produce(_q); // fill buffer
    //printf("%s", _q->buffer);
    //printf("\n\n");
    unsigned int token_index, n;

    // look for next token
    int token_found = htmlgen_get_token(_q,
                                        htmlgen_token_tab,
                                        HTMLGEN_NUM_TOKENS,
                                        &token_index,
                                        &n);

    // TODO : check for EOF
    if (token_found) {
        printf("next token in buffer at %d is '%s'\n", n, htmlgen_token_tab[token_index].token);

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
        printf("no token found\n");

        // write all of buffer to html file
        htmlgen_buffer_dump_all(_q, _q->fid_html);

        // clear buffer
        htmlgen_buffer_consume_all(_q);
    }
}

