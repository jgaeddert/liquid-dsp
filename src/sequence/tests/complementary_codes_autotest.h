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

#ifndef __COMPLEMENTARY_CODES_AUTOTEST_H__
#define __COMPLEMENTARY_CODES_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: validate autocorrelation properties of
//           complementary codes
//
void autotest_ccodes() {
    // create and initialize codes
    unsigned int n=32;
    bsequence a = bsequence_create(n);
    bsequence b = bsequence_create(n);
    bsequence_create_ccodes(a, b);

    // print
    if (_autotest_verbose) {
        bsequence_print(a);
        bsequence_print(b);
    }
    
    // clean up memory
    bsequence_destroy(a);
    bsequence_destroy(b);
}

#endif 

