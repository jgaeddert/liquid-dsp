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
}

void htmlgen_env_parse_inline_eqn(htmlgen _q)
{
    printf("beginning inline equation environment parser...\n");
    htmlgen_env_parse_equation_help(_q, "$", 1);
}

void htmlgen_env_parse_equation(htmlgen _q)
{
    printf("beginning long equation environment parser...\n");
    htmlgen_env_parse_equation_help(_q, "\\end{equation}", 0);
}

void htmlgen_env_parse_verbatim(htmlgen _q)
{
    printf("beginning verbatim environment parser...\n");

    fprintf(_q->fid_html,"<tt>\n");

    // read buffer until \end{verbatim} tag is found


    fprintf(_q->fid_html,"</tt>\n");
}

void htmlgen_env_parse_figure(htmlgen _q)
{
    printf("beginning figure environment parser...\n");

    // read buffer until \end{figure} tag is found

}

void htmlgen_env_parse_tabular(htmlgen _q)
{
    printf("beginning tabular environment parser...\n");

    // read buffer until \end{tabular} tag is found
}

void htmlgen_env_parse_tabularstar(htmlgen _q)
{
    printf("beginning tabularstar environment parser...\n");

    // read buffer until \end{tabular*} tag is found
}


void htmlgen_env_parse_table(htmlgen _q)
{
    printf("beginning table environment parser...\n");

    // read buffer until \end{table} tag is found
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

    while (!escape_token_found) {
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
    }
}

#define HTMLGEN_ENV_EQUATION_NUM_TOKENS (2)
void htmlgen_env_parse_equation_help(htmlgen _q,
                                     char * _escape,
                                     int _inline)
{
    printf("************ adding equation %u\n", _q->equation_id);

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
    fprintf(fid_eqn,"\\[");

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

    while (!escape_token_found) {
        // fill buffer
        htmlgen_buffer_produce(_q);

        token_found = htmlgen_get_token(_q,
                                        token_tab,
                                        HTMLGEN_ENV_EQUATION_NUM_TOKENS,
                                        &token_index,
                                        &n);

        if (token_found) {
            // check for escape token, e.g. '\end{itemize}'
            if (token_index == 0)
                escape_token_found = 1;

            // write output to equation file up to this point
            htmlgen_buffer_dump(_q, fid_eqn, n);

            // consume buffer through token
            unsigned int token_len = strlen(token_tab[token_index].token);
            htmlgen_buffer_consume(_q, n + token_len);

            // execute token-specific function
            token_tab[token_index].func(_q);
        } else {
            // no token found; write output to equation file
            htmlgen_buffer_dump_all(_q, fid_eqn);

            // clear buffer
            htmlgen_buffer_consume_all(_q);
        }
    }

    fprintf(fid_eqn,"\\]\n");

    // write footer
    fprintf(fid_eqn,"\\end{document}\n");

    // close file
    fclose(fid_eqn);

    // insert equation into html file
    if (!_inline) fprintf(_q->fid_html,"<p align=\"center\">\n");
    fprintf(_q->fid_html,"<img src=\"eqn/eqn%.4u.png\" />\n", _q->equation_id);
    if (!_inline) fprintf(_q->fid_html,"</p>\n");

    // add equation to makefile: target collection
    fprintf(_q->fid_eqmk,"\\\n\t%s", filename_eqn);

    // increment equation id
    _q->equation_id++;

}

