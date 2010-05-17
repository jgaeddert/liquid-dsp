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

    FILE * fid_index = NULL;    // html index file
    FILE * fid_eqnidx = NULL;   // equation index
    FILE * fid_eqn = NULL;      // equation

    // html index file
    fid_index = fopen("html/index.html","w");
    if (!fid_index) {
        fprintf(stderr,"error, could not open html/index.html for writing\n");
        exit(1);
    }
    fprintf(fid_index,"liquid documentation\n");
    fclose(fid_index);

    // equations makefile
    fid_eqnidx = fopen("html/equations.mk","w");
    if (!fid_eqnidx) {
        fprintf(stderr,"error, could not open html/equations.mk for writing\n");
        exit(1);
    }
    fprintf(fid_eqnidx,"html/eqn/eqn0001.png : html/eqn/eqn0001.tex\n");
    fclose(fid_eqnidx);

    // equation
    fid_eqn = fopen("html/eqn/eqn0001.tex","w");
    if (!fid_eqn) {
        fprintf(stderr,"error, could not open html/eqn/eqn0001.tex for writing\n");
        exit(1);
    }
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
    fprintf(fid_eqn,"\\[ y = \\int_0^\\infty \\gamma^2 \\cos(x) dx \\]\n");
    fprintf(fid_eqn,"\\end{document}\n");
    fclose(fid_eqn);
}

