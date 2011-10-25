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

// create _M x _N matrix, initialized on array
smatrix smatrix_create_array(unsigned char * _v,
                             unsigned int    _m,
                             unsigned int    _n)
{
    // create object and allocate memory
    smatrix q = smatrix_create(_m,_n);

    // initialize elements
    unsigned int i;
    unsigned int j;
    for (i=0; i<_m; i++) {
        for (j=0; j<_n; j++) {
            if (_v[i*_n + j])
                smatrix_set(q,i,j);
        }
    }

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
    printf("dims : %u %u\n", _q->M, _q->N);
    printf("max  : %u %u\n", _q->max_num_mlist, _q->max_num_nlist);
    unsigned int i;
    unsigned int j;
    printf("rows :");
    for (i=0; i<_q->M; i++) printf(" %u", _q->num_mlist[i]);
    printf("\n");
    printf("cols :");
    for (j=0; j<_q->N; j++) printf(" %u", _q->num_nlist[j]);
    printf("\n");

    // print mlist
    printf("row indices:\n");
    for (i=0; i<_q->M; i++) {
        printf("  %3u :", i);
        for (j=0; j<_q->num_mlist[i]; j++)
            printf(" %u", _q->mlist[i][j]);
        printf("\n");
    }

    // print nlist
    printf("column indices:\n");
    for (j=0; j<_q->N; j++) {
        printf("  %3u :", j);
        for (i=0; i<_q->num_nlist[j]; i++)
            printf(" %u", _q->nlist[j][i]);
        printf("\n");
    }
}

