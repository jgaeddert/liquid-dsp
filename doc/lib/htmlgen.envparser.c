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
// doc-html-gen environment parser methods
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.doc.html.h"

void htmlgen_env_parse_unknown(htmlgen _q,
                               char * _envarg)
{
    fprintf(stderr,"warning: unknown/unsupported environment '%s'\n", _envarg);

    if (strlen(_envarg) > 50) {
        fprintf(stderr,"error: htmlgen_env_parse_unknown(), _envarg too long!\n");
        exit(1);
    }

    // generate escape sequence '\end{envarg}'
    char escape[64] = "\\end{";
    strcat(escape,_envarg);
    strcat(escape,"}");

    printf("escape : '%s'\n", escape);

    // start environment
    fprintf(_q->fid_html,"<pre class=\"unknown\">\n");
    fprintf(_q->fid_html,"warning: unknown/unsupported environment '%s'\n\n", _envarg);

    // configure escape token
    htmlgen_token_s escape_token = {escape, NULL};

    // read tokens until '\end{envarg}' is found
    int token_found = 0;
    unsigned int token_index = 0;
    unsigned int n = 0;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!token_found && _q->buffer_size > 0) {
        //printf("searching for '%s'\n", escape_token.token);

        token_found = htmlgen_get_token(_q, &escape_token, 1, &token_index, &n);

        htmlgen_buffer_dump(_q, _q->fid_html, n);
        htmlgen_buffer_dump(_q, stdout, n);

        // consume buffer
        htmlgen_buffer_consume(_q, n);

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (token_found) {
        // consume buffer through token
        htmlgen_buffer_consume(_q, strlen(escape) );
    } else if (feof(_q->fid_tex) ) {
        fprintf(stderr,"error: htmlgen_env_parse_unknown(), premature EOF\n");
        exit(1);
    }

    // end environment
    fprintf(_q->fid_html,"</pre>\n");

    printf("done with unknown environment\n");
}


void htmlgen_env_parse_itemize(htmlgen _q)
{
    // start un-orderd list
    fprintf(_q->fid_html,"<ul>\n");

    // read tokens until '\end{itemize}' is found, each
    // '\item' token invokes a new list item
    htmlgen_env_parse_list(_q, "\\end{itemize}");

    // end un-ordered list
    fprintf(_q->fid_html,"</ul>\n");
}


void htmlgen_env_parse_enumerate(htmlgen _q)
{
    // start orderd list
    fprintf(_q->fid_html,"<ol>\n");

    // read tokens until '\end{enumerate}' is found, each
    // '\item' token invokes a new list item
    htmlgen_env_parse_list(_q, "\\end{enumerate}");

    // end un-ordered list
    fprintf(_q->fid_html,"</ol>\n");
}


void htmlgen_env_parse_eqn(htmlgen _q)
{
    printf("beginning short equation environment parser...\n");
    htmlgen_env_parse_equation_help(_q, "\\]", 0);
    printf("ending short equation environment parser.\n");
}

void htmlgen_env_parse_inline_eqn(htmlgen _q)
{
    printf("beginning inline equation environment parser...\n");
    htmlgen_env_parse_equation_help(_q, "$", 1);
    printf("ending inline equation environment parser.\n");
}

void htmlgen_env_parse_equation(htmlgen _q)
{
    printf("beginning long equation environment parser...\n");
    htmlgen_env_parse_equation_help(_q, "\\end{equation}", 0);
    printf("ending long equation environment parser.\n");
}

void htmlgen_env_parse_verbatim(htmlgen _q)
{
    printf("beginning verbatim environment parser...\n");
    char escape[] = "\\end{verbatim}";

    // start environment
    fprintf(_q->fid_html,"<pre class=\"verbatim\">\n");

    // configure escape token
    htmlgen_token_s escape_token = {escape, NULL};

    // read tokens until escape token is found
    int token_found = 0;
    unsigned int token_index = 0;
    unsigned int n = 0;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!token_found && _q->buffer_size > 0) {
        //printf("searching for '%s'\n", escape_token.token);

        token_found = htmlgen_get_token(_q, &escape_token, 1, &token_index, &n);

        htmlgen_buffer_dump(_q, _q->fid_html, n);
        htmlgen_buffer_dump(_q, stdout, n);

        // consume buffer
        htmlgen_buffer_consume(_q, n);

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (token_found) {
        // consume buffer through token
        htmlgen_buffer_consume(_q, strlen(escape) );
    } else if (feof(_q->fid_tex) ) {
        fprintf(stderr,"error: htmlgen_env_parse_verbatim(), premature EOF\n");
        exit(1);
    }

    // end environment
    fprintf(_q->fid_html,"</pre>\n");

    printf("done with verbatim environment\n");
}

void htmlgen_env_parse_figure(htmlgen _q)
{
    printf("beginning figure environment parser...\n");
    char escape[] = "\\end{figure}";

    // start environment
    fprintf(_q->fid_html,"<img src=\"fig/fig%u.png\" />\n", _q->figure_id);
    _q->figure_id++;

    fprintf(_q->fid_figs,"\\newpage\n");
    fprintf(_q->fid_figs,"\\begin{figure}\n");

    // configure escape token
    htmlgen_token_s escape_token = {escape, NULL};

    // read tokens until escape token is found
    int token_found = 0;
    unsigned int token_index = 0;
    unsigned int n = 0;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!token_found && _q->buffer_size > 0) {
        //printf("searching for '%s'\n", escape_token.token);

        token_found = htmlgen_get_token(_q, &escape_token, 1, &token_index, &n);

        htmlgen_buffer_dump(_q, _q->fid_figs, n);
        htmlgen_buffer_dump(_q, stdout, n);

        // consume buffer
        htmlgen_buffer_consume(_q, n);

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (token_found) {
        // consume buffer through token
        htmlgen_buffer_consume(_q, strlen(escape) );
    } else if (feof(_q->fid_tex) ) {
        fprintf(stderr,"error: htmlgen_env_parse_figure(), premature EOF\n");
        exit(1);
    }

    fprintf(_q->fid_figs,"\\end{figure}\n");
    printf("ending figure environment parser.\n");
}

