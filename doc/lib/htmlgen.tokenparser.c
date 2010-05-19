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
#if HTMLGEN_TOKEN_DEBUG
    printf("htmlgen_get_token() : \n");
#endif

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
    *_len = token_found ? dmin : _q->buffer_size;

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

    if (strcmp(environment,"itemize")==0) {
        // un-ordered list
        htmlgen_env_parse_itemize(_q);

    } else if (strcmp(environment,"enumerate")==0) {
        // ordered list
        htmlgen_env_parse_enumerate(_q);

    } else if (strcmp(environment,"eqn")==0) {
        // short equation
        htmlgen_env_parse_eqn(_q);

    } else if (strcmp(environment,"equation")==0) {
        // long equation
        htmlgen_env_parse_equation(_q);

    } else if (strcmp(environment,"verbatim")==0) {
        // verbatim
        htmlgen_env_parse_verbatim(_q);

#if 0
    } else if (strcmp(environment,"figure")==0) {
        // figure
        htmlgen_env_parse_figure(_q);

    } else if (strcmp(environment,"tabular")==0) {
        // tabular
        htmlgen_env_parse_tabular(_q);

    } else if (strcmp(environment,"tabular*")==0) {
        // tabular*
        htmlgen_env_parse_tabularstar(_q);

    } else if (strcmp(environment,"table")==0) {
        // table
        htmlgen_env_parse_table(_q);

    } else {
        fprintf(stderr,"error: unknown environment '%s'\n", environment);
        exit(1);
    }
#else
    } else {
        htmlgen_env_parse_unknown(_q, environment);
    }
#endif
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
    fprintf(_q->fid_html,"<h1>%u %s</h1>\n", _q->chapter, name);
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
    fprintf(_q->fid_html,"<h2>%u.%u %s</h2>\n", _q->chapter,
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
    fprintf(_q->fid_html,"<h3>%u.%u.%u %s</h3>\n", _q->chapter,
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
    fprintf(_q->fid_html,"<h2>%u.%u.%u.%u %s</h2>\n", _q->chapter,
                                                      _q->section,
                                                      _q->subsection,
                                                      _q->subsubsection,
                                                      name);
}

// '\label{'
void htmlgen_token_parse_label(htmlgen _q)
{
    printf("parsing '\\label{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char label[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, label);
    
    printf("label : '%s'\n", label);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // print label as html comment
    fprintf(_q->fid_html,"<!-- label : '%s' -->\n", label);

    printf("done with label\n");
}

// '\input{'
void htmlgen_token_parse_input(htmlgen _q)
{
    printf("parsing '\\input{...\n");

    // fill buffer
    htmlgen_buffer_produce(_q);

    char filename[256];
    unsigned int n = htmlgen_token_parse_envarg(_q, filename);
    
    printf("filename : '%s'\n", filename);

    // clear buffer through trailing '}'
    htmlgen_buffer_consume(_q, n+1);

    // print input filename as html comment
    fprintf(_q->fid_html,"<!-- input : '%s' -->\n", filename);

    // ensure filename has '.tex' at the end
    if (strncmp(filename+strlen(filename)-4,".tex",4) != 0)
        strcat(filename,".tex");

    printf("****** input file '%s'\n", filename);

    // check to see if input is listing
    if (strncmp(filename,"listings/",8)==0) {
        printf("****** listing\n");
    } else {
        // rewind current file by buffer size
        printf("rewinding input file by %u\n", _q->buffer_size);
        fseek(_q->fid_tex, -(int)(_q->buffer_size), SEEK_CUR);

        // clear input buffer
        htmlgen_buffer_consume_all(_q);
        printf("buffer size: %u\n", _q->buffer_size);

        // create new file pointer
        FILE * fid_tmp = _q->fid_tex;

        // open input file
        printf("opening input file '%s'\n", filename);
        _q->fid_tex = fopen(filename,"r");
        if (!_q->fid_tex) {
            fprintf(stderr,"error, htmlgen_token_parse_input(), could not open '%s' for reading\n", filename);
            exit(1);
        }

        // parse input file
        do {
            htmlgen_buffer_produce(_q);
            htmlgen_parse(_q);
        } while (_q->buffer_size > 0);
        //} while (_q->buffer_size > 0 && !feof(_q->fid_tex));

        // close input file
        printf("closing input file '%s'\n", filename);
        fclose(_q->fid_tex);

        // ensure buffer is empty
        htmlgen_buffer_consume_all(_q);

        // restore old file pointer
        _q->fid_tex = fid_tmp;

        // fill buffer
        htmlgen_buffer_produce(_q);
    }
}

// '\bibliography{'
void htmlgen_token_parse_bibliography(htmlgen _q)
{
    printf("bibliography...\n");
}

// 
// short methods
//

void htmlgen_token_parse_tt(htmlgen _q)
{
    // fill buffer
    htmlgen_buffer_produce(_q);

    // initialize <tt> environment
    fprintf(_q->fid_html,"<tt>");

    // print until '}' is found
    unsigned int n = strcspn(_q->buffer, "}");

    // todo : continue to read until '}' is found
    if (n==_q->buffer_size) {
        fprintf(stderr,"error: htmlgen_token_parse_tt(), '}' not found\n");
        exit(1);
    }

    // print 'n' characters of buffer to html file
    htmlgen_buffer_dump(_q, _q->fid_html, n);

    // end <tt> environment
    fprintf(_q->fid_html,"</tt>");

    // consume 'n+1' characters from buffer (including
    // trailing '}' character)
    htmlgen_buffer_consume(_q, n+1);
}

void htmlgen_token_parse_it(htmlgen _q)
{
    // fill buffer
    htmlgen_buffer_produce(_q);

    // initialize <tt> environment
    fprintf(_q->fid_html,"<em>");

    // print until '}' is found
    unsigned int n = strcspn(_q->buffer, "}");

    // todo : continue to read until '}' is found
    if (n==_q->buffer_size) {
        fprintf(stderr,"error: htmlgen_token_parse_it(), '}' not found\n");
        exit(1);
    }

    // print 'n' characters of buffer to html file
    htmlgen_buffer_dump(_q, _q->fid_html, n);

    // end <tt> environment
    fprintf(_q->fid_html,"</em>");

    // consume 'n+1' characters from buffer (including
    // trailing '}' character)
    htmlgen_buffer_consume(_q, n+1);
}

void htmlgen_token_parse_em(htmlgen _q)
{
    htmlgen_token_parse_it(_q);
}

void htmlgen_token_parse_item(htmlgen _q)
{
    printf("*** item\n");
    fprintf(_q->fid_html,"<li>");

    // TODO : figure out how to close <li> tag
}


// 
// escaped characters
//

void htmlgen_token_parse_underscore(htmlgen _q)
{
    fprintf(_q->fid_html,"_");
}

void htmlgen_token_parse_leftbrace(htmlgen _q)
{
    fprintf(_q->fid_html,"{");
}

void htmlgen_token_parse_rightbrace(htmlgen _q)
{
    fprintf(_q->fid_html,"}");
}

//
// antiquated methods
//

void htmlgen_token_parse_fail(htmlgen _q)
{
    fprintf(stderr,"*** error: htmlgen_token_parse_fail()\n");
    exit(1);
}

// do nothing
void htmlgen_token_parse_null(htmlgen _q)
{
}


