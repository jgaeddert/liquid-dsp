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
// 
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "liquid.internal.h"

// esimate required filter length given
//   _b    : transition bandwidth (0 < b < 0.5)
//   _slsl : sidelobe suppression level [dB]
unsigned int estimate_req_filter_len(float _b, float _slsl)
{
    if (_b > 0.5f || _b <= 0.0f) {
        printf("error: estimate_req_filter_len(), invalid bandwidth : %f\n", _b);
        exit(0);
    }

    if (_slsl <= 0.0f) {
        printf("error: estimate_req_filter_len(), invalid sidelobe level : %f\n", _slsl);
        exit(0);
    }

    unsigned int h_len;
    if (_slsl < 8) {
        h_len = 2;
    } else {
        h_len = (unsigned int) lroundf((_slsl-8)/(14*_b));
    }
    validate_filter_length(&h_len);
    return h_len;
}


