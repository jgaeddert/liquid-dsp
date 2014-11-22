/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// bench/main.c
// 
// Benchmark generator script

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmarkgen.h"

int main(int argc, char*argv[])
{
    // create benchmarkgen object
    benchmarkgen q = benchmarkgen_create();

    // parse files
    int i;
    for (i=1; i<argc; i++) {
        benchmarkgen_parse(q,argv[i]);
    }

    // print results
    benchmarkgen_print(q);

    // destroy benchmarkgen object
    benchmarkgen_destroy(q);

    return 0;
}

