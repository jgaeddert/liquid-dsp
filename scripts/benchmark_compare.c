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
// benchmark_compare.c
// 
// compare benchmark runs
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// print usage/help message
void usage()
{
    printf("usage...\n");
    printf("  u/h   : print usage\n");
}

// define benchmark_t
typedef struct {
    //unsigned int id;
    char name[64];
    //unsigned int name_len;
    //unsigned int num_trials;
    //float extime;
    //float rate;
    float cycles_per_trial;
} benchmark_t;

struct benchlist_s {
    benchmark_t * benchmarks;
    unsigned int num_benchmarks;
};

typedef struct benchlist_s * benchlist;

// 
benchlist benchlist_create();
void benchlist_destroy(benchlist _q);
void benchlist_print(benchlist _q);
void benchlist_append(benchlist _q,
                      char * _name,
                      float _cycles_per_trial);

// is line a comment?
int parse_file(const char * _filename,
               benchlist _benchmarks);

// read line from file
//  _fid    :   input file
//  _buffer :   output buffer
//  _n      :   buffer length
void readline(FILE * _fid,
              char * _buffer,
              unsigned int _n);

// is line a comment?
int line_is_comment(char * _buffer);

int main(int argc, char*argv[])
{
    char filename_old[256] = "modem.benchmark.new.out";
    char filename_new[256] = "modem.benchmark.old.out";

    int dopt;
    while((dopt = getopt(argc,argv,"uh")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u':   usage(); return 0;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            exit(-1);
        }
    }

    // parse old benchmarks
    benchlist benchmarks_old = benchlist_create();
    parse_file(filename_old, benchmarks_old);
    benchlist_print(benchmarks_old);

    // parse new benchmarks
    benchlist benchmarks_new = benchlist_create();
    parse_file(filename_new, benchmarks_new);
    benchlist_print(benchmarks_new);

    printf("done.\n");
    return 0;
}

// 
benchlist benchlist_create()
{
    benchlist q = (benchlist) malloc(sizeof(struct benchlist_s));

    // initialize internals
    q->benchmarks = NULL;
    q->num_benchmarks = 0;

    // return main object
    return q;
}

void benchlist_destroy(benchlist _q)
{
    // free internals
    if (_q->benchmarks != NULL)
        free(_q->benchmarks);

    // free main object memory
    free(_q);
}

void benchlist_print(benchlist _q)
{
    unsigned int i;
    printf("benchlist [%u]:\n", _q->num_benchmarks);
    for (i=0; i<_q->num_benchmarks; i++) {
        printf("  - %-24s : %12.3f\n", _q->benchmarks[i].name,
                                       _q->benchmarks[i].cycles_per_trial);
    }
}

void benchlist_append(benchlist _q,
                      char * _name,
                      float _cycles_per_trial)
{
    // re-allocate memory array
    _q->num_benchmarks++;
    _q->benchmarks = (benchmark_t*)realloc(_q->benchmarks, (_q->num_benchmarks)*sizeof(benchmark_t));

    //printf("appending '%s' (%f)\n", _name, _cycles_per_trial);

    // 
    // append new element
    //

    // copy name
    strncpy(_q->benchmarks[_q->num_benchmarks-1].name, _name, 64);

    // copy properties
    _q->benchmarks[_q->num_benchmarks-1].cycles_per_trial = _cycles_per_trial;
}

// is line a comment?
int parse_file(const char * _filename,
               benchlist _benchmarks)
{
    //
    FILE * fid = fopen(_filename,"r");
    if (!fid) {
        fprintf(stderr,"error: parse_file(), could not open '%s' for reading\n", _filename);
        exit(1);
    }

    printf("parsing '%s'...\n", _filename);
    char buffer[256];   // line buffer

    int id;
    char name[64];
    unsigned long int num_trials;
    float execution_time;
    float rate;
    float cycles_per_trial;

    do {
        // read line into buffer
        readline(fid, buffer, 256);

        // skip comment lines
        if (line_is_comment(buffer))
            continue;

        // scan line for results
        int results = sscanf(buffer,"%d %s %lu %f %f %f\n",
                             &id,
                             name,
                             &num_trials,
                             &execution_time,
                             &rate,
                             &cycles_per_trial);
        if (results < 6) {
            //fprintf(stderr,"warning: skipping line '%s'\n", buffer);
            continue;
        }

        // append...
        benchlist_append(_benchmarks, name, cycles_per_trial);

    } while (!feof(fid));

    // close the file
    fclose(fid);

    // return
    return 0;
}


// read line from file
//  _fid    :   input file
//  _buffer :   output buffer
//  _n      :   buffer length
void readline(FILE * _fid,
              char * _buffer,
              unsigned int _n)
{
    unsigned int i=0;
    char c;
    while ( (c = fgetc(_fid)) != EOF && c != '\n' && i < _n-1) {
        _buffer[i] = c;
        i++;
    }
    _buffer[i] = '\0';
}

// is line a comment?
int line_is_comment(char * _buffer)
{
    // read first non-whitespace character
    unsigned int i=0;
    char c;
    do {
        c = _buffer[i];
        i++;
    } while (c == ' ' || c == '\t');

    if (c == '#' || c == '\0')
        return 1;

    return 0;
}

