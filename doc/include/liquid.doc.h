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

#ifndef __LIQUID_DOC_H__
#define __LIQUID_DOC_H__

#include <stdio.h>
#include <complex.h>
#include <stdbool.h>
#include <liquid/liquid.h>

// default gnuplot colors
#define LIQUID_DOC_COLOR_GRID   "#cccccc"   // very light gray
#define LIQUID_DOC_COLOR_GRAY   "#999999"   // light gray
#define LIQUID_DOC_COLOR_BLUE   "#004080"   // blue
#define LIQUID_DOC_COLOR_GREEN  "#008040"   // green
#define LIQUID_DOC_COLOR_RED    "#800000"   // dark red
#define LIQUID_DOC_COLOR_PURPLE "#400040"   // dark puple

// power spectral density window
typedef enum {
    LIQUID_DOC_PSDWINDOW_NONE=0,
    LIQUID_DOC_PSDWINDOW_HANN,
    LIQUID_DOC_PSDWINDOW_HAMMING
} liquid_doc_psdwindow;

// compute power spectral density (complex float input)
void liquid_doc_compute_psdcf(float complex * _x,
                              unsigned int _n,
                              float complex * _X,
                              unsigned int _nfft,
                              liquid_doc_psdwindow _wtype,
                              bool _normalize);

// compute power spectral density (float input)
void liquid_doc_compute_psdf(float * _x,
                             unsigned int _n,
                             float complex * _X,
                             unsigned int _nfft,
                             liquid_doc_psdwindow _wtype,
                             bool _normalize);


// Compute spectral response
void liquid_doc_freqz(float * _b,
                      unsigned int _nb,
                      float * _a,
                      unsigned int _na,
                      unsigned int _nfft,
                      float complex * _H);

// 
// html documentation generation
//

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

    // equation identifier
    unsigned int equation_id;

    //
    char buffer[HTMLGEN_BUFFER_LENGTH+1];
    unsigned int buffer_size;     // number of elements in buffer

    // list of tokens to escape mode
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

#endif // __LIQUID_DOC_H__

