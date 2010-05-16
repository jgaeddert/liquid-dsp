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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "liquid.doc.h"

// parse LaTeX file
void htmlgen_parse_latex_file(char * _latex_filename)
{
    printf("parsing latex file '%s'\n", _latex_filename);

    FILE * fid = fopen("html/index.html","w");
    if (!fid) {
        fprintf(stderr,"error, could not open html/index.html for writing\n");
        exit(1);
    }
    fprintf(fid,"liquid documentation\n");

    fclose(fid);
}

