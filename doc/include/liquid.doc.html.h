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
    char filename_eqns[128];
    char filename_figs[128];

    // file pointers
    FILE * fid_tex;
    FILE * fid_html;
    FILE * fid_eqns;
    FILE * fid_figs;

    // counters
    unsigned int equation_id;
    unsigned int figure_id;
    unsigned int chapter;
    unsigned int section;
    unsigned int subsection;
    unsigned int subsubsection;

    //
    char buffer[HTMLGEN_BUFFER_LENGTH+1];
    unsigned int buffer_size;     // number of elements in buffer
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
                              char * _filename_eqns);

// htmlgen object functions
htmlgen htmlgen_create(char * _filename_tex,
                       char * _filename_html,
                       char * _filename_eqns);
void htmlgen_open_files(htmlgen _q);
void htmlgen_close_files(htmlgen _q);
void htmlgen_destroy(htmlgen _q);

// html output
void htmlgen_html_write_header(htmlgen _q);
void htmlgen_html_write_footer(htmlgen _q);

// equations output
void htmlgen_eqns_write_header(htmlgen _q);
void htmlgen_eqns_write_footer(htmlgen _q);

// figures output
void htmlgen_figs_write_header(htmlgen _q);
void htmlgen_figs_write_footer(htmlgen _q);

// add equation from string
void htmlgen_add_equation_string(htmlgen _q,
                                 int _inline,
                                 char * _eqn);

// buffering methods
void htmlgen_buffer_produce(htmlgen _q);
void htmlgen_buffer_consume(htmlgen _q,
                            unsigned int _n);
void htmlgen_buffer_consume_eol(htmlgen _q);
void htmlgen_buffer_consume_all(htmlgen _q);
void htmlgen_buffer_dump(htmlgen _q,
                         FILE * _fid,
                         unsigned int _n);
void htmlgen_buffer_dump_all(htmlgen _q,
                             FILE * _fid);

// batch parsing methods
void htmlgen_parse_strip_preamble(htmlgen _q, char * _filename);
void htmlgen_parse_seek_first_chapter(htmlgen _q);
void htmlgen_parse(htmlgen _q);

// 
// token methods
//

// parse buffer, searching for next token
//  _q              :   htmlgen object
//  _token_tab      :   table of tokens [size: _num_tokens x 1]
//  _num_tokens     :   length of tokens table
//  _token_index    :   index of found token in table (output)
//  _len            :   number of elements in buffer before token was found (output)
int htmlgen_get_token(htmlgen _q,
                      htmlgen_token_s * _token_tab,
                      unsigned int _num_tokens,
                      unsigned int * _token_index,
                      unsigned int * _len);

// parse argument of \begin{...}, \chapter{...}, etc.
//  * buffer contains 'xxxx}...'
//  * returns length of 'xxxx' not including lagging '}' character
//  * fails if length >= 256
unsigned int htmlgen_token_parse_envarg(htmlgen _q, char * _arg);

void htmlgen_token_parse_comment(htmlgen _q);
void htmlgen_token_parse_begin(htmlgen _q);
void htmlgen_token_parse_chapter(htmlgen _q);
void htmlgen_token_parse_section(htmlgen _q);
void htmlgen_token_parse_subsection(htmlgen _q);
void htmlgen_token_parse_subsubsection(htmlgen _q);
void htmlgen_token_parse_label(htmlgen _q);
void htmlgen_token_parse_input(htmlgen _q);
void htmlgen_token_parse_bibliography(htmlgen _q);

void htmlgen_token_parse_tt(htmlgen _q);
void htmlgen_token_parse_it(htmlgen _q);
void htmlgen_token_parse_em(htmlgen _q);
void htmlgen_token_parse_item(htmlgen _q);

// escaped characters
void htmlgen_token_parse_underscore(htmlgen _q);
void htmlgen_token_parse_leftbrace(htmlgen _q);
void htmlgen_token_parse_rightbrace(htmlgen _q);

void htmlgen_token_parse_fail(htmlgen _q);
void htmlgen_token_parse_null(htmlgen _q);

// 
// environment parsers
//

// unknown environment; parse until '\end{envarg}' is found
void htmlgen_token_parse_unknown(htmlgen _q,
                                 char * _envarg);

void htmlgen_env_parse_itemize(htmlgen _q);     // \begin{itemize} (unordered list)
void htmlgen_env_parse_enumerate(htmlgen _q);   // \begin{enumerate} (ordered list)
void htmlgen_env_parse_eqn(htmlgen _q);         // \[ (short equation environment)
void htmlgen_env_parse_inline_eqn(htmlgen _q);  // $ (inline equation environment)
void htmlgen_env_parse_equation(htmlgen _q);    // \begin{equation} (long equation environment)
void htmlgen_env_parse_verbatim(htmlgen _q);    // \begin{verbatim}
void htmlgen_env_parse_figure(htmlgen _q);      // \begin{figure}
void htmlgen_env_parse_tabular(htmlgen _q);     // \begin{tabular}
void htmlgen_env_parse_tabularstar(htmlgen _q); // \begin{tabular*}
void htmlgen_env_parse_table(htmlgen _q);       // \begin{table}

void htmlgen_env_parse_list(htmlgen _q, char * _end);
void htmlgen_env_parse_equation_help(htmlgen _q,
                                     char * _end,
                                     int _inline);

// unknown environment
void htmlgen_env_parse_unknown(htmlgen _q,
                               char * _envarg);

#endif // __LIQUID_DOC_HTML_H__

