/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(keywords_subset,
    "test liquid_keywords_subset(_existing, _requested) that returns true when"
    " every non-empty keyword in _requested appears in _existing as an exact,"
    " comma-delimited token match (case-sensitive, no whitespace trimming)",
    "utility,keywords",0.1)
{
    // single keyword: exact match
    LIQUID_CHECK( liquid_keywords_subset("FIR,filter", "filter") );
    LIQUID_CHECK( liquid_keywords_subset("FIR,filter", "FIR") );

    // ensuring case sensitivity
    LIQUID_CHECK( !liquid_keywords_subset("FIR,filter", "fir") );
    LIQUID_CHECK( !liquid_keywords_subset("FIR,filter", "Filter") );

    // multiple requested keywords: order does not matter
    LIQUID_CHECK( liquid_keywords_subset("fft,composite,prime", "fft,composite") );
    LIQUID_CHECK( liquid_keywords_subset("fft,composite,prime", "prime,fft") );

    // missing keyword: not a subset
    LIQUID_CHECK( !liquid_keywords_subset("fft,composite", "radix2") );
    LIQUID_CHECK( !liquid_keywords_subset("fft,composite", "fft,radix2") );

    // matching is per-token, not substring: "fir" != "firfilt"
    LIQUID_CHECK( !liquid_keywords_subset("firfilt,filter", "fir") );

    // no whitespace trimming: tokens must match exactly, including spaces
    LIQUID_CHECK( !liquid_keywords_subset("FIR, filter", "filter") );  // "filter" != " filter"
    LIQUID_CHECK( liquid_keywords_subset("FIR, filter", " filter") );  // matches " filter" exactly

    // empty requested is a subset of anything (vacuously true)
    LIQUID_CHECK( liquid_keywords_subset("fft,composite", "") );
    LIQUID_CHECK( liquid_keywords_subset(NULL, "") );
    LIQUID_CHECK( liquid_keywords_subset("fft", NULL) );
    LIQUID_CHECK( liquid_keywords_subset("", ",,") );

    // non-empty requested against empty existing is not a subset
    LIQUID_CHECK( !liquid_keywords_subset("",   "fft") );
    LIQUID_CHECK( !liquid_keywords_subset(NULL, "fft") );

    // duplicate requested keywords are harmless
    LIQUID_CHECK( liquid_keywords_subset("fft,composite", "fft,fft") );
}

