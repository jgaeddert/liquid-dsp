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
// doc-html-gen token parser methods
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.doc.html.h"

#define HTMLGEN_TOKEN_DEBUG (0)

int htmlgen_get_token(htmlgen _q,
                      htmlgen_token_s * _token_tab,
                      unsigned int _num_tokens,
                      unsigned int * _token_index,
                      unsigned int * _len)
{
    int d, dmin=0;
    unsigned int i, index=0;
    char * loc = NULL;
    int token_found = 0;

    // search for next token
    for (i=0; i<_num_tokens; i++) {
        loc = strstr( _q->buffer, _token_tab[i].token );

#if HTMLGEN_TOKEN_DEBUG
        printf("  %20s : ", _token_tab[i].token);
        if (loc == NULL) {
            printf("-\n");
            continue;
        } else {
            d = (int) (loc - _q->buffer);
            printf("%d\n", d);
        }
#else
        if (loc == NULL)
            continue;
        else
            d = (int) (loc - _q->buffer);
#endif

        if (d < dmin || token_found == 0) {
            dmin = d;
            index = i;
        }

        // set flag to indicate at least one token has been found
        token_found = 1;
    }

    // set return values
    *_token_index = index;
    *_len = dmin;

    // return token found flag
    return token_found;
}

// parse argument of \begin{...}, \chapter{...}, etc.
unsigned int htmlgen_token_parse_envarg(htmlgen _q, char * _arg)
{
    // search for '}'
    unsigned int n = strcspn(_q->buffer, "}");

    // check length
    if (n >= 256) {
        fprintf(stderr,"error: htmlgen_token_parse_envarg(), invalid environment\n");
        exit(1);
    }

    // copy environment tag in '{' and '}'
    // TODO : strip white space
    memmove(_arg, _q->buffer, n);
    _arg[n] = '\0';

    return n;
}

void htmlgen_token_parse_comment(htmlgen _q)
{
    // consume buffer up until end-of-line
    htmlgen_buffer_consume_eol(_q);
}

// '\begin{'
void htmlgen_token_parse_begin(htmlgen _q)
{
    printf("parsing '\\begin{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char environment[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, environment);
    
    printf("environment '%s'\n", environment);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // TODO : call specific routine here
}

// '\chapter{'
void htmlgen_token_parse_chapter(htmlgen _q)
{
    printf("parsing '\\chapter{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char name[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, name);
    
    printf("name '%s'\n", name);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // TODO : look for \label{

    // increment chapter, reset sub-chapter counters
    _q->chapter++;
    _q->section = 1;
    _q->subsection = 1;
    _q->subsubsection = 1;

    // print heading to html file
    fprintf(_q->fid_html,"<h1>%d %s</h1>\n", _q->chapter, name);
}


// '\section{'
void htmlgen_token_parse_section(htmlgen _q)
{
    printf("parsing '\\section{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char name[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, name);
    
    printf("name '%s'\n", name);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // TODO : look for \label{

    // increment section, reset sub-counters
    _q->section++;
    _q->subsection = 1;
    _q->subsubsection = 1;

    // print heading to html file
    fprintf(_q->fid_html,"<h2>%d.%d %s</h2>\n", _q->chapter,
                                                _q->section,
                                                name);
}

// '\subsection{'
void htmlgen_token_parse_subsection(htmlgen _q)
{
    printf("parsing '\\section{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char name[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, name);
    
    printf("name '%s'\n", name);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // TODO : look for \label{

    // increment subsection, reset sub-counters
    _q->subsection++;
    _q->subsubsection = 1;

    // print heading to html file
    fprintf(_q->fid_html,"<h3>%d.%d%.d %s</h3>\n", _q->chapter,
                                                   _q->section,
                                                   _q->subsection,
                                                   name);
}

// '\subsubsection{'
void htmlgen_token_parse_subsubsection(htmlgen _q)
{
    printf("parsing '\\subsubsection{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char name[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, name);
    
    printf("name '%s'\n", name);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // TODO : look for \label{

    // increment subsubsection
    _q->subsubsection++;

    // print heading to html file
    fprintf(_q->fid_html,"<h2>%d.%d.%d.%d %s</h2>\n", _q->chapter,
                                                      _q->section,
                                                      _q->subsection,
                                                      _q->subsubsection,
                                                      name);
}

//
// antiquated methods
//

void htmlgen_token_parse_end(htmlgen _q)
{
}

void htmlgen_token_parse_document(htmlgen _q)
{
}

void htmlgen_token_parse_figure(htmlgen _q)
{
}

void htmlgen_token_parse_tabular(htmlgen _q)
{
}

void htmlgen_token_parse_enumerate(htmlgen _q)
{
}

void htmlgen_token_parse_itemize(htmlgen _q)
{
}

void htmlgen_token_parse_fail(htmlgen _q)
{
    fprintf(stderr,"*** error: htmlgen_token_parse_fail()\n");
}


