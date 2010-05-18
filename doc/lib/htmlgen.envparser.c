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
}

void htmlgen_env_parse_equation(htmlgen _q)
{
    printf("beginning long equation environment parser...\n");
}

void htmlgen_env_parse_verbatim(htmlgen _q)
{
    printf("beginning verbatim environment parser...\n");

    fprintf(_q->fid_html,"<tt>\n");

    // read buffer until \end{verbatim} tag is found


    fprintf(_q->fid_html,"</tt>\n");
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
        {"$",                htmlgen_token_parse_inline_eqn}
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
        }
    }
}

