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
// Float buffer
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION       "f"

#define BUFFER_TYPE_FLOAT

#define CBUFFER(name)   LIQUID_CONCAT(cbufferf, name)
//#define SBUFFER(name)   LIQUID_CONCAT(sbufferf, name)
#define WDELAY(name)    LIQUID_CONCAT(wdelayf,  name)
#define WINDOW(name)    LIQUID_CONCAT(windowf,  name)

#define T float
#define BUFFER_PRINT_LINE(B,I) \
    printf("  : %12.8f", B->v[I]);
#define BUFFER_PRINT_VALUE(V) \
    printf("  : %12.4e", V);

#include "cbuffer.c"
//#include "sbuffer.c"
#include "wdelay.c"
#include "window.c"

