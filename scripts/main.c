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
// scripts/main.c
// 
// Auto-script generator test program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "autoscript.h"

void usage() {
    printf("Usage: autoscript_test DELIM TYPE [LIST OF FILES]\n");
    printf("    DELIM   :   path separation delimiter (e.g. '/')\n");
    printf("    TYPE    :   base type for parsing (e.g. \"benchmark\")\n");
    printf("    [LIST]  :   list of auto-script files, named MYPACKAGE_TYPE.h\n");
    printf("                with internal scripts \"void TYPE_MYSCRIPT(...)\"\n");
}

int main(int argc, char*argv[])
{
    //
    if (argc < 3) {
        // print help
        fprintf(stderr,"error: %s, too few arguments\n", argv[0]);
        usage();
        exit(1);
    } else {
        //printf("//  delim :   '%c'\n", argv[1][0]);
        //printf("//  type  :   \"%s\"\n", argv[2]);
    }

    // create autoscript object
    //  first argument  :   type string (e.g. "benchmark")
    //  second argument :   delimiter character (file system)
    autoscript q = autoscript_create(argv[2], argv[1][0]);

    // parse files
    int i;
    for (i=3; i<argc; i++) {
        autoscript_parse(q,argv[i]);
    }

    // print results
    autoscript_print(q);

    // destroy autoscript object
    autoscript_destroy(q);

    return 0;
}

