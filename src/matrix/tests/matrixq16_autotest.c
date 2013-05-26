/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/matrix/tests/matrix_data.h"

// test matrix addition
void autotest_matrixq16_add()
{
    float tol = 0.008f;

    // convert floating-point precision to fixed
    q16_t x[20];
    q16_t y[20];
    q16_t z[20];
    q16_memmove_float_to_fixed(x, matrixf_data_add_x, 20);
    q16_memmove_float_to_fixed(y, matrixf_data_add_y, 20);

    matrixq16_add(x,y,z,5,4);

    unsigned int i;
    for (i=0; i<20; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), matrixf_data_add_z[i], tol );
}

// test matrix augmentation
void autotest_matrixq16_aug()
{
    float tol = 0.008f;

    // x [size: 5 x 4]
    // y [size: 5 x 3]
    // z [size: 5 x 7]
    q16_t x[20];
    q16_t y[15];
    q16_t z[35];
    q16_memmove_float_to_fixed(x, matrixf_data_aug_x, 20);
    q16_memmove_float_to_fixed(y, matrixf_data_aug_y, 15);
    matrixq16_aug(x, 5, 4,
                y, 5, 3,
                z, 5, 7);
    
    // print result
    if (liquid_autotest_verbose) {
        printf("augment:\n");
        printf("  x: ");        matrixq16_print(x,5,4);
        printf("  y: ");        matrixq16_print(y,5,3);
        printf("  expected: "); matrixf_print(matrixf_data_aug_z,5,7);
        printf("  z: ");        matrixq16_print(z,5,3);
    }

    unsigned int i;
    for (i=0; i<35; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), matrixf_data_aug_z[i], tol );
}

