/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
    if (liquid_autotest_verbose) {
        bsequence_print(a);
        bsequence_print(b);
    }

    // generate test sequences
    bsequence ax = bsequence_create(n);
    bsequence bx = bsequence_create(n);
    bsequence_create_ccodes(ax, bx);

    unsigned int i;
    signed int raa, rbb;
    for (i=0; i<n; i++) {
        // correlate like sequences
        raa = 2*bsequence_correlate(a,ax) - n;
        rbb = 2*bsequence_correlate(b,bx) - n;

        if (liquid_autotest_verbose)
            printf("    %3u : raa + rbb = %d\n", i, raa+rbb);

        if (i==0) { CONTEND_EQUALITY(raa+rbb,2*n);  }
        else      { CONTEND_EQUALITY(raa+rbb,0);    }

        bsequence_circshift(ax);
        bsequence_circshift(bx);
    }
    
    // clean up memory
    bsequence_destroy(a);
    bsequence_destroy(b);
    bsequence_destroy(ax);
    bsequence_destroy(bx);
}

#endif 