void htmlgen_env_parse_tabular(htmlgen _q)
{
    printf("beginning tabular environment parser...\n");

    // read buffer until \end{tabular} tag is found
    printf("ending tabular environment parser.\n");
}

void htmlgen_env_parse_tabularstar(htmlgen _q)
{
    printf("beginning tabularstar environment parser...\n");

    // read buffer until \end{tabular*} tag is found
    printf("ending tabularstar environment parser.\n");
}


void htmlgen_env_parse_table(htmlgen _q)
{
    printf("beginning table environment parser...\n");

    // read buffer until \end{table} tag is found
    printf("ending table environment parser.\n");
}

// 
// helper methods
//

#define HTMLGEN_ENV_LIST_NUM_TOKENS (9)
void htmlgen_env_parse_list(htmlgen _q, char * _end)
{
    // set up tokens
    htmlgen_token_s token_tab[HTMLGEN_ENV_LIST_NUM_TOKENS] = {
        {_end,               htmlgen_token_parse_null},
        {"\\item",           htmlgen_token_parse_item},
        {"{\\tt",            htmlgen_token_parse_tt},
        {"{\\it",            htmlgen_token_parse_it},
        {"{\\em",            htmlgen_token_parse_em},
        {"\\_",              htmlgen_token_parse_underscore},
        {"\\{",              htmlgen_token_parse_leftbrace},
        {"\\}",              htmlgen_token_parse_rightbrace},
        {"$",                htmlgen_env_parse_inline_eqn}
    };

    // look for tokens and execute
    int escape_token_found = 0; // escape token found flag
    int token_found = 0;        // any token found flag
    unsigned int token_index = 0;
    unsigned int n = 0;

    // fill buffer
    htmlgen_buffer_produce(_q);

    while (!escape_token_found && _q->buffer_size > 0) {
        token_found = htmlgen_get_token(_q,
                                        token_tab,
                                        HTMLGEN_ENV_LIST_NUM_TOKENS,
                                        &token_index,
                                        &n);

        if (token_found) {
            // check for escape token, e.g. '\end{itemize}'
            if (token_index == 0)
                escape_token_found = 1;

            // write output to html file up to this point
            htmlgen_buffer_dump(_q, _q->fid_html, n);

            // consume buffer through token
            unsigned int token_len = strlen(token_tab[token_index].token);
            htmlgen_buffer_consume(_q, n + token_len);

            // execute token-specific function
            token_tab[token_index].func(_q);
        } else {
            // no token found; write output to html file
            htmlgen_buffer_dump_all(_q, _q->fid_html);

            // clear buffer
            htmlgen_buffer_consume_all(_q);
        }

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (escape_token_found) {
        
    } else if (feof(_q->fid_tex)) {
        // token wasn't found, but EOF was reached
        fprintf(stderr,"error: htmlgen_env_parse_list(), premature EOF\n");
        exit(1);
    }

}

#define HTMLGEN_ENV_EQUATION_NUM_TOKENS (2)
void htmlgen_env_parse_equation_help(htmlgen _q,
                                     char * _escape,
                                     int _inline)
{
    printf("************ adding equation %u\n", _q->equation_id);
    fprintf(_q->fid_eqns,"\\newpage\n");
    fprintf(_q->fid_eqns,"\\[");

    // set up tokens
    htmlgen_token_s token_tab[HTMLGEN_ENV_EQUATION_NUM_TOKENS] = {
        {_escape,            htmlgen_token_parse_null},
        {"\\label{",         htmlgen_token_parse_label}
    };

    // look for tokens and execute
    int escape_token_found = 0; // escape token found flag
    int token_found = 0;        // any token found flag
    unsigned int token_index = 0;
    unsigned int n = 0;

    // fill buffer
    htmlgen_buffer_produce(_q);
    while (!escape_token_found && _q->buffer_size > 0) {
        //printf("searching for '%s'...\n", _escape);

        token_found = htmlgen_get_token(_q,
                                        token_tab,
                                        HTMLGEN_ENV_EQUATION_NUM_TOKENS,
                                        &token_index,
                                        &n);
        if (token_found) {
            // check for escape token, e.g. '\end{itemize}'
            if (token_index == 0)
                escape_token_found = 1;

            // execute token-specific function
            token_tab[token_index].func(_q);
        }

        // dump to output file
        htmlgen_buffer_dump(_q, _q->fid_eqns, n);

        // consume buffer
        htmlgen_buffer_consume(_q, n);

        // fill buffer
        htmlgen_buffer_produce(_q);
    }

    if (escape_token_found) {
        // consume through token
        unsigned int token_len = strlen(token_tab[token_index].token);
        htmlgen_buffer_consume(_q, token_len);
    } else if (feof(_q->fid_tex)) {
        fprintf(stderr,"error: htmlgen_env_parse_equation_help(), premature EOF\n");
        exit(1);
    }

    fprintf(_q->fid_eqns,"\\]\n");

    // insert equation into html file
    if (!_inline) fprintf(_q->fid_html,"<p align=\"center\">\n");
    fprintf(_q->fid_html,"<img src=\"eqn/eqn%u.png\" />\n", _q->equation_id);
    if (!_inline) fprintf(_q->fid_html,"</p>\n");

    // increment equation id
    _q->equation_id++;
}

