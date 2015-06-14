/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// 
// benchmark_compare.c
// 
// compare benchmark runs
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// print usage/help message
void usage()
{
    printf("benchmark_compare [old_benchmark] [new_benchmark]\n");
}

// define benchmark_t
struct benchmark_t {
    //unsigned int id;
    char name[64];
    //unsigned int name_len;
    //unsigned int num_trials;
    //float extime;
    //float rate;
    float cycles_per_trial;

    // link to other benchmark
    struct benchmark_t * link;
};

struct benchlist_s {
    struct benchmark_t * benchmarks;
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

void benchlist_link(benchlist _q0,
                    benchlist _q1);

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
    if (argc != 3) {
        usage();
        exit(1);
    }

    // parse old benchmarks
    benchlist benchmarks_old = benchlist_create();
    parse_file(argv[1], benchmarks_old);
    //benchlist_print(benchmarks_old);

    // parse new benchmarks
    benchlist benchmarks_new = benchlist_create();
    parse_file(argv[2], benchmarks_new);
    //benchlist_print(benchmarks_new);

    // link benchmarks and print results
    benchlist_link(benchmarks_old, benchmarks_new);
    benchlist_print(benchmarks_old);

    // destroy benchmark lists
    benchlist_destroy(benchmarks_old);
    benchlist_destroy(benchmarks_new);

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

#if 0
void benchlist_print(benchlist _q)
{
    unsigned int i;
    printf("benchlist [%u]:\n", _q->num_benchmarks);
    for (i=0; i<_q->num_benchmarks; i++) {
        printf("  - %-32s : %14.2f", _q->benchmarks[i].name,
                                     _q->benchmarks[i].cycles_per_trial);
        if (_q->benchmarks[i].link == NULL) {
            printf("\n");
        } else {
            // print delta
            float cycles_old = _q->benchmarks[i].cycles_per_trial;
            float cycles_new = _q->benchmarks[i].link->cycles_per_trial;
            float delta = (cycles_new - cycles_old)/cycles_old;

            // print static value
            printf(" : %12.3f   %8.2f %%\n", cycles_new, 100*delta);
        }

    }
}
#else
void benchlist_print(benchlist _q)
{
    unsigned int i;
    printf("benchlist [%u]:\n", _q->num_benchmarks);
    for (i=0; i<_q->num_benchmarks; i++) {
        if (_q->benchmarks[i].link == NULL)
            continue;

        printf("  - %-28s ", _q->benchmarks[i].name);
            // print delta
            float cycles_old = _q->benchmarks[i].cycles_per_trial;
            float cycles_new = _q->benchmarks[i].link->cycles_per_trial;
            float speedup    = (cycles_old + 1e-6f) / (cycles_new + 1e-6f);

            // print bar graph style response
            unsigned int num_spaces = 20;
            unsigned int num_hashes = (unsigned int) (0.4*fabsf(log2f(speedup)) * num_spaces);
            if (num_hashes > num_spaces) num_hashes = num_spaces;
            unsigned int j;
            if (speedup > 1.0) {
                // newer is better
                for (j=0; j<num_spaces; j++) printf(".");
                printf("[%7.3f]", speedup);
                for (j=0; j<num_hashes; j++) printf("#");
                for (   ; j<num_spaces; j++) printf(".");
                printf("\n");
            } else {
                // older is better
                for (j=0; j<num_spaces-num_hashes; j++) printf(".");
                for (   ; j<num_spaces; j++) printf("#");
                printf("[%7.3f]", speedup);
                for (j=0; j<num_spaces; j++) printf(".");
                printf("\n");
            }
        }
}
#endif

void benchlist_append(benchlist _q,
                      char * _name,
                      float _cycles_per_trial)
{
    // TODO : check for uniqueness
    unsigned int i;
    for (i=0; i<_q->num_benchmarks; i++) {
        if ( strncmp(_q->benchmarks[i].name, _name, 64) == 0) {
            fprintf(stderr,"warning: cannot append benchmark '%s'; benchmark already exists\n", _name);
            return;
        }
    }

    // re-allocate memory array
    _q->num_benchmarks++;
    _q->benchmarks = (struct benchmark_t*)realloc(_q->benchmarks, (_q->num_benchmarks)*sizeof(struct benchmark_t));

    //printf("appending '%s' (%f)\n", _name, _cycles_per_trial);

    // 
    // append new element
    //

    // copy name
    strncpy(_q->benchmarks[_q->num_benchmarks-1].name, _name, 64);

    // copy properties
    _q->benchmarks[_q->num_benchmarks-1].cycles_per_trial = _cycles_per_trial;

    // set link to NULL
    _q->benchmarks[_q->num_benchmarks-1].link = NULL;
}

void benchlist_link(benchlist _q0,
                    benchlist _q1)
{
    // link nodes
    unsigned int i;
    unsigned int j;
    for (i=0; i<_q0->num_benchmarks; i++) {
        for (j=0; j<_q1->num_benchmarks; j++) {
            if ( strncmp(_q0->benchmarks[i].name, _q1->benchmarks[j].name, 64)==0 ) {
                // match found; link nodes
                _q0->benchmarks[i].link = &_q1->benchmarks[j];
                _q1->benchmarks[j].link = &_q0->benchmarks[i];
            }
        }
    }
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

