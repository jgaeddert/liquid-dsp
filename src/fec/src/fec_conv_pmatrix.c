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

#include "liquid.internal.h"

// 2/3-rate K=7 punctured convolutional code
int fec_conv27p23_matrix[4] = {
    1, 1,
    1, 0
};

// 3/4-rate K=7 punctured convolutional code
int fec_conv27p34_matrix[6] = {
    1, 1, 0,
    1, 0, 1
};

// 4/5-rate K=7 punctured convolutional code
int fec_conv27p45_matrix[8] = {
    1, 1, 1, 1,
    1, 0, 0, 0
};

