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
// Complex Float buffer
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION       "cf"

#define BUFFER_TYPE_CFLOAT

#define CBUFFER(name)   LIQUID_CONCAT(cbuffercf, name)
//#define SBUFFER(name)   LIQUID_CONCAT(sbuffercf, name)
#define WDELAY(name)    LIQUID_CONCAT(wdelaycf,  name)
#define WINDOW(name)    LIQUID_CONCAT(windowcf,  name)

#define T float complex
#define BUFFER_PRINT_LINE(B,I) \
    printf("  : %12.8f + %12.8f", crealf(B->v[I]), cimagf(B->v[I]));
#define BUFFER_PRINT_VALUE(V) \
    printf("  : %12.4e + %12.4e", crealf(V), cimagf(V));

#include "cbuffer.c"
//#include "sbuffer.c"
#include "window.c"
#include "wdelay.c"

