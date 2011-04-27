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
#define DEBUG_BENCHMARK     0

struct benchmark_s {
    char name[NAME_LEN];
    struct package_s * package;
};

struct package_s {
    char name[NAME_LEN];
    char filename[NAME_LEN];
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
    benchmarkgen_addpackage(q, "null", "bench/bench.c");
    benchmarkgen_addbenchmark(q, "null", "nullbench");

    return q;
}

void benchmarkgen_destroy(benchmarkgen _q)
{
    // free all internal benchmarks
    unsigned int i;
    for (i=0; i<_q->num_packages; i++) {
        if (_q->packages[i].benchmarks != NULL)
            free(_q->packages[i].benchmarks);
    }

    // free all internal packages
    if (_q->packages != NULL)
        free(_q->packages);

    // free main object memory
    free(_q);
}

void benchmarkgen_parse(benchmarkgen _q,
                        char * _filename)
{
#if DEBUG_BENCHMARK
    fprintf(stderr,"benchmarkgen_parse('%s')...\n", _filename);
#endif

    char package_name[NAME_LEN];

    // parse filename...
    benchmarkgen_parsefilename(_q, _filename, package_name);

    // parse actual file...
    benchmarkgen_parsefile(_q, _filename, package_name);
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
        printf("// %s\n", p->filename);
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

// parse filename
//  _q              :   generator object
//  _filename       :   name of file
//  _package_name   :   output package name (stripped filename)
void benchmarkgen_parsefilename(benchmarkgen _q,
                                char * _filename,
                                char * _package_name)
{
#if DEBUG_BENCHMARK
    fprintf(stderr,"benchmarkgen_parsefilename('%s')...\n", _filename);
#endif
    char * sptr;                        // pointer to base name
    char * tptr;                        // pointer to terminating tag
    char pathsep = '/';                 // path separator character
    const char tag[] = "_benchmark.h";  // terminating tag

    // try to strip out path: find rightmost occurrence of pathsep
    //printf("%s\n", _filename);
    sptr = strrchr(_filename, pathsep);   // obtain index of last pathsep
    if (sptr == NULL) {
        printf("// path delimiter not found\n");
    } else {
        sptr++;   // increment to remove path separator character
    }

    // try to strip out tag: e.g. "_benchmark.h"
    tptr = strstr( sptr, tag );
    if (tptr == NULL) {
        // TODO : handle this case differently otherwise
        fprintf(stderr,"error: benchmarkgen_parsefilename('%s'), tag '%s' not found\n", _filename, tag);
        exit(1);
    }

    // copy base name to output
    int n = tptr - sptr;
    strncpy(_package_name, sptr, n);
    _package_name[n] = '\0';
}


// parse file
//  _q              :   generator object
//  _filename       :   name of file
//  _package_name   :   input package name (stripped filename)
void benchmarkgen_parsefile(benchmarkgen _q,
                            char * _filename,
                            char * _package_name)
{
    // flag indicating if package has been added or not
    int package_added = 0;

#if DEBUG_BENCHMARK
    fprintf(stderr,"benchmarkgen_parsefile('%s')...\n", _filename);
#endif
    // try to open file...
    FILE * fid = fopen(_filename,"r");
    if (!fid) {
        fprintf(stderr,"error: benchmarkgen_parsefile('%s'), could not open file for reading\n", _filename);
        exit(1);
    }
    const char tag[] = "void benchmark_";
    //const char tag[] = "benchmark_";

    // parse file, looking for key
    char buffer[1024];      // line buffer
    char basename[1024];    // base benchmark name
    char * cptr = NULL;     // readline return value
    char * sptr = NULL;     // tag string pointer
    int cterm;              // terminating character
    unsigned int n=0;       // line number
    do {
        // increment line number
        n++;

        // read line
        cptr = fgets(buffer, 1024, fid);

        if (cptr != NULL) {
            // search for key
            sptr = strstr(cptr, tag);
            if (sptr == NULL) {
                // no value found
                continue;
            }
            // key found: find terminating character and strip out base name
            sptr += strlen(tag);    // increment string by tag length
            cterm = strcspn( sptr, " (\t\n\r" );
            if (cterm == strlen(sptr)) {
                // no terminating character found
                continue;
            }
            // copy base name
            strncpy( basename, sptr, cterm );
            basename[cterm] = '\0';
            //printf("// line %3u : '%s'\n", n, basename);
                    
            // key found: add package if not already done
            if (!package_added) {
                // TODO : add base name
                benchmarkgen_addpackage(_q, _package_name, _filename);
                package_added = 1;
            }
            benchmarkgen_addbenchmark(_q, _package_name, basename);
        }
    } while (!feof(fid));

    // close file
    fclose(fid);
}

void benchmarkgen_addpackage(benchmarkgen _q,
                             char * _package_name,
                             char * _filename)
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

    // initialize new package
    strncpy(_q->packages[_q->num_packages-1].filename,
            _filename,
            NAME_LEN);

    // initialize number of benchmarks
    _q->packages[_q->num_packages-1].num_benchmarks = 0;

    // set benchmarks pointer to NULL
    _q->packages[_q->num_packages-1].benchmarks = NULL;
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

