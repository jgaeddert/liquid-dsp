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

#define NAME_LEN            (256)
#define BENCHMARK_VERSION   "0.3.0"

struct benchmark_s {
    char name[NAME_LEN];
    struct package_s * package;
};

struct package_s {
    char name[NAME_LEN];
    struct benchmark_s * benchmarks;
    unsigned int num_benchmarks;
};

struct benchmarkgen_s {
    unsigned int num_packages;
    struct package_s * packages;
};

// create benchmark generator object
benchmarkgen benchmarkgen_create()
{
    benchmarkgen q = (benchmarkgen) malloc(sizeof(struct benchmarkgen_s));

    q->num_packages = 0;
    q->packages = NULL;

    // add defulat null package/benchmark
    benchmarkgen_addpackage(q, "null");
    benchmarkgen_addbenchmark(q, "null", "nullbench");

    return q;
}

void benchmarkgen_destroy(benchmarkgen _q)
{
    // free all internal benchmarks
    //if (_q->benchmarks != NULL) free(_q->benchmarks);

    // free all internal packages
    if (_q->packages != NULL) free(_q->packages);

    // free main object memory
    free(_q);
}

void benchmarkgen_parse(benchmarkgen _q,
                        char * _filename)
{
    fprintf(stderr,"benchmarkgen_parse('%s')...\n", _filename);

    // parse filename...
    benchmarkgen_parsefilename(_q, _filename);

    // parse actual file...
    benchmarkgen_parsefile(_q, _filename);
}

void benchmarkgen_print(benchmarkgen _q)
{
    unsigned int i;
    unsigned int j;
    unsigned int n;

    // count total number of benchmarks
    unsigned int num_benchmarks = 0;
    for (i=0; i<_q->num_packages; i++)
        num_benchmarks += _q->packages[i].num_benchmarks;

    // print header
    printf("// auto-generated file, do not edit\n\n");
    
    printf("#ifndef __LIQUID_BENCHMARK_INCLUDE_H__\n");
    printf("#define __LIQUID_BENCHMARK_INCLUDE_H__\n\n");

    printf("#define BENCHMARK_VERSION \"%s\"\n\n", BENCHMARK_VERSION);

    printf("// define arguments pre-processor directive\n");
    printf("#define BENCHMARK_ARGS                 \\\n");
    printf("   struct rusage * _start,             \\\n");
    printf("   struct rusage * _finish,            \\\n");
    printf("   unsigned long int * _num_iterations\n\n");

    printf("// number of packages\n");
    printf("#define NUM_PACKAGES (%u)\n\n", _q->num_packages);

    printf("// number of benchmarks\n");
    printf("#define NUM_BENCHMARKS (%u)\n\n", num_benchmarks);

    printf("// function declarations\n");
    for (i=0; i<_q->num_packages; i++) {
        struct package_s * p = &_q->packages[i];
        for (j=0; j<p->num_benchmarks; j++)
            printf("void benchmark_%s(BENCHMARK_ARGS);\n", p->benchmarks[j].name);
    }
    printf("\n");


    printf("// array of benchmarks\n");
    printf("bench_t benchmarks[NUM_BENCHMARKS] = {\n");
    n=0;
    for (i=0; i<_q->num_packages; i++) {
        struct package_s * p = &_q->packages[i];
        for (j=0; j<p->num_benchmarks; j++) {
            printf("    {%4u, &benchmark_%s,\"%s\",0,0,0,0,0}",
                n,
                p->benchmarks[j].name,
                p->benchmarks[j].name);
            if ( n < num_benchmarks-1 )
                printf(",");
            printf("\n");
            n++;
        }
    }
    printf("};\n\n");

    n=0;
    printf("// array of packages\n");
    printf("package_t packages[NUM_PACKAGES] = {\n");
    for (i=0; i<_q->num_packages; i++) {
        printf("    {%4u, %4u, %4u, \"%s\"}",
            i,
            n,
            _q->packages[i].num_benchmarks,
            _q->packages[i].name);
        if (i<_q->num_packages-1)
            printf(",");
        printf("\n");

        n += _q->packages[i].num_benchmarks;
    }
    printf("};\n\n");

    printf("#endif // __LIQUID_BENCHMARK_INCLUDE_H__\n\n");

}



