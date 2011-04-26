/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// bench/benchmarkgen.c
// 
// Benchmark generator object definition
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "benchmarkgen.h"

struct benchmarkgen_s {
    unsigned int num_packages;
    unsigned int num_benchmarks;
};

// create benchmark generator object
benchmarkgen benchmarkgen_create()
{
    benchmarkgen q = (benchmarkgen) malloc(sizeof(struct benchmarkgen_s));

    q->num_packages     = 0;
    q->num_benchmarks   = 0;

    return q;
}

void benchmarkgen_parse(benchmarkgen _q,
                        char * _filename)
{
#if 0
    char substr[256];
    char * subptr;
    int i0, i1;
    char pathsep = '/';
    const char tag[] = "_benchmark.h";

    // try to strip out path: find rightmost occurrence of pathsep
    //printf("%s\n", _filename);
    subptr = strrchr(_filename, pathsep);   // obtain index of last pathsep
    if (subptr == NULL) {
        //printf("path delimiter not found\n");
        i0 = 0;
    } else {
        i0 = subptr - _filename + 1;
    }
    //printf("  i0 : %d\n", i0);

    // try to strip out tag: "_benchmark.h"
    subptr = strrchr( _filename, tag[0] );
    if (subptr == NULL) {
        //printf("  tag not found\n");
        //return false;
        return;
    } else {
        i1 = subptr - _filename;
        //printf("  i1 : %d\n", i1);
    }

    // ensure the last occurrence of tag is not in the path name
    if (i0 >= i1) {
        //printf("invalid path name\n");
        //return false;
        return;
    }
    
    // ensure tag is valid
    //strncpy(substr,&_filename[i1],256);
    //printf("  comparing %s with %s\n", tag, substr);
    if (strncmp(tag,&_filename[i1],strlen(tag)) != 0 ) {
        //printf("  invalid tag (comparison failed)\n");
        //return false;
        return;
    } else {
        //printf("  comparison passed!\n");
    }

    // copy base name
    strncpy( substr, _filename+i0, i1-i0 );
    // add null character to end
    substr[i1-i0] = '\0';
    //printf("base: \"%s\"\n", substr);

    strncpy(_basename, substr, 256);

    //printf("\n\n");
#endif
}

void benchmarkgen_print(benchmarkgen _q)
{
    unsigned int i;

    // print header
    printf("// auto-generated file, do not edit\n\n");
    
    printf("#ifndef __LIQUID_BENCHMARK_INCLUDE_H__\n");
    printf("#define __LIQUID_BENCHMARK_INCLUDE_H__\n\n");

    printf("// header files from which this file was generated\n");
    for (i=0; i<_q->num_packages; i++) {
        //printf("#include \"%s\"\n", benchmarks[i].filename);
        printf("// #include ...\n");
    }
    printf("\n");

    printf("// number of benchmarks\n");
    printf("#define NUM_BENCHMARKS %u\n\n", _q->num_benchmarks);

    printf("bench_t benchmarks[NUM_BENCHMARKS] = {\n");
    for (i=0; i<_q->num_benchmarks; i++) {
#if 0
        printf("    {%4u, &precision_%s, &benchmark_%s, \"%s\"}",
            i,
            benchmarks[i].basename,
            benchmarks[i].basename,
            benchmarks[i].basename);
        if (i<_q->num_benchmarks-1)
            printf(",");
        printf("\n");
#endif
    }
    printf("};\n\n");

    printf("#endif // __LIQUID_BENCHMARK_INCLUDE_H__\n\n");

}

void benchmarkgen_destroy(benchmarkgen _q)
{
    free(_q);
}