// print expanded form
void smatrix_print_expanded(smatrix _q)
{
    unsigned int i;
    unsigned int j;
    unsigned int t;

    // print in expanded 'regular' form
    for (i=0; i<_q->M; i++) {
        // reset counter
        t = 0;
        for (j=0; j<_q->N; j++) {
            if (t == _q->num_mlist[i])
                printf(" 0");
            else if (_q->mlist[i][t] == j) {
                printf(" 1");
                t++;
            } else
                printf(" 0");
        }
        printf("\n");
    }
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

// query element at index
unsigned char smatrix_get(smatrix _q,
                          unsigned int _m,
                          unsigned int _n)
{
    // validate input
    if (_m >= _q->M || _n >= _q->N) {
        fprintf(stderr,"error: smatrix_get(), index exceeds matrix dimension\n");
        exit(1);
    }

    unsigned int j;
    for (j=0; j<_q->num_mlist[_m]; j++) {
        if (_q->mlist[_m][j] == _n)
            return 1;
    }
    return 0;
}

// set element at index
void smatrix_set(smatrix _q,
                 unsigned int _m,
                 unsigned int _n)
{
    // validate input
    if (_m >= _q->M || _n >= _q->N) {
        fprintf(stderr,"error: smatrix_set(), index exceeds matrix dimension\n");
        exit(1);
    }

    // check to see if element is already set
    if (smatrix_get(_q,_m,_n))
        return;

    // increment list sizes
    _q->num_mlist[_m]++;
    _q->num_nlist[_n]++;

    // reallocate lists at this index
    _q->mlist[_m] = (unsigned short int*) realloc(_q->mlist[_m], _q->num_mlist[_m]*sizeof(unsigned short int));
    _q->nlist[_n] = (unsigned short int*) realloc(_q->nlist[_n], _q->num_nlist[_n]*sizeof(unsigned short int));

    // append index to end of list
    // TODO : insert index at appropriate point
    _q->mlist[_m][_q->num_mlist[_m]-1] = _n;
    _q->nlist[_n][_q->num_nlist[_n]-1] = _m;

    // update maximum
    if (_q->num_mlist[_m] > _q->max_num_mlist) _q->max_num_mlist = _q->num_mlist[_m];
    if (_q->num_nlist[_n] > _q->max_num_nlist) _q->max_num_nlist = _q->num_nlist[_n];
}

// clear element at index
void smatrix_clear(smatrix _q,
                   unsigned int _m,
                   unsigned int _n)
{
    // validate input
    if (_m > _q->M || _n > _q->N) {
        fprintf(stderr,"error: smatrix_clear(), index exceeds matrix dimension\n");
        exit(1);
    }

    // check to see if element is already set
    if (!smatrix_get(_q,_m,_n))
        return;

    // remove value from mlist (shift left)
    unsigned int i;
    unsigned int j;
    unsigned int t=0;
    for (j=0; j<_q->num_mlist[_m]; j++) {
        if (_q->mlist[_m][j] == _n)
            t = j;
    }
    for (j=t; j<_q->num_mlist[_m]-1; j++)
        _q->mlist[_m][j] = _q->mlist[_m][j+1];

    // remove value from nlist (shift left)
    t = 0;
    for (i=0; i<_q->num_nlist[_n]; i++) {
        if (_q->nlist[_n][i] == _m)
            t = i;
    }
    for (i=t; i<_q->num_nlist[_n]-1; i++)
        _q->nlist[_n][i] = _q->nlist[_n][i+1];

    // reduce sizes
    _q->num_mlist[_m]--;
    _q->num_nlist[_n]--;

    // reallocate
    _q->mlist[_m] = (unsigned short int*) realloc(_q->mlist[_m], _q->num_mlist[_m]*sizeof(unsigned short int));
    _q->nlist[_n] = (unsigned short int*) realloc(_q->nlist[_n], _q->num_nlist[_n]*sizeof(unsigned short int));

    // reset maxima
    if (_q->max_num_mlist == _q->num_mlist[_m]+1)
        smatrix_reset_max_mlist(_q);

    if (_q->max_num_nlist == _q->num_nlist[_n]+1)
        smatrix_reset_max_nlist(_q);
}

// initialize to identity matrix
void smatrix_eye(smatrix _q)
{
    // zero all elements
    smatrix_zero(_q);

    // set values along diagonal
    unsigned int i;
    unsigned int dmin = _q->M < _q->N ? _q->M : _q->N;
    for (i=0; i<dmin; i++)
        smatrix_set(_q, i, i);
}

// multiply two sparse binary matrices
void smatrix_mul(smatrix _a,
                 smatrix _b,
                 smatrix _c)
{
    // validate input
    if (_c->M != _a->M || _c->N != _b->N || _a->N != _b->M) {
        fprintf(stderr,"error: smatrix_mul(), invalid dimensions\n");
        exit(1);
    }

    // clear output matrix
    smatrix_zero(_c);

    unsigned int r; // output row
    unsigned int c; // output column
    
    unsigned int i;
    unsigned int j;

    unsigned int p; // running binary sum

    for (r=0; r<_c->M; r++) {
        for (c=0; c<_c->N; c++) {

            p = 0;
            // find common elements between non-zero elements in
            // row 'r' of matrix '_a' and col 'c' of matrix '_b'
#if 0
            // 'get' value for each index (slow method)
            for (i=0; i<_a->N; i++)
                p += smatrix_get(_a,r,i) & smatrix_get(_b,i,c);
#else
            // parse lists looking for commonality
            for (i=0; i<_a->num_mlist[r]; i++) {
                for (j=0; j<_b->num_nlist[c]; j++)
                    p += (_a->mlist[r][i] == _b->nlist[c][j]) ? 1 : 0;
            }
#endif
            // set output (modulo 2)
            if ( p & 0x001 )
                smatrix_set(_c, r, c);
        }
    }
}

// multiply by vector (modulo 2)
//  _q  :   sparse matrix
//  _x  :   input vector [size: _N x 1]
//  _y  :   output vector [size: _M x 1]
void smatrix_vmul(smatrix _q,
                  unsigned char * _x,
                  unsigned char * _y)
{
    unsigned int i;
    unsigned int j;
    
    // initialize to zero
    for (i=0; i<_q->M; i++)
        _y[i] = 0;

    // only flip necessary bits
    for (j=0; j<_q->N; j++) {
        if (_x[j]) {
            for (i=0; i<_q->num_nlist[j]; i++)
                _y[ _q->nlist[j][i] ] ^= 1; // add 1 (modulo 2)
        }
    }
}

// semi-internal methods


// find maximum mlist length
void smatrix_reset_max_mlist(smatrix _q)
{
    unsigned int i;

    _q->max_num_mlist = 0;
    for (i=0; i<_q->M; i++) {
        if (_q->num_mlist[i] > _q->max_num_mlist)
            _q->max_num_mlist = _q->num_mlist[i];
    }
}

// find maximum nlist length
void smatrix_reset_max_nlist(smatrix _q)
{
    unsigned int j;

    _q->max_num_nlist = 0;
    for (j=0; j<_q->N; j++) {
        if (_q->num_nlist[j] > _q->max_num_nlist)
            _q->max_num_nlist = _q->num_nlist[j];
    }
}
