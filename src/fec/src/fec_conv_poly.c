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
// convolutional code polynomials
//

#include "liquid.internal.h"

#if LIBFEC_ENABLED
#include <fec.h>

int fec_conv27_poly[2]  = {V27POLYA,
                           V27POLYB};

int fec_conv29_poly[2]  = {V29POLYA,
                           V29POLYB};

int fec_conv39_poly[3]  = {V39POLYA,
                           V39POLYB,
                           V39POLYC};

int fec_conv615_poly[6] = {V615POLYA,
                           V615POLYB,
                           V615POLYC,
                           V615POLYD,
                           V615POLYE,
                           V615POLYF};

#else

int fec_conv27_poly[2]  = {0,0};
int fec_conv29_poly[2]  = {0,0};
int fec_conv39_poly[3]  = {0,0,0};
int fec_conv615_poly[6] = {0,0,0,0,0,0};

#endif

