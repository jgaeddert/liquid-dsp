/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
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

//
// fixed-point buffer
//

#include "liquid.internal.h"

//#define BUFFER(name)    LIQUID_CONCAT(buffercq16, name)
//#define WDELAY(name)    LIQUID_CONCAT(wdelaycq16, name)
#define WINDOW(name)    LIQUID_CONCAT(windowcq16, name)

#define T cq16_t
#define BUFFER_PRINT_LINE(B,I)  printf("    : %12.8f + j%12.8f", q16_fixed_to_float(B->v[I].real), q16_fixed_to_float(B->v[I].imag));
#define BUFFER_PRINT_VALUE(V)   printf("    : %12.4f + j%12.8f", q16_fixed_to_float(V.real), q16_fixed_to_float(V.imag));

//#include "buffer.c"
//#include "wdelay.c"
#include "window.c"

