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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: validate autocorrelation properties of
//           complementary codes
//
void complementary_codes_test(unsigned int _n)
{
    // create and initialize codes
    bsequence a = bsequence_create(_n);
    bsequence b = bsequence_create(_n);
    bsequence_create_ccodes(a, b);

    // print
    if (liquid_autotest_verbose) {
        bsequence_print(a);
        bsequence_print(b);
    }

    // generate test sequences
    bsequence ax = bsequence_create(_n);
    bsequence bx = bsequence_create(_n);
    bsequence_create_ccodes(ax, bx);

    unsigned int i;
    signed int raa, rbb;
    for (i=0; i<_n; i++) {
        // correlate like sequences
        raa = 2*bsequence_correlate(a,ax) - _n;
        rbb = 2*bsequence_correlate(b,bx) - _n;

        if (i==0) { CONTEND_EQUALITY(raa+rbb,2*_n); }
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

void autotest_complementary_code_n8()       {   complementary_codes_test(8);    }
void autotest_complementary_code_n16()      {   complementary_codes_test(16);   }
void autotest_complementary_code_n32()      {   complementary_codes_test(32);   }
void autotest_complementary_code_n64()      {   complementary_codes_test(64);   }
void autotest_complementary_code_n128()     {   complementary_codes_test(128);  }
void autotest_complementary_code_n256()     {   complementary_codes_test(256);  }
void autotest_complementary_code_n512()     {   complementary_codes_test(512);  }

