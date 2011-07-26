/*
 * Copyright (c) 2011, Joseph Gaeddert
 * Copyright (c) 2011, Virginia Polytechnic Institute & State University
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
// sparse matrices
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create _M x _N matrix, initialized with zeros
smatrix smatrix_create(unsigned int _M,
                       unsigned int _N)
{
    // create object and allocate memory
    smatrix q = (smatrix) malloc(sizeof(struct smatrix_s));
    q->M = _M;
    q->N = _N;

    unsigned int i;
    unsigned int j;

    // initialize size of each pointer list
    q->num_mlist = (unsigned int*)malloc( q->M*sizeof(unsigned int) );
    q->num_nlist = (unsigned int*)malloc( q->N*sizeof(unsigned int) );
    for (i=0; i<q->M; i++) q->num_mlist[i] = 0;
    for (j=0; j<q->N; j++) q->num_nlist[j] = 0;

    // initialize lists
    q->mlist = (unsigned short int **) malloc( q->M*sizeof(unsigned short int *) );
    q->nlist = (unsigned short int **) malloc( q->N*sizeof(unsigned short int *) );
    for (i=0; i<q->M; i++)
        q->mlist[i] = (unsigned short int *) malloc( q->num_mlist[i]*sizeof(unsigned short int) );
    for (j=0; j<q->N; j++)
        q->nlist[j] = (unsigned short int *) malloc( q->num_nlist[j]*sizeof(unsigned short int) );

    // set maximum list size
    q->max_num_mlist = 0;
    q->max_num_nlist = 0;

    // return main object
    return q;
}

// destroy object
void smatrix_destroy(smatrix _q)
{
    // free internal memory
    free(_q->num_mlist);
    free(_q->num_nlist);

    // free lists
    unsigned int i;
    unsigned int j;
    for (i=0; i<_q->M; i++) free(_q->mlist[i]);
    for (j=0; j<_q->N; j++) free(_q->nlist[j]);
    free(_q->mlist);
    free(_q->nlist);

    // free main object memory
    free(_q);
}

// print compact form
void smatrix_print(smatrix _q)
{
    printf("%u %u\n", _q->M, _q->N);
    printf("%u %u\n", _q->max_num_mlist, _q->max_num_nlist);
    unsigned int i;
    unsigned int j;
    for (i=0; i<_q->M; i++) printf("%u ", _q->num_mlist[i]);
    printf("\n");
    for (j=0; j<_q->N; j++) printf("%u ", _q->num_nlist[j]);
    printf("\n");

    // print mlist
    for (i=0; i<_q->M; i++) {
        for (j=0; j<_q->num_mlist[i]; j++)
            printf("%u ", _q->mlist[i][j]);
        if (_q->num_mlist[i] > 0)
            printf("\n");
    }

    // print nlist
    for (j=0; j<_q->N; j++) {
        for (i=0; i<_q->num_nlist[j]; i++)
            printf("%u ", _q->nlist[j][i]);
        if (_q->num_nlist[j] > 0)
            printf("\n");
    }
}

// print expanded form
void smatrix_print_expanded(smatrix _q)
{
}

// zero all elements
void smatrix_zero(smatrix _q)
{
    unsigned int i;
    unsigned int j;
    for (i=0; i<_q->M; i++) _q->num_mlist[i] = 0;
    for (j=0; j<_q->N; j++) _q->num_nlist[j] = 0;

    _q->max_num_mlist = 0;
    _q->max_num_nlist = 0;
}

// initialize to identity matrix
void smatrix_eye(smatrix _q)
{
}

