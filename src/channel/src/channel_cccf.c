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
// Complex floating-point channel
//

#include <complex.h>
#include "liquid.internal.h"

#define CHANNEL(name)   LIQUID_CONCAT(channel_cccf,name)
#define TO              float complex   // output type
#define TC              float complex   // coefficients type
#define TI              float complex   // input type
#define T               float           // primitive type

#include "channel.c"

