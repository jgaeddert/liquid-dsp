/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

#ifndef __LIQUID_SCRAMBLE_AUTOTEST_H__
#define __LIQUID_SCRAMBLE_AUTOTEST_H__

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

// compute basic entropy metric
float liquid_scramble_test_entropy(unsigned char * _x,
                                   unsigned int _n)
{
    unsigned int i;

    // count ones
    unsigned int num_ones=0;
    for (i=0; i<_n; i++)
        num_ones += liquid_c_ones[_x[i]];

    // compute probabilities (add small value to prevent
    // possible log(0))
    float p1 = (float)num_ones / (float)(8*_n) + 1e-12f;
    float p0 = 1.0 - p1;
    return -p0*log2f(p0) - p1*log2f(p1);
}

// helper function to keep code base small
void liquid_scramble_test(unsigned int _n)
{
    unsigned char x[_n];    // input data
    unsigned char y[_n];    // scrambled data
    unsigned char z[_n];    // unscrambled data

    unsigned int i;

    // initialize data array
    for (i=0; i<_n; i++)
        x[i] = 0x00;

    // scramble input
    memmove(y,x,_n);
    scramble_data(y,_n);

    // unscramble result
    memmove(z,y,_n);
    unscramble_data(z,_n);

    // ensure data are equivalent
    CONTEND_SAME_DATA(x,z,_n*sizeof(unsigned char));

    // compute entropy metric
    float H = liquid_scramble_test_entropy(y,_n);
    CONTEND_EXPRESSION( H > 0.8f );
}

void autotest_scramble_n16()     { liquid_scramble_test(16);  };
void autotest_scramble_n64()     { liquid_scramble_test(64);  };
void autotest_scramble_n256()    { liquid_scramble_test(256); };

void autotest_scramble_n11()     { liquid_scramble_test(11);  };
void autotest_scramble_n33()     { liquid_scramble_test(33);  };
void autotest_scramble_n277()    { liquid_scramble_test(277); };

#endif // __LIQUID_SCRAMBLE_AUTOTEST_H__

