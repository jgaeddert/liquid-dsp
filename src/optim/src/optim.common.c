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
// optim.common.c
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"


// optimization threshold switch
//  _u0         :   first utility
//  _u1         :   second utility
//  _minimize   :   minimize flag
//
// returns:
//  (_u0 > _u1) if (_minimize == 1)
//  (_u0 < _u1) otherwise
int optim_threshold_switch(float _u0,
                           float _u1,
                           int _minimize)
{
    return _minimize ? _u0 > _u1 : _u0 < _u1;
}

// sort values by index
//  _v          :   input values [size: _len x 1]
//  _rank       :   output rank array (indices) [size: _len x 1]
//  _len        :   length of input array
//  _descending :   descending/ascending
void optim_sort(float *_v,
                unsigned int * _rank,
                unsigned int _len,
                int _descending)
{
    unsigned int i, j, tmp_index;

    for (i=0; i<_len; i++)
        _rank[i] = i;

    for (i=0; i<_len; i++) {
        for (j=_len-1; j>i; j--) {
            //if (_v[_rank[j]]>_v[_rank[j-1]]) {
            if ( optim_threshold_switch(_v[_rank[j]], _v[_rank[j-1]], _descending) ) {
                // swap elements
                tmp_index = _rank[j];
                _rank[j] = _rank[j-1];
                _rank[j-1] = tmp_index;
            }
        }
    }
}