//
// internal methods
//

void benchmarkgen_parsefilename(benchmarkgen _q,
                                char * _filename)
{
    fprintf(stderr,"benchmarkgen_parsefilename('%s')...\n", _filename);
    char substr[NAME_LEN];
    char _basename[NAME_LEN];
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
    //printf("//  i0 : %d\n", i0);

    // try to strip out tag: "_benchmark.h"
    subptr = strrchr( _filename, tag[0] );
    if (subptr == NULL) {
        fprintf(stderr,"error: benchmarkgen_parsefilename('%s'), tag not found\n", _filename);
        exit(1);
    } else {
        i1 = subptr - _filename;
        //printf("  i1 : %d\n", i1);
    }

    // ensure the last occurrence of tag is not in the path name
    if (i0 >= i1) {
        fprintf(stderr,"error: benchmarkgen_parsefilename('%s'), invalid path name\n", _filename);
        exit(1);
    }
    
    // ensure tag is valid
    //strncpy(substr,&_filename[i1],NAME_LEN);
    //printf("  comparing %s with %s\n", tag, substr);
    if (strncmp(tag,&_filename[i1],strlen(tag)) != 0 ) {
        fprintf(stderr,"error: benchmarkgen_parsefilename('%s'), invalid tag (comparison failed)\n", _filename);
        exit(1);
    } else {
        //printf("  comparison passed!\n");
    }

    // copy base name
    strncpy( substr, _filename+i0, i1-i0 );
    // add null character to end
    substr[i1-i0] = '\0';
    //printf("base: \"%s\"\n", substr);

    strncpy(_basename, substr, NAME_LEN);

    //printf("// basename : %s\n", _basename);

}

void benchmarkgen_parsefile(benchmarkgen _q,
                            char * _filename)
{
    fprintf(stderr,"benchmarkgen_parsefile('%s')...\n", _filename);
    // try to open file...
    FILE * fid = fopen(_filename,"r");
    if (!fid) {
        fprintf(stderr,"error: benchmarkgen_parsefile('%s'), could not open file for reading\n", _filename);
        exit(1);
    }

    // parse file...

    // close file
    fclose(fid);
}

void benchmarkgen_addpackage(benchmarkgen _q,
                             char * _package_name)
{
    // increase package size
    _q->num_packages++;

    // re-allocate memory for packages
    _q->packages = (struct package_s *) realloc(_q->packages,
                                                _q->num_packages*sizeof(struct package_s));

    // initialize new package
    strncpy(_q->packages[_q->num_packages-1].name,
            _package_name,
            NAME_LEN);

    // initialize number of benchmarks
    _q->packages[_q->num_packages-1].num_benchmarks = 0;
}

void benchmarkgen_addbenchmark(benchmarkgen _q,
                               char * _package_name,
                               char * _benchmark_name)
{
    // first validate that package exists
    unsigned int i;
    int package_found = 0;
    for (i=0; i<_q->num_packages; i++) {
        if ( strcmp(_q->packages[i].name, _package_name)==0 ) {
            package_found = 1;
            break;
        }
    }

    if (!package_found) {
        fprintf(stderr,"error: benchmarkgen_addbenchmark(), unknown package '%s'\n", _package_name);
        exit(1);
    }

    struct package_s * p = &_q->packages[i];

    // increase benchmark size
    p->num_benchmarks++;

    // re-allocate memory for benchmarks
    p->benchmarks = (struct benchmark_s *) realloc(p->benchmarks,
                                                   p->num_benchmarks*sizeof(struct benchmark_s));

    // initialize new benchmark
    strncpy(p->benchmarks[p->num_benchmarks-1].name,
            _benchmark_name,
            NAME_LEN);

    // associate benchmark with package
    //_q->benchmarks[_q->num_benchmarks-1].package = &_q->packages[i];
}

