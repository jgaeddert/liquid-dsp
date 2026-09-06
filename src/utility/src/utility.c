/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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

//
// utility.c
//
// Useful generic utilities
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"

// get scale for constant, particularly for plotting purposes
//  _val    : input value (e.g. 100e6)
//  _unit   : output unit character (e.g. 'M')
//  _scale  : output scale (e.g. 1e-6)
int liquid_get_scale(float   _val,
                     char *  _unit,
                     float * _scale)
{
    float v = fabsf(_val);
    if      (v < 1e-9) { *_scale = 1e12;  *_unit = 'p'; }
    else if (v < 1e-6) { *_scale = 1e9 ;  *_unit = 'n'; }
    else if (v < 1e-3) { *_scale = 1e6 ;  *_unit = 'u'; }
    else if (v < 1e+0) { *_scale = 1e3 ;  *_unit = 'm'; }
    else if (v < 1e3)  { *_scale = 1e0 ;  *_unit = ' '; }
    else if (v < 1e6)  { *_scale = 1e-3;  *_unit = 'k'; }
    else if (v < 1e9)  { *_scale = 1e-6;  *_unit = 'M'; }
    else if (v < 1e12) { *_scale = 1e-9;  *_unit = 'G'; }
    else if (v < 1e15) { *_scale = 1e-12; *_unit = 'T'; }
    else               { *_scale = 1e-15; *_unit = 'P'; }
    return LIQUID_OK;
}

// convert a raw value into a metric-scaled magnitude and return the unit prefix
// example: 0.01397 -> 13.97 with unit 'm'
char liquid_convert_units(float * _v)
{
    float scale;
    char  unit;
    liquid_get_scale(*_v, &unit, &scale);
    *_v *= scale;
    return unit;
}

// compare two values (e.g. qsort), single-precision float
int liquid_compare_float(const void * _a, const void* _b)
{
    return *(float*)_a > *(float*)_b ? 1 : -1;
}

// determine if all requested keywords are present in the existing keywords
//  _existing  : comma-separated list of existing keywords, e.g. "fir,filter"
//  _requested : comma-separated list of requested keywords, e.g. "fir"
// returns true if every non-empty keyword in _requested also appears in
// _existing as an exact, comma-delimited token match; an empty _requested
// is considered a subset of any _existing (returns true)
bool liquid_keywords_subset(const char * _existing,
                            const char * _requested)
{
    if (_requested == NULL)
        return true;

    const char * r = _requested;
    for (;;) {
        // extent of the current requested token (up to next comma or end)
        const char * rcomma = strchr(r, ',');
        size_t        rlen  = rcomma ? (size_t)(rcomma - r) : strlen(r);

        // each non-empty requested token must appear verbatim in _existing
        if (rlen > 0) {
            bool found = false;
            if (_existing != NULL) {
                const char * e = _existing;
                for (;;) {
                    const char * ecomma = strchr(e, ',');
                    size_t        elen  = ecomma ? (size_t)(ecomma - e) : strlen(e);
                    if (elen == rlen && strncmp(e, r, elen) == 0) {
                        found = true;
                        break;
                    }
                    if (ecomma == NULL)
                        break;
                    e = ecomma + 1;
                }
            }
            if (!found)
                return false;
        }

        if (rcomma == NULL)
            break;
        r = rcomma + 1;
    }
    return true;
}

