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
// html documentation generation
//

#ifndef __LIQUID_DOC_HTML_H__
#define __LIQUID_DOC_HTML_H__

// html gen structure
typedef struct htmlgen_s * htmlgen;

#define HTMLGEN_BUFFER_LENGTH   512
struct htmlgen_s {
    // file names
    char filename_tex[128];
    char filename_html[128];
    char filename_eqmk[128];

    // file pointers
    FILE * fid_tex;
    FILE * fid_html;
    FILE * fid_eqmk;

    // counters
    unsigned int equation_id;
    unsigned int chapter;
    unsigned int section;
    unsigned int subsection;
    unsigned int subsubsection;

    //
    char buffer[HTMLGEN_BUFFER_LENGTH+1];
    unsigned int buffer_size;     // number of elements in buffer

    // list of tokens to escape mode

#if 0
    // mode
    enum {
        HTMLGEN_MODE_SEEK_DOC=0,    // look for \begin{document}
        HTMLGEN_MODE_PARSE,         // parse
        HTMLGEN_MODE_GET_ENV,       // inside environment : \begin{
        HTMLGEN_MODE_GET_
#endif
};

// doc-html-gen function
typedef void (*htmlgen_parse_func) (htmlgen _q);

// token structure
typedef struct {
    char * token;
    htmlgen_parse_func func;
} htmlgen_token_s;

// main parsing function
void htmlgen_parse_latex_file(char * _filename_tex,
                              char * _filename_html,
                              char * _filename_eqmk);

htmlgen htmlgen_create(char * _filename_tex,
                       char * _filename_html,
                       char * _filename_eqmk);
void htmlgen_open_files(htmlgen _q);
void htmlgen_close_files(htmlgen _q);
void htmlgen_destroy(htmlgen _q);

// html output
void htmlgen_html_write_header(htmlgen _q);
void htmlgen_html_write_footer(htmlgen _q);

// equation files
void htmlgen_add_equation(htmlgen _q, char * _eqn, int _inline);

// buffering methods
void htmlgen_buffer_produce(htmlgen _q);
void htmlgen_buffer_consume(htmlgen _q,
                            unsigned int _n);
void htmlgen_buffer_consume_eol(htmlgen _q);

// batch parsing methods
void htmlgen_strip_preamble(htmlgen _q);

// token methods
int htmlgen_get_token(htmlgen _q,
                      htmlgen_token_s * _token_tab,
                      unsigned int _num_tokens,
                      unsigned int * _token_index,
                      unsigned int * _len);
void htmlgen_token_parse_begin(htmlgen _q);
void htmlgen_token_parse_end(htmlgen _q);
//void htmlgen_token_parse_eqn_begin(htmlgen _q);
//void htmlgen_token_parse_eqn_end(htmlgen _q);
void htmlgen_token_parse_comment(htmlgen _q);
void htmlgen_token_parse_document(htmlgen _q);
void htmlgen_token_parse_section(htmlgen _q);
void htmlgen_token_parse_subsection(htmlgen _q);
void htmlgen_token_parse_subsubsection(htmlgen _q);
void htmlgen_token_parse_figure(htmlgen _q);
void htmlgen_token_parse_tabular(htmlgen _q);
void htmlgen_token_parse_enumerate(htmlgen _q);
void htmlgen_token_parse_itemize(htmlgen _q);

#endif // __LIQUID_DOC_HTML_H__

