/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// optimal 16-QAM
//

#include <complex.h>

// optimal 16-QAM
const float complex modem_arb16opt[16] = {
     -0.87119000+ -0.87970000*_Complex_I,  -1.15090000+ -0.26101000*_Complex_I, 
     -1.10090000+  0.87457000*_Complex_I,  -0.81088000+  0.29689000*_Complex_I, 
     -0.21295000+ -0.91897000*_Complex_I,  -0.46984000+ -0.29804000*_Complex_I, 
     -0.43443000+  0.91820000*_Complex_I,  -0.16786000+  0.30338000*_Complex_I, 
      1.10000000+ -0.87511000*_Complex_I,   0.81125000+ -0.29671000*_Complex_I, 
      0.87134000+  0.87989000*_Complex_I,   1.15170000+  0.26143000*_Complex_I, 
      0.43379000+ -0.91801000*_Complex_I,   0.16807000+ -0.30335000*_Complex_I, 
      0.21246000+  0.91867000*_Complex_I,   0.47033000+  0.29787000*_Complex_I
};

