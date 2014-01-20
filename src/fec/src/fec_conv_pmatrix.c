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

// 5/6-rate K=7 punctured convolutional code
int fec_conv27p56_matrix[10] = {
    1, 1, 0, 1, 0,
    1, 0, 1, 0, 1
};

// 6/7-rate K=7 punctured convolutional code
int fec_conv27p67_matrix[12] = {
    1, 1, 1, 0, 1, 0,
    1, 0, 0, 1, 0, 1
};

// 7/8-rate K=7 punctured convolutional code
int fec_conv27p78_matrix[14] = {
    1, 1, 1, 1, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 1
};




// 2/3-rate K=9 punctured convolutional code
int fec_conv29p23_matrix[4] = {
    1, 1,
    1, 0
};

// 3/4-rate K=9 punctured convolutional code
int fec_conv29p34_matrix[6] = {
    1, 1, 1,
    1, 0, 0
};

// 4/5-rate K=9 punctured convolutional code
int fec_conv29p45_matrix[8] = {
    1, 1, 0, 1,
    1, 0, 1, 0
};

// 5/6-rate K=9 punctured convolutional code
int fec_conv29p56_matrix[10] = {
    1, 0, 1, 1, 0,
    1, 1, 0, 0, 1
};

// 6/7-rate K=9 punctured convolutional code
int fec_conv29p67_matrix[12] = {
    1, 1, 0, 1, 1, 0,
    1, 0, 1, 0, 0, 1
};

// 7/8-rate K=9 punctured convolutional code
int fec_conv29p78_matrix[14] = {
    1, 1, 0, 1, 0, 1, 1,
    1, 0, 1, 0, 1, 0, 0
};

