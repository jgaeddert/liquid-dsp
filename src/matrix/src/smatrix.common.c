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
// sparse matrices: common methods
//

// search for index placement in list
unsigned short int smatrix_indexsearch(unsigned short int * _list,
                                       unsigned int         _num_elements,
                                       unsigned short int   _value)
{
    // TODO: use bisection method
    
    unsigned int i;
    for (i=0; i<_num_elements; i++) {
        if (_list[i] > _value)
            break;
    }

    //
    return i;
}

