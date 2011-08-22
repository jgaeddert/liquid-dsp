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
// optimal 32-QAM
//

#include <complex.h>

// optimal 32-QAM
const float complex modem_arb32opt[32] = {
     -1.04570000+ -0.72639000*_Complex_I,  -1.26710000+ -0.00824020*_Complex_I, 
     -0.99868000+  0.92359000*_Complex_I,  -1.13660000+  0.45833000*_Complex_I, 
     -0.67812000+ -1.07470000*_Complex_I,  -0.88363000+ -0.25914000*_Complex_I, 
     -0.64443000+  0.62407000*_Complex_I,  -0.76238000+  0.18163000*_Complex_I, 
     -0.20985000+ -0.91032000*_Complex_I,  -0.08603100+ -0.45730000*_Complex_I, 
     -0.18073000+  0.75747000*_Complex_I,   0.02826900+ -0.01908400*_Complex_I, 
     -0.55206000+ -0.61092000*_Complex_I,  -0.42946000+ -0.16611000*_Complex_I, 
     -0.51187000+  1.09190000*_Complex_I,  -0.30769000+  0.27731000*_Complex_I, 
      1.20280000+ -0.58421000*_Complex_I,   1.29120000+ -0.09249800*_Complex_I, 
      1.10450000+  0.69170000*_Complex_I,   0.96433000+  0.24011000*_Complex_I, 
      0.60130000+ -1.16500000*_Complex_I,   0.83106000+ -0.23576000*_Complex_I, 
      0.73090000+  1.01880000*_Complex_I,   0.62090000+  0.54470000*_Complex_I, 
      0.23884000+ -0.81583000*_Complex_I,   0.36064000+ -0.36749000*_Complex_I, 
     -0.02439300+  1.23570000*_Complex_I,   0.15492000+  0.41465000*_Complex_I, 
      0.11172000+ -1.28340000*_Complex_I,   0.70560000+ -0.68005000*_Complex_I, 
      0.28323000+  0.89268000*_Complex_I,   0.48844000+  0.10367000*_Complex_I
};

