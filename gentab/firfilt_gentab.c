/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

// generate a small set of pre-computed FIR filter coefficients

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <liquid/liquid.h>

// design halfband filter and export to file
int design_halfband(unsigned int _m, float _as, const char * _varname, FILE * _fid);

//
int main(int argc, char*argv[])
{
    FILE * fid = stdout;

    // design halfband filters
    design_halfband(12, 60, "liquid_firfilt_0", fid);
    design_halfband(12, 80, "liquid_firfilt_1", fid);
    design_halfband(24, 60, "liquid_firfilt_2", fid);
    design_halfband(24, 80, "liquid_firfilt_3", fid);

    return 0;
}

// design halfband filter and export to file
int design_halfband(unsigned int _m,
                    float        _as,
                    const char * _varname,
                    FILE *       _fid)
{
    // design filter
    unsigned int h_len = 4*_m + 1;
    float h[h_len];
    liquid_firdespm_halfband_as(_m, _as, h);

    // TODO: scale coefficients by 2 to have peak value at 1?

    // export coefficients
    fprintf(_fid,"\n");
    fprintf(_fid,"// half-band filter\n");
    fprintf(_fid,"unsigned int %s_m   = %3u;  // filter quarter-length, h_len = 4*m+1\n", _varname, _m);
    fprintf(_fid,"float        %s_as  = %.1f; // dB, filter stop-band suppression\n", _varname, _as);
    //fprintf(_fid,"float        %s_ft  = %.9f; // transition bandwidth...
    fprintf(_fid,"const float  %s[%u] =       // coefficients\n", _varname, h_len);
    fprintf(_fid,"{\n");
    unsigned int i;
    for (i=0; i<h_len; i++) {
        if ( ((i+1)%2)==0 )
            fprintf(_fid,"%20.12e,\n", h[i]);
        else if (i== 2*_m)
            fprintf(_fid,"  %4.1f,", h[i]);
        else {
            assert( h[i] == 0 );
            fprintf(_fid,"     0,");
        }
    }
    fprintf(_fid,"\n};\n");
    return LIQUID_OK;
}

